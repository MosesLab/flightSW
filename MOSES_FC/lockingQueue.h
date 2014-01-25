/************************************************
 *Author: Roy Smart				*
 *1/21/14					*
 *MOSES LAB					*
 *						*
 *queue.h declares functions to be used by queues*
 *namely push and pop				*
 ************************************************/
#include "packet.h"
#include <pthread.h>

typedef struct {
    Packet *first;
    Packet *last;
    int count;  //current number of items
    
    pthread_mutex_t lock;
    pthread_cond_t cond;
} LockingQueue;

lockingQueue_init(LockingQueue);
enqueue(LockingQueue);
dequeue(LockingQueue);

