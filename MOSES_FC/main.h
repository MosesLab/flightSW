/* 
 * File:   main.h
 * Author: byrdie
 *
 * Created on January 24, 2014, 7:39 PM
 */


#include "system.h"
#include <signal.h>
#include <pthread.h>
#include "control.h"
#include "science_timeline.h"

#define NUM_THREADS 5
#define NUM_IO 4


volatile sig_atomic_t ts_alive = 1;     //variable modified by signal handler, setting this to false will end the threads

pthread_attr_t attrs;           //attributes of executed threads
pthread_t threads[NUM_THREADS];         //array of running threads

struct sigaction quit_action;   //action to be taken when ^C (SIGINT) is entered
sigset_t mask, oldmask;         //masks for SIGINT signal

/*configuration variables*/
char * config_path = "moses.conf";
int thread_config[NUM_THREADS];         //array of configurations for executed threads
int io_config[NUM_IO];          // array of configurations for moses io
char * config_strings[NUM_THREADS + NUM_IO];
    


void quit_signal(int);  //signal handler
void start_threads();
void join_threads();
void init_signal_handler();
void config_strings_init();
void read_moses_config();

enum thread{
    hlp_down_thread,
    hlp_control_thread,
    hlp_hk_thread,
    sci_timeline_thread,
    telem_thread,
};

enum moses_io{
  hlp_up_interface,
  hlp_down_interface,
  roe_interface,
  synclink_interface,
};




