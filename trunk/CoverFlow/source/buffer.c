#include "buffer.h"
#include "core/disc.h"
#include "core/fat.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define USBLOADER_PATH		"SD:/usb-loader"

#define MEM2_START_ADDRESS 0x91000000
#define MEM2_EXTENT 0x3000000
#define TEXTURE_DATA_SIZE 225*160*4
#define DENSITY_METHOD 1
#define ALL_CACHED 2

//this is just over 32MB Hope it works
#define NUMBER_OF_MEM2_TEST_COVERS 250

extern const u8		back_cover_png[];
int loadedCovers=0;

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
	// end of private vars


#include <unistd.h>
void Sleep(unsigned long milliseconds)
{
	if (milliseconds<1000)
		usleep(milliseconds*1000);
	else
		sleep(milliseconds/1000);
}
	
void BUFFER_InitBuffer(int thread_count)
{
	int i = 0;
	
	/*Initialize Mutexs*/
	pthread_mutex_init(&count_mutex, 0);
	pthread_mutex_init(&queue_mutex, 0);
	pthread_mutex_init(&quit_mutex, 0);
	
	for(i = 0; i < MAX_BUFFERED_COVERS; i++)
	{
		pthread_mutex_init(&buffer_mutex[i], 0);
		_texture_data[i].data=0;
	}
	
	BUFFER_ClearCovers();
	loadedCovers=0;
	pthread_mutex_lock(&quit_mutex);
	_requestQuit = false;
	pthread_mutex_unlock(&quit_mutex);
	
	for(i = 0; i < thread_count; i++)
	{
		if(i < MAX_THREADS)
			pthread_create(&thread[i], 0, process, (void *)i);
	}
}

void FreeTextureData(int index)
{
	if (index>=NUMBER_OF_MEM2_TEST_COVERS)
	{
		pthread_mutex_lock(&buffer_mutex[index]);
		if(_texture_data[index].data != 0)
			free(_texture_data[index].data);
			
		_texture_data[index].data = 0;
		pthread_mutex_unlock(&buffer_mutex[index]);
		loadedCovers--;
	}
}


