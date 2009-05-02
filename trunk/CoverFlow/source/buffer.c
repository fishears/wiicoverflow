#include "buffer.h"
#include "disc.h"
#include "fat.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define USBLOADER_PATH		"SD:/usb-loader"

extern const u8		back_cover_png[];

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
		pthread_mutex_init(&buffer_mutex[i], 0);
	
	
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

void BUFFER_RequestCover(int index, struct discHdr *header)
{
	int i;
	
	if(index < MAX_BUFFERED_COVERS)
	{
		pthread_mutex_lock(&queue_mutex);
		_cq.request[index]   = true;
		//for(i = 0; i < 6; i++)
		//{
		_cq.requestId[index] = header;
		//}
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
		pthread_mutex_lock(&queue_mutex);
		_cq.remove[index] = true;
		pthread_mutex_unlock(&queue_mutex);
	}
}

bool BUFFER_LockTexture(int index, GRRLIB_texImg* tex)
{
	if(index >= MAX_BUFFERED_COVERS || index < 0)
		return false;
		
	pthread_mutex_lock(&queue_mutex);
	if(_cq.ready[index])
	{
		pthread_mutex_unlock(&queue_mutex);
		
		pthread_mutex_lock(&buffer_mutex[index]);
		tex = &_texture_data[index];
		
		if(tex->data == 0)
		{
			pthread_mutex_unlock(&buffer_mutex[index]);
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		pthread_mutex_unlock(&queue_mutex);
	}
	
	return false;
}

void BUFFER_ReleaseTexture(int index)
{
	if(index < MAX_BUFFERED_COVERS)
		pthread_mutex_unlock(&buffer_mutex[index]);
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
	
	pthread_mutex_lock(&count_mutex);
	_cover_count = 0;
	pthread_mutex_unlock(&count_mutex);

	pthread_mutex_lock(&queue_mutex);
	for(i = 0; i < MAX_BUFFERED_COVERS; i++)
	{
		_cq.ready[i]   = false;
		_cq.request[i] = false;
		_cq.remove[i]  = false;
		
		pthread_mutex_lock(&buffer_mutex[i]);
		if(_texture_data[i].data)
			free(_texture_data[i].data);
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
			b = _cq.request[i];
			pthread_mutex_unlock(&queue_mutex);
		
			if(b)
			{
				pthread_mutex_lock(&buffer_mutex[i]);
				
				/*Definitely dont need to load the same texture twice*/
				if(!(_texture_data[i].data))
				{
					
					void *imgData;// = (void *)no_cover_png;

					char filepath[128];
					s32  ret;

					//u8 tId[6];
					//int j = 0;
					
					pthread_mutex_lock(&queue_mutex);
					
					//tId = _cq.requestId;
						
					pthread_mutex_unlock(&queue_mutex);
					
					
					//TODO add download capability to thread
					//Download_Cover(header);
					sprintf(filepath, USBLOADER_PATH "/covers/%s.png", _cq.requestId[i]->id);

					ret = Fat_ReadFile(filepath, &imgData);
					
					if (ret > 0) {
						_texture_data[i] = GRRLIB_LoadTexture((const unsigned char*)imgData);
						
						pthread_mutex_lock(&count_mutex);
						_cover_count++;
						pthread_mutex_unlock(&count_mutex);
					}
					else
					{
						_texture_data[i].data = 0;
					}
				
					pthread_mutex_lock(&queue_mutex);
					_cq.request[i] = false;
					_cq.ready[i]   = true;
					pthread_mutex_unlock(&queue_mutex);
				}	
				pthread_mutex_unlock(&buffer_mutex[i]);
			}
			
			/*Handle Remove Requests*/
			pthread_mutex_lock(&queue_mutex);
			b = _cq.remove[i];
			pthread_mutex_unlock(&queue_mutex);
		
			if(b)
			{
				pthread_mutex_lock(&buffer_mutex[i]);
				if(_texture_data[i].data != 0)
					free(_texture_data[i].data);
				pthread_mutex_unlock(&buffer_mutex[i]);
			
				pthread_mutex_lock(&queue_mutex);
				_cq.remove[i]  = false;
				_cq.ready[i]   = false;
				_cq.request[i] = false;
				pthread_mutex_unlock(&queue_mutex);
				
				pthread_mutex_lock(&count_mutex);
				_cover_count--;
				if(_cover_count < 0)
					_cover_count = 0;
				pthread_mutex_unlock(&count_mutex);

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
	
}

