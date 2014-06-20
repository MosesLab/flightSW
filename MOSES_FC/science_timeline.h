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
//#include "roeImage.h"
#include "defs.h"
#include "hlp.h"
#include "packet.h"

#define PULSE 150000

sigset_t maskstl, oldmaskstl;
int caught_signal;
struct sigaction run_action;   //action to be taken when ^C is entered
char sindex[2];
char sframe[10];

extern volatile sig_atomic_t ts_alive;
//extern struct ops ops;
extern LockingQueue hkdownQueue;

extern moses_ops_t ops; //struct storing operational state


void run_sig();
void init_signal_handler_stl();
void* science_timeline(void *);
int wait(int);
int takeExposure(double duration,int sig);
int write_data(int duration ,int num, short **BUFFER,int *index);



#endif	/* SCIENCETIMELINE_H */