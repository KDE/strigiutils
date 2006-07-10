#ifndef STRIGI_THREAD_H
#define STRIGI_THREAD_H

#if defined(CMAKE_USE_PTHREADS_INIT)
	#include <pthread.h>
	#define STRIGI_DEFINE_MUTEX(x) pthread_mutex_t x;
	#define STRIGI_INIT_MUTEX(x) pthread_mutex_init(&x, 0)
	#define STRIGI_CLOSE_MUTEX(x) pthread_mutex_destroy(&x)
	#define STRIGI_LOCK_MUTEX(x) pthread_mutex_lock(&x)
	#define STRIGI_UNLOCK_MUTEX(x) pthread_mutex_unlock(&x)
#elif defined(CMAKE_USE_WIN32_THREADS_INIT)
	#define STRIGI_DEFINE_MUTEX(x) CRITICAL_SECTION x;
	#define STRIGI_INIT_MUTEX(x) InitializeCriticalSection(&x)
	#define STRIGI_CLOSE_MUTEX(x) DeleteCriticalSection(&x)
	#define STRIGI_LOCK_MUTEX(x) EnterCriticalSection(&x)
	#define STRIGI_UNLOCK_MUTEX(x) LeaveCriticalSection(&x)
#else
	#error A valid thread library was not found
#endif //mutex types

class StrigiMutex{
public:
	STRIGI_DEFINE_MUTEX(lock);
	StrigiMutex(){
		STRIGI_INIT_MUTEX(lock);
	}
	~StrigiMutex(){
		STRIGI_CLOSE_MUTEX(lock);
	}
};

#endif
