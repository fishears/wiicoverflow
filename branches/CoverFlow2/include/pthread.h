#pragma once
#include <ogcsys.h>
#include <gccore.h>
#define STACKSIZE 8*1024
 
typedef lwp_t pthread_t;
typedef mutex_t pthread_mutex_t;
typedef void* pthread_mutexattr_t;
typedef int pthread_attr_t;
 
inline int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg);
//int pthread_cancel(pthread_t thread);
 
inline int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
inline int pthread_mutex_destroy(pthread_mutex_t *mutex);
inline int pthread_mutex_lock(pthread_mutex_t *mutex);
inline int pthread_mutex_unlock(pthread_mutex_t *mutex);
 
//imp

