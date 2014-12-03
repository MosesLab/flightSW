/* 
 * File:   fpga.h
 * Author: Roy Smart
 * Montana State University
 * MOSES LAB
 * 
 * Contains functions for interacting with Connect Tech FPGA and PLX 9506
 * PCI bridge
 *
 * Created on September 4, 2014, 9:10 AM
 */

#ifndef FPGA_H
#define	FPGA_H

#include "system.h"
#include "gpio.h"
#include "dma.h"

void * fpga_server(void *);

int interrupt_wait(U32*);

roeimage_t * dma_image;

/*variables to syncronize science timeline and fpga sever*/
extern pthread_condattr_t dma_done_attr;
extern pthread_cond_t dma_done_cond;
extern pthread_mutex_t dma_done_mutex;

#endif	/* FPGA_H */

