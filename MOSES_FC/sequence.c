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
    int i;
    char input_data[50];

    FILE* file;
    tempSequence.sequenceName = (char *) malloc(30);
    strncpy(tempSequence.sequenceName, filepath, 30);
    if (strstr(tempSequence.sequenceName, "data") != NULL) {
        tempSequence.seq_type = 1; // type is data
    }
    file = fopen(filepath, "r");

    //printf("filepath: %s\n", filepath);
    
    int rc;
    
    rc += fscanf(file, "%s\n", input_data); //scan "SEQUENCE"
    if(rc < 0) record("Error reading sequence\n");
    
    rc += fscanf(file, "%s %s\n", input_data, input_data); //scan "NAME" and filepath
    if(rc < 0) record("Error reading sequence\n");
    
    rc += fscanf(file, "%s %d\n", input_data, &tempSequence.numFrames); //scan "COUNT" and number of exposures 
    if(rc < 0) record("Error reading sequence\n");
    
    rc += fscanf(file, "%s ", input_data); //scan "BEGIN"
    if(rc < 0) record("Error reading sequence\n");

    //scan the exposures
    for (i = 0; i < (tempSequence.numFrames - 1); i++) {
        rc += fscanf(file, "%lf ", &tempSequence.exposureTimes[i]);
        if(rc < 0) record("Error reading sequence\n");
    }
    rc += fscanf(file, "%lf\n", &tempSequence.exposureTimes[tempSequence.numFrames - 1]);
    if(rc < 0) record("Error reading sequence\n");

    return tempSequence;
}

void loadSequences() {
    char filepath[] = "sequence"; //default folder path
    int numfiles = 0;
    char strdir[100];

    /*Open up all files that */
    DIR *d;
    struct dirent *dir;
    d = opendir("sequence");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".seq") != NULL) {
                numfiles++;
            }

            //printf("%s\n", dir->d_name);
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
                //seq_map_size = i;  //Need to send this variable to science_timeline
            }
        }

        closedir(d);

    }
}
