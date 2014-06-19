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
    //char* foo = "foo";
    record("HLP thread started.\n");
    
    sleep(3);
    
    activateROE(); usleep(100000);
    
    record("Sending command to exit default mode.\n");
    exitDefault(); usleep(100000);
    
    
    //resetReal();       usleep(10000);
    
    //deactivate();  usleep(10000);
    
    //selftestMode();
    //usleep(10000);
    //stimOn();
    //usleep(10000);
    //stimOff();
    //usleep(10000);
    //reset();
    //usleep(10000);
    //readOut(foo, 5);
    //usleep(10000);
    //flush();
    //usleep(10000);
    //getHK(foo); usleep(10000);
    
    //getAE();
    
    //usleep(10000);
    
    //setAE(foo);
    
    //usleep(10000);
    
    //manualWrite(foo, 5);
    
    usleep(10000);
    
    record("Done driving HLP commands\n");
    return 0;
}