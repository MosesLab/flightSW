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

void constructEmpty();
void constructImage(roeimage_t*, short** pdata,int* psize,char channels,int pbitpix);
void init(roeimage_t*, short** data, int *size,char, int);
void setData(roeimage_t*, short **data, int *size, char channels);
void writeToFile(roeimage_t*);



#endif	/* ROEIMAGE_H */
