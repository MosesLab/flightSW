/* 
 * File:   packetType.h
 * Author: byrdie
 *
 * Created on January 23, 2014, 2:50 PM
 */


#define STARTBIT '%'
#define ENDBIT '^'

typedef struct {

	char timeStamp[7];
	char type[2];
	char subtype[4];
	char dataLength[3];
	char data[256];
	char checksum[2];
        
        void * next;
        
	int dataSize;
        int valid;
        
} Packet;

