/**
 *Author: Roy Smart
 *MOSES LAB
 * lockingQueue.c uses mutexes 
 **/
#include <sys/time.h>

#include "lockingQueue.h"

void lockingQueue_init(LockingQueue * queue) {
    pthread_mutex_init(&queue->lock, NULL); //initialize mutex

    /*initialize conditional variable*/
    pthread_condattr_init(&queue->cattr);
    pthread_cond_init(&queue->cond, &queue->cattr);

    queue->count = 0;   //amount of objects in the queue
}

void enqueue(LockingQueue * queue, Packet * p) {
    pthread_mutex_lock(&queue->lock);

    if (queue->first == NULL) {
        queue->first = p;
        queue->last = queue->first;

    } else {
        queue->last->next = p;
        queue->last = p;
    }
    queue->count++;
    
    /*TRY unlocking before broadcasting (may not work)*/
    pthread_mutex_unlock(&queue->lock);
    pthread_cond_broadcast(&queue->cond); // Wake up consumer waiting for input
    

}

Packet* dequeue(LockingQueue * queue) {
    struct timespec timeToWait;
    struct timeval now;
    Packet* p;

    gettimeofday(&now, NULL);
    timeToWait.tv_sec = now.tv_sec + 1;
    timeToWait.tv_nsec = 0;

    /*The thread must be locked for pthread_cond_timedwait() to work*/
    pthread_mutex_lock(&queue->lock);
    while ((queue->count == 0) && ts_alive) {
        pthread_cond_timedwait(&queue->cond, &queue->lock, &timeToWait);        //unlocks the mutex and waits on the conditional variable
    }

    /*check if program is still active*/
    if (ts_alive) {
        p = queue->first;
        queue->first = (Packet*) p->next;
        queue->count--;
    }

    pthread_mutex_unlock(&queue->lock);

    return p;
}

void lockingQueue_destroy(LockingQueue * queue) {
    pthread_mutex_lock(&queue->lock);
    pthread_cond_destroy(&queue->cond);
    pthread_mutex_destroy(&queue->lock);
}
