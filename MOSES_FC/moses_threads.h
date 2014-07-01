/* 
 * File:   moses_threads.h
 * Author: birdie
 *
 * Created on June 19, 2014, 12:35 PM
 */

#ifndef MOSES_THREADS_H
#define	MOSES_THREADS_H

#include <pthread.h>

#define NUM_THREADS 6

pthread_attr_t attrs;           //attributes of executed threads
pthread_t threads[NUM_THREADS];         //array of running threads

enum thread{
    hlp_down_thread,
    hlp_control_thread,
    hlp_hk_thread,
    hlp_shell_thread,
    sci_timeline_thread,
    telem_thread,
};

#endif	/* MOSES_THREADS_H */

