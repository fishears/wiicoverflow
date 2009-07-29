#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "pthread.h"

#include "GRRLIB.h"

#define MAX_BUFFERED_COVERS  500

#define MAX_THREADS 3
#define GRAPHIC_MODES 2

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

/*Protect covers3d*/
pthread_mutex_t covers_3d_mutex;

bool _requestQuit;

bool _covers3d;

extern GRRLIB_texImg _texture_data[];
GRRLIB_texImg _texture_data[MAX_BUFFERED_COVERS];

int _cover_count;
inline void Sleep(unsigned long milliseconds);


bool BUFFER_IsCoverMissing(int index);

inline void BUFFER_InitBuffer();

void CoversDownloaded();
void SetSelectedCover(int index);
void InitializeBuffer(struct discHdr *gameList,int gameCount,int numberOfCoversToBeShown,int initialSelection,int graphMode);

inline bool BUFFER_IsCoverReady(int index);


inline void BUFFER_KillBuffer();

inline void BUFFER_ClearCovers();

inline void* process(void *arg);

void ClearBufferSlotMemory();

/* If you need memory, add the amount you need to the FreeMemorySlots array
	Pass the ordinal to this and it will give you a consistent address for your need
	The memory does not need to be freed as there is no associated malloc
*/
void * GetSlotBufferAddress(int slot);

/* If you need memory for an image, add the amount you need to the FreeMemorySlots array
	Pass the ordinal to this and it will give you a consistent address for your need
	The memory does not need to be freed as there is no associated malloc
*/
void BufferImageToSlot(GRRLIB_texImg * my_texture,const unsigned char* pngDataAddress,int slot);

void LoadTextureToBuffer(GRRLIB_texImg * my_texture,const unsigned char* pngDataAddress);

#endif
