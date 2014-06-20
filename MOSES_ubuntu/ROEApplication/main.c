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

   /* if ((roe.roeLink = open(ROE_DEV, O_WRONLY | O_NOCTTY) == -1)) {

        record("Failed to Open Serial Port to ROE\n");
        //pthread_mutex_unlock(&roe.mx);
        //return 0;
    }

    struct termios newtio, oldtio;
    tcgetattr(roe.roeLink, &oldtio);
    bzero(&newtio, sizeof (newtio));

    newtio.c_cflag |= UPBAUD | CS8 | CSTOPB | HUPCL | CREAD | CLOCAL;
    newtio.c_cflag &= ~(PARENB | PARODD);
    newtio.c_iflag &= ~(IGNBRK | BRKINT | IGNPAR | PARMRK | INPCK | INLCR | IGNCR | ICRNL | IXON | IXOFF | IUCLC | IXANY | IMAXBEL);
    //options.c_iflag |= ISTRIP;
    newtio.c_oflag &= ~OPOST;
    newtio.c_lflag &= ~(ISIG | ICANON | XCASE | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE | IEXTEN);
    //newtio.c_lflag = ICANON;

    /*set non-canonical attributes*//*
    newtio.c_cc[VTIME] = 1;
    newtio.c_cc[VMIN] = 255;

    tcflush(roe.roeLink, TCIFLUSH);
    tcsetattr(roe.roeLink, TCSANOW, &newtio); //Apply new settings
    //record("Connection established. DEFAULT MODE\n");

    int blockSize = 10;
    int block1[] = {65,65,65,65,65,65,65,65,65,65};
    int counter = 0;
    while (counter < 10) {
        //if (roeCustomRead) {
        //if (status != -1) {
        int i;
        for (i = 0; i < blockSize; i++) {
            write(roe.roeLink, &block1[i], sizeof (block1[i]));
        }
        counter ++;
    }*/

    //roe.active = FALSE;
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

