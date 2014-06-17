/* 
 * File:   DummyEM.c
 * Author: David Keltgen
 *
 * Created on May 28, 2014, 11:50 AM
 * 
 * Will be used for demonstrating the ability to send signals to the science timeline thread.
 * 
 */
//#include "main.h"
#include "system.h"


int EMSim(void)
{
    
    record("Inside EMSim!\n");
    
    sleep(1000);
    record("sending SIGUSR1\n");
    //pthread_kill(threads[STL_thread],SIGUSR1);
    record("After thread kill\n");
    return 0;
}
