/****************************************/
/*Author: Roy Smart			*/
/*1/21/14				*/
/*MOSES LAB				*/
/*					*/
/*packet struct encapsulates attributes */
/*of serial command packets		*/
/****************************************/
#include "packetType.h"
#include <math.h>
#include "system.h"
#include <termios.h>

#define UPBAUD B1200
#define HKUP "/dev/hkup"


char lookupTable[128];	//this lookup table is global, but since it is only written to once, it does not need to be locked

char encode(char dataByte) {return lookupTable[dataByte];}
char decode(char dataByte) {return (dataByte & 0x7F);}
int res;	//variables for reading serial data
char buf[255];	//serial port buffer
char packet_array[270];		//final packetr array
struct termios oldtio_up, newtio_up; //structures for old device settings and new 


void buildLookupTable();
char calcCheckSum(Packet * p);

int ahtoi(char*, int);
void itoah(int, char *, int);

int init_serial_connection();	//initialize attributs of serial connection
Packet readPacket(int, Packet *);
int readData(int, char *, int);