#include "control.h"

void * controlThread(void * arg){
    while(ts_alive){
        lockingQueue_init(hkupQueue);
        
        Packet p = dequeue(hkupQueue);
        if(p.valid){
                printf("%s%s%s%s%s%s\n",p.timeStamp, p.type, p.subtype, p.dataLength, p.data, p.checksum);
        }
        else{
            printf("Bad Packet\n");
        }
    }
}
