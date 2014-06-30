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
#include <dirent.h>

sequence_t constructSequence(char *filepath) {
    int i, value;
    char stuff[50];

    FILE* file;
    tempSequence.sequenceName = (char *) malloc(30);
    strncpy(tempSequence.sequenceName, filepath, 30);
    if (strstr(tempSequence.sequenceName, "data") != NULL) {
        tempSequence.seq_type = 1; // type is data
    }
    file = fopen(filepath, "r");

    //printf("filepath: %s\n", filepath);

    value = fscanf(file, "%s\n", stuff); //scan "SEQUENCE"
    value = fscanf(file, "%s %s\n", stuff, stuff); //scan "NAME" and filepath
    value = fscanf(file, "%s %d\n", stuff, &tempSequence.numFrames); //scan "COUNT" and number of exposures 
    value = fscanf(file, "%s ", stuff); //scan "BEGIN"   

    //scan the exposures
    for (i = 0; i < (tempSequence.numFrames - 1); i++) {
        value = fscanf(file, "%lf ", &tempSequence.exposureTimes[i]);
    }
    value = fscanf(file, "%lf\n", &tempSequence.exposureTimes[tempSequence.numFrames - 1]);

    return tempSequence;
}

void loadSequences() {
    char filepath[] = "sequence"; //default folder path
    int numfiles = 0;
    char strdir[50];

    DIR *d;
    struct dirent *dir;
    d = opendir("sequence");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".seq") != NULL) {
                numfiles++;
            }

            printf("%s\n", dir->d_name);
        }

        closedir(d);
    }

    sequenceMap = (sequence_t *) malloc(numfiles * sizeof (sequence_t));

    d = opendir("sequence");
    int i = 0;
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".seq") != NULL) {
                sprintf(strdir, "%s/%s", filepath, dir->d_name);\
                sequenceMap[i] = constructSequence(strdir);\
                i++;
                
            }
        }

        closedir(d);

    }
}
