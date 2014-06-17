/* 
 * File:   main.c
 * Author: byrdie
 *
 * Created on January 22, 2014, 5:21 PM
 */
#include <signal.h>
#include <pthread.h>
#include "main.h"
#include "sequence.h"

/*
 * 
 */
int main(void) {
    record("Program started\n");
    //int i = 0;
    //while(1)
    //{
      //  printf("%d\n", i);
     //   i++;
      //  usleep(10);
    //}
    char msg[100];     // for writing strings
    //int i;
    
    ops.seq_pause = FALSE;
    ops.seq_run = TRUE;
    ops.channels = CH1 | CH2 | CH3;
    ops.dma_write = TRUE;
    
    /*Setup the signal handler*/
    init_signal_handler_stl();
  
    /*Load the Sequence Map*/
    char _sequence1[21]	= "sequences/dark1demo";
    char _sequence2[21]	= "sequences/dark2demo";
    char _sequence3[21]	= "sequences/datademo";
    char _sequence4[21]	= "sequences/dark3demo";
    char _sequence5[21]	= "sequences/dark4demo";
    sequenceMap[0] = constructSequence(_sequence1);
    sequenceMap[1] = constructSequence(_sequence2);
    sequenceMap[2] = constructSequence(_sequence3);
    sequenceMap[3] = constructSequence(_sequence4);
    sequenceMap[4] = constructSequence(_sequence5);
    
    start_threads();
    
    sleep(1);

    join_threads();
    
    return 0;
}

/*this method takes a function pointer and starts it as a new thread*/
void start_threads(){
    
    pthread_attr_init(&attrs);
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);
    pthread_create(&threads[EMSim_thread], &attrs, (void * (*)(void*))EMSim, NULL);
    pthread_create(&threads[STL_thread], &attrs, (void * (*)(void*))ScienceTimeline, NULL);

}

/*join_threads waits for the threads to return, then exits the program*/
void join_threads(){
    void * returns;
    int m;
    for(m = 0; m < NUM_THREADS; m++){
        pthread_join(threads[m], &returns);
    }
    record("Threads returned, program done.\n");
}



