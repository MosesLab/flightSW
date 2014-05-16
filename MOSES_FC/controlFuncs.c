/*
 * controlFuncs.c
 * 
 * Author: Roy Smart
 * Created: May 15, 2014
 * 
 * contains the functions that direct the flight computer what to do in the 
 * event of telemetry input, e.g. timers, uplinks and hlp packets.
 */
#include "controlFuncs.h"

/*initiates function pointers and maps them to corresponding uplink packet subtypes*/
void uplinkInit(){
    
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
    
    return -1;  //return -1 if no matching string is found
}

/*Determines how to execute HLP uplink packets*/
int hlpUplink (Packet * p){
   
}
