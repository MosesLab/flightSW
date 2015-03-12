
/********************************************************************************
 * MOSES telemetry downlink test code
 *
 * Author: Jackson Remington, Roy Smart, Jake Plavonic
 * History:
 *  Created Dec 17 2013
 *  Tested  Dec 18 2013 
 *  Tested  May 9 2014 successfully at White Sands Missile Range (See results below)
 *  Tested  March 10 2015 successfully and integrated into flightSW
 *
 * Uses the Microgate USB SyncLink adapter to achieve synchronous serial speeds
 * of 10 Mbps to downlink MOSES science data. The data are contained in 12 MB files.
 *
 * This program will configure the SyncLink device for use as a linux serial node
 * with the appropriate framing, bit rate, desired error checking, idle pattern,
 * and preamble. It will then send out the 12 MB .roe file. This program was 
 * developed as stand-along before being implemented into the main flight software
 * program.
 *
 * Required files:
 *  synclink.h
 *
 * Compiling for the TS7600 ARM9 flight computer requires the appropriate version
 * of gcc, which is native to the FC's Debian distribution or included in the cross-
 * compiling toolchain available from EmbeddedArm.
 *
 * Compiling sequence:
 *  Cross Compiling:
 *    (CC Toolchain Directory)/arm-fsl-linux-gnueabi-gcc -mtune=arm9 -ggdb3 sendTM.c
 *      -o sendTM
 *
 *  Native Compiling:
 *    gcc -mtune=arm9 -ggdb3 sendTM.c -o sendTM
 *
 * Test Results:
 *      ./sendtm 
 *       fsynth device=/dev/ttyUSB0
 *       USB device detected
 *       Found programming information for output frequency = 20000000Hz
 *       send HDLC data on /dev/ttyUSB0
 *       Turn on RTS and DTR serial outputs
 *       Sending data...
 *       all data sent
 *       Sent 16777216 bytes of data from file 36image.bin.
 *       Time elapsed: 13.58 seconds.
 *       Sending data...
 *       all data sent
 *       Sent 28672 bytes of data from file /home/ts-7600-linux/roysmart/images/imageindex.xml.
 *       Time elapsed: 0.02 seconds.
 *
 *
 *  The 13.51 seconds is consistent with a 10 Mbps data rate for 16 MB of data. It
 *  looks like there is very little time used to get the data through the USB. The
 *  16777216 bytes sent is the same size as the file reported by Debian. 
 * 
 * In previous implementations of this code, the buffer holding the images was read using the 
 * function fgets(). This function was incorrect as it didn't treat chars the same as all other bytes.
 * Testing revealed that the code was correctly sending chars, but other bytes were being replaced 
 * by random data. This problem was rectified by using the function fread() to parse the buffer in a 
 * binary fashion.
 * 
 *
 ******************************************************************************/

#include "send_TM.h"
#include "synclink.h"

#ifndef N_HDLC
#define N_HDLC 13
#endif

#ifndef BUFSIZ
#define BUFSIZ 4096
#endif

/*Function to demonstrate correct command line input*/
void display_usage(void) {
    record("Usage: sendTM <devname> \n"
            "devname = device name (optional) (e.g. /dev/ttyUSB0 etc. "
            "Default is /dev/ttyUSB0)\n");
}

