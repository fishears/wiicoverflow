#include "buffer.h"
#include "core/disc.h"
#include <fatsvn.h>
#include "coverflow.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define DENSITY_METHOD 1
#define ALL_CACHED 2

// this is the start adrress of MEM2 see http://wiibrew.org/wiki/Memory_Map
#define MEM2_START_ADDRESS 0x90000000
//this is lower than the extent address of MEM2 which should be 54394880 (0x33E0000) - but there is a crash before that point
//  IOS has the range 0x933E0000-0x93400000  for a heap but who knows what IOS 249 uses, if there are issues at the end of memory
// then lower this number
#define MEM2_EXTENT 54217216 //don't see why this cant be used

// private cars
typedef struct COVERQUEUE 
{
	bool ready[MAX_BUFFERED_COVERS];
	bool request[MAX_BUFFERED_COVERS];
	struct discHdr *requestId[MAX_BUFFERED_COVERS];
	bool remove[MAX_BUFFERED_COVERS];
	int permaBufferPosition[MAX_BUFFERED_COVERS];
	bool coverMissing[MAX_BUFFERED_COVERS];
	int floatingQueuePosition[MAX_BUFFERED_COVERS];
} COVERQUEUE;

COVERQUEUE _cq;
int BufferMethod=DENSITY_METHOD; // what buffering method is used either density or all buffered
float Density; // if density method is used then this is the fill rate for the permanent cache
int MainCacheSize; // the number of slots available to permanently cache covers
int maxSlots; // the total number of slots available for cache
int FloatingCacheSize=0; // the number of floating slots available (for non permanent covers)
int CurrentSelection;
struct discHdr *CoverList;
int nCovers=0;
int nCoversInWindow; // number of covers shown in the GUI
int graphicMode=0; // the graphic mode for the covers - this relates to the cover sizes
int textureDataSize; // the size needed for a texture in this graphic mode
// end of private vars

int GraphicModes[GRAPHIC_MODES][2] =
{
	{160,224},//2d
	{512,340} //3d
};

// this is the chunk of MEM2 allocated for other tasks (i.e. things that don't fit in normal memory)
#define BUFFER_SLOTS 7
unsigned int FreeMemorySlots[BUFFER_SLOTS+1] =
	{
	128,//0 string buffer for thread 0
	128,//1 string buffer for thread 1
	128,//2 string buffer for thread 2
	128,//3 string buffer for thread 3
	128,//4 string buffer for thread 4
	512*340*4, // 5 3d no cover
	240*480*4, //6 ambilight
	0};

// returns the offset to the memory slot required
int GetOffsetToSlot(int slot)
{
	int i=0;
	unsigned int offset=0;
	if ((slot>BUFFER_SLOTS)) return offset;
	for (i=0;i<slot;i++)
	{
		offset+=FreeMemorySlots[i];
	}
	return offset;
}

//gets the address of a memory slot
void * GetSlotBufferAddress(int slot)
{
	unsigned int offset=0;
	if ((slot>BUFFER_SLOTS)) return (void *)offset;
	offset=GetOffsetToSlot(slot);
	return (void *) MEM2_START_ADDRESS+MEM2_EXTENT-GetOffsetToSlot(BUFFER_SLOTS)+offset;
}


#include <unistd.h>
void Sleep(unsigned long milliseconds)
{
	if (milliseconds<1000)
		usleep(milliseconds*1000);
	else
		sleep(milliseconds/1000);
}

// sets up mutexes and creates the threads
void BUFFER_InitBuffer()
{
	int i = 0;
	
	/*Initialize Mutexs*/
	pthread_mutex_init(&count_mutex, 0);
	pthread_mutex_init(&queue_mutex, 0);
	pthread_mutex_init(&quit_mutex, 0);
	pthread_mutex_init(&lock_thread_mutex, 0);
	pthread_mutex_init(&covers_3d_mutex, 0);
	
	for(i = 0; i < MAX_BUFFERED_COVERS; i++)
	{
		pthread_mutex_init(&buffer_mutex[i], 0);
		_texture_data[i].data=0;
	}
	
	_requestQuit = false;

	for(i = 0; i < MAX_THREADS; i++)
	{
		if(i < MAX_THREADS)
			pthread_create(&thread[i], 0, process, (void *)i);
	}
}

