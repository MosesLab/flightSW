/****************************************
*Author: Roy Smart			*
*1/21/14				*
*MOSES LAB				*
*					*
*Mutexes are used to sychronize threads *
*when a global variable is accessed,	*
*it must be locked by the mutex to avoid*
*race conditions inbetween threads	*
****************************************/

#include <pthread.h>

void initiateMutex(pthread_mutex_t mx){
	pthread_mutex_lock(&mx);
}

void unlockMutex(pthread_mutex_t mx){
	pthread_mutex_unlock(&mx);
}

void mutexWait(pthread_cond_t cond, pthread_mutex_t mx){
	p_thread_cond_wait(cond, mx)
}

void destroyMutex(pthread_mutex_t mx){
	pthread_mutex_destroy(&mx);
}