/* Function to (de)initialize synclink before send files */
int synclink_init(int killSwitch) {
    int rc, fd = 0;
    int sigs, idle;
    char *devname = "/dev/ttyUSB0";
    char msg[255];

    if (killSwitch == 1) { 
        /* Then we are shutting down the synclink */
        record("synclink killSwitch: Turn off RTS and DTR\n");
        sigs = TIOCM_RTS + TIOCM_DTR;
        rc = ioctl(fd, TIOCMBIC, &sigs);
        if (rc < 0) {
            sprintf(msg, "negate DTR/RTS error=%d %s\n", errno, strerror(errno));
            record(msg);
            return rc;
        }

        /* Close the device*/
        close(fd);
    }
    else if (killSwitch == 0) {
        /* Then we are preparing the synclink */
        int ldisc = N_HDLC, child_status;
        MGSL_PARAMS params;

        /* 
         * Fork and exec the fsynth program to set the clock source on the SyncLink
         * to use the synthesized 20 MHz clock from the onboard frequency synthesizer
         * chip, for accurate generation of a 10 Mbps datastream. fsynth needs to be
         * in the PATH. 
         */
        pid_t pid = fork();
        if (pid == -1) { //Check Fork
            perror("Fork failure");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            /* Child should die after exec call. If it gets here then the exec failed*/
            /* fsynth was compiled with 20MHz */
            execlp("fsynth", "fsynth", devname, (char *) NULL);
            perror("execlp");
            _exit(EXIT_FAILURE); 
        } else if (pid > 0) {
            wait(&child_status);
        }

        sprintf(msg, "send HDLC data on %s\n", devname);
        record(msg);

        /* open serial device with O_NONBLOCK to ignore DCD input */
        fd = open(devname, O_RDWR | O_NONBLOCK, 0);
        if (fd < 0) {
            sprintf(msg, "open error=%d %s\n", errno, strerror(errno));
            record(msg);
            return fd;
        } else {
            sprintf(msg, "device opened on %s\n", devname);
            record(msg);
        }

        /*
         * set N_HDLC line discipline
         *
         * A line discipline is a software layer between a tty device driver
         * and user application that performs intermediate processing,
         * formatting, and buffering of data.
         */
        rc = ioctl(fd, TIOCSETD, &ldisc);
        if (rc < 0) {
            sprintf(msg, "set line discipline error=%d %s\n",
                    errno, strerror(errno));
            record(msg);
            return rc;
        }

        /* get current device parameters */
        rc = ioctl(fd, MGSL_IOCGPARAMS, &params);
        if (rc < 0) {
            sprintf(msg, "ioctl(MGSL_IOCGPARAMS) error=%d %s\n",
                    errno, strerror(errno));
            record(msg);
            return rc;
        }

        /*
         * modify device parameters
         *
         * HDLC/SDLC mode, loopback disabled (external loopback connector), NRZIs encoding
         * Data transmit clock sourced from BRG
         * Output 10000000bps clock on auxclk output
         * No hardware CRC
         */
        params.mode            = MGSL_MODE_HDLC;
        params.loopback        = 0;
        params.flags           = HDLC_FLAG_RXC_RXCPIN + HDLC_FLAG_TXC_BRG;
        params.encoding        = HDLC_ENCODING_NRZ;
        params.clock_speed     = 10000000;
        params.crc_type        = HDLC_CRC_16_CCITT;
        params.preamble        = HDLC_PREAMBLE_PATTERN_ONES;
        params.preamble_length = HDLC_PREAMBLE_LENGTH_16BITS;

        /* set current device parameters */
        rc = ioctl(fd, MGSL_IOCSPARAMS, &params);
        if (rc < 0) {
            sprintf(msg, "ioctl(MGSL_IOCSPARAMS) error=%d %s\n",
                    errno, strerror(errno));
            record(msg);
            return rc;
        }

        /* set transmit idle pattern to all flags between frames ("idle = HDLC_TXIDLE_FLAGS;") */
        idle = HDLC_TXIDLE_FLAGS;
        rc = ioctl(fd, MGSL_IOCSTXIDLE, idle);
        if (rc < 0) {
            sprintf(msg, "ioctl(MGSL_IOCSTXIDLE) error=%d %s\n",
                    errno, strerror(errno));
            record(msg);
            return rc;
        }

        /* set device to blocking mode for reads and writes */
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
        
        record("Turn on RTS and DTR serial outputs\n");
        sigs = TIOCM_RTS + TIOCM_DTR;
        rc = ioctl(fd, TIOCMBIS, &sigs);
        if (rc < 0) {
            sprintf(msg, "assert DTR/RTS error=%d %s\n",
                    errno, strerror(errno));
            record(msg);
            return rc;
        }

        /*enable transmitter*/
        int enable = 1;
        rc = ioctl(fd, MGSL_IOCTXENABLE, enable);
    } 

    /* exit with synclink's file pointer */
    return fd;
}

