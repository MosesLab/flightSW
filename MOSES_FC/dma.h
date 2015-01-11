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

#define SIZE_DS_BUFFER          4194304        // Number of bytes to transfer per fragment (1/4 of image)
#define NUM_FRAGMENT            4
#define FPGA_MEMORY_LOC_0       0x0000020
#define DMA_CHAN                0x0
#define DMA_TIMEOUT             5000 //ms

#define INP_INT             0x00000001
#define TIMEOUT_INT         0xFFFFFFFF
#define DMA_AVAILABLE       0x00000002

extern PLX_DEVICE_OBJECT fpga_dev;

/*DMA variables*/
//extern PLX_DMA_PROP      DmaProp;
//extern PLX_INTERRUPT     plx_intr;
//extern PLX_NOTIFY_OBJECT plx_event;
//extern U8                dmaChannel;

/*Driver can only allocate 1/4 of image. DMA transfer must be fragmented*/
extern PLX_DMA_PARAMS    dma_params[NUM_FRAGMENT];
extern PLX_PHYSICAL_MEM  pci_buffer[NUM_FRAGMENT];
extern short*            virt_buf[NUM_FRAGMENT];
extern PLX_INTERRUPT     plx_intr;
extern PLX_NOTIFY_OBJECT plx_event;

int initializeDMA();
int open_fpga();
int reset_fpga();
int close_fpga();

int dmaRead(PLX_DMA_PARAMS, U64);
int allocate_buffer(PLX_DMA_PARAMS*, PLX_PHYSICAL_MEM*, short **);
void sort(roeimage_t*);
void unsort(roeimage_t * image);
void beef(roeimage_t * image);
int dmaClearBlock();
void dmaClose();
short rotr(short);
short rotl(short val);

#endif	/* DMA_H */

