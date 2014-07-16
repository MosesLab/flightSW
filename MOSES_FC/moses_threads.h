/* 
 * File:   moses_threads.h
 * Author: birdie
 *
 * Created on June 19, 2014, 12:35 PM
 */

#ifndef MOSES_THREADS_H
#define	MOSES_THREADS_H

#include <pthread.h>

#define NUM_THREADS 7

pthread_attr_t attrs; //attributes of executed threads
pthread_t threads[NUM_THREADS]; //array of running threads

enum thread_config {
    hlp_down_thread,
    hlp_control_thread,
    hlp_hk_thread,
    hlp_shell_thread,
    sci_timeline_thread,
    telem_thread,
    image_writer_thread,
    hlp_up_interface,
    hlp_down_interface,
    roe_interface,
    synclink_interface,
};

#endif	/* MOSES_THREADS_H */

