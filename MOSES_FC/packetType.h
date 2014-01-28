/* 
 * File:   packetType.h
 * Author: byrdie
 *
 * Created on January 23, 2014, 2:50 PM
 */

#define BAD_PACKET "BAD_PACKET"

#define STARTBIT '%'
#define ENDBIT '^'

typedef struct {

	char * timeStamp;
	char * type;
	char * subtype;
	char * dataLength;
	char * data;
	char * checksum;

        void * next;
        
	int dataSize;
        int valid;
        
} Packet;

