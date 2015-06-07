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
    char input_data[256];

    FILE* file;
    tempSequence.sequenceName = (char *) malloc(100);
    strncpy(tempSequence.sequenceName, filepath, 100);
    if (strstr(tempSequence.sequenceName, "data") != NULL) {
        tempSequence.seq_type = 1; // type is data
    }
    file = fopen(filepath, "r");

    int rc;

    rc = fscanf(file, "%s\n", input_data); //scan "SEQUENCE"
    if (rc < 0) record("Error reading sequence\n");

    rc = fscanf(file, "%s %s\n", input_data, input_data); //scan "NAME" and filepath
    if (rc < 0) record("Error reading sequence\n");

    rc = fscanf(file, "%s %ud\n", input_data, &tempSequence.numFrames); //scan "COUNT" and number of exposures 
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
    char filepath[] = PATH "sequence"; //default folder path
    int numfiles = 0;
    char strdir[256];
    char msg[256];

    sprintf(msg, "%s/\n", filepath);
    record(msg);
    /*Open up all files that */
    DIR *d;
    struct dirent *dir;
    d = opendir(filepath);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".seq") != NULL) {
                numfiles++;
            }
        }
        closedir(d);
    }

    sequenceMap = (sequence_t *) malloc(numfiles * sizeof (sequence_t));

    d = opendir(filepath);
    int i = 0;
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".seq") != NULL) {
                sprintf(strdir, "%s/%s", filepath, dir->d_name);
                sprintf(msg, "%s/%s\n", filepath, dir->d_name);
                record(msg);
                sequenceMap[i] = constructSequence(strdir);
                i++;
                //seq_map_size = i;  //Need to send this variable to science_timeline
            }
        }

        closedir(d);

    }
}


void reload(char* file) {
    pthread_mutex_lock(&currentSequence->mx);
    currentSequence->corrupt = FALSE;

    if (strstr(file, ".seq") != NULL) {
        constructSequence(file);
    }

    pthread_mutex_lock(&currentSequence->mx);

}

int findAndJump_seq(double num) {
    pthread_mutex_lock(&currentSequence->mx);
    int i;
    for(i = 0; i < currentSequence->numFrames; i++)
    {
        if (currentSequence->exposureTimes[i] == num)
        {
            currentSequence->currentFrame = i;
        }
    }
    pthread_mutex_unlock(&currentSequence->mx);
    
    if(currentSequence->exposureTimes[currentSequence->currentFrame] == num)
    {
        return currentSequence->currentFrame;
    }
    else
    {
        return -1;
    }
}

int jump(int frame) {
    pthread_mutex_lock(&currentSequence->mx);
    if(frame < currentSequence->numFrames && frame > -1)
    {
        currentSequence->currentFrame = frame;
    }
    
    pthread_mutex_unlock(&currentSequence->mx);
    if(currentSequence->currentFrame == frame)
    {
        return currentSequence->currentFrame;
    }
    else
    {
        return -1;
    }
}

void save() {
    int i;
    char msg[100];
    FILE *outfile; // for writing
    outfile = fopen(currentSequence->filename, "w"); // write to this file
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
    pthread_mutex_lock(&currentSequence->mx);
    char s1[100];
    sprintf(s1, "sequence/");
    strcat(s1,file);
    currentSequence->filename = s1;
    save();
    pthread_mutex_unlock(&currentSequence->mx);
}

void scale(double ratio) {
    int i;
    pthread_mutex_lock(&currentSequence->mx);
    if (ratio <= 0) return; //Check if ratio is valid
    for (i = 0; i < currentSequence->numFrames; i++)
        currentSequence->exposureTimes[i] *= ratio;
    pthread_mutex_unlock(&currentSequence->mx);

}

const char * findAndReplace_seq(double original, double newValue) {
    int i;
    pthread_mutex_lock(&currentSequence->mx); //Lock Mutex
    char result[50]; //Start with empty string
    char* result2;
    result2 = (char *) calloc(50, sizeof (char*));
    char next[20];
    for (i = 0; i < currentSequence->numFrames; i++) //Look through entire sequence
    {
        if (currentSequence->exposureTimes[i] == original) //Compare current exposure with target
        {
            currentSequence->exposureTimes[i] = newValue; //replace
            sprintf(next, "%d ", i); //format string with current index
            strncat(result, next, i*2);
        }
    }
    pthread_mutex_unlock(&currentSequence->mx); //Unlock mutex
    strcpy(result2, result);
    return result2;
}

void translate(double delta) {
    int i;
    pthread_mutex_lock(&currentSequence->mx); //Lock Mutex
    for (i = 0; i < currentSequence->numFrames; i++) {
        currentSequence->exposureTimes[i] += delta; //Translate exposure
        if (currentSequence->exposureTimes[i] < 0) currentSequence->exposureTimes[i] = 0; //Set any negative exposures to zero
    }
    pthread_mutex_unlock(&currentSequence->mx);
}

void reset_seq() {
    currentSequence->currentFrame = 0;
}

void setNum(int n) {
    sequenceMap[n].num = n;
}

int getNum() {
    return currentSequence->num;
}

int getExposureCount() {
    return currentSequence->numFrames;
}


double getCurrentExposure()
{
    return currentSequence->exposureTimes[currentSequence->currentFrame];
}

/*toString returns a string representation of the whole sequence*/
const char * toString(int n) {
    int i;
    char next[20];
    //char* next2;
    char result[256]; //Start with empty string
    char* result2;
    result2 = (char *) calloc(50, sizeof (char*));
    //pthread_mutex_lock(&currentSequence.mx); //Lock Mutex
    sprintf(result,"Sequence: ");
    //strcpy(next, next2);
    for(i = 0; i < sequenceMap[n].numFrames; i++)
    {
        sprintf(next, "%6.3f", sequenceMap[n].exposureTimes[i]);
        strncat(result, next, i*2);
    }
    
    //pthread_mutex_unlock(&currentSequence.mx); //Lock Mutex
    strcpy(result2, result);
    return result2;
    

} 







