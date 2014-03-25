/****************************************/
/*Author: Roy Smart			*/
/*1/21/14				*/
/*MOSES LAB				*/
/*					*/
/*packet struct encapsulates attributes */
/*of serial command packets		*/
/****************************************/
#define _GNU_SOURCE 1

#include "packetType.h"
#include <math.h>
#include "system.h"
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>

#define _POSIX_SOURCE 1
#define UPBAUD B1200
#define DOWNBAUD B9600
#define HKUP "/dev/hkup"
#define HKDOWN "/dev/hkdown"

extern volatile sig_atomic_t ts_alive;

char lookupTable[128];	//this lookup table is global, but since it is only written to once, it does not need to be locked

char encode(char dataByte) {return lookupTable[dataByte];}
char decode(char dataByte) {return (dataByte & 0x7F);}
int res;	//variables for reading serial data
char buf[255];	//serial port buffer
char packet_array[270];		//final packet array
struct termios oldtio_up, newtio_up; //structures for old device settings and new 


void buildLookupTable();
char calcCheckSum(Packet * p);

int ahtoi(char*, int);
void itoah(int, char *, int);

int init_hkup_serial_connection();	//initialize attributes of serial connection
int init_hkdown_serial_connection();
void readPacket(int, Packet *);
int readData(int, char *, int);


