#include "pthread.h"

inline int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg)
{
	*thread = 0;
	return LWP_CreateThread(thread, start_routine, arg, 0, STACKSIZE, 64);
}
 
//int pthread_cancel(pthread_t thread);
 
inline int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
	return LWP_MutexInit(mutex, 0);
}
inline int pthread_mutex_destroy(pthread_mutex_t *mutex){ return LWP_MutexDestroy(*mutex);}
 
inline int pthread_mutex_lock(pthread_mutex_t *mutex)   { return LWP_MutexLock(*mutex);   }
inline int pthread_mutex_trylock(pthread_mutex_t *mutex){ return LWP_MutexTryLock(*mutex);}
inline int pthread_mutex_unlock(pthread_mutex_t *mutex) { return LWP_MutexUnlock(*mutex); }
