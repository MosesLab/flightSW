/*
 * controlFuncs.c
 * 
 * Author: Roy Smart
 * Created: May 15, 2014
 * 
 * contains the functions that direct the flight computer what to do in the 
 * event of user input, e.g. timers, uplinks and hlp packets.
 */
#include "controlFuncs.h"


void uplinkInit(){
    uplinkMap[] = {DATASTART, DATASTOP, DARK1, DARK2, DARK3, DARK4, SLEEP, WAKE, TEST};
}

/*matches a packet subtype with the corresponding index of tmufuncs pointer*/
enum tmu findUplink(char * target){
    int size = sizeof(uplinkMap) / sizeof(uplinkMap[0]);     
    
    /*loop to find matching string*/
    int located = FALSE;
    int i = 0;
    while(!located && i < size){
        if(strcmp(uplinkMap[i], target) == 0){
            return i;
        }
        else{
            i++;
        }
    }
    
    return -1;
}

int hlpUplink (Packet * p){
   
}