// waits for the threads to stop and cleans up
void BUFFER_Shutdown()
{
	int i,m;
	
	for(i = 0; i < MAX_THREADS; i++)
	{
		if (thread[i])
			LWP_JoinThread(thread[i], NULL);
	}
	/*destroy Mutexs*/
	pthread_mutex_destroy(&count_mutex);
	pthread_mutex_destroy(&queue_mutex);
	pthread_mutex_destroy(&quit_mutex);
	pthread_mutex_destroy(&lock_thread_mutex);
	pthread_mutex_destroy(&covers_3d_mutex);
	
	for(m = 0; m < MAX_BUFFERED_COVERS; m++)
		pthread_mutex_destroy(&buffer_mutex[m]);
	
}


bool BUFFER_IsCoverReady(int index)
{
	bool retval = false;
	
	if(index < MAX_BUFFERED_COVERS)
	{
		pthread_mutex_lock(&queue_mutex);
		retval = _cq.ready[index];
		pthread_mutex_unlock(&queue_mutex);
	}
	
	return retval;
}

bool BUFFER_IsCoverQueued(int index)
{
	bool retval = false;
	
	if(index < MAX_BUFFERED_COVERS)
	{
		pthread_mutex_lock(&queue_mutex);
		retval = _cq.request[index];
		pthread_mutex_unlock(&queue_mutex);
	}
	
	return retval;
}


//determines if a cover is missing or a bad image
bool BUFFER_IsCoverMissing(int index)
{
	bool retval = false;
	
	if(index < MAX_BUFFERED_COVERS)
	{
		pthread_mutex_lock(&queue_mutex);
		retval = _cq.coverMissing[index];
		pthread_mutex_unlock(&queue_mutex);
	}
	
	return retval;
}

// sets the thread stop flag and waits for shutdown
void BUFFER_KillBuffer()
{
	
	pthread_mutex_lock(&quit_mutex);
	_requestQuit = true;
	pthread_mutex_unlock(&quit_mutex);
	BUFFER_Shutdown();
}

//finds the position in the floating queue for a cover
int GetFLoatingQueuePosition(int index)
{
	if(index >= MAX_BUFFERED_COVERS)
		return -1;

	int i,ret=-1;
	for (i=0;i<FloatingCacheSize;i++)
	{
		if (FloatingCacheCovers[i] == index) ret= i;
	}
	return ret;
}

