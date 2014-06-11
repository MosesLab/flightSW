/* 
 * File:   control.h
 * Author: Roy Smart
 *
 * Created on January 24, 2014, 10:49 PM
 */

#include "system.h"
#include "lockingQueue.h"
#include <termios.h>
#include <signal.h>
#include "hlp.h"
#include "packet.h"
#include "hashTable.h"

extern volatile sig_atomic_t ts_alive;

LockingQueue hkdownQueue;

/*hlp_contorl initializations*/
void* hlp_control(void*);

/*hlp_down thread initializations*/
int fdown;
void* hlp_down(void*);

/*housekeeping initializations*/
void * hlp_housekeeping(void * arg);
