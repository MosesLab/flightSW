/****************************************************************
*Author: Roy Smart						*
*1/21/14							*
*MOSES LAB							*
*								*
*hkup connects to a serial device and reads raw data from the	*
*serial device. hkup then strips the start byte, stop byte and	*
*parity bits from the data and encloses the data in the packet	*
*structure to add to the packet queue connected to control.c	*
****************************************************************/

#include "hkdown.h"

void * hkupThread(void * arg){
    fdown = init_hkdown_serial_connection();
    while(ts_alive){
        
        Packet p = dequeue(&hkupQueue);
        
        if(p.valid){
                printf("%s%s%s%s%s%s%d\n",p.timeStamp, p.type, p.subtype, p.dataLength, p.data, p.checksum, p.valid);
                char packetString[15 + p.dataSize];
                int size = asprintf(&packetString,"%c%s%s%s%s%s%s%c\n", STARTBIT, p.timeStamp, p.type, p.subtype, p.dataLength, p.data, p.checksum, ENDBIT);
                write(fdown, packetString, size);
            //printf("%c%c\n", p.type, p.checksum);
        }
        else{
            printf("Bad Packet\n");
        }
        printf("\n");
    }
    lockingQueue_destroy(&hkupQueue);
    
    return;
}


