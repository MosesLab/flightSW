/* 
 * File:   dma.h
 * Author: david
 *
 * Created on July 17, 2014, 2:17 PM
 */

#ifndef DMA_H
#define	DMA_H


#include "plx/PlxApi.h"
#include "plx/PlxInit.h"
#include "system.h"
#include "gpio.h"

#define FPGA_TIMEOUT 50 //milliseconds

#define SIZE_DS_BUFFER          256           // Number of bytes to transfer
#define FPGA_MEMORY_LOC_0	0x100
#define DMA_CHAN                0x0

#define INP_INT             0x00000001
#define DMA_INP_INT         0x80000000
#define GPIO_INP_INT        0x7FFFFFFF
#define TIMEOUT_INT         0xFFFFFFFF

extern PLX_DEVICE_OBJECT fpga_dev;

/*DMA variables*/
extern PLX_DMA_PROP      DmaProp;
extern PLX_INTERRUPT     plx_intr;
extern PLX_NOTIFY_OBJECT plx_event;
extern U8                dmaChannel;
extern PLX_DMA_PARAMS    DmaParams;
extern PLX_PHYSICAL_MEM  PciBuffer; 
extern U32*		  pBufferMem;
extern void**		  pVa;

int initializeDMA();
int dmaRead();
int interrupt_wait(U32*);
int dmaClearBlock();
void dmaClose();
#endif	/* DMA_H */

