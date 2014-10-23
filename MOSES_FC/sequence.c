/**************************************************
 * Author: David Keltgen                           *
 * Company: Montana State University: MOSES LAB    *
 * File Name: sequence.c               *
 * Date:  June 4 2014                              *
 * Description: Functions used for working with    *
 *              sequences                          *
 **************************************************/

#include "sequence.h"
#include "system.h"
#include <dirent.h>

/*global variable declaration*/
int seq_map_size;
sequence_t *sequenceMap;
sequence_t tempSequence;

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

    int rc;

    rc = fscanf(file, "%s\n", input_data); //scan "SEQUENCE"
    if (rc < 0) record("Error reading sequence\n");

    rc = fscanf(file, "%s %s\n", input_data, input_data); //scan "NAME" and filepath
    if (rc < 0) record("Error reading sequence\n");

    rc = fscanf(file, "%s %d\n", input_data, &tempSequence.numFrames); //scan "COUNT" and number of exposures 
    if (rc < 0) record("Error reading sequence\n");

    rc = fscanf(file, "%s ", input_data); //scan "BEGIN"
    if (rc < 0) record("Error reading sequence\n");

    //scan the exposures
    for (i = 0; i < (tempSequence.numFrames - 1); i++) {
        rc = fscanf(file, "%lf ", &tempSequence.exposureTimes[i]);
        if (rc < 0) record("Error reading sequence\n");
    }
    rc = fscanf(file, "%lf\n", &tempSequence.exposureTimes[tempSequence.numFrames - 1]);
    if (rc < 0) record("Error reading sequence\n");

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

void reload(char* file) {
    pthread_mutex_lock(&currentSequence_copy->mx);
    currentSequence->corrupt = FALSE;

    if (strstr(file, ".seq") != NULL) {
        constructSequence(file);
    }

    pthread_mutex_lock(&currentSequence->mx);

}

int findAndJump_seq(double num) {
    return -1;
}

void jump(int frame) {

}

void save() {
    int i;
    char msg[100];
    FILE *outfile; // for writing
    outfile = fopen(currentSequence_copy->filename, "w"); // write to this file
    sprintf(msg, "SEQUENCE: \n     NAME: %s", currentSequence->sequenceName);
    fwrite(msg, sizeof (msg[0]), strlen(msg), outfile);
    sprintf(msg, "\n     COUNT: %d", currentSequence->numFrames);
    fwrite(msg, sizeof (msg[0]), strlen(msg), outfile);
    sprintf(msg, "\n     BEGIN: \n          ");
    fwrite(msg, sizeof (msg[0]), strlen(msg), outfile);
    for(i = 0; i < currentSequence->numFrames; i++)
    {
        sprintf(msg,"%6.3f ",currentSequence->exposureTimes[i]);
        fwrite(msg, sizeof (msg[0]), strlen(msg), outfile);
    }
    sprintf(msg, "\n     END: ");
    fwrite(msg, sizeof (msg[0]), strlen(msg), outfile);
    
}
void saveAs(char* file) {
    pthread_mutex_lock(&currentSequence_copy->mx);
    currentSequence_copy->filename = file;
    save();
    pthread_mutex_unlock(&currentSequence_copy->mx);
}

void scale(double ratio) {
    int i;
    pthread_mutex_lock(&currentSequence_copy->mx);
    if (ratio <= 0) return; //Check if ratio is valid
    for (i = 0; i < currentSequence->numFrames; i++)
        currentSequence_copy->exposureTimes[i] *= ratio;
    pthread_mutex_unlock(&currentSequence_copy->mx);

}

const char * findAndReplace_seq(double original, double newValue) {
    int i;
    pthread_mutex_lock(&currentSequence_copy->mx); //Lock Mutex
    char result[50]; //Start with empty string
    char* result2;
    result2 = (char *) calloc(50, sizeof (char*));
    char next[20];
    for (i = 0; i < currentSequence_copy->numFrames; i++) //Look through entire sequence
    {
        if (currentSequence_copy->exposureTimes[i] == original) //Compare current exposure with target
        {
            currentSequence_copy->exposureTimes[i] = newValue; //replace
            sprintf(next, "%d,", i); //format string with current index
            strncat(result, next, sizeof(result));
        }
    }
    pthread_mutex_unlock(&currentSequence_copy->mx); //Unlock mutex
    strcpy(result2, result);
    return result2;
}

void translate(double delta) {
    int i;
    pthread_mutex_lock(&currentSequence_copy->mx); //Lock Mutex
    for (i = 0; i < currentSequence_copy->numFrames; i++) {
        currentSequence_copy->exposureTimes[i] += delta; //Translate exposure
        if (currentSequence_copy->exposureTimes[i] < 0) currentSequence_copy->exposureTimes[i] = 0; //Set any negative exposures to zero
    }
    pthread_mutex_unlock(&currentSequence_copy->mx);
}

void reset_seq() {
    currentSequence_copy->currentFrame = 0;
}

void setNum(int n) {
    sequenceMap[n].num = n;
}

int getNum() {
    return currentSequence_copy->num;
}

int getExposureCount() {
    return currentSequence_copy->numFrames;
}


double getCurrentExposure()
{
    return currentSequence_copy->exposureTimes[currentSequence_copy->currentFrame];
}

/*toString returns a string representation of the whole sequence*/
const char * toString(int n) {
    int i;
    char next[20];
    char* next2;
    char result[50]; //Start with empty string
    char* result2;
    result2 = (char *) calloc(50, sizeof (char*));
    //pthread_mutex_lock(&currentSequence.mx); //Lock Mutex
    next2 = "Sequence: ";
    strcpy(next, next2);
    for(i = 0; i < sequenceMap[n].numFrames; i++)
    {
        sprintf(next, "%6.3f", sequenceMap[n].exposureTimes[i]);
        strncat(result, next, sizeof(result));
    }
    
    //pthread_mutex_unlock(&currentSequence.mx); //Lock Mutex
    strcpy(result2, result);
    return result2;
    

}







