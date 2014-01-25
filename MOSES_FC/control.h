/* 
 * File:   control.h
 * Author: Roy Smart
 *
 * Created on January 24, 2014, 10:49 PM
 */

#include "system.h"
#include "lockingQueue.h"
#include <signal.h>

extern volatile sig_atomic_t ts_alive;

LockingQueue hkupQueue;

void * controlThread(void);
