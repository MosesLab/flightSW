/* 
 * File:   send_TM.h
 * Author: jackson.remington
 *
 * Created on June 18, 2014, 2:36 PM
 */

#ifndef SEND_TM_H
#define	SEND_TM_H


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <termios.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "lockingQueue.h"
#include "roe_image.h"

int send_image(imgPtr_t*, int, int);
void display_usage(void);
int synclink_init(int);

extern volatile sig_atomic_t ts_alive;

#endif	/* SEND_TM_H */
