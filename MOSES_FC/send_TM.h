/* 
 * File:   send_TM.h
 * Author: jackson.remington
 *
 * Created on June 18, 2014, 2:36 PM
 */

#ifndef SEND_TM_H
#define	SEND_TM_H

#include "tm_queue.h"

int send_image(tm_queue_t, int);
void display_usage(void);
int synclink_init(int);

extern volatile sig_atomic_t ts_alive;

#endif	/* SEND_TM_H */