//  This does the actual work
void HandleLoadRequest(int index,int threadNo)
{
	if(index >= nCovers)
		return ;
		
//	pthread_mutex_lock(&buffer_mutex[index]);
	
	/*Definitely dont need to load the same texture twice*/
	if(!_cq.ready[index] && !_cq.coverMissing[index])
	{
		
		//void *imgData=0;
		
		char * filepath=GetSlotBufferAddress(threadNo);
		s32  ret;
		
		int tW = GraphicModes[graphicMode][0];
		int tH = GraphicModes[graphicMode][1];
		
		// determines the cover location
		if(!graphicMode)
		{
			snprintf(filepath,256, USBLOADER_PATH "/covers/%s.png", _cq.requestId[index]->id);
		}
		else
		{
			snprintf(filepath,256, USBLOADER_PATH "/3dcovers/%c%c%c%c.png", _cq.requestId[index]->id[0], _cq.requestId[index]->id[1], _cq.requestId[index]->id[2], _cq.requestId[index]->id[3]);
		}
		
		int imgDataAddress=MEM2_START_ADDRESS + tW * tH * 4 * (maxSlots+threadNo);
		ret = Fat_ReadFileToBuffer(filepath,(void *) imgDataAddress,tW * tH * 4);
		
		if (ret > 0) 
		{
			int thisDataMem2Address=-1;
			if (_cq.permaBufferPosition[index]!=-1)// permanent cache
			{
				thisDataMem2Address=MEM2_START_ADDRESS + tW * tH * 4 * _cq.permaBufferPosition[index];
			}
			else // floating cache
			{
				int floatingPosition=GetFLoatingQueuePosition(index); // should have a slot allocated
				if (floatingPosition!=-1)
				{
					thisDataMem2Address=MEM2_START_ADDRESS+ (MainCacheSize + floatingPosition) * tW * tH * 4 ;
				}
				else
				{
					thisDataMem2Address=-1;
				}
			}
			if (thisDataMem2Address!=-1) // we have a slot, load the texture to it
			{
				// a garbled image will not cause an issue here
				_texture_data[index] = GRRLIB_LoadTexturePNGToMemorySized((const unsigned char*)imgDataAddress, (void *)thisDataMem2Address, tW * tH * 4);
				GRRLIB_texImg textureData=_texture_data[index];
				pthread_mutex_lock(&queue_mutex);
				if (!(textureData.h ==tH && textureData.w == tW && textureData.data!=0)) // sanity check
				{
					_cq.coverMissing[index]=true; // bad image size
					_cq.ready[index]   = false;
				}
				else
				{
					_cq.ready[index]   = true;
				}
				pthread_mutex_unlock(&queue_mutex);
			}
			//free(imgData);
			
		}
		else // no cover file
		{
			pthread_mutex_lock(&queue_mutex);
			if (_cq.permaBufferPosition[index]!=-1)
			{
				int floatingPosition=GetFLoatingQueuePosition(index);
				if (floatingPosition!=-1)
				{
				  FloatingCacheCovers[floatingPosition]=-1;
				}
			}
			_cq.coverMissing[index]=true;
			_cq.ready[index]   = false;
			pthread_mutex_unlock(&queue_mutex);
		}
		
	}
	//pthread_mutex_unlock(&buffer_mutex[index]);
}

// get a cover in view which hasn't been loaded yet
int GetPrioritisedCover(int selection)
{
	int i,j,ret=-1;
	for(i = 0; i <= (nCoversInWindow+1)/2; i++)
	{
		for (j=0;j<2;j++)
		{
			int index = selection +i*(j*2-1); 
			if (index>=0 && index<=nCovers)
			{
				if(index >= MAX_BUFFERED_COVERS)
					return -1;
		
				if (_cq.request[index] && !_cq.ready[index]) return index;
			}
		}
	}
	return ret;
}

// thread loop
void* process(void *arg)
{
	int thread = (int)arg;
	int i,j = 0;
	bool b = false;
	/*Main Buffering Thread*/
	//Fat_MountSDHC();
	
	while(1)
	{
		
		//Load Covers from the middle out
		for(i = 0; i <= (nCovers+1)/2; i++)
		{
			for (j=0;j<2;j++)
			{
				int index=GetPrioritisedCover(CurrentSelection);
				
				if (index==-1) index= (nCovers+1)/2 +i*(j*2-1);
				if (index>=0 && index<=nCovers)
				{
					pthread_mutex_lock(&lock_thread_mutex);
					
					/*Handle Load Requests*/
					pthread_mutex_lock(&queue_mutex);
					b = _cq.request[index] && !_cq.ready[index];
					if (b) _cq.request[index]=false;
					pthread_mutex_unlock(&queue_mutex);
					
					if(b) HandleLoadRequest(index,thread);
					
					pthread_mutex_unlock(&lock_thread_mutex);
					
					pthread_mutex_lock(&quit_mutex);
					if(_requestQuit)
					{
						pthread_mutex_unlock(&quit_mutex);
						return 0;
					}
					pthread_mutex_unlock(&quit_mutex);
				}
			}        
		}
		
		
		usleep(10);// need to get the threads separated but this should free some processor
	}
	
	
}

// is the cover already cached
bool InCache(int index)
{
	if(index >= MAX_BUFFERED_COVERS)
		return false;
		
	bool ret=false;
	int i;
	for (i=0;i<FloatingCacheSize;i++)
	{
		if (FloatingCacheCovers[i]==index) ret=true;
	}
	
	return ret;
}

