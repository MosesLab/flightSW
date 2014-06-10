/* 
 * File:   main.c
 * Author: david
 *
 * Created on June 4, 2014, 3:15 PM
 */
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
    int block1[] = {0x46, 0x08, 0x00, 0x09, 0x12, 0x18, 0x25, 0x28,
        0x30, 0xF8, 0x34, 0xF8, 0x34, 0xF8, 0xF8, 0x80,
        0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xC0, 0xF8, 0x94,
        0x34, 0xF8, 0x30, 0xF8, 0xF8, 0x34, 0xF8, 0x30,
        0xF8, 0x34, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8,
        0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0x30, 0xF8,
        0xF8, 0xF8, 0x80, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8,
        0xC0, 0x34, 0xFA, 0x30, 0xFC, 0xFC, 0xFC, 0xFC,
        0xFC, 0xFC, 0xFC};

    printf("Size of block: %d\n", sizeof (block1));
    printf("Size of block[0]: %d  %d\n", sizeof (block1[0]), block1[0]);
    printf("size of char: %d \n", sizeof (char));
    
    roe.active = FALSE;
    record("Program Started\n");
    start_threads();

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

