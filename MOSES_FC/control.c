#include "control.h"

void * controlThread(void * arg){
    
    lockingQueue_init(&hkupQueue);
    
    while(ts_alive){
        
        Packet p = dequeue(&hkupQueue);
        
        if(p.valid){
                printf("%s%s%s%s%s%s%d\n",p.timeStamp, p.type, p.subtype, p.dataLength, p.data, p.checksum, p.valid);
                
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
