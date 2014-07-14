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

int wait_exposure(int);
int takeExposure(double duration,int sig);
int write_data(int duration ,int num, short **BUFFER,int *index);

#endif	/* SCI_TIMELINE_FUNCS_H */

