/**************************************************
* Author: David Keltgen                           *
* Company: Montana State University: MOSES LAB    *
* File Name: sequence.c               *
* Date:  June 4 2014                              *
* Description: Functions used for working with    *
*              sequences                          *
**************************************************/

#include "sequence.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <stdarg.h>

sequence_t constructSequence(char *filepath)
{
    int i, value;
    char stuff[50];
    
    FILE* file;
    tempSequence.sequenceName = (char *) malloc(21);
    strncpy(tempSequence.sequenceName, filepath, 21);
    
    if(strstr(tempSequence.sequenceName, "data") != NULL)
    {
        tempSequence.seq_type = 1; // type is data
    }

    
    file = fopen(filepath, "r");
    
    value = fscanf(file, "%s\n", stuff); //scan "SEQUENCE"
    value = fscanf(file, "%s %s\n", stuff, stuff); //scan "NAME" and filepath
    value = fscanf(file, "%s %d\n", stuff, &tempSequence.numFrames); //scan "COUNT" and number of exposures 
    value = fscanf(file, "%s ", stuff);  //scan "BEGIN"   
    
    //scan the exposures
    for (i = 0; i < (tempSequence.numFrames - 1); i++ ){
       value = fscanf(file, "%lf ", &tempSequence.exposureTimes[i]);
    }
    value = fscanf(file, "%lf\n", &tempSequence.exposureTimes[tempSequence.numFrames - 1]);
    
    return tempSequence;
}
