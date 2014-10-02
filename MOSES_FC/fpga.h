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

#endif	/* FPGA_H */

