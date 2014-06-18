/**
 *Author: Roy Smart, Jackson Remington
 *MOSES LAB
 * lockingQueue.c uses mutexes 
 **/
#include <sys/time.h>
//#include "main.h"
#include "tm_queue.h"

 void tm_queue_init(tm_queue_t * queue) {
    pthread_mutex_init(&queue->lock, NULL); //initialize mutex

    /*initialize conditional variable*/
    pthread_condattr_init(&queue->cattr);
    pthread_cond_init(&queue->cond, &queue->cattr);

    queue->count = 0;   //amount of objects in the queue
}

void tm_enqueue(tm_queue_t * queue, imgPtr_t * p) {
    pthread_mutex_lock(&queue->lock);

    if (queue->first == NULL) {
        queue->first = p;
        queue->last = queue->first;

    } else {
        queue->last->next = p;
        queue->last = queue->last->next;
    }
    queue->count++;
    
    /*TRY unlocking before broadcasting (may not work)*/
    pthread_mutex_unlock(&queue->lock);
    pthread_cond_broadcast(&queue->cond); // Wake up consumer waiting for input
    

}

void tm_dequeue(tm_queue_t * queue) {
//    struct timespec timeToWait;
//    struct timeval now;
//
//    gettimeofday(&now, NULL);
//    timeToWait.tv_sec = now.tv_sec + 1;
//    timeToWait.tv_nsec = 0;
//
//    /*The thread must be locked for pthread_cond_timedwait() to work*/
//    pthread_mutex_lock(&queue->lock);
//    while ((queue->count == 0) && ts_alive) {
//        pthread_cond_timedwait(&queue->cond, &queue->lock, &timeToWait);        //unlocks the mutex and waits on the conditional variable
//        //pthread_cond_wait(&queue->cond, &queue->lock);
//    }

    imgPtr_t* p;
    /*check if program is still active*/
    if (ts_alive) {
        p = queue->first;
        queue->first = (imgPtr_t *) p->next;
        queue->count--;
    }

    pthread_mutex_unlock(&queue->lock);

}

void tm_queue_destroy(tm_queue_t * queue) {
    pthread_mutex_lock(&queue->lock);
    pthread_cond_destroy(&queue->cond);
    pthread_mutex_destroy(&queue->lock);
}