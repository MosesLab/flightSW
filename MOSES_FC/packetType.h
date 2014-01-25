/* 
 * File:   packetType.h
 * Author: byrdie
 *
 * Created on January 23, 2014, 2:50 PM
 */

#define BAD_PACKET "BAD_PACKET"

typedef struct {
	char timeStamp[6];
	char type;
	char subtype[3];
	char dataLength[2];
	char data[255];
	char checksum[2];

        void * next;
        
	int dataSize;
        int valid = 1;
        
} Packet;

