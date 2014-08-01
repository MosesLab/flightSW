/* 
 * File:   sci_timeline_funcs.h
 * Author: birdie
 *
 * Created on June 18, 2014, 8:44 PM
 */

#ifndef SCI_TIMELINE_FUNCS_H
#define	SCI_TIMELINE_FUNCS_H

#include "science_timeline.h"
#include "roe_image.h"
#include "control.h"
#include "gpio.h"
#include "dma.h"


sigset_t maskimage, oldmaskimage;
int caught_image_signal;

struct sigaction run_action_image;  

int wait_exposure(int);
int takeExposure(double duration,int sig);
int write_data();
void init_signal_handler_image();
void runsig2();


#endif	/* SCI_TIMELINE_FUNCS_H */

