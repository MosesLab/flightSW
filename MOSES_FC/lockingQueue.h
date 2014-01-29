/************************************************
 *Author: Roy Smart				*
 *1/21/14					*
 *MOSES LAB					*
 *						*
 *queue.h declares functions to be used by queues*
 *namely push and pop				*
 ************************************************/
#include <pthread.h>
#include "packetType.h"
#include <signal.h>

extern volatile sig_atomic_t ts_alive;

typedef struct {
    volatile Packet *first;
    volatile Packet *last;
    int count;  //current number of items
    
    pthread_condattr_t cattr;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} LockingQueue;

void lockingQueue_init(LockingQueue *);
void enqueue(LockingQueue *, Packet);
Packet dequeue(LockingQueue *);
void destroy(LockingQueue *);

