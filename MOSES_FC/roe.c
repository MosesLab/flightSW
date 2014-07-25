/**************************************************
 * Author: David Keltgen                           *
 * Company: Montana State University: MOSES LAB    *
 * File Name: roe.c              *
 * Date:  June 4 2014                              *
 * Description: Software representation of the     *
 *              Read Out Electronics(ROE).         *
 **************************************************/
#include "roe.h"

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

int activate() {
    //char msg[100];
    pthread_mutex_lock(&roe_struct.mx);
    if (roe_struct.active == FALSE) {
        //Open Serial Device
        struct termios oldtio_up, newtio_up;
        int fd = open(ROE_DEV, O_RDWR); // | O_NOCTTY);
        if (fd < 0) {
            printf("%s\n", strerror(errno));
            record("Couldnt connect\n");
            exit(-1);
        }
        else
        {
        /*save current serial port settings*/
        tcgetattr(fd, &oldtio_up);

        /*clear struct for new port settings*/
        bzero(&newtio_up, sizeof (newtio_up));

        /*set flags for non-canonical serial connection*/
        newtio_up.c_cflag |= DOWNBAUD | CS8 | CSTOPB | HUPCL | CLOCAL;
        newtio_up.c_cflag &= ~(PARENB | PARODD);
        newtio_up.c_iflag &= ~(IGNBRK | BRKINT | IGNPAR | PARMRK | INPCK | INLCR | IGNCR | ICRNL | IXON | IXOFF | IUCLC | IXANY | IMAXBEL);
        //newtio_up.c_iflag |= ISTRIP;
        newtio_up.c_oflag &= ~OPOST;
        newtio_up.c_lflag &= ~(ISIG | ICANON | XCASE | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE | IEXTEN);

        /*set non-canonical attributes*/
        newtio_up.c_cc[VTIME] = 1;
        newtio_up.c_cc[VMIN] = 255;

        tcflush(fd, TCIFLUSH);
        tcsetattr(fd, TCSANOW, &newtio_up);
        roe_struct.roeLink = fd;
        record("Connection established. DEFAULT MODE\n");
        }
    }
        //roe.roeLink = fd;
        roe_struct.active = TRUE;
        pthread_mutex_unlock(&roe_struct.mx);
        record("ROE Active\n");
        return 0;
    
}

int deactivate() {
    pthread_mutex_lock(&roe_struct.mx);
    if (roe_struct.active == TRUE) {
        close(roe_struct.roeLink);
    }
    roe_struct.active = FALSE;
    pthread_mutex_unlock(&roe_struct.mx);
    record("ROE Deactivated.\n");
    return 0;
}




//Exit Roe default mode and enter manual mode

