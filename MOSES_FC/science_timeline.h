/* 
 * File:   ScienceTimeline.h
 * Author: david
 *
 * Created on May 28, 2014, 11:34 AM
 */
#ifndef SCIENCETIMELINE_H
#define	SCIENCETIMELINE_H

#include <signal.h>
#include <pthread.h>
#include "sequence.h"
#include "system.h"
#include "lockingQueue.h"
#include "sci_timeline_funcs.h"
#include "defs.h"
#include "hlp.h"
#include "packet.h"

#define PULSE 150000

extern volatile sig_atomic_t ts_alive;

extern moses_ops_t ops; //struct storing operational state


void * science_timeline(void *);
void * write_data(void *);
void * telem(void *);



#endif	/* SCIENCETIMELINE_H */
