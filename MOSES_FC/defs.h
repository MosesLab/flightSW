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
#include <pthread.h>
#include "plx/PlxApi.h"
#include "plx/PlxInit.h"

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
    /*Packet attributes*/
    //        void * next;    //next item in the locking queue    
    unsigned int dataSize;
    int status;
    char* control;

    /*HLP fields*/
    char timeStamp[7];
    char type[2];
    char subtype[4];
    char dataLength[3];
    char data[256];
    char checksum[2];


} packet_t;

typedef struct node_t {
    struct node_t * next; //next chained entry in hash table
    char* name; //defined name
    void* def; //functional definition

} node_t;

/*structure for thread-safe mutex-protected queue*/
typedef struct {
    int count; //current number of items
    node_t * first;
    node_t * last;

    pthread_condattr_t cattr;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} LockingQueue;

/*typedef for array of function pointers for HLP packets*/
typedef int(*hlpControl)();

/*Node structure for hash table*/

/* This struct will contain the variables that represent the status of the system*/
typedef struct {
    unsigned int sequence;
    char channels;
    unsigned int seq_run;
    unsigned int roe_power;
    unsigned int roe_custom_read;
    unsigned int dma_write;
    unsigned int tm_write;

} moses_ops_t;

/*structure to store attributes of system*/
typedef struct {
    unsigned int numFrames;
    double exposureTimes[10]; //need to figure out how to assign size based on sequence file
    unsigned int currentFrame;
    char* sequenceName;
    unsigned int seq_type; //1 will be data
} sequence_t;

typedef struct {
    char* filename; //filename associated with image
    char* name; //image name
    unsigned int bitpix; //set the number of bits per pixel
    unsigned int width; // the width of each channel
    unsigned int height; // the height of each channel
    char* date; // the date the image was taken
    char* time; // the time the image was taken
    char* origin; // the images origin
    char* instrument; // the instrument used to obtain the image
    char* observer; // observer of the image
    char* object; // the object in the image
    unsigned int duration; // the duration of the image exposure
    unsigned int size[4];
    short* data[4]; // the image data
    char channels; //channels included in the image;
    sequence_t * seq; //sequence that initiated the image
} roeimage_t;

typedef struct imgPtr { //Create image path node
    char* filePath;
    struct imgPtr * next;
} imgPtr_t;

/*bit field for passing gpio inputs */
typedef struct {
    unsigned int udatastart : 1;
    unsigned int udatastop : 1;
    unsigned int udark1 : 1;
    unsigned int udark2 : 1;
    unsigned int udark3 : 1;
    unsigned int udark4 : 1;
    unsigned int usleep : 1;
    unsigned int uwake : 1;
    unsigned int utest : 1;

    unsigned int tdark2 : 1;
    unsigned int tdark4 : 1;
    unsigned int tdatastart : 1;
    unsigned int tdatastop : 1;
    unsigned int tsleep : 1;

    unsigned int shutter_sig : 1;

    unsigned int unused_in : 17; //structure needs to be 32 bits
} gpio_in_bf;

/*union to allow gpio in bit fields to be taken as ints*/
typedef union gpio_in_uni {
    U32 val;
    gpio_in_bf bf;
} gpio_in_uni;

/*bit field for passing gpio outputs*/
typedef struct {
    unsigned int shutter : 1;
    unsigned int roe : 1;
    unsigned int premod : 1;
    unsigned int tcs0 : 1;
    unsigned int tcs1 : 1;
    unsigned int tcs2 : 1;
    unsigned int tcs3 : 1;
    unsigned int reg_5V : 1;
    unsigned int reg_12V : 1;
    unsigned int h_alpha : 1;
    unsigned int latch : 1;

    unsigned int unused_out : 19;

} gpio_out_bf;

/*union to allow gpio out bit fields to be taken as ints*/
typedef union gpio_out_uni {
    U32 val;
    gpio_in_bf bf;
} gpio_out_uni;


#endif	/* DEFS_H */

