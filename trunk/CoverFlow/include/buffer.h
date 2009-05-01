#pragma once

#include "pthread.h"

#include "GRRLIB.h"

#define BUFFER_SIZE 20
#define MAX_BUFFERED_COVERS  100

#define MAX_THREADS 5

pthread_t thread[MAX_THREADS];

/*Protect cover count*/
pthread_mutex_t count_mutex;

/*Protect the Ready Queue*/
pthread_mutex_t queue_mutex;

/*Protect each buffer*/
pthread_mutex_t buffer_mutex[MAX_BUFFERED_COVERS];
	
/*Protect quit*/
pthread_mutex_t quit_mutex;

typedef struct COVERQUEUE {
	bool ready[MAX_BUFFERED_COVERS];
	bool request[MAX_BUFFERED_COVERS];
	u8 requestId[MAX_BUFFERED_COVERS][6];
	bool remove[MAX_BUFFERED_COVERS];
} COVERQUEUE;

COVERQUEUE _cq;

bool _requestQuit;

GRRLIB_texImg _texture_data[MAX_BUFFERED_COVERS];

int _cover_count;

inline void BUFFER_InitBuffer(int thread_count);

inline void BUFFER_RequestCover(int index, u8 id[6]);

inline bool BUFFER_IsCoverReady(int index);
inline bool BUFFER_IsCoverQueued(int index);

inline void BUFFER_RemoveCover(int index);

inline bool BUFFER_LockTexture(int index, GRRLIB_texImg* tex);
inline void BUFFER_ReleaseTexture(int index);

inline void BUFFER_KillBuffer();

inline void BUFFER_ClearCovers();

inline void* process(void *arg);
