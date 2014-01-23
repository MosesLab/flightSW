/****************************************/
/*Author: Roy Smart			*/
/*1/21/14				*/
/*MOSES LAB				*/
/*					*/
/*packet struct encapsulates attributes */
/*of serial command packets		*/
/****************************************/
#include "packetType.h"

const char start = '%';
const char end = '^';
char lookupTable[128];	//this lookup table is global, but since it is only written to once, it does not need to be locked

char encode(char dataByte) {return lookupTable[dataByte];}
char decode(char dataByte) {return (dataByte & 0x7F);}


void buildLookupTable();

char calcCheckSum(packet p);
