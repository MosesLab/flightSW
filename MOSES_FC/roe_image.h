/* 
 * File:   roe_image.h
 * Author: birdie
 *
 * Created on June 17, 2014, 9:19 PM
 */

#ifndef ROE_IMAGE_H
#define	ROE_IMAGE_H

#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stddef.h>
#include "system.h"
#include "hlp.h"

#define IMAGE_HEIGHT 1024
#define IMAGE_WIDTH 2048

//typedef struct {
//	char *name;
//	int **data[1024][2048 * 3];      //Adjust for data size
//        
//        void *next;                    //next item in the locking queue    
//	int dataSize;
//        int status;  
//        char *control;
//} roeImage_t;

typedef struct imgPtr {                         //Create image path node
    char* filePath;
    struct imgPtr * next;
} imgPtr_t;

/*should be obsfucated by sendTM.c*/
//roeImage_t* constructImage(char*, int data[][IMAGE_HEIGHT], int);
//void recordImage(roeImage_t*);
//void sendImage(roeImage_t *, int);


roeimage_t image;        //should be declared using malloc --RTS

void constructEmpty();
void constructImage(short** pdata,int* psize,char channels,int pbitpix);
void init(short** data, int *size,char, int);
void setData(short **data, int *size, char channels);
void writeToFile(char* file, char* catalog);
void pointToEnd(FILE* a);


#endif	/* ROEIMAGE_H */
