/****************************************/
/*Author: Roy Smart			*/
/*1/21/14				*/
/*MOSES LAB				*/
/*					*/
/*packet struct encapsulates attributes */
/*of serial command packets		*/
/****************************************/
#define _GNU_SOURCE 1

#include <math.h>
#include "system.h"
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include "hlp.h"

#define _POSIX_SOURCE 1
#define UPBAUD B1200
#define DOWNBAUD B9600
#define HKUP "/dev/hkup"
#define HKDOWN "/dev/hkdown"
//#define HKUP "/dev/ttyUSB0"
//#define HKDOWN "/dev/ttyUSB1"

extern volatile sig_atomic_t ts_alive;

/*variable declaration*/
char lookupTable[128];	//this lookup table is global, but since it is only written to once, it does not need to be locked
int res;                //variables for reading serial data
char buf[255];          //serial port buffer
char packet_array[270];		//final packet array
struct termios oldtio_up, newtio_up; //structures for old device settings and new 

/*function declaration*/
void buildLookupTable();
char calcCheckSum(Packet * p);
char encode(char);
char decode(char);
int ahtoi(char*, int);
void itoah(int, char *, int);

Packet* constructPacket(char*, char *, char *);
void getCurrentTime(char *);
int init_hkup_serial_connection();	//initialize attributes of serial connection
int init_hkdown_serial_connection();
void readPacket(int, Packet *);
int readData(int, char *, int);
void sendPacket(Packet *, int);
void sendData(char *, int, int);