void BUFFER_RequestCover(int index, struct discHdr *header)
{
	//int i;
	
	if(index < MAX_BUFFERED_COVERS)
	{
		pthread_mutex_lock(&queue_mutex);
		_cq.request[index]   = true;
		_cq.requestId[index] = header;
		pthread_mutex_unlock(&queue_mutex);
	}
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

void BUFFER_RemoveCover(int index)
{
	if(index < MAX_BUFFERED_COVERS)
	{
		if (index>=NUMBER_OF_MEM2_TEST_COVERS)
		{
			pthread_mutex_lock(&queue_mutex);
			FreeTextureData(index);
			_cq.request[index] = false;
			_cq.remove[index]  = false;
			_cq.ready[index]   = false;
			pthread_mutex_unlock(&queue_mutex);
		}
	}
}



void BUFFER_KillBuffer()
{

	pthread_mutex_lock(&quit_mutex);
	_requestQuit = true;
	pthread_mutex_unlock(&quit_mutex);
}

void BUFFER_ClearCovers()
{
	int i;
	

	pthread_mutex_lock(&queue_mutex);
	for(i = 0; i < MAX_BUFFERED_COVERS; i++)
	{
		BUFFER_RemoveCover(i); //this is to free any allocated memory
		//special code to stop showing if covers change incorrect covers
			pthread_mutex_lock(&buffer_mutex[i]);

			_cq.request[i] = false;
			_cq.remove[i]  = false;
			_cq.ready[i]   = false;
			_texture_data[i].data = 0; //don't need to check as it hasn't actually had memory allocated in mem2

			pthread_mutex_unlock(&buffer_mutex[i]);
			
			
	}
	pthread_mutex_unlock(&queue_mutex);
}

void* process(void *arg)
{
	int  i = 0;
	bool b = false;
	/*Main Buffering Thread*/
	//Fat_MountSDHC();
	
	while(1)
	{
		for(i = 0; i < MAX_BUFFERED_COVERS; i++)
		{

			/*Handle Load Requests*/
			pthread_mutex_lock(&queue_mutex);
			b = _cq.request[i] && !_cq.remove[i]&&!_cq.ready[i]&&loadedCovers<15;
			if (b) _cq.request[i]=false;
			pthread_mutex_unlock(&queue_mutex);
		
			if(b)
			{
				pthread_mutex_lock(&buffer_mutex[i]);
				
				/*Definitely dont need to load the same texture twice*/
				if(!(_texture_data[i].data))
				{
					
					void *imgData=0;

					char filepath[128];
					s32  ret;

					sprintf(filepath, USBLOADER_PATH "/covers/%s.png", _cq.requestId[i]->id);

					ret = Fat_ReadFile(filepath, &imgData);
					
					if (ret > 0) {

						_texture_data[i] = GRRLIB_LoadTexture((const unsigned char*)imgData);
						free(imgData);
						// MEM2 test
						if (i<NUMBER_OF_MEM2_TEST_COVERS)
						{
							int thisDataMem2Address=MEM2_START_ADDRESS + TEXTURE_DATA_SIZE * i;
							memcpy((void*) thisDataMem2Address,_texture_data[i].data,TEXTURE_DATA_SIZE);
							free(_texture_data[i].data);
							_texture_data[i].data = (void*) thisDataMem2Address;
							loadedCovers--; //don't count this one ;-)
						}
						// END MEM2 Test removecover and free texture have code too

						pthread_mutex_lock(&queue_mutex);
						loadedCovers++;
						_cq.ready[i]   = true;
						pthread_mutex_unlock(&queue_mutex);
					}
					else
					{
						pthread_mutex_lock(&queue_mutex);
						_cq.ready[i]   = false;
						pthread_mutex_unlock(&queue_mutex);
						FreeTextureData(i);
					}
				
				}
				pthread_mutex_unlock(&buffer_mutex[i]);
			}
			
			Sleep(1);
		}
		
		
		pthread_mutex_lock(&quit_mutex);
		if(_requestQuit)
		{
			pthread_mutex_unlock(&quit_mutex);
			
			int m = 0;
			
			/*Initialize Mutexs*/
			pthread_mutex_destroy(&count_mutex);
			pthread_mutex_destroy(&queue_mutex);
			pthread_mutex_destroy(&quit_mutex);
			
			for(m = 0; m < MAX_BUFFERED_COVERS; m++)
				pthread_mutex_destroy(&buffer_mutex[m]);

			return 0;
		}
		pthread_mutex_unlock(&quit_mutex);
	}



	int BufferMethod;
	double Density;
	int MainCacheSize;
	int FloatingCacheSize;
	int CurrentSelection;
	struct discHdr *CoverList;
	int nCovers;
	int nCoversInWindow;

	void RemoveFarthestFloatingCacheItem()
	{
		int i=0;
		int maxDistance=0;
		int maxIndex=-1;
		for (i=0;i<FloatingCacheSize;i++)
		{
			if (FloatingCacheCovers[i]!=-1)
			{
				if (abs(FloatingCacheCovers[i]-CurrentSelection)>maxDistance)
				{
					maxDistance=abs(FloatingCacheCovers[i]-CurrentSelection)>maxDistance;
					maxIndex=i;
				}
			}
		}
		FloatingCacheCovers[maxIndex]=-1;
	}

	void iSetSelectedCover(int index, bool doNotRemoveFromFloating)
	{
		int i=0;
		int searchSize=nCoversInWindow*(2+doNotRemoveFromFloating);
		//this'll do for now - a more elegant algrorithm would be better
		for (i=index-searchSize/2;i<index+searchSize/2;i++)
		{
			if (!_cq.ready[i] && !_cq.request[i])
			{
				// this one isn't permenantly cached make a space for it in the floating cache
				if (_cq.permaBufferPosition[i]==-1 && !doNotRemoveFromFloating) RemoveFarthestFloatingCacheItem();
				_cq.request[i]=true;
			}
		}
	}

	void SetSelectedCover(int index)
	{
		iSetSelectedCover(index,false);
	}


	void ResetQueueItem(int index)
	{
		_cq.ready[index]=false;
		_cq.request[index]=false;
		_cq.requestId[index]=0;
		_cq.remove[index]=false;
		_cq.permaBufferPosition[index]=-1;
		_cq.coverMissing[index]=false;
		_cq.floatingQueuePosition[index]=-1;
	}

	void RequestForCache(int index)
	{
		_cq.request[index]=true;
		_cq.requestId[index]=&CoverList[index];
	}

	// call at start, on add or on delete
	void InitializeBuffer(struct discHdr *gameList,int gameCount,int numberOfCoversToBeShown,int initialSelection)
	{
		int i=0;
		CoverList=gameList;
		nCovers=gameCount;
		nCoversInWindow=numberOfCoversToBeShown;
		//start from a clear point
		for (i=0;i<gameCount;i++) ResetQueueItem(i);
		//decide on buffering method
		if (gameCount<=MEM2_EXTENT/TEXTURE_DATA_SIZE)
		{
			BufferMethod=ALL_CACHED;
			MainCacheSize=gameCount;
		}
		else
		{
			BufferMethod=DENSITY_METHOD;
			Density = ((float)gameCount)/((float)MAX_BUFFERED_COVERS);
			FloatingCacheSize=(1-Density)*numberOfCoversToBeShown*5;
			MainCacheSize=MEM2_EXTENT/TEXTURE_DATA_SIZE-FloatingCacheSize;
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
				
			}
		}
		iSetSelectedCover(initialSelection,true);
		//the the visible ones get cached
		Sleep(1);
		//now request all the permenant covers
		for (i=0;i<gameCount;i++) 
		{
			if (_cq.permaBufferPosition[i]!=-1)
			{
				RequestForCache(i);
			}
		}
		
	}
	
	void CoversDownloaded()
	{
		int i;
		//recheck all the missing covers
		for (i=0;i<nCovers;i++)
		{
			if (_cq.coverMissing[i])
			{
				_cq.coverMissing[i]=false;
				if (_cq.permaBufferPosition[i]!=-1)
				{
					RequestForCache(i);
				}
			}
		}
	}
		
}

