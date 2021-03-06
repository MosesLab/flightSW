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
#include <sys/stat.h>
#include "sequence.h"
#include "system.h"
#include "lockingQueue.h"
#include "fpga.h"
#include "sci_timeline_funcs.h"
#include "defs.h"
#include "hlp.h" 
#include "packet.h"
#include "roe.h"

#define PULSE 13000     //50 ms result of 150ms open time and 50 ms closing time-->75ms - 25ms = 50ms

extern volatile sig_atomic_t ts_alive;

extern moses_ops_t ops; //struct storing operational state
sequence_t * currentSequence;

void * science_timeline(void *);
void * write_data(void *);
void * telem(void *);

/*signal handling variables*/
sigset_t maskstl, oldmaskstl;
//int caught_signal;
struct sigaction run_action;   //action to be taken when ^C is entered


#endif	/* SCIENCETIMELINE_H */
