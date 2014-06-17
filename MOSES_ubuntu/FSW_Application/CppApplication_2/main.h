/* 
 * File:   main.h
 * Author: byrdie
 *
 * Created on January 24, 2014, 7:39 PM
 */

#include "roe.h"
#include "EMSim.h"
#include "scienceTimeline.h"
#include "system.h"
#include "sequence.h"
#include <signal.h>
#include <pthread.h>

#define NUM_THREADS 2

volatile sig_atomic_t ts_alive = 1;     //variable modified by signal handler, setting this to false will end the threads
pthread_attr_t attrs;
struct sigaction quit_action;   //action to be taken when ^C is entered
sigset_t mask, oldmask;

void quit_signal(int);  //signal handler
void start_threads();
void join_threads();
void init_signal_handler();

pthread_t threads[NUM_THREADS];
