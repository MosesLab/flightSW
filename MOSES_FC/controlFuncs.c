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


/*Determines how to execute HLP uplink packets*/
int hlpUplink (Packet * p){
    char* target = p->subtype;
    enum tmu funcResult = findUplink(target);
    if(funcResult == NoFunc){
        return BAD_PACKET;
    }
    else{
        (*tmuFuncs[funcResult]);
        return GOOD_PACKET;
    }
    
}

/*Uplink control functions*/
void uDataStart(){
    puts("Received data start Uplink");
}
void uDataStop(){
    puts("Received data stop Uplink");
}
void uDark1(){
    puts("Received Dark1 Uplink");
}
void uDark2(){
    puts("Received Dark2 Uplink");
}
void uDark3(){
    puts("Received Dark3 Uplink");
}
void uDark4(){
    puts("Received Dark4 Uplink");
}
void uSleep(){
    puts("Received shutdown Uplink");
}
void uWake(){
    puts("Received shutdown pending Uplink");
}
void uTest(){
    puts("Received test Uplink");
}

/*Timer control functions*/
void tDataStart(){
    puts("Received data start Timer");
}
void tDataStop(){
    puts("Received data stop Timer");
}
void tDark2(){
    puts("Received Dark2 Timer");
}
void tDark4(){
    puts("Received Dark4 Timer");
}
void tSleep(){
    puts("Received shutdown Timer");
}

/*initiates function pointers and maps them to corresponding uplink packet subtypes*/
void uplinkInit(){
    uplinkMap[UDataStart] = DATASTART;
    uplinkMap[UDataStop] = DATASTOP;
    uplinkMap[UDark1] = DARK1;
    uplinkMap[UDark2] = DARK2;
    uplinkMap[UDark3] = DARK3;
    uplinkMap[UDark4] = DARK4;
    uplinkMap[USleep] = SLEEP;
    uplinkMap[UWake] = WAKE;
    uplinkMap[UTest] = TEST;
    
    tmuFuncs[UDataStart] = uDataStart;
    tmuFuncs[UDataStop] = uDataStop;
    tmuFuncs[UDark1] = uDark1;
    tmuFuncs[UDark2] = uDark2;
    tmuFuncs[UDark3] = uDark3;
    tmuFuncs[UDark4] = uDark4;
    tmuFuncs[USleep] = uSleep;
    tmuFuncs[UWake] = uWake;
    tmuFuncs[UTest] = uTest;
    tmuFuncs[TDataStart] = tDataStart;
    tmuFuncs[TDataStop] = tDataStop;
    tmuFuncs[TDark2] = tDark2;
    tmuFuncs[TDark4] = tDark4;
    tmuFuncs[TSleep] = tSleep;
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
    
    return NoFunc;  //return -1 if no matching string is found
}