int exitDefault() {
    //char msg[100];
    record("Attempting to exit default mode.\n");
    //record(msg);

    pthread_mutex_lock(&roe_struct.mx);

    //New Roe Program
    int blockSize = 67;
    int block1[] = {0x46, 0x08, 0x00, 0x09, 0x12, 0x18, 0x25, 0x28,
        0x30, 0xF8, 0x34, 0xF8, 0x34, 0xF8, 0xF8, 0x80,
        0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xC0, 0xF8, 0x94,
        0x34, 0xF8, 0x30, 0xF8, 0xF8, 0x34, 0xF8, 0x30,
        0xF8, 0x34, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8,
        0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0x30, 0xF8,
        0xF8, 0xF8, 0x80, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8,
        0xC0, 0x34, 0xFA, 0x30, 0xFC, 0xFC, 0xFC, 0xFC,
        0xFC, 0xFC, 0xFC};

    int block2[] = {0x46, 0x88, 0x00, 0x66, 0xB3, 0x15, 0x55, 0x2A,
        0x00, 0x54, 0x7D, 0x0F, 0x7D, 0x03, 0x03, 0x01,
        0x02, 0x06, 0x04, 0x05, 0x01, 0x03, 0x07, 0x00,
        0x7D, 0x07, 0x27, 0x57, 0x57, 0x27, 0x0F, 0x27,
        0x54, 0x7D, 0x0B, 0x0B, 0x0A, 0x0E, 0x0C, 0x0D,
        0x09, 0x0B, 0x0B, 0x0F, 0x0F, 0x0F, 0x00, 0x55,
        0x41, 0x43, 0x32, 0x42, 0x5E, 0x5C, 0xDD, 0xC9,
        0xCB, 0x00, 0x0F, 0x00, 0x42, 0x4A, 0x4A, 0x4A,
        0x4A, 0x4A, 0xCA};

    int block3[] = {0x46, 0x08, 0x01, 0xFC, 0xF8, 0xF8, 0xF8, 0xF8,
        0xF8, 0xF8, 0xF8, 0xF8, 0x87, 0xFC, 0xFC, 0xFC,
        0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF8,
        0xF8, 0xF8, 0xF8, 0xF8, 0xC0, 0xFC, 0xFC, 0xFC,
        0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF8, 0x34,
        0xF8, 0xC8, 0x30, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8,
        0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8,
        0xF8, 0xF8, 0x30, 0xF8, 0x34, 0xF8, 0x00, 0x00,
        0x00, 0x00, 0x00};

    int block4[] = {0x46, 0x88, 0x01, 0xDE, 0xDC, 0xDD, 0xC9, 0xCB,
        0xCA, 0xCA, 0x8A, 0xAA, 0xFF, 0x22, 0x2A, 0x2A,
        0x2A, 0x2A, 0x2A, 0xAA, 0xBE, 0xDC, 0xDD, 0xC9,
        0xCB, 0xCA, 0xCA, 0x8A, 0xAA, 0x22, 0x2A, 0x2A,
        0x2A, 0x2A, 0x2A, 0xAA, 0xBE, 0xDC, 0xDC, 0x00,
        0x0F, 0x0F, 0x00, 0xDC, 0xDC, 0xDC, 0xDC, 0xDC,
        0x54, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0xDC, 0xDC,
        0xDC, 0xDC, 0x00, 0x54, 0x00, 0x2F, 0x0F, 0x00,
        0x00, 0x00, 0x00};

    char command = EXIT_DEFAULT;
    if (write(roe_struct.roeLink, &command, 1) != 1) //Write Command to ROE Link
    {
        record("Exit Default Error, write\n");
        pthread_mutex_unlock(&roe_struct.mx);
        return -1;
    }
    usleep(500000); //Wait for command to finish
    char ack;
    if (receiveAck(roe_struct.roeLink, &ack, 1, 0x03) == -1) {
        record("Did not receive ack from ROE!\n");
        pthread_mutex_unlock(&roe_struct.mx);
        return -1; //Get Acknowledgment of Command
    }
    printf("received ack, reading the status\n");
    char status;
    if (readRoe(roe_struct.roeLink, &status, 1) == -1) {
        record("Exit Default Error, status\n");
        pthread_mutex_unlock(&roe_struct.mx);
        return -1; //Get Status of Command Execution
    }
    roe_struct.mode = MANUAL; //Update variable to reflect current state
    //Update the ROE's CSG Program
    //New program writes a dummy pixel at the end of a readout.
    int roeCustomRead = 0;
    //if(ops1.roe_custom_read == TRUE)
    if (roeCustomRead) {
        if (status != -1) {
            int i;
            for (i = 0; i < blockSize; i++)
                write(roe_struct.roeLink, &block1[i], sizeof (block1[i]));
            for (i = 0; i < blockSize; i++)
                write(roe_struct.roeLink, &block2[i], sizeof (block2[i]));
            for (i = 0; i < blockSize; i++)
                write(roe_struct.roeLink, &block3[i], sizeof (block3[i]));
            for (i = 0; i < blockSize; i++)
                write(roe_struct.roeLink, &block4[i], sizeof (block4[i]));

            record("Exiting Default Mode\n");
        } else {
            record("Status = -1, not entering default mode\n");
        }
    }
        pthread_mutex_unlock(&roe_struct.mx);
        return 0;
    
}

/*For testing serial connection only*/
int sendDummyData() {
    int blockSize = 67;
    int block1[] = {0x46, 0x08, 0x00, 0x09, 0x12, 0x18, 0x25, 0x28,
        0x30, 0xF8, 0x34, 0xF8, 0x34, 0xF8, 0xF8, 0x80,
        0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xC0, 0xF8, 0x94,
        0x34, 0xF8, 0x30, 0xF8, 0xF8, 0x34, 0xF8, 0x30,
        0xF8, 0x34, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8,
        0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0x30, 0xF8,
        0xF8, 0xF8, 0x80, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8,
        0xC0, 0x34, 0xFA, 0x30, 0xFC, 0xFC, 0xFC, 0xFC,
        0xFC, 0xFC, 0xFC};

    int i;
    for (i = 0; i < blockSize; i++)
        write(roe_struct.roeLink, &block1[i], sizeof (block1[i]));
    
    return 0;
}

//Request House Keeping data from roe;

int getHK(char hkparam) {
    pthread_mutex_lock(&roe_struct.mx);
    char command[2] = {ROE_HK_REQ, hkparam};
    int i;
    for (i = 0; i < 2; i++) //Write the command to the serial link 
        if (write(roe_struct.roeLink, (char *) &command[i], 1) != 1) {
            pthread_mutex_unlock(&roe_struct.mx);
            record("getHK error, write\n");
            return -1;
        }
    char response;
    if (receiveAck(roe_struct.roeLink, (char *) &response, 1, ROE_HK_RES) == -1) {
        pthread_mutex_unlock(&roe_struct.mx);
        record("getHK error, ack\n");
        return -1; //Get Ack
    }
    char value;
    if (readRoe(roe_struct.roeLink, &value, 1) == -1) {
        pthread_mutex_unlock(&roe_struct.mx);
        record("getHK error, value\n");
        return -1; //Read HK Value	mx.unlock();
    }
    pthread_mutex_unlock(&roe_struct.mx);
    return value; //Return the HK Value

}

