/**************************************************
 * Author: David Keltgen                           *
 * Company: Montana State University: MOSES LAB    *
 * File Name: roeReal.c              *
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
#include "main.h"
#include "logger.h"

int activateROE() {
    //char msg[100];
    int fd;
    pthread_mutex_lock(&roe_struct.mx);
    if (roe_struct.active == FALSE) {
        //Open Serial Device
        fd = open(ROE_DEV, O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd < 0) {
            printf("%s\n", strerror(errno));
            record("Couldnt connect\n");
            pthread_mutex_unlock(&roe_struct.mx);
            exit(-1);
        }
       	fcntl(fd,F_SETFL,FNDELAY);
	struct termios options;
	tcgetattr(fd,&options);
	cfsetispeed(&options,B9600);
	cfsetospeed(&options,B9600);
	options.c_cflag |= (CLOCAL|CREAD);
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_oflag &= ~OPOST;
	options.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);
	tcsetattr(fd,TCSANOW,&options);
        
        record("Connection established. DEFAULT MODE\n");
        roe_struct.roeLink = fd;
        roe_struct.active = TRUE;
}
        pthread_mutex_unlock(&roe_struct.mx);
        record("ROE Active\n");
        return fd;
    
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
    printf("Attempting to exit default mode.\n");
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
    int val;
    val = write(roe_struct.roeLink, (char *)&command, 1); //Write Command to ROE Link
    printf("write: %d\n", val);
    if(val != 1)
    {
        record("Exit Default Error, write\n");
        pthread_mutex_unlock(&roe_struct.mx);
        return -1;
    }
    
    usleep(5000000); //Wait for command to finish
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
    printf("Status = %c\n", status);
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




//Request House Keeping data from roe;

char* getHK(char hkparam) {
    
    if(!roe_struct.active)
    {
        record("ROE not active in getHK\n");
    }
    pthread_mutex_lock(&roe_struct.mx);
    char command[2] = {ROE_HK_REQ, hkparam};
    int i;
    for (i = 0; i < 2; i++) //Write the command to the serial link 
        if (write(roe_struct.roeLink, (char *) &command[i], 1) != 1) {
            pthread_mutex_unlock(&roe_struct.mx);
            record("getHK error, write\n");
            return "FF";
        }
    char ack;
    if (receiveAck(roe_struct.roeLink, (char *) &ack, 1, ROE_HK_RES) == -1) {
        pthread_mutex_unlock(&roe_struct.mx);
        record("getHK error, ack\n");
        return "FF"; //Get Ack
    }
    char value;
    if (readRoe(roe_struct.roeLink, &value, 1) == -1) {
        pthread_mutex_unlock(&roe_struct.mx);
        record("getHK error, value\n");
        return "FF"; //Read HK Value	mx.unlock();
    }
    /*This value will be sent over telemetry as a string,
      convert this value(byte) to a char array */

    sprintf(value_char,"%c",value);
   
    pthread_mutex_unlock(&roe_struct.mx);
    return value_char; //Return the HK Value

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
    roe_struct.mode = DEFAULT;
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
            pthread_mutex_unlock(&roe_struct.mx);
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
            pthread_mutex_unlock(&roe_struct.mx);
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
            pthread_mutex_unlock(&roe_struct.mx);
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

int readOut(int block, int delay) {
    pthread_mutex_lock(&roe_struct.mx);

    //char command[2] = {START_CSG,block};
    char command[2] = {0x45, 0x99};

    int i;
    for (i = 0; i < 2; i++) //Write the command to the serial link
        if (write(roe_struct.roeLink, (char*) &command[i], 1) != 1)
        {
            pthread_mutex_unlock(&roe_struct.mx);
            record("Readout Error, write\n");
            return -1;
        }
    usleep(delay); //Wait for the command to complete
    char ack;
    if (receiveAck(roe_struct.roeLink, &ack, 1, 0x03) == -1) {
        pthread_mutex_unlock(&roe_struct.mx);
        record("Readout Error, ack\n");
        return -1; //Get Acknowledgement of Command
    }
    char status;
    if (readRoe(roe_struct.roeLink, &status, 1) == -1) {
        pthread_mutex_unlock(&roe_struct.mx);
        record("Readout Error, read\n");
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
        if (write(roe_struct.roeLink, (char *) &command[i], 1) != 1)
        {
            pthread_mutex_unlock(&roe_struct.mx);
            record("Flush Error, write\n");
            return -1;
        }

    char ack;
    if (receiveAck(roe_struct.roeLink, &ack, 1, 0x03) == -1) {
        pthread_mutex_unlock(&roe_struct.mx);
        record("Flush Error, ack\n");
        return -1; //Get Acknowledgement of Command
    }
    char status = 0;
    for (i = 0; (status != END_OF_SEQUENCE)&&(i < 5); i++) {
        readRoe(roe_struct.roeLink, &status, 1); //Wait for endofsequence, faster than usleep(***)
    }
    readRoe(roe_struct.roeLink, &status, 1); //Get the status of command execution
    printf("Flush status: %c\n", status);
    
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
            //printf("data read, exiting readRoe %c<---data\n", *data);
            return 0;
        }
    }
    //printf("readRoe Error\n");
    return -1;
}

//Receive Acknowledgement from ROE

int receiveAck(int fd, char *data, int size, char target) {
    int timeout;
	char msg[100];
    record("Inside receiveAck\n");

    for (timeout = 0; timeout < 5; timeout++) //Times out after 5 seconds
    {
        if (readRoe(fd, data, size) != -1) { //Return only if read data is an acknowledgement
      sprintf(msg,"Data:%c, target:%c\n", *data, target);
      record(msg);
      if (*data == target) {
                record("Acknowledgment successful\n");
                return 0;
            }
        }
    }
    sprintf(msg,"Aknowledgment timeout %c<--data\n", *data);
	record(msg);
    return -1;
}

int atoh_roe(char c) {
    return (c >= 0 && c <= 9) ? (c & 0x0F) : ((c & 0x0F) + 9);
}

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