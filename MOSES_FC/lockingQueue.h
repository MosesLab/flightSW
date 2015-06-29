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
extern volatile sig_atomic_t img_wr_alive;

void lockingQueue_init(LockingQueue *);
node_t * construct_queue_node(void *);
void enqueue(LockingQueue *, void *);
void * dequeue(LockingQueue *);
int occupied(LockingQueue *);
void destroy(LockingQueue *);

#endif /*LOCKINGQUEUE_H*/
