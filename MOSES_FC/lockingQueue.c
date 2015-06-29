/**
 *Author: Roy Smart
 *MOSES LAB
 * lockingQueue.c uses mutexes 
 **/
#include <sys/time.h>

#include "lockingQueue.h"

void lockingQueue_init(LockingQueue * queue) {
    queue->first = NULL; //Initialize queue
    queue->last = NULL;
    pthread_mutex_init(&queue->lock, NULL); //initialize mutex

    /*initialize conditional variable*/
    pthread_condattr_init(&queue->cattr);
    pthread_cond_init(&queue->cond, &queue->cattr);

    queue->count = 0; //amount of objects in the queue
}

node_t * construct_queue_node(void * content) {
    node_t* n;
    if((n = (node_t *)malloc(sizeof(node_t))) == NULL){
        record("malloc failed to allocate node\n");
    }
    n->def = content;
    n->next = NULL; //initialize next node to null
    return n;
}

void enqueue(LockingQueue * queue, void * content) {
    pthread_mutex_lock(&queue->lock);

    node_t * n = construct_queue_node(content);

    if (queue->first == NULL) {
        queue->first = n;
        queue->last = queue->first;

    } else {
        queue->last->next = n;
        queue->last = n;
    }
    queue->count++;

    pthread_cond_broadcast(&queue->cond); // Wake up consumer waiting for input
    pthread_mutex_unlock(&queue->lock);

}

void * dequeue(LockingQueue * queue) {
    //    struct timespec timeToWait;
    //    struct timeval now;
    node_t * n = NULL;
    void * ptr = NULL;

    //    gettimeofday(&now, NULL);
    //    timeToWait.tv_sec = now.tv_sec + 1;
    //    timeToWait.tv_nsec = 0;

    /*The thread must be locked for pthread_cond_timedwait() to work*/
    pthread_mutex_lock(&queue->lock);
//    while ((queue->count == 0)) {
        //        pthread_cond_timedwait(&queue->cond, &queue->lock, &timeToWait);        //unlocks the mutex and waits on the conditional variable
        pthread_cond_wait(&queue->cond, &queue->lock);
//    }

    /*check if program is still active*/
    if ((ts_alive && img_wr_alive) || queue->count) {
        n = queue->first;
        queue->first = n->next;
        queue->count--;
        
        ptr = n->def;
        free(n);
    }

    pthread_mutex_unlock(&queue->lock);

    return ptr;
}

int occupied(LockingQueue * queue) {
    return queue->count;
}

void lockingQueue_destroy(LockingQueue * queue) {
    pthread_mutex_lock(&queue->lock);
    pthread_cond_destroy(&queue->cond);
    pthread_mutex_destroy(&queue->lock);
}