//Reset Roe back to default mode

int reset() {
    pthread_mutex_lock(&roe_struct.mx);

    //do reset stuff 
    char command = RESET_ROE;
    if (write(roe_struct.roeLink, (char *) &command, 1) != 1) //Write Command to ROE Link
    {
        record("Reset Error\n");
        pthread_mutex_unlock(&roe_struct.mx);
        return -1;
    }
    roe_struct.mode = MANUAL;
    usleep(500000); //Wait for the command to complete
    pthread_mutex_unlock(&roe_struct.mx);
    record("Reseting to Default Mode\n");
    return 0;
}

//Put Roe into Selftest Mode

int selftestMode() {
    pthread_mutex_lock(&roe_struct.mx);

    char selftst[9] = {0x45, 0x99, 0x99, 0x77, 0x4F, 0x0F, 0x00, 0x00, 0x00};
    int i;
    for (i = 0; i < 9; i++) //Write the command to the serial link
        if (write(roe_struct.roeLink, (char*) &selftst[i], 1) != 1)
        {
            record("selfTest error, write\n");
            return -1;
        }

    usleep(500000); //Wait for the command to complete   		
    char ack;
    if (receiveAck(roe_struct.roeLink, &ack, 1, 0x03) == -1) {
        pthread_mutex_unlock(&roe_struct.mx);
        record("selfTest error, ack\n");
        return -1; //Get Acknowledgement of Command
    }
    char status;
    if (readRoe(roe_struct.roeLink, &status, 1) == -1) {
        pthread_mutex_unlock(&roe_struct.mx);
        record("selfTest error, read\n");
        return -1; //Get Status of Command Execution
    }
    

    pthread_mutex_unlock(&roe_struct.mx);
    record("Entering Self Test Mode\n");
    return 0;
}

//Turn on the Stims

int stimOn() {
    pthread_mutex_lock(&roe_struct.mx);

    //0x45 = Set Analogue Electronics command, the other 8 bytes are the parameters
    char stimon[9] = {0x45, 0x99, 0x99, 0x77, 0x3F, 0x0F, 0x00, 0x00, 0x00};
    int i;
    for (i = 0; i < 9; i++) //Write the command to the serial link
        if (write(roe_struct.roeLink, (char*) &stimon[i], 1) != 1)
        {
            record("stimOn error, write\n");
            return -1;
        }

    usleep(500000); //Wait for the command to complete
    char ack;
    if (receiveAck(roe_struct.roeLink, &ack, 1, 0x03) == -1) {
        pthread_mutex_unlock(&roe_struct.mx);
        record("stimOn error, ack\n");
        return -1; //Get Acknowledgement of Command
    }
    char status;
    if (readRoe(roe_struct.roeLink, &status, 1) == -1) {
        pthread_mutex_unlock(&roe_struct.mx);
        record("stimOn error, read\n");
        return -1; //Get Status of Command Execution
    }

    pthread_mutex_unlock(&roe_struct.mx);
    record("Entering Stims Mode\n");
    return 0;
}

//Turn off Stims

int stimOff() {
    pthread_mutex_lock(&roe_struct.mx);

    //0x45 = Set Analogue Electronics command, the other 8 bytes are the parameters
    char stimoff[9] = {0x45, 0x99, 0x99, 0x77, 0x2F, 0x0F, 0x00, 0x00, 0x00};
    int i;
    for (i = 0; i < 9; i++) //Write the command to the serial link
        if (write(roe_struct.roeLink, (char*) &stimoff[i], 1) != 1)
        {
            record("stimOff error, write\n");
            return -1;
        }

    usleep(500000); //Wait for the command to complete
    char ack;
    if (receiveAck(roe_struct.roeLink, &ack, 1, 0x03) == -1) {
        pthread_mutex_unlock(&roe_struct.mx);
        record("stimOff error, ack\n");
        return -1; //Get Acknowledgement of Command
    }
    char status;
    if (readRoe(roe_struct.roeLink, &status, 1) == -1) {
        pthread_mutex_unlock(&roe_struct.mx);
        record("stimOff error, read\n");
        return -1; //Get Status of Command Execution
    }

    pthread_mutex_unlock(&roe_struct.mx);
    record("Exiting Stims Mode\n");
    return 0;
}

