/* 
 * File:   roeImage.h
 * Author: david
 *
 * Created on June 4, 2014, 9:53 AM
 */

#include "system.h"

#ifndef ROEIMAGE_H
#define	ROEIMAGE_H

/* The RoeImage structure can be used to format and read and write ROE based images.
   These images are given unique names usually based on date and time. These names can
   then be used as indexes for an accompaning xml file which holds relavent 
   information pertaining to the images. This file is parsed when reading images 
   from a file. Each image is actually the sum of 1 to 4 images, each from a different
   channel of the ROE. The present channels appear in order with all of the data from
   channel zero appearing before channel one and so on.*/


void constructEmpty();
void constructImage(short** pdata,int* psize,char channels,int pbitpix);
void init(short** data, int *size,char, int);
void setData(short **data, int *size, char channels);
void writeToFile(char* file, char* catalog);
#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* ROEIMAGE_H */

