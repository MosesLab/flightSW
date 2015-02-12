/* 
 * File:   moses_threads.h
 * Author: Roy Smart
 *
 * Created on June 19, 2014, 12:35 PM
 */

#ifndef MOSES_THREADS_H
#define	MOSES_THREADS_H

#include <pthread.h>

/*Threads are either round-robin or fifo schedule policy*/
#define NUM_RROBIN      4
#define NUM_FIFO        4

extern unsigned int num_threads;

typedef void*(*thread_func)(void*);

extern thread_func tfuncs[NUM_RROBIN + NUM_FIFO];
extern void * targs[NUM_RROBIN + NUM_FIFO];

extern pthread_t threads[NUM_RROBIN + NUM_FIFO]; //array of running threads

/*enum must match define RROBIN and FIFO above*/
enum thread_config {
    /*FIFO threads*/
    hlp_control_thread,
    sci_timeline_thread,
    image_writer_thread,
    fpga_server_thread,
    
    /*Round-Robin threads*/
    telem_thread,
    hlp_down_thread,
    hlp_shell_thread,
    gpio_control_thread,
    
    /*IO configs*/
    hlp_up_interface,
    hlp_down_interface,
    roe_interface,
    image_sim_interface,
    synclink_interface,
};

#endif	/* MOSES_THREADS_H */

