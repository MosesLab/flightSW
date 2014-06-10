/**************************************************
* Author: David Keltgen                           *
* Company: Montana State University: MOSES LAB    *
* File Name: roe.h                                *
* Date: June 4 2014                               *
* Description: Software representation of the     *
*              roe.                               *
**************************************************/

#ifndef ROE_H
#define	ROE_H
#define TRUE   1
#define FALSE  0
#define UPBAUD B9600
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

#include <fcntl.h>
#include <pthread.h>
#include <termios.h>
#include <time.h>

struct ReadOutElectronics
{
    int isActive;
    int activate;
    int deactivate;
    int exitDefault;
    int selfTestMode;
    int stimOn;
    int stimOff;
    int readOut;
    int flush;
    int getHK;
    int getMode;
    int reset;
    char* getAE;
    int manualWrite;
    
    int active;
    int state;
    int atoh; //ascii to hex
    int readRoe;
    int recieveAck;
    int roeLink;
    int mode; //default or manual
    pthread_mutex_t mx;
};

struct ReadOutElectronics roe;

int activateROEReal();
int deactivateReal();
int exitDefaultReal();
int selftestModeReal();
int stimOnReal();
int stimOffReal();
int resetReal();
int readOutReal(char* block, int delay);
int flushReal();
int getHKReal(char* hkparam);
char* getAEReal();
int setAEReal(char* paramstring);
int manualWriteReal(char* msg, int size);
int recieveAck(int fd,char *data,int size,char target);
int readRoe(int fd, char *data, int size);

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* ROE_H */

