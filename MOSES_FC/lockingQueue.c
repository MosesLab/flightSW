/**
 *Author: Roy Smart
 *MOSES LAB
 **/
#include "lockingQueue.h"

void lockingQueue_init(LockingQueue queue){
    pthread_mutex_init(&queue.lock, NULL);
    pthread_condattr_init(&queue.cattr);
    pthread_cond_init(&queue.cond, &queue.cattr);

	queue.count=0;
}

void enqueue(LockingQueue queue, Packet p) {
    pthread_mutex_lock(&queue.lock);
    
    if (queue.first == NULL) {
        queue.first = &p;
        queue.last = queue.first;
    } else {
        queue.last->next = &p;
        queue.last = &p;
    }
    queue.count++;
    
    pthread_cond_broadcast(&queue.cond);         // Wake up consumer waiting for input
    pthread_mutex_unlock(&queue.lock);
}

Packet dequeue(LockingQueue queue) {
    pthread_mutex_lock(&queue.lock);
    
    while(!queue.count){
        pthread_cond_wait(&queue.cond, &queue.lock);
    }
    Packet p = *(queue.first);
    queue.first = p.next;
    queue.count--;
    
    pthread_mutex_unlock(&queue.lock);
    return p;
}

void lockingQueue_destroy(LockingQueue queue){
    pthread_cond_destroy(&queue.cond);
    pthread_mutex_destroy(&queue.lock);
}
