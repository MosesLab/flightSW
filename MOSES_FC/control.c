#include "control.h"

void * controlThread(void){
    while(ts_alive){
        lockingQueue_init(hkupQueue);
        
        dequeue(hkupQueue);
    }
}