#pragma once

#include "pthread.h"

#include "GRRLIB.h"

#define MAX_BUFFERED_COVERS  500

#define MAX_THREADS 5

#include "core/disc.h"

pthread_t thread[MAX_THREADS];

/*Protect cover count*/
pthread_mutex_t count_mutex;

/*Protect the Ready Queue*/
pthread_mutex_t queue_mutex;

/*Protect the Thread Lock*/
extern pthread_mutex_t lock_thread_mutex;
pthread_mutex_t lock_thread_mutex;

extern pthread_mutex_t buffer_mutex[];

//Cache that can be cleared
int FloatingCacheCovers[MAX_BUFFERED_COVERS];

/*Protect each buffer*/
pthread_mutex_t buffer_mutex[MAX_BUFFERED_COVERS];
	
/*Protect quit*/
pthread_mutex_t quit_mutex;

bool _requestQuit;

extern GRRLIB_texImg _texture_data[];
GRRLIB_texImg _texture_data[MAX_BUFFERED_COVERS];

int _cover_count;
inline void Sleep(unsigned long milliseconds);

inline void BUFFER_InitBuffer(int thread_count);

void CoversDownloaded();
void SetSelectedCover(int index);
void InitializeBuffer(struct discHdr *gameList,int gameCount,int numberOfCoversToBeShown,int initialSelection);
 

inline bool BUFFER_IsCoverReady(int index);


inline void BUFFER_KillBuffer();

inline void BUFFER_ClearCovers();

inline void* process(void *arg);


GRRLIB_texImg BufferStaticImage(const unsigned char* pngDataAddress);
