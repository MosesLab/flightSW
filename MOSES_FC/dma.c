/**************************************************
 * Author: David Keltgen, Roy Smart                 *
 * Company: Montana State University: MOSES LAB     *
 * File Name: dma.c                                 *
 * Date:  July 17 2014                              *
 * Description: dma.c will use various functions of *
 *              the PlxApi to initiate the transfer *
 *              of image data from the FPGA to the  *
 *              flight computer memory.             *
 **************************************************/

#include "dma.h"

/*global variable declaration*/
PLX_DEVICE_OBJECT fpga_dev;
PLX_DMA_PROP DmaProp;
PLX_INTERRUPT plx_intr;
PLX_NOTIFY_OBJECT plx_event;
U8 dmaChannel;
PLX_DMA_PARAMS dma_params[NUM_FRAGMENT];
PLX_PHYSICAL_MEM pci_buffer[NUM_FRAGMENT];
short* virt_buf[NUM_FRAGMENT];

/**
 * Opens PLX PCI device and starts the dma engine
 * @return 1 if success, 0 if failure
 */
int initializeDMA() {
    int rc;

    /*Lock PLX mutex*/
    rc = GetAndOpenDevice(&fpga_dev, 0x9056);

    if (rc != ApiSuccess) {
        //printf("*ERROR* - API failed, unable to open PLX Device\n");
        PlxSdkErrorDisplay(rc);
        exit(-1);
    }
    
    /*reset device dma transfer*/
//    rc = PlxPci_DeviceReset(&fpga_dev);

    // Clear DMA properties 
    memset(&DmaProp, 0, sizeof (PLX_DMA_PROP));

    // Setup DMA configuration structure 
    DmaProp.ReadyInput = 1; // Enable READY# input 
    DmaProp.Burst = 1; // Use burst of 4LW 
    DmaProp.LocalBusWidth = 3; // 2 is indicates 32 bit in API pdf, but is 3 in sample code?
    DmaProp.ConstAddrLocal = 1; //set to only transfer one dword ONLY FOR TESTING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    dmaChannel = DMA_CHAN;

    // Use Channel 0 
    rc = PlxPci_DmaChannelOpen(&fpga_dev, dmaChannel, &DmaProp);
    if (rc != ApiSuccess) {
        printf("*ERROR* - API failed\n");
        PlxSdkErrorDisplay(rc);
        return (FALSE);
    }

    return TRUE;
}

/**
 * Allocates a contiguous, page-locked buffer(CPLB) for DMAing images into memory
 * @param dma_param:struct containing dma parameters to be linked to CPLB
 * @param pci_buf:stuct to be filled with CPLB parameters
 * @param user_buf:pointer to the virtual address allocated to the CPLB
 * @return 1 fi success, 0 if failure
 */
int allocate_buffer(PLX_DMA_PARAMS * dma_param, PLX_PHYSICAL_MEM * pci_buf, short ** user_buf) {
    int rc;

    memset(pci_buf, 0, sizeof (PLX_PHYSICAL_MEM));

    pci_buf->Size = SIZE_DS_BUFFER;

    /*allocate page-locked contiguous common buffer*/
    rc = PlxPci_PhysicalMemoryAllocate(&fpga_dev, pci_buf, TRUE);
    if (rc != ApiSuccess) {
        printf("*ERROR* - API failed\n");
        PlxSdkErrorDisplay(rc);
    }


    rc = PlxPci_PhysicalMemoryMap(&fpga_dev, pci_buf);
    if (rc != ApiSuccess) {
        printf("*ERROR* - API failed\n");
        PlxSdkErrorDisplay(rc);
        /*Unlock Mutex*/
        return (FALSE);
    }

    printf("\n    Mem Allocator information:\n"
            "     Bus Physical Addr:  %016llx\n"
            "     CPU Physical Addr:  %016llx\n"
            "     Size             :  %d bytes\n",
            pci_buf->PhysicalAddr,
            pci_buf->CpuPhysical,
            pci_buf->Size);

    *user_buf = PLX_INT_TO_PTR(pci_buf->UserAddr);

    // Fill in DMA transfer parameters 
    dma_param->PciAddr = pci_buf->PhysicalAddr;
    dma_param->LocalAddr = FPGA_MEMORY_LOC_0;
    dma_param->ByteCount = SIZE_DS_BUFFER;
    dma_param->Direction = PLX_DMA_LOC_TO_PCI;

    return TRUE;
}

/**
 * dmaRead() will initiate a slave DMA transfer with the PLX 9056 and then 
 * wait until the transfer has completed.
 * 
 * @return 
 */
int dmaRead(PLX_DMA_PARAMS dma_param, U64 timeout) {
    int rc;

    rc = PlxPci_DmaTransferBlock(&fpga_dev, dmaChannel, &dma_param, timeout); 

    if (rc != ApiSuccess) {
        if (rc == ApiWaitTimeout) {
            // Timed out waiting for DMA completion 
            printf("Timeout\n");
            return (FALSE);
        } else {
            printf("*ERROR* - API failed\n");
            PlxSdkErrorDisplay(rc);
            return (FALSE);
        }
    }

    return TRUE;
}

void finish() {

}

void sort(roeimage_t * image) {
    register uint i, j, k = 0;
    uint frag = NUM_FRAGMENT;
    uint buf_size = SIZE_DS_BUFFER / 2;
    short ** dest_buf = image->data;
    uint * dest_size = image->size;
    for (i = 0; i < frag; i++) {
        for (j = 0; j < (buf_size); j++) {
               dest_buf[k][(j * i) /4] = virt_buf[i][j];
               k = (k + 1) % 4;
        }
       dest_size[i] = buf_size;    //number of pixels
    }
}

void unsort(roeimage_t * image) {
    register uint i, j = 0;
    uint frag = NUM_FRAGMENT;
    uint buf_size = SIZE_DS_BUFFER / 2;
    short ** dest_buf = image->data;
    uint * dest_size = image->size;
    for (i = 0; i < frag; i++) {
        for (j = 0; j < (buf_size); j++) {
               dest_buf[i][j] = virt_buf[i][j];
        }
       dest_size[i] = buf_size;    //number of pixels
    }
}


/**
 * Clear page-locked DMA buffer out of memory
 * 
 * @return 1 if success, 0 if failure
 */
int dmaClearBlock() {

    int i, rc;
    for (i = 0; i < NUM_FRAGMENT; i++) {
        rc = PlxPci_PhysicalMemoryUnmap(&fpga_dev, &pci_buffer[i]);

        //    PlxPci_CommonBufferUnmap(&fpga_dev, pVa);
        if (rc != ApiSuccess) {
            printf("*ERROR* - API failed\n");
            PlxSdkErrorDisplay(rc);
            return (FALSE);
        }
    }

    printf("DMA Block Cleared\n");
    return (TRUE);
}

/**
 * Close DMA pipe after program close
 */
void dmaClose() {
    int rc;

    printf("Closing DMA Channel: \n");
    rc = PlxPci_DmaChannelClose(&fpga_dev, dmaChannel);

    if (rc != ApiSuccess) {
        printf("API Failed, attempting to reset PLX device\n");

        // Reset the device if a DMA is in-progress 
        if (rc == ApiDmaInProgress) {
            PlxPci_DeviceReset(&fpga_dev);

            // Attempt to close again 
            PlxPci_DmaChannelClose(&fpga_dev, 0);

            //printf("%d", bPass);
        }
    }
    PlxPci_DeviceClose(&fpga_dev);
}
