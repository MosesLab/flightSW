/* 
 * File:   defs.h
 * Author: Roy Smart
 * Company: Montana State University, MOSES Lab
 *
 * Created on May 15, 2014, 1:50 PM
 */

#ifndef DEFS_H
#define	DEFS_H

#include <signal.h>

#define _GNU_SOURCE 1

#define FALSE 0
#define TRUE 1
#define STARTBYTE '%'
#define ENDBYTE '^'

#define CH0		0x1	//0x8
#define CH1		0x2	//0x4
#define CH2		0x4	//0x2
#define CH3		0x8	//0x1




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
        char* control;
} Packet;

/*structure for thread-safe mutex-protected queue*/
typedef struct {
    Packet* first;
    Packet* last;
    int count;  //current number of items
    
    pthread_condattr_t cattr;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} LockingQueue;

/*typedef for array of function pointers for HLP packets*/
typedef int(*hlpControl)(Packet*);

/*Node structure for hash table*/
typedef struct node_t {
    
    struct node_t * next; //next chained entry in hash table
    char* name; //defined name
    void* def; //functional definition
    
} node_t;

/*this should be changed to make it more general --RTS*/
enum seq
{
    sequence1,
    sequence2,
    sequence3,
    sequence4,
    sequence5
    
    //etc....
};

/* This struct will contain the variables that represent the status of the system*/
typedef struct 
{
    enum seq sequence;
    char channels;
    sig_atomic_t seq_run;
    sig_atomic_t seq_pause;
    sig_atomic_t roe_power;
    sig_atomic_t roe_custom_read;
    sig_atomic_t dma_write;
    sig_atomic_t tm_write;
    
}moses_ops_t;

/*structure to store attributes of system*/
typedef struct  
{
	int numFrames;
	double exposureTimes[10]; //need to figure out how to assign size based on sequnce file
	int currentFrame;
	char* sequenceName;
        int seq_type; //1 will be data
}sequence_t;

typedef struct 
{
    char* filename;             //filename associated with image
    char* name;                 //image name
    int   bitpix;              //set the number of bits per pixel
    int   width;                // the width of each channel
    int   height;               // the height of each channel
    char* date;                 // the date the image was taken
    char* time;                 // the time the image was taken
    char* origin;               // the images origin
    char* instrument;           // the instrument used to obtain the image
    char* observer;             // observer of the image
    char* object;                  // the object in the image
    int   duration;             // the duration of the image exposure
    int   size[4];
    short* data[4];             // the image data
    char  channels;             //channels included in the image;
}roeimage_t;


#endif	/* DEFS_H */

