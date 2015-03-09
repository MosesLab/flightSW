
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
    /* 
     * Fork and exec the fsynth program to set the clock source on the SyncLink
     * to use the synthesized 20 MHz clock from the onboard frequency synthesizer
     * chip, for accurate generation of a 10 Mbps datastream. fsynth needs to be
     * in the PATH. 
     */

    int rc, fd = 0;
    int sigs, idle;
    char *devname = "/dev/ttyUSB0";
    char msg[255];


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

        sprintf(msg, "send HDLC data on %s\n", devname);
        record(msg);

        /* open serial device with O_NONBLOCK to ignore DCD input */
        fd = open(devname, O_RDWR | O_NONBLOCK, 0); // Changed from 'O_TRUNC' to 'O_RDWR | O_NONBLOCK'
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

        /*enable transmitter; should this appear before "return"?*/
        int enable = 1;
        rc = ioctl(fd, MGSL_IOCTXENABLE, enable);
    } else if (killSwitch == 1) { //Turns off synclink
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

    return fd;
}

int send_image(roeimage_t * image, xml_t * xml_buf, int fd) {

    int rc = 0;
    int i;
    int totalSize = 0;
    char *imagename = NULL; //Number to determine how much data is sent
    struct timeval time_begin, time_end;
    int time_elapsed;
    char msg[255];

    /* 
     * Write file to TM. This requires input from memory in the form of roeimage_t OR xml_t struct, 
     * then sending the data buffer to the device via a write call.
     */

    if (ts_alive) {
        //        if (roeQueue->count != 0)

        record("Sending data: \n");
        gettimeofday(&time_begin, NULL); //Determine elapsed time for file write to TM

        if (image == NULL && xml_buf != NULL) { //If we are on an odd loop send an xml file
            record("XML File...\n");
            imagename = "imageindex.xml";

            /*write xml lines*/
            rc = write(fd, xml_buf->data_buf, xml_buf->size);
            if (rc < 0) {
                sprintf(msg, "write error=%d %s\n", errno, strerror(errno));
                record(msg);
            }
            /* block until all data sent */
            totalSize += rc;
            rc = tcdrain(fd);

            if (rc < 0) return rc; //Finishes the write error handling after the break

            /*write terminating characters*/
            rc = write(fd, imagename, strlen(imagename));
            if (rc < 0) {
                sprintf(msg, "write error=%d %s\n", errno, strerror(errno));
                record(msg);
                return rc;
            }
            /*block until all data sent*/
            rc = tcdrain(fd);

        } else if (image != NULL && xml_buf == NULL) { //If we are on an even loop send an image
            record("ROE File...\n");
            imagename = image->filename;

            /*Write image frames here*/
            for (i = 1; i < 4; i++) {
                rc = write(fd, image->data[i], image->size[i] * 2);      // size corresponds to num pixels, so multiply by 2 to get bytes
                if (rc < 0) {
                    sprintf(msg, "write error=%d %s\n", errno, strerror(errno));
                    record(msg);
                    break;
                }
                /* block until all data sent */
                totalSize += rc;
                rc = tcdrain(fd);

            }
            if (rc < 0) return rc; //Finishes the write error handling after the break

            /*write terminating characters*/
            sprintf(msg, "Sending image file %s \n", imagename);
            record(msg);
            rc = write(fd, imagename + 7, strlen(imagename) - 7); //Ending characters "YYMMDDHHmmss.roe"
            if (rc < 0) {
                sprintf(msg, "write error=%d %s\n", errno, strerror(errno));
                record(msg);
                return rc;
            }
            /*block until all data sent*/
            rc = tcdrain(fd);
        }

        gettimeofday(&time_end, NULL); //Timing
        record("all data sent\n");
        sprintf(msg, "Sent %d bytes of data from file %s.\n", totalSize, imagename);
        record(msg);
        time_elapsed = 1000000 * ((long) (time_end.tv_sec) - (long) (time_begin.tv_sec))
                + (long) (time_end.tv_usec) - (long) (time_begin.tv_usec);
        sprintf(msg, "Time elapsed: %-3.2f seconds.\n", (float) time_elapsed / (float) 1000000);
        record(msg);

        return 1;

    }
    else return 2;

}
