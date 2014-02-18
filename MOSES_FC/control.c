#include "control.h"

void * controlThread(void * arg){
    
    lockingQueue_init(&hkupQueue);
    
    int fup = init_serial_connection();
    
    buildLookupTable();
    
    while(ts_alive){
	Packet p;
        readPacket(fup, &p);
        
        enqueue(&hkupQueue, &p);
        
    }
    
  
    
    
    return;
}
