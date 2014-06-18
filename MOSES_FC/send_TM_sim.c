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

#include "send_TM.h"
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


int send_images_sim(int argc, char ** argv, tm_queue_t roeQueue){
    
    int fd;                                     //Initialize Variables
    int rc;
    int sigs, idle;
    int i;
    int ldisc = N_HDLC;
    int imageAmount = roeQueue.count;
    int killTrigger = 0;
    MGSL_PARAMS params;
    int size = 1024;
    unsigned char databuf[1024];
    unsigned char temp[1024];
    unsigned char endbuf[] = "smart";           //Used this string as end-frame to terminate seperate files
    char *devname;
    char *imagename;
    FILE *fp;
    int count = 0;                              //Number to determine how much data is sent
    struct timeval time_begin, time_end;
    int time_elapsed;
    
    char* outputpath = "/students/jackson.remington/esus/testFiles/testOutput/";
    char* imagepath = "/students/jackson.remington/esus/testFiles/imageFiles/";
    char* xmlfile = "/students/jackson.remington/esus/testFiles/xmlFiles/imageindex.xml";
    
    /*Check for correct arguments*/
    if (argc > 2 || argc < 1) {
        printf("Incorrect number of arguments\n");
        display_usage_sim();
        return 1;
    }

    /*Set device name, either from command line or use default value*/
    if (argc == 3)
        devname = argv[1];
    else
        devname = "/dev/ttyUSB0"; //Set the default name of the SyncLink device
    
    /* Fork and exec the fsynth program to set the clock source on the SyncLink
     * to use the synthesized 20 MHz clock from the onboard frequency synthesizer
     * chip, for accurate generation of a 10 Mbps datastream. fsynth needs to be
     * in the PATH. 
     */

    pid_t pid = fork();

    if (pid == -1) {
        perror("Fork failure");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        execlp("fsynth", "fsynth", devname, (char *) NULL); //fsynth was compiled with 20MHz
        perror("execlp"); //selected in code
        _exit(EXIT_FAILURE); //Child should die after exec call. If it gets
        //here then the exec failed
    } else if (pid > 0) {
        wait(); //Wait for child to finish
    }

    printf("send HDLC data on %s\n", devname);

    /* open serial device with O_NONBLOCK to ignore DCD input */
    fd = open(outputpath, O_TRUNC, 0);
    if (fd < 0) {
        printf("open error=%d %s\n", errno, strerror(errno));
        return fd;
    }

    /*
     * set N_HDLC line discipline
     *
     * A line discipline is a software layer between a tty device driver
     * and user application that performs intermediate processing,
     * formatting, and buffering of data.
     */
//    rc = ioctl(fd, TIOCSETD, &ldisc);
//    if (rc < 0) {
//        printf("set line discipline error=%d %s\n",
//                errno, strerror(errno));
//        return rc;
//    }
//
//    /* get current device parameters */
//    rc = ioctl(fd, MGSL_IOCGPARAMS, &params);
//    if (rc < 0) {
//        printf("ioctl(MGSL_IOCGPARAMS) error=%d %s\n",
//                errno, strerror(errno));
//        return rc;
//    }

    /*
     * modify device parameters
     *
     * HDLC/SDLC mode, loopback disabled (external loopback connector), NRZIs encoding
     * Data transmit clock sourced from BRG
     * Output 10000000bps clock on auxclk output
     * No hardware CRC
     */

    params.mode = MGSL_MODE_HDLC;
    params.loopback = 0;
    params.flags = HDLC_FLAG_RXC_RXCPIN + HDLC_FLAG_TXC_BRG;
    params.encoding = HDLC_ENCODING_NRZ;
    params.clock_speed = 10000000;
    params.crc_type = HDLC_CRC_16_CCITT;
    params.preamble = HDLC_PREAMBLE_PATTERN_ONES;
    params.preamble_length = HDLC_PREAMBLE_LENGTH_16BITS;

//    /* set current device parameters */
//    rc = ioctl(fd, MGSL_IOCSPARAMS, &params);
//    if (rc < 0) {
//        printf("ioctl(MGSL_IOCSPARAMS) error=%d %s\n",
//                errno, strerror(errno));
//        return rc;
//    }
//
//    /* set transmit idle pattern (sent between frames) */
//    idle = HDLC_TXIDLE_ALT_ZEROS_ONES;
//    rc = ioctl(fd, MGSL_IOCSTXIDLE, idle);
//    if (rc < 0) {
//        printf("ioctl(MGSL_IOCSTXIDLE) error=%d %s\n",
//                errno, strerror(errno));
//        return rc;
//    }
//
//
//
//    /* set device to blocking mode for reads and writes */
//    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
//
//    printf("Turn on RTS and DTR serial outputs\n");
//    sigs = TIOCM_RTS + TIOCM_DTR;
//    rc = ioctl(fd, TIOCMBIS, &sigs);
//    if (rc < 0) {
//        printf("assert DTR/RTS error=%d %s\n",
//                errno, strerror(errno));
//        return rc;
//    }
//
//    /*enable transmitter*/
//    int enable = 1;
//    rc = ioctl(fd, MGSL_IOCTXENABLE, enable);

    /* Write imagefile to TM. This requires reading a set number of bytes (1024 currently)
     * from the file into the data buffer, then sending the data buffer to the device 
     * via a write call.
     */
    while(ts_alive){
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
                break;
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
            
            if(killTrigger == 1){               //If everything is done
                break;                          
            }
            
            sleep(2);
            
        }
    }
    
    printf("Turn off RTS and DTR\n");
    sigs = TIOCM_RTS + TIOCM_DTR;
    rc = ioctl(fd, TIOCMBIC, &sigs);
    if (rc < 0) {
        printf("negate DTR/RTS error=%d %s\n", errno, strerror(errno));
        return rc;
    }

    /* Close the device and the image file*/
    close(fd);
    fclose(fp);

    return 0;
    
}
