/* 
 * File:   control_sim.h
 * Author: birdie
 *
 * Created on June 17, 2014, 9:14 PM
 */


#ifndef CONTROL_SIM_H
#define	CONTROL_SIM_H

#define EVENT_SIZE (sizeof (struct inotify_event))
#define EVENT_BUFFER_LENGTH (1024 * EVENT_SIZE + NAME_MAX + 1)

#include <termios.h>
#include <signal.h>
#include <sys/inotify.h>
#include <dirent.h>
//#include <memory.h>
//#include <limits.h>
#include "system.h"
//#include "lockingQueue.h"
#include "hlp.h"
#include "packet.h"
#include "hashTable.h"
#include "roe_image.h"
#include "tm_queue.h"

tm_queue_t roeQueue;

extern volatile sig_atomic_t ts_alive;

void* science(void*);
void* telem(void*);

#endif	/* CONTROL_SIM_H */

