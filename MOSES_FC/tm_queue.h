/* 
 * File:   roeQueue.h
 * Author: birdie
 *
 * Created on June 17, 2014, 9:44 PM
 */

#ifndef TM_QUEUE_H
#define	TM_QUEUE_H

#include <signal.h>
#include <pthread.h>
#include "system.h"
#include "roe_image.h"

extern volatile sig_atomic_t ts_alive;


typedef struct {
    imgPtr_t* first;
    imgPtr_t* last;
    int count;                                  //current number of items
    
    pthread_condattr_t cattr;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} tm_queue_t;

void tm_queue_init(tm_queue_t *);
void tm_enqueue(tm_queue_t*, imgPtr_t*);
void tm_dequeue(tm_queue_t *);
void tm_queuedestroy(tm_queue_t *);


#endif	/* TM_QUEUE_H */