/*Reads out the CCD's using the specified block, and delaying for 
        'delay' microseconds after the command has been issued.
        Refer to the Preamble in the header file for legal blocks 
        and their descriptions. */

int readOut(char* block, int delay) {
    pthread_mutex_lock(&roe_struct.mx);

    //char command[2] = {START_CSG,block};
    char command[2] = {0x45, 0x99};

    int i;
    for (i = 0; i < 2; i++) //Write the command to the serial link
        if (write(roe_struct.roeLink, (char *) &command[i], 1) != 1) return -1;

    usleep(delay); //Wait for the command to complete
    char ack;
    if (receiveAck(roe_struct.roeLink, &ack, 1, 0x03) == -1) {
        pthread_mutex_unlock(&roe_struct.mx);
        return -1; //Get Acknowledgement of Command
    }
    char status;
    if (readRoe(roe_struct.roeLink, &status, 1) == -1) {
        pthread_mutex_unlock(&roe_struct.mx);
        return -1; //Get Status of Command Execution
    }

    pthread_mutex_unlock(&roe_struct.mx);
    record("Reading Out CCD's\n");
    return 0;
}

//flush the ccd's

int flush() {
    pthread_mutex_lock(&roe_struct.mx);

    int i;
    char command[2] = {START_CSG, 0x00};

    for (i = 0; i < 2; i++) //Write the command to the serial link
        if (write(roe_struct.roeLink, (char *) &command[i], 1) != 1) return -1;

    char ack;
    if (receiveAck(roe_struct.roeLink, &ack, 1, 0x03) == -1) {
        pthread_mutex_unlock(&roe_struct.mx);
        return -1; //Get Acknowledgement of Command
    }
    char status = 0;
    for (i = 0; (status != END_OF_SEQUENCE)&&(i < 5); i++) {
        readRoe(roe_struct.roeLink, &status, 1); //Wait for endofsequence, faster than usleep(***)
    }
    readRoe(roe_struct.roeLink, &status, 1); //Get the status of command execution

    pthread_mutex_unlock(&roe_struct.mx);
    record("Flushing CCD's\n");
    return 0;
}



//Request the Analogue Electronics

/*char* getAE() {
    pthread_mutex_lock(&roe.mx);

    //do getAE stuff 

    pthread_mutex_unlock(&roe.mx);

    return 0;
    char aTable[17] = "0123456789ABCDEF";
    unsigned char params[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    char* result = "";
    for(int i = 0; i < 8; i++)
    {
            result += aTable[params[i]/16];
            result += aTable[params[i]%16];
            //result += aTable[params[i]/16];
    }
    return result;
}*/

//Set the Analogue Electronics

/*int setAE(char* paramstring) {
    pthread_mutex_lock(&roe.mx);

    //do setAE stuff 

    pthread_mutex_unlock(&roe.mx);
    record("Setting AE Params.\n");
    return 0;
}*/

//write string to roe

/*int manualWrite(char* msg, int size) {
    pthread_mutex_lock(&roe.mx);

    //do manual Write stuff 

    pthread_mutex_unlock(&roe.mx);
    record("Writing manual string to ROE.\n");
    return 0;
}*/

int readRoe(int fd, char *data, int size) {
    //printf("fd readRoe: %d\n", fd);
    //Times out after 50000 tries
    //int timeout;
    int input;
    //wait for one second
    input = input_timeout_roe(fd, 1);
    if (input > 0) {
        if (read(fd, data, size) != -1) {
            printf("data read, exiting readRoe\n");
            return 0;
        }
    }
    printf("readRoe Error\n");
    return -1;
}

//Receive Acknowledgement from ROE

int receiveAck(int fd, char *data, int size, char target) {
    int timeout;

    for (timeout = 0; timeout < 5; timeout++) //Times out after 5 seconds
    {
        if (readRoe(fd, data, size) != -1) { //Return only if read data is an acknowledgement
            if (*data == target) {
                printf("Acknowledgment successful\n");
                return 0;
            }
        }
    }
    printf("Aknowledgment timeout\n");
    return -1;
}

/*int atoh(char c) {
    return (c >= 0 && c <= 9) ? (c & 0x0F) : ((c & 0x0F) + 9);
}*/

int input_timeout_roe(int filedes, unsigned int seconds) {
    fd_set set;
    struct timeval timeout;

    /*initialize the file descriptor set for select function*/
    FD_ZERO(&set);
    FD_SET(filedes, &set);

    /*initialize timout data structure for select function*/
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;

    /*select returns 0 if timeout, 1 if input data is available, -1 if error*/
    //return TEMP_FAILURE_RETRY(select(FD_SETSIZE, &set, NULL, NULL, &timeout));
    return select(FD_SETSIZE, &set, NULL, NULL, &timeout);
}