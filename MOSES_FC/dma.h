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

#define SIZE_DS_BUFFER          4194304          // Number of bytes to transfer (1/4 of image)
#define NUM_FRAGMENT            4
#define FPGA_MEMORY_LOC_0       0x0000004
#define DMA_CHAN                0x0
#define DMA_TIMEOUT             10000 //ms

#define INP_INT             0x00000001
#define DMA_INP_INT         0x80000000
#define GPIO_INP_INT        0x7FFFFFFF
#define TIMEOUT_INT         0xFFFFFFFF

extern PLX_DEVICE_OBJECT fpga_dev;

/*DMA variables*/
//extern PLX_DMA_PROP      DmaProp;
//extern PLX_INTERRUPT     plx_intr;
//extern PLX_NOTIFY_OBJECT plx_event;
//extern U8                dmaChannel;

/*Driver can only allocate 1/4 of image. DMA transfer must be fragmented*/
extern PLX_DMA_PARAMS    dma_params[NUM_FRAGMENT];
extern PLX_PHYSICAL_MEM  pci_buffer[NUM_FRAGMENT];
extern U64*              virt_buf[NUM_FRAGMENT];


int initializeDMA();
int interrupt_wait(U32*);
int dmaRead(PLX_DMA_PARAMS, U64);
int allocate_buffer(PLX_DMA_PARAMS*, PLX_PHYSICAL_MEM*, U64 **);
void sort(roeimage_t*);
int dmaClearBlock();
void dmaClose();
#endif	/* DMA_H */

