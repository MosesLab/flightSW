#include "control.h"

void * controlThread(void * arg){
    while(ts_alive){
        lockingQueue_init(hkupQueue);
        
        dequeue(hkupQueue);
    }
}
