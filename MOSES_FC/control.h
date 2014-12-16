/* 
 * File:   control.h
 * Author: Roy Smart
 *
 * Created on January 24, 2014, 10:49 PM
 */

#ifndef CONTROL_H
#define	CONTROL_H

#include "system.h"
#include "main.h"
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
#include "roe_image.h"

#define HLP_PACKET      1
#define GPIO_INP        2

#define SYNCLINK_START 0
#define SYNCLINK_END 1

/*struct to control state of experiment*/
extern moses_ops_t ops;
extern pid_t vshell_pid;



/*hlp_control initializations*/
void* hlp_control(void *);

void * gpio_control(void *);

/*hlp_down thread initializations*/
void* hlp_down(void*);

/*housekeeping initializations*/ 
void * hlp_housekeeping(void * arg);

void * hlp_shell_out(void *);



#endif /*CONTROL_H*/
