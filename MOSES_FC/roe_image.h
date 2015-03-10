/* 
 * File:   roe_image.h
 * Author: birdie
 *
 * Created on June 17, 2014, 9:19 PM
 */

#ifndef ROE_IMAGE_H
#define	ROE_IMAGE_H

#include <pthread.h>
#include "system.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stddef.h>
#include "system.h"
#include "hlp.h"
#include "control.h"

#define IMAGE_HEIGHT 1024
#define IMAGE_WIDTH 2048

#define CATALOG "/mdata/imageindex.xml"
#define DATADIR "/mdata"

extern int xml_index; // index for storing how many xml entries have been saved
extern char * xml_snips[1000]; // array for storing xml snippets, room for 1000 images per program run.
extern int xml_snips_sz[1000]; // array for storing the amount of bytes in each xml snippe

char dataStub[128];

void constructEmpty();
void constructImage(roeimage_t*, int* psize, char channels, int pbitpix);
void init(roeimage_t*, int *size, char, int);
void setData(roeimage_t*, int *size, char channels);
void writeToFile(roeimage_t*);



#endif	/* ROEIMAGE_H */
