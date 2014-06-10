/**************************************************
* Author: David Keltgen                           *
* Company: Montana State University: MOSES LAB    *
* File Name: HLPDriver.c                          *
* Date: 5 June 2014                               *
* Description: Function calls to test the         *
 *             interface between the flight       *
 *             computer and the ROE.              *
**************************************************/

#include "HLPDriver.h"

int SendHLPCmds()
{
    char* foo = "foo";
    record("HLP thread started\n");
    
    activateROEReal();
    usleep(10000);
    deactivateReal();
    usleep(10000);
    
    exitDefaultReal();
    usleep(10000);
    
    selftestModeReal();
    
    usleep(10000);
    
    stimOnReal();
    
    usleep(10000);
    
    stimOffReal();
    
    usleep(10000);
    
    resetReal();
    
    usleep(10000);
    
    readOutReal(foo, 5);
    
    usleep(10000);
    
    flushReal();
    
    usleep(10000);
    
    getHKReal(foo);
    
    usleep(10000);
    
    getAEReal();
    
    usleep(10000);
    
    setAEReal(foo);
    
    usleep(10000);
    
    manualWriteReal(foo, 5);
    
    usleep(10000);
    
    record("Done driving HLP commands\n");
    return 0;
}