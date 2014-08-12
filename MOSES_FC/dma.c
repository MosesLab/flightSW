/**************************************************
 * Author: David Keltgen                            *
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
    U8 dmaChannel = DMA_CHAN;
    /*Lock PLX mutex*/

    rc = GetAndOpenDevice(&fpga_dev, 0x9056);

    if (rc != ApiSuccess) {
        //printf("*ERROR* - API failed, unable to open PLX Device\n");
        PlxSdkErrorDisplay(rc);
        exit(-1);
    }

    /*Set up interrupt from FPGA*/
    PlxPci_InterruptEnable(&fpga_dev, &DmaIntr);

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
#if PLX_SDK_VERSION_MAJOR==5 && PLX_SDK_VERSION_MINOR==1
    DmaParams.u.PciAddrLow = (U32) PciBuffer.PhysicalAddr;
    DmaParams.PciAddrHigh = 0x0;
    DmaParams.LocalAddr = FPGA_MEMORY_LOC_0;
    DmaParams.ByteCount = SIZE_DS_BUFFER;
    DmaParams.LocalToPciDma = 1; // FPGA to physical mem
#else
    DmaParams.PciAddr = PciBuffer.PhysicalAddr;
    DmaParams.LocalAddr = FPGA_MEMORY_LOC_0;
    DmaParams.ByteCount = SIZE_DS_BUFFER;
    DmaParams.Direction = PLX_DMA_LOC_TO_PCI;
#endif   

    return TRUE;
}

/*dmaRead will wait for the FPGA Interrupt*/
/*Once the interrupt is received, it will */
/*update BUFFER with the data that DMA */
/*stored on the VDX as pBufferMem, */

/*  then disable the interrupt*/
int dmaRead() {
    int rc;

    /*Allocate Space for incoming data*/
    pBufferMem = malloc(SIZE_DS_BUFFER);
    if (pBufferMem == NULL) {
        printf("*ERROR* - Source buffer allocation failed");
        /*Unlock Mutex*/
        return (FALSE);
    }
    memset(pBufferMem, 0, SIZE_DS_BUFFER);

    record("Waiting on DMA Transfer finish interrupt\n");

    PlxPci_NotificationRegisterFor(&fpga_dev, &DmaIntr, &DmaEvent);
    rc = PlxPci_NotificationWait(&fpga_dev, &DmaEvent, (U64) 5000);

    switch (rc) {
        case ApiSuccess:
            printf("Success\n");
            break;

        case ApiWaitTimeout:
            rc = ApiWaitTimeout;
            printf("Timeout\n");
            break;

        case ApiWaitCanceled:
            printf("Failed\n");
            rc = ApiFailed;
            break;

        default:
            printf("Default\n");
            // Added to avoid compiler warning
            break;
    }

    /*Tell FPGA to send interrupt???*/

    //rc = PlxPci_DmaTransferBlock(&fpga_dev, dmaChannel, &DmaParams, (3 * 1000)); //last parameter is timeout in ms

    /*if (rc != ApiSuccess) {
        if (rc == ApiWaitTimeout) {
            // Timed out waiting for DMA completion 
            printf("Timeout");  return(FALSE);
        } else {
            printf("*ERROR* - API failed\n");
            PlxSdkErrorDisplay(rc); return(FALSE);
        }
    }*/

    printf("DMA Read\n");

    PlxPci_InterruptDisable(&fpga_dev, &DmaIntr);



    return TRUE;
}

void finish() {

}

int dmaClearBlock() {
    int rc;

    pVa = (void**) &pBufferMem; //pVa = A pointer to a buffer to hold the virtual address

    rc = PlxPci_CommonBufferUnmap(&fpga_dev, pVa);
    if (rc != ApiSuccess) {
        printf("*ERROR* - API failed\n");
        PlxSdkErrorDisplay(rc);
        return (FALSE);
    }

    printf("DMA Block Cleared");
    return (TRUE);
}

void dmaClose() {
    int rc;
    U8 dmaChannel = DMA_CHAN;

    printf("Closing DMA Channel: \n");
    rc = PlxPci_DmaChannelClose(&fpga_dev, dmaChannel);

    if (rc != ApiSuccess) {
        printf("API Failed, attempting to reset PLX device");

        // Reset the device if a DMA is in-progress 
        if (rc == ApiDmaInProgress) {
            PlxPci_DeviceReset(&fpga_dev);

            // Attempt to close again 
            PlxPci_DmaChannelClose(&fpga_dev, 0);

            //printf("%d", bPass);
        }
    }
}
