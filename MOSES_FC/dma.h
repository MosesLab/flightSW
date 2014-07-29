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

#define SIZE_DS_BUFFER         256           // Number of bytes to transfer
#define FPGA_MEMORY_LOC_0		0x100

PLX_DEVICE_OBJECT fpga_dev;
PLX_DMA_PROP      DmaProp;
PLX_INTERRUPT     DmaIntr;
PLX_NOTIFY_OBJECT DmaEvent;
U8                dmaChannel;
PLX_DMA_PARAMS    DmaParams;
PLX_PHYSICAL_MEM  PciBuffer; 
int               rc;
U32*		  pBufferMem;
void**		  pVa;
struct timeval expstop, expstart;





int initializeDMA();
int dmaRead();
int dmaClearBlock();
void dmaClose();
#endif	/* DMA_H */

