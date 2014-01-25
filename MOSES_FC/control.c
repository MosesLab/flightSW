#include "control.h"

void controlThread(){
    while(ts_alive){
        lockingQueue_init(hkupQueue);
        
        dequeue(hkupQueue);
    }
}