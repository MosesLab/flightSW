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

void initializeDMA() {
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
}

void dmaRead() {
    record("Performing block transfer, FPGA -> MEM:\n ");

    /*Allocate Space for incoming data*/
    pBufferMem = malloc(SIZE_DS_BUFFER);

    if (pBufferMem == NULL) {
        printf("*ERROR* - Source buffer allocation failed");
    }

    memset(pBufferMem, 0, SIZE_DS_BUFFER);

    pVa = (void**) &pBufferMem;//pVa = A pointer to a buffer to hold the virtual address 

    rc = PlxPci_CommonBufferMap(&fpga_dev, pVa ); 
    if (rc != ApiSuccess){printf("*ERROR* - API failed\n"); PlxSdkErrorDisplay(rc);}// return(FALSE);}

    rc = PlxPci_DmaTransferBlock(&fpga_dev, dmaChannel, &DmaParams, (3 * 1000)); //timeout in ms

    if (rc != ApiSuccess) {
        if (rc == ApiWaitTimeout) {
            // Timed out waiting for DMA completion 
            printf("Timeout"); // return(FALSE);
        } else {
            printf("*ERROR* - API failed\n");
            PlxSdkErrorDisplay(rc); // return(FALSE);
        }
    }

    printf("OK\n");
}

void dmaClearBlock() {
    
    pVa = (void**) &pBufferMem;//pVa = A pointer to a buffer to hold the virtual address
    
    rc = PlxPci_CommonBufferUnmap(&fpga_dev, pVa);
    if (rc != ApiSuccess) {
        printf("*ERROR* - API failed\n");
        PlxSdkErrorDisplay(rc);
    }//return(bPass);}

    printf("OK");
}

void dmaClose() {
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