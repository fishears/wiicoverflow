#include "buffer.h"
#include "disc.h"
#include "fat.h"

#define USBLOADER_PATH		"SD:/usb-loader"

void InitBuffer()
{
	int i = 0;
	
	pthread_mutex_lock(&count_mutex);
	_cover_count = 0;
	pthread_mutex_unlock(&count_mutex);

	pthread_mutex_lock(&queue_mutex);
	for(i = 0; i < MAX_COVERS; i++)
	{
		_cq.ready[i]   = false;
		_cq.request[i] = false;
		_cq.remove[i]  = false;
	}
	
	pthread_mutex_unlock(&queue_mutex);
	
	pthread_mutex_lock(&quit_mutex);
	_requestQuit = false;
	pthread_mutex_unlock(&quit_mutex);
	
}

void RequestCover(int index, u8 id[6])
{
	int i;
	
	if(index < MAX_COVERS)
	{
		pthread_mutex_lock(&queue_mutex);
		_cq.request[index]   = true;
		for(i = 0; i < 6; i++)
		{
			_cq.requestId[index][i] = id[i];
		}
		pthread_mutex_unlock(&queue_mutex);
	}
}

bool IsCoverReady(int index)
{
	bool retval = false;
	
	if(index < MAX_COVERS)
	{
		pthread_mutex_lock(&queue_mutex);
		retval = _cq.ready[index];
		pthread_mutex_unlock(&queue_mutex);
	}
	
	return retval;
}

void RemoveCover(int index)
{
	if(index < MAX_COVERS)
	{
		pthread_mutex_lock(&queue_mutex);
		_cq.remove[index] = true;
		pthread_mutex_unlock(&queue_mutex);
	}
}

bool LockTexture(int index, GRRLIB_texImg* tex)
{
	if(index >= MAX_COVERS)
		return false;
		
	pthread_mutex_lock(&queue_mutex);
	if(_cq.ready[index])
	{
		pthread_mutex_unlock(&queue_mutex);
		
		pthread_mutex_lock(&buffer_mutex[index]);
		tex = &_texture_data[index];
		
		if(tex == 0)
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

void ReleaseTexture(int index)
{
	if(index < MAX_COVERS)
		pthread_mutex_unlock(&buffer_mutex[index]);
}

void KillBuffer()
{

	pthread_mutex_lock(&quit_mutex);
	_requestQuit = true;
	pthread_mutex_unlock(&quit_mutex);
}

void* process(void *arg)
{
	int  i = 0;
	bool b = false;
	/*Main Buffering Thread*/
	while(1)
	{
		for(i = 0; i < MAX_COVERS; i++)
		{
			/*Handle Load Requests*/
			pthread_mutex_lock(&queue_mutex);
			b = _cq.request[i];
			pthread_mutex_unlock(&queue_mutex);
		
			if(b)
			{
				pthread_mutex_lock(&buffer_mutex[i]);
				if(_texture_data[i].data)
				{
					free(_texture_data[i].data);
				}
				
				void *imgData;// = (void *)no_cover_png;

				char filepath[128];
				s32  ret;

				u8 tId[6];
				int j = 0;
				pthread_mutex_lock(&queue_mutex);
				
				for(j = 0; j < 6; j++)
					tId[j] = _cq.requestId[j];
					
				pthread_mutex_unlock(&queue_mutex);
				
				
				//TODO add download capability to thread
				//Download_Cover(header);
				sprintf(filepath, USBLOADER_PATH "/covers/%s.png", tId);

				ret = Fat_ReadFile(filepath, &imgData);
				
				if (ret > 0) {
					_texture_data[i] = GRRLIB_LoadTexture((const unsigned char*)imgData);
					
					pthread_mutex_lock(&count_mutex);
					_cover_count++;
					pthread_mutex_unlock(&count_mutex);
				}
			
				pthread_mutex_lock(&queue_mutex);
				_cq.request[i] = false;
				_cq.ready[i]   = true;
				pthread_mutex_unlock(&queue_mutex);
				
				pthread_mutex_unlock(&buffer_mutex[i]);
			}
			
			/*Handle Remove Requests*/
			pthread_mutex_lock(&queue_mutex);
			b = _cq.remove[i];
			pthread_mutex_unlock(&queue_mutex);
		
			if(b)
			{
				pthread_mutex_lock(&buffer_mutex[i]);
				free(_texture_data[i].data);
				pthread_mutex_unlock(&buffer_mutex[i]);
			
				pthread_mutex_lock(&queue_mutex);
				_cq.remove[i] = false;
				pthread_mutex_unlock(&queue_mutex);
				
				pthread_mutex_lock(&count_mutex);
				_cover_count--;
				if(_cover_count < 0)
					_cover_count = 0;
				pthread_mutex_unlock(&count_mutex);

			}			
		}
		
		
		pthread_mutex_lock(&quit_mutex);
		if(_requestQuit)
		{
			pthread_mutex_unlock(&quit_mutex);
			return 0;
		}
		pthread_mutex_unlock(&quit_mutex);
	}

}

