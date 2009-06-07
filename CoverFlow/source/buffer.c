#include "buffer.h"
#include "core/disc.h"
#include <fatsvn.h>
#include "coverflow.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define COVER_WIDTH 160
#define COVER_HEIGHT 224

#define COVER_WIDTH_3D 512
#define COVER_HEIGHT_3D 340


#define TEXTURE_DATA_SIZE COVER_WIDTH*COVER_HEIGHT*4
#define TEXTURE_DATA_SIZE_3D COVER_WIDTH_3D*COVER_HEIGHT_3D*4

#define DENSITY_METHOD 1
#define ALL_CACHED 2

#define MEM2_START_ADDRESS 0x90000000
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
int BufferMethod=DENSITY_METHOD;
float Density;
int MainCacheSize;
int maxSlots;
int FloatingCacheSize=0;
int CurrentSelection;
struct discHdr *CoverList;
int nCovers=0;
int nCoversInWindow;
bool bCleanedUp=false;
// end of private vars

#define BUFFER_SLOTS 5
unsigned int FreeMemorySlots[BUFFER_SLOTS+1] =
	{
	128,//0 string buffer for thread 0
	128,//1 string buffer for thread 1
	128,//2 string buffer for thread 2
	128,//3 string buffer for thread 3
	128,//4 string buffer for thread 4
	0};

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

void BUFFER_2D_COVERS()
{
	pthread_mutex_lock(&covers_3d_mutex);
	_covers3d = 0;
	pthread_mutex_unlock(&covers_3d_mutex);
}

void BUFFER_3D_COVERS()
{
	pthread_mutex_lock(&covers_3d_mutex);
	_covers3d = 1;
	pthread_mutex_unlock(&covers_3d_mutex);
}

void BUFFER_KillBuffer()
{
	
	pthread_mutex_lock(&quit_mutex);
	_requestQuit = true;
	pthread_mutex_unlock(&quit_mutex);
	BUFFER_Shutdown();
}

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
		
		bool covers3d = false;
		
		int tW = 0;
		int tH = 0;
		
		pthread_mutex_lock(&covers_3d_mutex);
		covers3d = _covers3d;
		pthread_mutex_unlock(&covers_3d_mutex);
		
		//TODO Will this be a problem?? might need to create different setting for covers used by this thread and protected with mutex
		if(!covers3d)
		{
			tW = COVER_WIDTH;
			tH = COVER_HEIGHT;
			snprintf(filepath,256, USBLOADER_PATH "/covers/%s.png", _cq.requestId[index]->id);
		}
		else
		{
			tW = COVER_WIDTH_3D;
			tH = COVER_HEIGHT_3D;
			snprintf(filepath,256, USBLOADER_PATH "/3dcovers/%c%c%c%c.png", _cq.requestId[index]->id[0], _cq.requestId[index]->id[1], _cq.requestId[index]->id[2], _cq.requestId[index]->id[3]);
		}
		
		int imgDataAddress=MEM2_START_ADDRESS + tW * tH * 4 * (maxSlots+threadNo);
		ret = Fat_ReadFileToBuffer(filepath,(void *) imgDataAddress,tW * tH * 4);
		
		if (ret > 0) 
		{
			int thisDataMem2Address=-1;
			if (_cq.permaBufferPosition[index]!=-1)
			{
				thisDataMem2Address=MEM2_START_ADDRESS + tW * tH * 4 * _cq.permaBufferPosition[index];
			}
			else
			{
				int floatingPosition=GetFLoatingQueuePosition(index);
				if (floatingPosition!=-1)
				{
					thisDataMem2Address=MEM2_START_ADDRESS+ (MainCacheSize + floatingPosition) * tW * tH * 4 ;
				}
				else
				{
					thisDataMem2Address=-1;
				}
			}
			if (thisDataMem2Address!=-1)
			{
				_texture_data[index] = GRRLIB_LoadTexturePNGToMemory((const unsigned char*)imgDataAddress, (void *)thisDataMem2Address);
				GRRLIB_texImg textureData=_texture_data[index];
				pthread_mutex_lock(&queue_mutex);
				if (!(textureData.h ==tH && textureData.w == tW))
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
		else
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
void SetFloatingCacheItem(int selection, int newCacheItem)
{
	if (InCache(newCacheItem)) return;
	int i=0;
	bool spaceFound = false;
	int maxDistance=0;
	int maxIndex=-1;
	for (i=0;i<FloatingCacheSize;i++)
	{
		if (FloatingCacheCovers[i] == -1)
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
	if (!spaceFound)
	{
		
		RemoveFromCache(maxIndex);
		FloatingCacheCovers[maxIndex] = newCacheItem;
		_cq.floatingQueuePosition[newCacheItem]=maxIndex;
	}
	
	
}

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

void SetSelectedCover(int index)
{
	iSetSelectedCover(index+nCovers/2.0,false);
}


// internal only - no need to lock (already locked)
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
void RequestForCache(int index)
{
	if(index >= MAX_BUFFERED_COVERS)
		return;
		
	_cq.requestId[index]=&CoverList[index];
	_cq.request[index]=true;
}

// call at start, on add or on delete - kill the buffer first
void InitializeBuffer(struct discHdr *gameList,int gameCount,int numberOfCoversToBeShown,int initialSelection)
{
	CurrentSelection=initialSelection+gameCount/2.0;
	int i=0;
	CoverList=gameList;
	nCovers=gameCount;
	nCoversInWindow=numberOfCoversToBeShown;
	
	//start from a clear point
	for (i=0;i<gameCount;i++) ResetQueueItem(i);
	memset((void *)MEM2_START_ADDRESS,0,MEM2_EXTENT-GetOffsetToSlot(BUFFER_SLOTS));
	
	bool covers3d = false;
	
	covers3d = _covers3d;
	
	if(covers3d)
	{
		maxSlots=(MEM2_EXTENT-GetOffsetToSlot(BUFFER_SLOTS))/(TEXTURE_DATA_SIZE_3D)-MAX_THREADS;
	}
	else
	{
		maxSlots=(MEM2_EXTENT-GetOffsetToSlot(BUFFER_SLOTS))/(TEXTURE_DATA_SIZE)-MAX_THREADS;
	}
	
	//decide on buffering method
	if (gameCount<=maxSlots)
	{
		BufferMethod=ALL_CACHED;
		MainCacheSize=gameCount;
	}
	else
	{
		BufferMethod=DENSITY_METHOD;
		Density = ((float)maxSlots) / ((float)(gameCount));
		FloatingCacheSize=(int)(((1-Density)*numberOfCoversToBeShown+1)*4);
		if (FloatingCacheSize>maxSlots)
		{
			FloatingCacheSize=maxSlots;
		}
		MainCacheSize = maxSlots - FloatingCacheSize;
		Density = ((float)(MainCacheSize-1)) / ((float)(gameCount));
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
	else
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


void ClearBufferSlotMemory()
{
		memset((void *)MEM2_START_ADDRESS+MEM2_EXTENT-GetOffsetToSlot(BUFFER_SLOTS),0,GetOffsetToSlot(BUFFER_SLOTS));
}

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