/* Program to send passed images through synclink */
int send_image(roeimage_t * image, int fd) {
    /* 
     * Write file to TM. This requires input from memory in the form of roeimage_t struct, 
     * then sending the xml and image buffers to the device via a write call.
     */
    int rc = 0;
    int i;
    int totalSize = 0;
    char *imagename = NULL;
    struct timeval time_begin, time_end;
    int time_elapsed;
    char msg[255];

    /* Main loop - enter and exit with each image */
    if (ts_alive) {

        record("Sending data: \n");
        
        /* start sending imageindex.xml snippets */
        record("Sending XML File...\n");
        totalSize = 0;
        imagename = "imageindex.xml";
        
        /*Determine elapsed time for xml file write to TM*/
        gettimeofday(&time_begin, NULL); 

        /*write xml snippets*/
        for (i = 0; i < (image->xml_cur_index + 1); i++) {
            rc = write(fd, xml_snips[i], xml_snips_sz[i]);
            if (rc < 0) {
                sprintf(msg, "write error=%d %s\n", errno, strerror(errno));
                record(msg);
                break;
            }
            else totalSize += rc;
            
            /* block until all data sent */
            rc = tcdrain(fd);
        }

        /*Finishes the write error handling after the break*/
        if (rc < 0) return rc;
        
        /*write terminating characters*/
        rc = write(fd, imagename, strlen(imagename));
        if (rc < 0) {
            sprintf(msg, "write error=%d %s\n", errno, strerror(errno));
            record(msg);
            return rc;
        }
        
        /*block until all data sent*/
        rc = tcdrain(fd);

        /*report time taken for XML file*/
        gettimeofday(&time_end, NULL);
        record("all data sent\n");
        sprintf(msg, "Sent %d bytes of data from file %s.\n", totalSize, imagename);
        record(msg);
        time_elapsed = 1000000 * ((long) (time_end.tv_sec) - (long) (time_begin.tv_sec))
                + (long) (time_end.tv_usec) - (long) (time_begin.tv_usec);
        sprintf(msg, "Time elapsed: %-3.2f seconds.\n", (float) time_elapsed / (float) 1000000);
        record(msg);

        /*prepare to send science data*/
        record("ROE File...\n");
        totalSize = 0;
        imagename = image->filename;
        gettimeofday(&time_begin, NULL);
        
        /*Write image frames here*/
        for (i = 1; i < 4; i++) {
            /* size corresponds to num pixels, so multiply by 2 to get bytes */
            rc = write(fd, image->data[i], image->size[i] * 2);
            if (rc < 0) {
                sprintf(msg, "write error=%d %s\n", errno, strerror(errno));
                record(msg);
                break;
            }
            else totalSize += rc;
            
            /* block until all data sent */
            rc = tcdrain(fd);
        }
        
        /*Finishes the write error handling after the break*/
        if (rc < 0) return rc; 

        /*write terminating characters after removing dir path*/
        rc = write(fd, imagename + 7, strlen(imagename) - 7);
        if (rc < 0) {
            sprintf(msg, "write error=%d %s\n", errno, strerror(errno));
            record(msg);
            return rc;
        }
        
        /*block until all data sent*/
        rc = tcdrain(fd);

        gettimeofday(&time_end, NULL);
        record("all data sent\n");
        sprintf(msg, "Sent %d bytes of data from file %s.\n", totalSize, imagename);
        record(msg);
        time_elapsed = 1000000 * ((long) (time_end.tv_sec) - (long) (time_begin.tv_sec))
                + (long) (time_end.tv_usec) - (long) (time_begin.tv_usec);
        sprintf(msg, "Time elapsed: %-3.2f seconds.\n", (float) time_elapsed / (float) 1000000);
        record(msg);
    }
    
    /* return success marker */
    return 1;
}