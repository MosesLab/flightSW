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



#define SYNCLINK_START 0
#define SYNCLINK_END 1

extern volatile sig_atomic_t ts_alive;

/*struct to control state of experiment*/
moses_ops_t ops;

/*Locking queues to safely pass data between threads*/
LockingQueue hkdownQueue;
LockingQueue roeQueue;

/*hlp_control initializations*/
void* hlp_control(void *);

/*hlp_down thread initializations*/

void* hlp_down(void*);

/*housekeeping initializations*/ 
void * hlp_housekeeping(void * arg);

void * telem(void *);

void * hlp_shell_out(void *);

#endif /*CONTROL_H*/
