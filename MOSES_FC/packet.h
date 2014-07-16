/****************************************/
/*Author: Roy Smart			*/
/*1/21/14				*/
/*MOSES LAB				*/
/*					*/
/*packet struct encapsulates attributes */
/*of serial command packets		*/
/****************************************/
#ifndef PACKET_H
#define	PACKET_H

//#define _GNU_SOURCE 1

#include "system.h"
#include <math.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include "hlp.h"

#define _POSIX_SOURCE 1
#define UPBAUD B1200
#define DOWNBAUD B9600
#define HKUP_REAL "/dev/ttyS2"
#define HKDOWN_REAL "/dev/ttyS3"
//#define HKUP "/dev/ttyUSB0"
//#define HKDOWN "/dev/ttyUSB1"
#define HKUP_SIM "/dev/pts/26"
#define HKDOWN_SIM "/dev/pts/29"

#define HKUP 1
#define HKDOWN 0

extern volatile sig_atomic_t ts_alive;

/*variable declaration*/
char lookupTable[128];	//this lookup table is global, but since it is only written to once, it does not need to be locked
int res;                //variables for reading serial data
char buf[255];          //serial port buffer
char packet_array[270];		//final packet array


/*function declaration*/
void buildLookupTable();
char calcCheckSum(packet_t * p);
char encode(char);
char decode(char);
inline int ahtoi(char*, int);
inline void itoah(int, char *, int);

packet_t* constructPacket(char*, char *, char *);
void recordPacket(packet_t*);
void getCurrentTime(char *);
int init_serial_connection();	//initialize attributes of serial connection
int init_hkdown_serial_connection();
void readPacket(int, packet_t *);
int readData(int, char *, int);
void sendPacket(packet_t *, int);
void sendData(char *, int, int);
int input_timeout(int, unsigned int);

#endif /*PACKET_H*/
