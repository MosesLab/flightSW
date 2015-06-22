/**************************************************
* Author: David Keltgen                           *
* Company: Montana State University: MOSES LAB    *
* File Name: roe.h                                *
* Date: June 4 2014                               *
* Description: Software representation of the     *
*              roe.                               *
**************************************************/

#include "logger.h"

#ifndef ROE_H
#define	ROE_H
#define TRUE   1
#define FALSE  0
#define UPBAUD_ROE B9600
#define DOWNBAUD B9600

#define EXIT_DEFAULT  	0x41	//Exit default mode
#define RESET_ROE  	0x40	//Reset to default mode
#define START_CSG  	0x42	//Readout/flush command
#define END_OF_SEQUENCE	0x0C	//returned after START_CSG
#define ROE_AE_REQ  	0x49	//Request Analogue Electronics
#define ROE_AE_RES  	0xC0	//Analogue Electronics Response
#define ROE_HK_REQ  	0x47	//Request House Keeping value
#define ROE_HK_RES  	0xC0	//House Keeping Response
#define ROE_ACK  	0x0300	//Roe Acknowledgement
#define ROE_UCH  	0x0301	//Unrecognized Command Header
#define ROE_TOE  	0x03FF	//Time Out Error
#define DEFAULT  	1
#define MANUAL  	0
#define ROE_DEV         "/dev/ttyS0" // comes out of COM2 but its COM1 because of a hardware error

#include <fcntl.h>
#include <pthread.h>
#include <termios.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

struct ReadOutElectronics
{   
    int present;        // moses.conf roe field is 1 and gpio_ou_bf.roe == 1
    int active;
    int state;
    int atoh; //ascii to hex
    int readRoe;
    int recieveAck;
    int roeLink;
    int mode; //default or manual
    pthread_mutex_t mx;
};


extern struct ReadOutElectronics roe_struct;


int activateROE();
int deactivate();
int exitDefault();
int selftestMode();
int stimOn();
int stimOff();
int reset();
int readOut(int block, int delay);
int flush();
double getHK(char hkparam);
char* getAE();
int setAE(char* paramstring);
int manualWrite(char* msg, int size);
int receiveAck(int fd,unsigned char *data,int size,char target);
int input_timeout_roe(int filedes, unsigned int seconds);
int atoh_roe(char c);
int readRoe(int fd, unsigned char *data, int size);
double get_x(unsigned int);      // Calculate the x value for ROE calibration


#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* ROE_H */

