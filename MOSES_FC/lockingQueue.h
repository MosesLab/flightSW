/************************************************
 *Author: Roy Smart				*
 *1/21/14					*
 *MOSES LAB					*
 *						*
 *queue.h declares functions to be used by queues*
 *namely push and pop				*
 ************************************************/
#ifndef LOCKINGQUEUE_H
#define	LOCKINGQUEUE_H

#include <pthread.h>
#include "system.h"
#include <signal.h>

extern volatile sig_atomic_t ts_alive;

void lockingQueue_init(LockingQueue *);
void enqueue(LockingQueue *, Packet *);
Packet* dequeue(LockingQueue *);
void destroy(LockingQueue *);

#endif /*LOCKINGQUEUE_H*/