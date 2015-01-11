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

int open_fpga() {
    int rc;

    rc = GetAndOpenDevice(&fpga_dev, 0x9056);
    if (rc != ApiSuccess) {
        //printf("*ERROR* - API failed, unable to open PLX Device\n");
        PlxSdkErrorDisplay(rc);
        exit(-1);
    }
    return TRUE;

}

/**
 * pulls reset register low and then high to put fpga back into original state
 * @return 
 */
int reset_fpga() {
    record("Resetting FPGA...\n");
    

//    PlxPci_DeviceReset(&fpga_dev);

    gpio_out_state.bf.fpga_reset = 0;
    poke_gpio(OUTPUT_GPIO_ADDR, gpio_out_state.val);

    gpio_out_state.bf.fpga_reset = 1;
    poke_gpio(OUTPUT_GPIO_ADDR, gpio_out_state.val);

    /*pause program to give fpga a chance to reset completely*/
    sleep(6);

    

    return TRUE;
}

/**
 * Opens PLX PCI device and starts the dma engine
 * @return 1 if success, 0 if failure
 */
int initializeDMA() {
    int rc;

    record("Open DMA channel\n");

    // Clear DMA properties 
    memset(&DmaProp, 0, sizeof (PLX_DMA_PROP));

    // Setup DMA configuration structure 
    DmaProp.ReadyInput = 1; // Enable READY# input 
    DmaProp.Burst = 1; // Use burst of 4LW 
    DmaProp.LocalBusWidth = 3; // 2 is indicates 32 bit in API pdf, but is 3 in sample code?
    DmaProp.ConstAddrLocal = 1; //Don't increment address, FPGA does this for us.
    //    DmaProp.DoneInterrupt = 1;  //TEST DONE INTERRUPT. NOT IN SAMPLE!!!
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
    register uint i, j;
    register uint i0 = 0, i1 = 0, i2 = 0, i3 = 0;
    uint frag = NUM_FRAGMENT;
    uint buf_size = SIZE_DS_BUFFER / 2;
    unsigned short next_pixel = 0;
    unsigned short ** dest_buf = image->data; //Copy pointer to destination buffer so as not to evaluate pointer chain within loop
    uint * dest_size = image->size;


    for (i = 0; i < frag; i++) {
        for (j = 0; j < (buf_size); j++) {
            next_pixel = virt_buf[i][j]; // Check the channel of the next pixel

            if (next_pixel >= 0xC000) { // Channel 3
                dest_buf[3][i3] = next_pixel;
                i3++;
            } else if (next_pixel >= 0x8000) { // Channel 2
                dest_buf[2][i2] = next_pixel;
                i2++;
            } else if (next_pixel >= 0x4000) { // Channel 1
                dest_buf[1][i1] = next_pixel;
                i1++;
            } else { // Channel 0
                dest_buf[0][i0] = next_pixel;
                i0++;
            }

            /*make sure the indices aren't too big*/
            if (i0 > buf_size || i1 > buf_size || i2 > buf_size || i3 > buf_size) {
                record("SCIENCE DATA BUFFER OVERFLOW!\n");
                goto end_sort; // Don't freak out, breaking out of double loop 
            }
        }
    }
end_sort: // To break out of double loop

    /*assign sizes for imageindex.xml*/
    dest_size[0] = i0;
    dest_size[1] = i1;
    dest_size[2] = i2;
    dest_size[3] = i3;

}

void unsort(roeimage_t * image) {
//    char buf[255];

    register uint i, j = 0;
    uint frag = NUM_FRAGMENT;
    uint buf_size = SIZE_DS_BUFFER / 2;
    unsigned short ** dest_buf = image->data;
    unsigned short next_pixel;

    /*values for predicting next pixel*/
    unsigned short pred_val = 0;
    unsigned short pred_pixel;
    unsigned int count = 0;

//    int beef = 0;
    int expected_size = frag * buf_size;
    
    uint * dest_size = image->size;
    for (i = 0; i < frag; i++) {
        for (j = 0; j < (buf_size); j++) {      
            
            /*roll counter to the right by two*/
            pred_pixel = rotr(pred_val);
            
            next_pixel = virt_buf[i][j];
            
            if ((next_pixel != pred_pixel)) {
                printf("Pixel lost! Got %04x but expected %04x at index %d out of %d\n", next_pixel, pred_pixel, count, expected_size);
                pred_val = (rotl(next_pixel) + 1) % (2048 * 4);
            }
            else {
                pred_val = (pred_val + 1) % (2048 * 4); 
            }
            
            dest_buf[i][j] = next_pixel;
            count++;

        }
        dest_size[i] = buf_size; //number of pixels
    }
//    if (beef) {
//        sprintf(buf, "*ERROR* Not 0xBEEF, %d times\n", beef);
//        record(buf);
//    }
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

    record("DMA Block Cleared\n");
    return (TRUE);
}

/**
 * Close DMA pipe after program close
 */
void dmaClose() {
    int rc;

    record("Closing DMA Channel: \n");
    rc = PlxPci_DmaChannelClose(&fpga_dev, dmaChannel);

    if (rc != ApiSuccess) {
        printf("API Failed, attempting to reset PLX device\n");

        // Reset the device if a DMA is in-progress 
        if (rc == ApiDmaInProgress) {
            PlxPci_DeviceReset(&fpga_dev);

            // Attempt to close again 
            PlxPci_DmaChannelClose(&fpga_dev, 0);

        }
    }
}

int close_fpga() {
    int rc;

    rc = PlxPci_DeviceClose(&fpga_dev);
    if (rc != ApiSuccess) {
        //printf("*ERROR* - API failed, unable to open PLX Device\n");
        PlxSdkErrorDisplay(rc);
        exit(-1);
    }
    return TRUE;

}

short rotr(short val) {
    short temp = val >> 2;
    short temp2 = val << 14;
    
    return temp | temp2;
}

short rotl(short val) {
    short temp = val << 2;
    short temp2 = val >> 14;
    
    return temp | temp2;
}
