/* 
 * File:   control.h
 * Author: Roy Smart
 *
 * Created on January 24, 2014, 10:49 PM
 */

#ifndef CONTROL_H
#define	CONTROL_H

#include "system.h"
#include "lockingQueue.h"
#include <termios.h>
#include <signal.h>
#include "hlp.h"
#include "packet.h"
#include "hashTable.h"
#include "sequence.h"
#include "moses_threads.h"
#include "controlFuncs.h"
#include "send_TM.h"
#include "v_shell.h"
#include "gpio.h"
#include "dma.h"


#define SYNCLINK_START 0
#define SYNCLINK_END 1

extern volatile sig_atomic_t ts_alive;

/*struct to control state of experiment*/
moses_ops_t ops;

LockingQueue hkdownQueue; //Pass packets from flight SW to hkdown thread

/*FPGA server queues*/
LockingQueue scit_image_queue; //Pass images from science timeline to fpga server
LockingQueue gpio_out_queue;    // Pass gpio values from producers to fpga server

LockingQueue gpio_in_queue;     //Pass GPIO values from fpga server to gpio control

LockingQueue telem_image_queue; //Pass image filepaths to telemetry

/*Current state of powered subsystems*/
gpio_out_uni gpio_power_state;

/*hlp_control initializations*/
void* hlp_control(void *);

/*hlp_down thread initializations*/

void* hlp_down(void*);

/*housekeeping initializations*/ 
void * hlp_housekeeping(void * arg);
void * telem(void *);

void * hlp_shell_out(void *);

void * fpga_server(void *);

#endif /*CONTROL_H*/
