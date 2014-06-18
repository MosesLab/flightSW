/********************************************************************************
 * MOSES telemetry downlink debugging code
 *
 * Saves .ROE files to disk, rather than downlinking
 * 
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <termios.h>
#include <errno.h>
#include <sys/time.h>

#include "send_TM_sim.h"
#include "synclink.h"

#ifndef N_HDLC
#define N_HDLC 13
#endif

#ifndef BUFSIZ
#define BUFSIZ 1024
#endif

/*Function to demonstrate correct command line input*/
void display_usage_sim(void) {
    printf("Usage: sendTM <devname> \n"
            "devname = device name (optional) (e.g. /dev/ttyUSB0 etc. "
            "Default is /dev/ttyUSB0)\n");
}


int synclink_init_sim(){
    
//    /* Fork and exec the fsynth program to set the clock source on the SyncLink
//     * to use the synthesized 20 MHz clock from the onboard frequency synthesizer
//     * chip, for accurate generation of a 10 Mbps datastream. fsynth needs to be
//     * in the PATH. 
//     */
//
//    pid_t pid = fork();
//
//    if (pid == -1) {
//        perror("Fork failure");
//        exit(EXIT_FAILURE);
//    }
//
//    if (pid == 0) {
//        execlp("fsynth", "fsynth", devname, (char *) NULL); //fsynth was compiled with 20MHz
//        perror("execlp"); //selected in code
//        _exit(EXIT_FAILURE); //Child should die after exec call. If it gets
//        //here then the exec failed
//    } else if (pid > 0) {
//        wait(); //Wait for child to finish
//    }
//
//    printf("send HDLC data on %s\n", devname);
//
//    /* open serial device with O_NONBLOCK to ignore DCD input */
//    fd = open(outputpath, O_TRUNC, 0);
//    if (fd < 0) {
//        printf("open error=%d %s\n", errno, strerror(errno));
//        return fd;
//    }
//    
//    printf("Turn off RTS and DTR\n");
//    sigs = TIOCM_RTS + TIOCM_DTR;
//    rc = ioctl(fd, TIOCMBIC, &sigs);
//    if (rc < 0) {
//        printf("negate DTR/RTS error=%d %s\n", errno, strerror(errno));
//        return rc;
//    }
//
//    
//
//    return 1;
    
}

int send_image_sim(tm_queue_t roeQueue, char* imgName) {
    /* Write imagefile to TM. This requires reading a set number of bytes (1024 currently)
     * from the file into the data buffer, then sending the data buffer to the device 
     * via a write call.
     */
    
    int fd;                                     //Initialize Variables
    int rc;
    int size = 1024;
    unsigned char databuf[1024];
    unsigned char temp[1024];
    unsigned char endbuf[] = "smart";
    char *imagename;
    FILE *fp;
    int count = 0;                              //Number to determine how much data is sent
    struct timeval time_begin, time_end;
    int time_elapsed;

    char* outputpath = "/students/jackson.remington/esus/testFiles/testOutput/";
    char* outputFile = malloc(strlen(outputpath) + strlen(imgName) + 1);
    strcpy(outputFile, outputpath);
    strcat(outputFile, imgName);
    
    char* xmlfile = "/students/jackson.remington/esus/testFiles/xmlFiles/imageindex.xml";
    
    fd = open(outputFile, O_CREAT, 0);
    if (fd < 0) {
        printf("open error=%d %s\n", errno, strerror(errno));
        return fd;
    }
    
    if(ts_alive){
        if (roeQueue.count != 0){
            count = 0;
            int xmlTrigger = 1;
            if (xmlTrigger == 1) {              //If we are on an odd loop send an xml file
                imagename = xmlfile;
                xmlTrigger = 0;
            }
            else if (xmlTrigger == 0) {         //If we are on an even loop send an image
                imagename = roeQueue.first->filePath;
                xmlTrigger = 1;
            }
                
            /*Open image file for reading into a buffered stream*/
            fp = fopen(imagename, "r");
            if (fp == NULL) {
                printf("fopen(%s) error=%d %s\n", imagename, errno, strerror(errno));
                return 1;
            }
            /*Buffer the stream using the standard system bufsiz*/
            rc = setvbuf(fp, NULL, _IOFBF, BUFSIZ);
            if (rc != 0) {
                printf("setvbuf error=%d %s\n", errno, strerror(errno));
                return rc;
            }

            printf("Sending data...\n");
            gettimeofday(&time_begin, NULL); //Determine elapsed time for file write to TM
            int totalSize = 0;
            unsigned int rd = fread(databuf, 1, size, fp);
            while (rd > 0) { //RTS changed buffer reading function from fgets to fread to allow for binary data
                if (count == 10) memcpy(temp, databuf, size); //Store the contents of databuf
                //into the temp buffer
                rc = write(fd, databuf, rd);
                if (rc < 0) {
                    printf("write error=%d %s\n", errno, strerror(errno));
                    break;
                }
                /* block until all data sent */
                rc = tcdrain(fd);
                count++;
                totalSize += rd;
                rd = fread(databuf, 1, size, fp);
            }
            if (rc < 0) return rc; //Finishes the write error handling after the break
            rc = write(fd, endbuf, 5);
            if (rc < 0) {
                printf("write error=%d %s\n", errno, strerror(errno));
            }

            /*block until all data sent*/
            rc = tcdrain(fd);

            /*clear the data buffer*/
            fflush(fp);


            gettimeofday(&time_end, NULL); //Timing
            printf("all data sent\n");
            printf("Sent %d bytes of data from file %s.\n", totalSize, imagename);
            time_elapsed = 1000000 * ((long) (time_end.tv_sec) - (long) (time_begin.tv_sec))
                    + (long) (time_end.tv_usec) - (long) (time_begin.tv_usec);
            printf("Time elapsed: %-3.2f seconds.\n", (float) time_elapsed / (float) 1000000);    
        
        }
        else {
            printf("roeQueue is empty;\n");
        }
    }
    
    else printf("ts_alive = false;\n"); return 0;
    
    /* Close the device and the image file*/
    close(fd);
    fclose(fp);
    return 1;
}