/* 
 * File:   main.h
 * Author: byrdie
 *
 * Created on January 24, 2014, 7:39 PM
 */


#include "system.h"
#include <signal.h>
#include <pthread.h>

#define NUM_THREADS 2

volatile sig_atomic_t ts_alive = 1;     //variable modified by signal handler, setting this to false will end the threads

struct sigaction quit_action;   //action to be taken when ^C is entered
sigset_t mask, oldmask;

//functions
void *hkupThread(void *);
void *controlThread(void *);

void quit_signal(int);  //signal handler
void start_threads();
void join_threads();
void init_signal_handler();

enum thread{
    HkupThread,
    ControlThread,
};

pthread_t threads[NUM_THREADS];
