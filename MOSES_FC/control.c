#include "control.h"

void * controlThread(void * arg){
    
    lockingQueue_init(&hkupQueue);
    
    int fup = init_hkup_serial_connection();
    
    buildLookupTable();
    
    while(ts_alive){
        int status = GOOD_PACKET;
	Packet new_packet;
        Packet * p = &new_packet;
        readPacket(fup, p);
        if(!p->valid){
                status = BAD_PACKET;
        }
        else{
            switch(p->type[0]){
                case SHELL:
                    printf("Shell packet\n");
                    break;
                case MDAQ_RQS:
                    printf("DAQ packet\n");
                    break;
                case UPLINK:
                    printf("Uplink packet\n");
                    break;
                case PWR:
                    printf("Power packet\n");
                    break;
                case HK_RQS:
                    printf("HK Request Packet\n");
                    break;
                default:
                    printf("Bad Packet");
                    status = BAD_PACKET;
                    break;
            }
        }
        if(ts_alive) enqueue(&hkupQueue, p);
        
    }
    /*need to clean up properly but these don't work*/
    //close(fup);  
    //lockingQueue_destroy(&hkupQueue);
    
    return 0;
    
}
