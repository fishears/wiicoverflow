#include "buffer.h"
 #include "core/disc.h"
 #include <fatsvn.h>
 #include "coverflow.h"
 
 #include <stdio.h>
 #include <ctype.h>
 #include <stdlib.h>
 #include <string.h>
 #include <malloc.h>
 
 #define MEM2_START_ADDRESS 0x91000000
 #define MEM2_EXTENT 225*160*4*260
 #define TEXTURE_DATA_SIZE 225*160*4
 #define DENSITY_METHOD 1
 #define ALL_CACHED 2
 
 //this is just over 32MB Hope it works

 extern const u8         back_cover_png[];
 
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
int BufferMethod;
 double Density;
 int MainCacheSize;
 int FloatingCacheSize;
 int CurrentSelection;
 struct discHdr *CoverList;
 int nCovers;
 int nCoversInWindow;
 bool bCleanedUp=false;
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
                 
         pthread_mutex_lock(&quit_mutex);
         _requestQuit = false;
         pthread_mutex_unlock(&quit_mutex);
         
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
 
 
 void BUFFER_KillBuffer()
 {
 
         pthread_mutex_lock(&quit_mutex);
         _requestQuit = true;
         pthread_mutex_unlock(&quit_mutex);
		 BUFFER_Shutdown();
 }
 
 int GetFLoatingQueuePosition(int index)
 {
	int i,ret=-1;
	for (i=0;i<FloatingCacheSize;i++)
	 {
		if (FloatingCacheCovers[i] == index) ret= i;
	 }
	 return ret;
}
 
 
 void HandleLoadRequest(int index)
 {
	pthread_mutex_lock(&buffer_mutex[index]);
	 
	 /*Definitely dont need to load the same texture twice*/
	if(!_cq.ready[index] && !_cq.coverMissing[index])
	{
	 
		void *imgData=0;
	 
		char filepath[128];
		s32  ret;

		sprintf(filepath, USBLOADER_PATH "/covers/%s.png", _cq.requestId[index]->id);

		ret = Fat_ReadFile(filepath, &imgData);
				 
		if (ret > 0) 
		{
			int thisDataMem2Address;
			if (_cq.permaBufferPosition[index]!=-1)
			{
				thisDataMem2Address=MEM2_START_ADDRESS + TEXTURE_DATA_SIZE * _cq.permaBufferPosition[index];
			}
			else
			{
				int floatingPosition=GetFLoatingQueuePosition(index);
				if (floatingPosition!=-1)
				{
					thisDataMem2Address=MEM2_START_ADDRESS+ (MainCacheSize + floatingPosition) * TEXTURE_DATA_SIZE ;
				}
				else
				{
					thisDataMem2Address=-1;
				}
			}
			if (thisDataMem2Address!=-1)
			{
				_texture_data[index] = GRRLIB_LoadTexturePNGToMemory((const unsigned char*)imgData, (void *)thisDataMem2Address);
			}
			free(imgData);

			pthread_mutex_lock(&queue_mutex);
			GRRLIB_texImg textureData=_texture_data[index];
			if (!((textureData.h ==224 || textureData.h ==225) && textureData.w == 160))
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
		 else
		 {
			pthread_mutex_lock(&queue_mutex);
			_cq.coverMissing[index]=true;
			_cq.ready[index]   = false;
			pthread_mutex_unlock(&queue_mutex);
		 }
	 
	}
	pthread_mutex_unlock(&buffer_mutex[index]);
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
				if (_cq.request[index] && !_cq.remove[index]&&!_cq.ready[index]) return index;
			}
		}
	}
	return ret;
 }
 
 
 void* process(void *arg)
 {
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
					b = _cq.request[index] && !_cq.remove[index]&&!_cq.ready[index];
					if (b) _cq.request[index]=false;
					pthread_mutex_unlock(&queue_mutex);
			 
					if(b) HandleLoadRequest(index);
			 
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
	  CurrentSelection=nCovers-index;
//		GRRLIB_Printf(500, 20, font_texture, 0x808080FF, 1, "Sel: %d", CurrentSelection);
//       GRRLIB_Render();

	  if (doNotRemoveFromFloating) extra = 1;
	  int searchSize=nCoversInWindow*(2+extra);
	  //this'll do for now - a more elegant algrorithm would be better
	  for (i=CurrentSelection-searchSize/2;i<CurrentSelection+searchSize/2;i++)
	  {
		  if (i >= 0 && i < nCovers)
		  {
			  if (!_cq.ready[i] && !_cq.request[i])
			  {
				  _cq.requestId[i]=&CoverList[i];
				  _cq.request[i] = true;
				  // this one isn't permenantly cached make a space for it in the floating cache
				  if (_cq.permaBufferPosition[i] == -1 && !doNotRemoveFromFloating) SetFloatingCacheItem(CurrentSelection,i);

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
	_cq.ready[index]=false;
	_cq.request[index]=false;
	_cq.requestId[index]=0;
	_cq.remove[index]=false;
	_cq.permaBufferPosition[index]=-1;
	_cq.coverMissing[index]=false;
	_cq.floatingQueuePosition[index]=-1;
	pthread_mutex_lock(&buffer_mutex[index]);
	_texture_data[index].data=0;
	pthread_mutex_unlock(&buffer_mutex[index]);
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
		pthread_mutex_lock(&lock_thread_mutex);
        initialSelection+=gameCount/2.0;
		CurrentSelection=initialSelection;
        int i=0;
        CoverList=gameList;
        nCovers=gameCount;
        nCoversInWindow=numberOfCoversToBeShown;
        pthread_mutex_lock(&queue_mutex);
        //start from a clear point
        for (i=0;i<gameCount;i++) ResetQueueItem(i);
        pthread_mutex_unlock(&queue_mutex);
 
          
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
		pthread_mutex_unlock(&lock_thread_mutex);
		sleep(2);
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

