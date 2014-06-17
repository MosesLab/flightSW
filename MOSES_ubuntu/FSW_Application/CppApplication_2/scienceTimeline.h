/* 
 * File:   ScienceTimeline.h
 * Author: david
 *
 * Created on May 28, 2014, 11:34 AM
 */

#include <signal.h>
#include <pthread.h>
#include "sequence.h"
#include "system.h"
#include "lockingQueue.h"
//#include "roeImage.h"
#include "defs.h"

#ifndef SCIENCETIMELINE_H
#define	SCIENCETIMELINE_H

sigset_t maskstl, oldmaskstl;
struct sigaction run_action;   //action to be taken when ^C is entered
char sindex[2];
char sframe[10];

extern volatile sig_atomic_t ts_alive;
//extern struct ops ops;
extern LockingQueue hkdownQueue;

void run_sig();
void init_signal_handler_stl();
int ScienceTimeline(void);

int wait(int);
int takeExposure(double duration,int sig);
int write_data(int duration ,int num, short **BUFFER,int *index);


#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* SCIENCETIMELINE_H */

