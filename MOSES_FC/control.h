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
#include "tm_queue.h"
#include "sequence.h"
#include "moses_threads.h"
#include "controlFuncs.h"

extern volatile sig_atomic_t ts_alive;

/*struct to control state of experiment*/
moses_ops_t ops;


LockingQueue hkdownQueue;
tm_queue_t roeQueue;

/*hlp_control initializations*/
void* hlp_control(void*);

/*hlp_down thread initializations*/
int fdown;
void* hlp_down(void*);

/*housekeeping initializations*/ 
void * hlp_housekeeping(void * arg);

void * telem(void * arg);

#endif /*CONTROL_H*/
