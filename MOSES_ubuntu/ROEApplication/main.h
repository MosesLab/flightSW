/* 
 * File:   main.h
 * Author: david
 *
 * Created on June 4, 2014, 3:52 PM
 */

#include <pthread.h>
#include "roe.h"
#include "roehk.h"
#include "HKeepingDriver.h"
#include "HLPDriver.h"


#define NUM_THREADS 2
#define TRUE        1
#define FALSE       0

#ifndef MAIN_H
#define	MAIN_H

enum thread{
    HLP_thread,
    HK_thread
};

pthread_attr_t attrs;
pthread_t threads[NUM_THREADS];

void start_threads();
void join_threads();

extern struct ReadOutElectronics roe;

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

