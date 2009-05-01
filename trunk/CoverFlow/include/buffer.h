#pragma once

#include "pthread.h"

#include "GRRLIB.h"

#define BUFFER_SIZE 20
#define MAX_COVERS  100

pthread_t thread;

/*Protect cover count*/
pthread_mutex_t count_mutex;

/*Protect the Ready Queue*/
pthread_mutex_t queue_mutex;

/*Protect each buffer*/
pthread_mutex_t buffer_mutex[MAX_COVERS];
	
/*Protect quit*/
pthread_mutex_t quit_mutex;

typedef struct COVERQUEUE {
	bool ready[MAX_COVERS];
	bool request[MAX_COVERS];
	u8 requestId[MAX_COVERS][6];
	bool remove[MAX_COVERS];
} COVERQUEUE;

COVERQUEUE _cq;

bool _requestQuit;

GRRLIB_texImg _texture_data[MAX_COVERS];

int _cover_count;

void InitBuffer();

void RequestCover(int index, u8 id[6]);

bool IsCoverReady(int index);

void RemoveCover(int index);

bool LockTexture(int index, GRRLIB_texImg* tex);
void ReleaseTexture(int index);

void KillBuffer();

void* process(void *arg);
