/* 
 * File:   main.h
 * Author: Roy Smart
 *
 * Created on January 24, 2014, 7:39 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include "system.h"
#include <pthread.h>
#include <signal.h>
#include "moses_threads.h"
#include "control.h"
#include "science_timeline.h"
#include "sequence.h"
#include "lockingQueue.h"
#include "fpga.h"
#include "controlFuncs.h"

/*IO constants*/
#define HLP_CONTROL_CONF "HLP_CONTROL_THREAD"
#define DOWN_CONF "HLP_DOWN_THREAD"
#define GPIO_CONTROL_CONF "GPIO_CONTROL_THREAD"
#define SCIENCE_CONF "SCIENCE_THREAD"
#define TELEM_CONF "TELEM_THREAD"
#define SHELL_CONF "HLP_SHELL_THREAD"
#define IMAGE_WRITER_CONF "IMAGE_THREAD"
#define FPGA_SERVER_CONF "FPGA_SERVER_THREAD"

#define NUM_IO 5
#define HKUP_CONF "HLP_UP"
#define HKDOWN_CONF "HLP_DOWN"
#define ROE_CONF "ROE"
#define IMAGE_SIM_CONF "IMAGE_SIM"
#define SYNCLINK_CONF "SYNCLINK_TM"




/*Global quit variable declaration*/
extern volatile sig_atomic_t ts_alive;     //variable modified by signal handler, setting this to false will end the threads

/*Quit signal varibles*/
extern pid_t main_pid;
extern int quit_sig;
extern struct sigaction quit_action;   //action to be taken when ^C (SIGINT) is entered
extern sigset_t mask, oldmask;         //masks for SIGINT signal

/*configuration variables*/
extern int config_values[NUM_RROBIN + NUM_FIFO + NUM_IO];        //array of values holding moses program configurations   
extern node_t** config_hash_table;

/*enum and definition must match! or unhappiness will occur*/
#define QUEUE_NUM 8
enum queues{
    sequence,
    scit_image,
    fpga_image,
    telem_image,
    gpio_in,
    gpio_out,
    gpio_req,
    hkdown   
};

/*lock queues for thread sync */
extern LockingQueue lqueue[QUEUE_NUM];
extern uint lqueue_num;

int moses();
int main(int argc, char **argv);
void main_init();
void read_moses_config();
void quit_signal(int);  //signal handler
void init_signal(int sig);
void start_signal(int sig);
void reset_signal(int sig);
void start_threads();
void join_threads();
void init_quit_signal_handler();
void cleanup();






#endif /*MAIN_H*/


