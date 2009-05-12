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
	pthread_mutex_init(&lock_thread_mutex, 0);
	
	for(i = 0; i < MAX_BUFFERED_COVERS; i++)
	{
		pthread_mutex_init(&buffer_mutex[i], 0);
		_texture_data[i].data=0;
	}
		
	BUFFER_ClearCovers();
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

			pthread_mutex_lock(&lock_thread_mutex);
	
			/*Handle Load Requests*/
			pthread_mutex_lock(&queue_mutex);
			b = _cq.request[i] && !_cq.remove[i]&&!_cq.ready[i];
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

						uint thisDataMem2Address=MEM2_START_ADDRESS + TEXTURE_DATA_SIZE * i;
						_texture_data[i] = GRRLIB_LoadTexturePNGToMemory((const unsigned char*)imgData, (void *)thisDataMem2Address);
						free(imgData);

						pthread_mutex_lock(&queue_mutex);
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
			
			pthread_mutex_unlock(&lock_thread_mutex);
			
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


}

	int BufferMethod;
	double Density;
	int MainCacheSize;
	int FloatingCacheSize;
	int CurrentSelection;
	struct discHdr *CoverList;
	int nCovers;
	int nCoversInWindow;

void RemoveFromCache(int index)
 {
	 if (_cq.floatingQueuePosition[index] != -1)
	 {
		 _cq.ready[index] = false;
	 }
	
 }
  
  //already locked
void SetFloatingCacheItem(int selection, int newCacheItem)
 {
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
				 break;
			 }
				 if (FloatingCacheCovers[i]!=-1)
				 {
					 if (abs(FloatingCacheCovers[i] - selection) > maxDistance)
						 {
							 maxDistance = abs(FloatingCacheCovers[i] - selection) - maxDistance;
								 maxIndex=i;
						 }
				 }
		 }
		 if (!spaceFound)
		 {
			 RemoveFromCache(maxIndex);
			 FloatingCacheCovers[maxIndex] = newCacheItem;
		 }
 }

 void iSetSelectedCover(int index, bool doNotRemoveFromFloating)
 {
		 int i=0;
		 int extra = 0;
		 if (doNotRemoveFromFloating) extra = 1;
		 int searchSize=nCoversInWindow*(2+extra);
		 //this'll do for now - a more elegant algrorithm would be better
		 for (i=index-searchSize/2;i<index+searchSize/2;i++)
		 {
			 if (i >= 0 && i < nCovers)
			 {
				 if (!_cq.ready[i] && !_cq.request[i])
				 {
					 _cq.requestId[index]=&CoverList[index];
					 _cq.request[i] = true;
					 // this one isn't permenantly cached make a space for it in the floating cache
					 if (_cq.permaBufferPosition[i] == -1 && !doNotRemoveFromFloating) SetFloatingCacheItem(index,i);

				 }
			 }
		 }
 }
 
 void SetSelectedCover(int index)
 {
	 iSetSelectedCover(index,false);
 }


 // internal only - no need to lock (already locked)
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
 
 // internal only - no need to lock (already locked)
 void RequestForCache(int index)
 {
	 _cq.requestId[index]=&CoverList[index];
	 _cq.request[index]=true;
 }


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


 // call at start, on add or on delete
 void InitializeBuffer(struct discHdr *gameList,int gameCount,int numberOfCoversToBeShown,int initialSelection)
 {
	initialSelection+=gameCount/2.0;
	 int i=0;
	 CoverList=gameList;
	 nCovers=gameCount;
	 nCoversInWindow=numberOfCoversToBeShown;
//	 pthread_mutex_lock(&queue_mutex);
	 //start from a clear point
	 for (i=0;i<gameCount;i++) ResetQueueItem(i);
//	 pthread_mutex_unlock(&queue_mutex);

	 
	 int maxSlots=MEM2_EXTENT/TEXTURE_DATA_SIZE;
	 //decide on buffering method
	 if (gameCount<=maxSlots)
	 {
		 BufferMethod=ALL_CACHED;
		 MainCacheSize=gameCount;
	 }
	 else
	 {
		 Density = ((float)maxSlots) / ((float)(gameCount));
		 FloatingCacheSize=(int)(((1-Density)*numberOfCoversToBeShown+1)*4);
		 MainCacheSize = maxSlots - FloatingCacheSize;
		 //FloatingCacheCovers = new int[FloatingCacheSize];// may be in .h
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

	 iSetSelectedCover(initialSelection,true);
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

