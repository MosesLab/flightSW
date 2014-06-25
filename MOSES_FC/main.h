/* 
 * File:   main.h
 * Author: Roy Smart
 *
 * Created on January 24, 2014, 7:39 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include "system.h"
#include <signal.h>
#include "moses_threads.h"
#include "control.h"
#include "science_timeline.h"
#include "sequence.h"

#define NUM_IO 4

#define CONTROL_CONF "HLP_CONTROL_THREAD"
#define DOWN_CONF "HLP_DOWN_THREAD"
#define HK_CONF "HLP_HK_THREAD"
#define SCIENCE_CONF "SCIENCE_THREAD"
#define TELEM_CONF "TELEM_THREAD"
#define HKUP_CONF "HLP_UP"
#define HKDOWN_CONF "HLP_DOWN"
#define ROE_CONF "ROE"
#define SYNCLINK_CONF "SYNCLINK_TM"


volatile sig_atomic_t ts_alive = 1;     //variable modified by signal handler, setting this to false will end the threads



struct sigaction quit_action;   //action to be taken when ^C (SIGINT) is entered
sigset_t mask, oldmask;         //masks for SIGINT signal

/*configuration variables*/
char * config_path = "moses.conf";
int config_size;
int config_values[NUM_THREADS + NUM_IO];        //array of values holding moses program configurations
char * config_strings[NUM_THREADS + NUM_IO];
    
node_t** config_hash_table;

void quit_signal(int);  //signal handler
void start_threads();
void join_threads();
void init_quit_signal_handler();
void config_strings_init();
void read_moses_config();



enum moses_io{
  hlp_up_interface,
  hlp_down_interface,
  roe_interface,
  synclink_interface,
};

#endif /*MAIN_H*/


