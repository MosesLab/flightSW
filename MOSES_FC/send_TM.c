
/********************************************************************************
 * MOSES telemetry downlink test code
 *
 * Author: Jacob Plovanic, Roy Smart
 * History:
 *  Created Dec 17 2013
 *  Tested  Dec 18 2013 
 *  Tested May 9 2014 successfully at White Sands Missile Range (See results below)
 *
 * Uses the Microgate USB SyncLink adapter to achieve synchronous serial speeds
 * of 10 Mbps to downlink MOSES science data. The data are contained in 16 MB 
 * (possibly 12 MB) files.
 *
 * This program will configure the SyncLink device for use as a linux serial node
 * with the appropriate framing, bitrate, desired error checking, idle pattern,
 * and preamble. It will then send out the 16 MB .roe file. As a first step, we 
 * will use this program to test the timing and capabilities of sending entire
 * MOSES images through the telemetry, since there might be delays associated with
 * the USB transfer. We can then fold this program into the main flight software
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

int synclink_init(int killSwitch) {
    /* Fork and exec the fsynth program to set the clock source on the SyncLink
     * to use the synthesized 20 MHz clock from the onboard frequency synthesizer
     * chip, for accurate generation of a 10 Mbps datastream. fsynth needs to be
     * in the PATH. 
     */

    int rc, fd = NULL;
    int sigs, idle;
    char *devname = "/dev/ttyUSB0";
    char *msg = NULL;
    

    if (killSwitch == 0) {

        
        int ldisc = N_HDLC, child_status;
        MGSL_PARAMS params;

        pid_t pid = fork();
        if (pid == -1) { //Check Fork
            perror("Fork failure");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            execlp("fsynth", "fsynth", devname, (char *) NULL); //fsynth was compiled with 20MHz
            perror("execlp"); //selected in code
            _exit(EXIT_FAILURE); //Child should die after exec call. If it gets
            //here then the exec failed
        } else if (pid > 0) {
            wait(&child_status); //Wait for child to finish
        }

        sprintf(msg,"send HDLC data on %s\n", devname);
        record(msg);

        /* open serial device with O_NONBLOCK to ignore DCD input */
        fd = open(devname, O_TRUNC, 0);                 // Why are we opening like this instead of "open(devname, O_RDWR | O_NONBLOCK, 0)"?
        if (fd < 0) {
            sprintf(msg,"open error=%d %s\n", errno, strerror(errno));
            record(msg);
            return fd;
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
            sprintf(msg,"set line discipline error=%d %s\n",
                    errno, strerror(errno));
            record(msg);
            return rc;
        }

        /* get current device parameters */
        rc = ioctl(fd, MGSL_IOCGPARAMS, &params);
        if (rc < 0) {
            sprintf(msg,"ioctl(MGSL_IOCGPARAMS) error=%d %s\n",
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

        params.mode = MGSL_MODE_HDLC;
        params.loopback = 0;
        params.flags = HDLC_FLAG_RXC_RXCPIN + HDLC_FLAG_TXC_BRG;
        params.encoding = HDLC_ENCODING_NRZ;
        params.clock_speed = 10000000;
        params.crc_type = HDLC_CRC_16_CCITT;
        params.preamble = HDLC_PREAMBLE_PATTERN_ONES;
        params.preamble_length = HDLC_PREAMBLE_LENGTH_16BITS;

        /* set current device parameters */
        rc = ioctl(fd, MGSL_IOCSPARAMS, &params);
        if (rc < 0) {
            sprintf(msg,"ioctl(MGSL_IOCSPARAMS) error=%d %s\n",
                    errno, strerror(errno));
            record(msg);
            return rc;
        }

        /* set transmit idle pattern (sent between frames) */
        idle = HDLC_TXIDLE_ALT_ZEROS_ONES;
        rc = ioctl(fd, MGSL_IOCSTXIDLE, idle);
        if (rc < 0) {
            sprintf(msg,"ioctl(MGSL_IOCSTXIDLE) error=%d %s\n",
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
            sprintf(msg,"assert DTR/RTS error=%d %s\n",
                    errno, strerror(errno));
            record(msg);
            return rc;
        }

        /*enable transmitter*/
        int enable = 1;
        rc = ioctl(fd, MGSL_IOCTXENABLE, enable);
    }
    else if (killSwitch == 1) { //Turns off synclink
        record("synclink killSwitch: Turn off RTS and DTR\n");
        sigs = TIOCM_RTS + TIOCM_DTR;
        rc = ioctl(fd, TIOCMBIC, &sigs);
        if (rc < 0) {
            sprintf(msg,"negate DTR/RTS error=%d %s\n", errno, strerror(errno));
            record(msg);
            return rc;
        }
    
        /* Close the device*/
        close(fd);
    
    }

    return fd;
}

int send_image(imgPtr_t * image, int xmlTrigger, int fd) {

    int rc;
//    int killTrigger = 0;
    int size = BUFSIZ;
    unsigned char databuf[BUFSIZ];
    unsigned char temp[BUFSIZ];
    unsigned char endbuf[] = "smart"; //Used this string as end-frame to terminate seperate files

    char *imagename = NULL;
    FILE *fp = NULL;
    int frame_count = 0; //Number to determine how much data is sent
    struct timeval time_begin, time_end;
    int time_elapsed;
    char *msg = NULL;

    char* xmlfile = "/mdata/imageindex.xml";

    /* Write imagefile to TM. This requires reading a set number of bytes (1024 currently)
     * from the file into the data buffer, then sending the data buffer to the device 
     * via a write call.
     */
    if (ts_alive) {
//        if (roeQueue->count != 0) {
            frame_count = 0;
            if (xmlTrigger == 1) { //If we are on an odd loop send an xml file
                imagename = xmlfile;
            } else if (xmlTrigger == 0) { //If we are on an even loop send an image
                imagename = image->filePath;
            }

            /*Open image file for reading into a buffered stream*/
            fp = fopen(imagename, "r");
            if (fp == NULL) {
                sprintf(msg,"fopen(%s) error=%d %s\n", imagename, errno, strerror(errno));
                record(msg);
                return 1;
            }
            /*Buffer the stream using the standard system bufsiz*/
            rc = setvbuf(fp, NULL, _IOFBF, BUFSIZ);
            if (rc != 0) {
                sprintf(msg,"setvbuf error=%d %s\n", errno, strerror(errno));
                record(msg);
                return rc;
            }

            record("Sending data...\n");
            gettimeofday(&time_begin, NULL); //Determine elapsed time for file write to TM
            int totalSize = 0;
            unsigned int rd = fread(databuf, 1, size, fp);
            while (rd > 0) { //RTS changed buffer reading function from fgets to fread to allow for binary data
                if (frame_count == 10) memcpy(temp, databuf, size); //Store the contents of databuf
                //into the temp buffer
                rc = write(fd, databuf, rd);
                if (rc < 0) {
                    sprintf(msg,"write error=%d %s\n", errno, strerror(errno));
                    record(msg);
                    break;
                }
                /* block until all data sent */
                rc = tcdrain(fd);
                frame_count++;
                totalSize += rd;
                rd = fread(databuf, 1, size, fp);
            }
            if (rc < 0) return rc; //Finishes the write error handling after the break
            rc = write(fd, endbuf, 5);
            if (rc < 0) {
                sprintf(msg,"write error=%d %s\n", errno, strerror(errno));
                record(msg);
            }

            /*block until all data sent*/
            rc = tcdrain(fd);

            /*clear the data buffer and close image file*/
            fflush(fp);
            fclose(fp);


            gettimeofday(&time_end, NULL); //Timing
            record("all data sent\n");
            sprintf(msg,"Sent %d bytes of data from file %s.\n", totalSize, imagename);
            record(msg);
            time_elapsed = 1000000 * ((long) (time_end.tv_sec) - (long) (time_begin.tv_sec))
                    + (long) (time_end.tv_usec) - (long) (time_begin.tv_usec);
            sprintf(msg,"Time elapsed: %-3.2f seconds.\n", (float) time_elapsed / (float) 1000000);
            record(msg);
            
            return 1;

    }

    /* We shouldn't need to turn off the synclink */
//    record("Turn off RTS and DTR\n");
//    int sigs = TIOCM_RTS + TIOCM_DTR;
//    rc = ioctl(fd, TIOCMBIC, &sigs);
//    if (rc < 0) {
//        record("negate DTR/RTS error=%d %s\n", errno, strerror(errno));
//        return rc;
//    }
//
//    /* Close the device and the image file*/
//    close(fd);
//    fclose(fp);
    
    return 2;

}
