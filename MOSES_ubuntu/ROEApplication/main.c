/* 
 * File:   main.c
 * Author: david
 *
 * Created on June 4, 2014, 3:15 PM
 */
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "logger.h"

/*
 * 
 */
int main(int argc, char** argv) {

    roe.active = FALSE;
    record("Program Started\n");
    start_threads();
    sleep(5);
    join_threads();

    return 0;
}

/*this method takes a function pointer and starts it as a new thread*/
void start_threads() {

    pthread_attr_init(&attrs);
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);
    pthread_create(&threads[HLP_thread], &attrs, (void * (*)(void*))SendHLPCmds, NULL);\
    pthread_create(&threads[HK_thread], &attrs, (void * (*)(void*))SendHKCmds, NULL);

}

void join_threads() {
    void * returns;
    //record("threads returned, program done\n");
    int i;
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], &returns);
    }
    record("Threads returned, program done.\n");
}

