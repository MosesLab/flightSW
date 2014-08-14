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

int initializeDMA() {
    int rc;

    /*Lock PLX mutex*/
    rc = GetAndOpenDevice(&fpga_dev, 0x9056);

    if (rc != ApiSuccess) {
        //printf("*ERROR* - API failed, unable to open PLX Device\n");
        PlxSdkErrorDisplay(rc);
        exit(-1);
    }

    // Clear DMA properties 
    memset(&DmaProp, 0, sizeof (PLX_DMA_PROP));

    // Setup DMA configuration structure 
    DmaProp.ReadyInput = 1; // Enable READY# input 
    DmaProp.Burst = 1; // Use burst of 4LW 
    DmaProp.LocalBusWidth = 3; // 2 is indicates 32 bit in API pdf, but is 3 in sample code?

    // Use Channel 0 
    rc = PlxPci_DmaChannelOpen(&fpga_dev, dmaChannel, &DmaProp);
    if (rc != ApiSuccess) {
        printf("*ERROR* - API failed\n");
        PlxSdkErrorDisplay(rc);
        return (FALSE);
    }

    printf("OK");
    // Get Common buffer information 
    rc = PlxPci_CommonBufferProperties(&fpga_dev, &PciBuffer);
    if (rc != ApiSuccess) {
        printf("*ERROR* - API failed\n");
        PlxSdkErrorDisplay(rc);
        /*Unlock Mutex*/
        return (FALSE);
    }

    printf("OK");

    printf("\n    Common buffer information:\n"
            "     Bus Physical Addr:  %016llx\n"
            "     CPU Physical Addr:  %016llx\n"
            "     Size             :  %d bytes\n",
            PciBuffer.PhysicalAddr,
            PciBuffer.CpuPhysical,
            PciBuffer.Size);


    pVa = (void**) &pBufferMem; //pVa = A pointer to a buffer to hold the virtual address 

    rc = PlxPci_CommonBufferMap(&fpga_dev, pVa);
    if (rc != ApiSuccess) {
        printf("*ERROR* - API failed\n");
        PlxSdkErrorDisplay(rc);
        /*Unlock Mutex*/
        return (FALSE);
    }

    // Fill in DMA transfer parameters 
    DmaParams.PciAddr = PciBuffer.PhysicalAddr;
    DmaParams.LocalAddr = FPGA_MEMORY_LOC_0;
    DmaParams.ByteCount = SIZE_DS_BUFFER;

    DmaParams.Direction = PLX_DMA_LOC_TO_PCI;

    /*initialize acknowledge register*/
    poke_gpio(GPIO_I_INT_ACK, 0xFFFFFFFF);

    /*enable GPIO pins on the FPGA*/
    poke_gpio(GPIO_I_INT_ENABLE, 0xFFFFFFFF);

    return TRUE;
}

/**
 * dmaRead() will initiate a slave DMA transfer with the PLX 9056 and then 
 * wait until the transfer has completed.
 * 
 * @return 
 */
int dmaRead() {
    int rc;

    /*Allocate Space for incoming data*/
    pBufferMem = malloc(SIZE_DS_BUFFER);
    if (pBufferMem == NULL) {
        printf("*ERROR* - Source buffer allocation failed");
        return (FALSE);
    }
    memset(pBufferMem, 0, SIZE_DS_BUFFER);



    printf("DMA Read\n");

    rc = PlxPci_DmaTransferBlock(&fpga_dev, dmaChannel, &DmaParams, (3 * 1000)); //last parameter is timeout in ms

    if (rc != ApiSuccess) {
        if (rc == ApiWaitTimeout) {
            // Timed out waiting for DMA completion 
            printf("Timeout");
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

/**
 * interrupt_wait() will wait on the PCI interrupt line and determine if GPIO 
 * input or DMA requested the interrupt
 * 
 * @return 1 if success 0 if failure
 */
int interrupt_wait(U32 * interrupt) {
    int rc;

    /*clear and reset interrupt structure*/
    memset(&plx_intr, 0, sizeof (PLX_INTERRUPT));
    
    /*set interrupt structure*/
    plx_intr.LocalToPci = 1; //set bit 11
    //    plx_intr.PciMain = 1;		// Bit 8 -- should already been on

    /*enable interrupt on PLX bridge*/
    rc = PlxPci_InterruptEnable(&fpga_dev, &plx_intr); // sets PCI9056_INT_CTRL_STAT
    if (rc != ApiSuccess) PlxSdkErrorDisplay(rc);

    /*register for interrupt with kernel*/
    rc = PlxPci_NotificationRegisterFor(&fpga_dev, &plx_intr, &plx_event);
    if (rc != ApiSuccess) PlxSdkErrorDisplay(rc);

    /*wait for interrupt*/
    int waitrc = PlxPci_NotificationWait(&fpga_dev, &plx_event, (U32) 100);

    /*disable interrupt*/
    rc = PlxPci_InterruptDisable(&fpga_dev, &plx_intr); // sets PCI9056_INT_CTRL_STAT
    if (rc != ApiSuccess) PlxSdkErrorDisplay(rc);

    /*handle return code of wait function*/
    if (waitrc == ApiWaitTimeout) {
        *interrupt = TIMEOUT_INT;
        return TRUE;
    } else if (waitrc == ApiSuccess) {
        *interrupt = INP_INT;
        return TRUE;
    } else if (waitrc == ApiWaitCanceled) {
        record("Wait canceled\n");
        return FALSE;
    } else {
        record("Wait returned an unknown value\n");
        return FALSE;
    }

}

/**
 * Clear page-locked DMA buffer out of memory
 * 
 * @return 1 if success, 0 if failure
 */
int dmaClearBlock() {
    int rc;

    pVa = (void**) &pBufferMem; //pVa = A pointer to a buffer to hold the virtual address

    rc = PlxPci_CommonBufferUnmap(&fpga_dev, pVa);
    if (rc != ApiSuccess) {
        printf("*ERROR* - API failed\n");
        PlxSdkErrorDisplay(rc);
        return (FALSE);
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
}
