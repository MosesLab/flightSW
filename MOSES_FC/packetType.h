/* 
 * File:   packetType.h
 * Author: byrdie
 *
 * Created on January 23, 2014, 2:50 PM
 */

typedef struct {
	char timeStamp[6];
	char type;
	char subtype[3];
	char dataLength[2];
	char data[255];
	char checksum;

        void * next;
        
	int dataSize;

} packet;

