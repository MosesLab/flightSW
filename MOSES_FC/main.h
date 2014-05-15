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
pthread_attr_t attrs;
struct sigaction quit_action;   //action to be taken when ^C is entered
sigset_t mask, oldmask;


void *hlp_down(void *);
void *hlp_control(void *);
void *hkupSimThread(void *);

void quit_signal(int);  //signal handler
void start_threads();
void join_threads();
void init_signal_handler();

enum thread{
    hlp_down_thread,
    hlp_control_thread,
};

pthread_t threads[NUM_THREADS];