// internal only - no need to lock (already locked)
// takes a cover out of the floating cache
void RemoveFromCache(int index)
{
		
	if (_cq.floatingQueuePosition[FloatingCacheCovers[index]] != -1)
	{
		_cq.ready[FloatingCacheCovers[index]] = false;
		_cq.request[FloatingCacheCovers[index]] = false;
		_texture_data[FloatingCacheCovers[index]].data=0;
		_cq.floatingQueuePosition[FloatingCacheCovers[index]]=-1;
		
	}
	
}

//already locked
// put a cover into the floating cache, the current selection means we can remove the furthest away
void SetFloatingCacheItem(int selection, int newCacheItem)
{
	if (InCache(newCacheItem)) return;
	int i=0;
	bool spaceFound = false;
	int maxDistance=0;
	int maxIndex=-1;
	for (i=0;i<FloatingCacheSize;i++)
	{
		if (FloatingCacheCovers[i] == -1) // empty slot
		{
			spaceFound = true;
			FloatingCacheCovers[i] = newCacheItem;
			_cq.floatingQueuePosition[newCacheItem]=i;
			break;
		}
		if (FloatingCacheCovers[i]!=-1)
		{
			if (abs(FloatingCacheCovers[i] - selection) > maxDistance)
			{
				maxDistance = abs(FloatingCacheCovers[i] - selection);
				maxIndex=i;
			}
		}
	}
	if (!spaceFound) // no empty slot remove the furthest
	{
		
		RemoveFromCache(maxIndex);
		FloatingCacheCovers[maxIndex] = newCacheItem;
		_cq.floatingQueuePosition[newCacheItem]=maxIndex;
	}
	
	
}

// request all the visible floating cache covers
void iSetSelectedCover(int index, bool doNotRemoveFromFloating)
{
	int i=0;
	int extra = 0;
	CurrentSelection=nCovers-index;
	
	if (doNotRemoveFromFloating) extra = 1;
	int searchSize=nCoversInWindow*(2+extra);
	//this'll do for now - a more elegant algrorithm would be better
	for (i=CurrentSelection-searchSize/2;i<CurrentSelection+searchSize/2;i++)
	{
		if (i >= 0 && i < nCovers)
		{
		
			if (!_cq.ready[i] && !_cq.request[i] && _cq.permaBufferPosition[i] == -1 && !_cq.coverMissing[i])
			{
				// this one isn't permenantly cached make a space for it in the floating cache
				if (_cq.permaBufferPosition[i] == -1) SetFloatingCacheItem(CurrentSelection,i);
				
				_cq.requestId[i]=&CoverList[i];
				_cq.request[i] = true;
				_cq.ready[i] = false;
			}
			
		}
	}
}

// main application entry point
void SetSelectedCover(int index)
{
	iSetSelectedCover(index+nCovers/2.0,false);
}


// internal only - no need to lock (already locked)
// reset all values for a cover
void ResetQueueItem(int index)
{
	if(index >= MAX_BUFFERED_COVERS)
		return;
		
	_cq.ready[index]=false;
	_cq.request[index]=false;
	_cq.requestId[index]=0;
	_cq.remove[index]=false;
	_cq.permaBufferPosition[index]=-1;
	_cq.coverMissing[index]=false;
	_cq.floatingQueuePosition[index]=-1;
	_texture_data[index].data=0;
}


// internal only - no need to lock (already locked)
// request for an item to be loaded (permanently)
void RequestForCache(int index)
{
	if(index >= MAX_BUFFERED_COVERS)
		return;
		
	_cq.requestId[index]=&CoverList[index];
	_cq.request[index]=true;
}

