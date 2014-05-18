/* 
 * File:   defs.h
 * Author: Roy Smart
 * Company: Montana State University, MOSES Lab
 *
 * Created on May 15, 2014, 1:50 PM
 */

#ifndef DEFS_H
#define	DEFS_H

#define FALSE 0
#define TRUE 1
#define STARTBYTE '%'
#define ENDBYTE '^'

/*Packet structure for parsing and storing HLP packets*/
typedef struct {
    /*HLP fields*/
	char timeStamp[7];
	char type[2];
	char subtype[4];
	char dataLength[3];
	char data[256];
	char checksum[2];
        
        /*Packet attributes*/
        void * next;    //next item in the locking queue    
	int dataSize;
        int status;        
} Packet;

/*Node structure for hash table*/
typedef struct{
    void* next; //next entry in hash table
    char* name; //defined name
    hlpControl* defn; //replacement text
}Node;

enum tmu{
    UDataStart,
    UDataStop,
    UDark1,
    UDark2,
    UDark3,
    UDark4,
    USleep,
    UWake,
    UTest,    
    TDataStart,
    TDataStop,
    TDark2,
    TDark4,
    TSleep,
    NoFunc      //Used by the map to return the absence of a matching function
};


#endif	/* DEFS_H */