// call at start, on add or on delete - kill the buffer first
void InitializeBuffer(struct discHdr *gameList,int gameCount,int numberOfCoversToBeShown,int initialSelection, int graphMode)
{
	graphicMode=graphMode;
	textureDataSize=GraphicModes[graphicMode][0]*GraphicModes[graphicMode][1]*4;
	CurrentSelection=initialSelection+gameCount/2.0;
	int i=0;
	CoverList=gameList;
	nCovers=gameCount;
	nCoversInWindow=numberOfCoversToBeShown;
	
	//start from a clear point
	for (i=0;i<gameCount;i++) ResetQueueItem(i);
	memset((void *)MEM2_START_ADDRESS,0,MEM2_EXTENT-GetOffsetToSlot(BUFFER_SLOTS)); // clear all the memory
	
	_covers3d=graphMode;
	
	maxSlots=(MEM2_EXTENT-GetOffsetToSlot(BUFFER_SLOTS))/(textureDataSize)-MAX_THREADS; // this is the number of slots available
	
	//decide on buffering method
	if (gameCount<=maxSlots) // can we fit all the covers in memory
	{
		BufferMethod=ALL_CACHED;
		MainCacheSize=gameCount;
	}
	else //we can't
	{
		BufferMethod=DENSITY_METHOD;
		Density = ((float)maxSlots) / ((float)(gameCount));
		FloatingCacheSize=(int)(((1-Density)*numberOfCoversToBeShown+1)*4);// works well for 2d may be too big for 3d
		if (FloatingCacheSize>maxSlots)
		{
			FloatingCacheSize=maxSlots;
		}
		MainCacheSize = maxSlots - FloatingCacheSize;
		Density = ((float)(MainCacheSize-1)) / ((float)(gameCount)); // the -1 is important - how many fence posts to cover 10 meters if they are 1 meter apart
		for (i=0;i<FloatingCacheSize;i++)
		{
			FloatingCacheCovers[i]=-1;
		}
	}
	if (BufferMethod==ALL_CACHED)
	{
		for (i=0;i<gameCount;i++)
		{
			_cq.permaBufferPosition[i]=i;
		}
	}
	else // allocate all the permanent covers we can, minimising the maximum number of cached covers on screen
	{
		int lastValue=-1;
		double dPosition=0;
		for (i=0;i<gameCount;i++)
		{
			dPosition+=Density;
			if ((int)dPosition!=lastValue)
			{
				_cq.permaBufferPosition[i]=(int)dPosition;
			}
			else
			{
				_cq.permaBufferPosition[i]=-1;
			}
			lastValue = (int)dPosition;
			
		}
	}
	
	//now request all the permenant covers
	for (i=0;i<gameCount;i++) 
	{
		if (_cq.permaBufferPosition[i]!=-1)
		{
			RequestForCache(i);
		}
	}
	iSetSelectedCover(initialSelection,true);
}

// covers got downloaded
void CoversDownloaded()
{
	int i;
	//recheck all the missing covers
	for (i=0;i<nCovers;i++)
	{
		pthread_mutex_lock(&queue_mutex);
		if (_cq.coverMissing[i])
		{
			_cq.coverMissing[i]=false;
			if (_cq.permaBufferPosition[i]!=-1)
			{
				RequestForCache(i);
			}
		}
		pthread_mutex_unlock(&queue_mutex);
	}
}


// clear out the extra section of MEM2
void ClearBufferSlotMemory()
{
		memset((void *)MEM2_START_ADDRESS+MEM2_EXTENT-GetOffsetToSlot(BUFFER_SLOTS),0,GetOffsetToSlot(BUFFER_SLOTS));
}

// put an image in the pre allocated slot (must be in the FreeMemorySlots array with the correct number of bytes) bad data = exit to HBC
GRRLIB_texImg BufferImageToSlot(const unsigned char* pngDataAddress,int slot)
{
	int i=0;
	unsigned int offset=0;
	GRRLIB_texImg ret;
	if ((slot>BUFFER_SLOTS)) return ret;
	for (i=0;i<slot;i++)
	{
		offset+=FreeMemorySlots[i];
	}
	// start half way up cache till all the calls are changed
	ret = GRRLIB_LoadTexturePNGToMemory(pngDataAddress, GetSlotBufferAddress(slot));
	if (FreeMemorySlots[slot]!=ret.w*ret.h*4)
	{
		exit(0);
	}
	return ret;
}

