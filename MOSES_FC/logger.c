/* 
 * File:     ProgramLogger.c
 * Author: David Keltgen
 * creates a log file that can be used to track the programs behavior 
 * Created on May 19, 2014, 11:58 AM
 */

#include "system.h"
#include "logger.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
//#include <string.h>

void record(const char* message) {

    FILE *outfile; // for writing
    outfile = fopen("/etc/moses_log.txt", "a"); // write to this file
    time_t now;
    time(&now);
    char* theTime;

    theTime = ctime(&now);
    theTime[ strlen(theTime) - 1 ] = ' '; //Replace the '\n' with a space
    
    fwrite("[", 1, 2, outfile);

    /*write the date and the message to the file*/
    fwrite(theTime, sizeof (theTime[0]), strlen(theTime), outfile);
    
    /*write the name of the thread to a file*/
    char thread_name[16];
    prctl(PR_GET_NAME, &thread_name, 0, 0, 0);
    fwrite(&thread_name, sizeof(thread_name[0]), strlen(thread_name), outfile);
    
    fwrite("]", 1, 2, outfile);
    
    char * delim = " : ";
    fwrite(delim, sizeof(char), strlen(delim), outfile);
    
    //fwrite(' ', 1, 1, outfile);
    fwrite(message, sizeof (message[0]), strlen(message), outfile);

    /* we are done with file, close it */
    fclose(outfile);
}

//void srecord(int count, ...)
//{
//    va_list ap;
//    int i;
//    FILE *outfile; // for writing
//    outfile = fopen("/etc/moses_log.txt", "a"); // write to this file
//    time_t now;
//    time(&now);
//    char* theTime;
//    
//    theTime = ctime(&now);
//    theTime[ strlen(theTime) - 1 ] = ' '; //Replace the '\n' with a space
//
//    // Find required length to store merged string
//    int len = 1; // room for NULL
//    va_start(ap, count);
//    for(i=0 ; i<count ; i++)
//        len += strlen(va_arg(ap, char*));
//    va_end(ap);
//
//    // Allocate memory to concat strings
//    char *merged = calloc(sizeof(char),len);
//    int null_pos = 0;
//
//    // Actually concatenate strings
//    va_start(ap, count);
//    for(i=0 ; i<count ; i++)
//    {
//        char *s = va_arg(ap, char*);
//        strcpy(merged+null_pos, s);
//        null_pos += strlen(s);
//    }
//    va_end(ap);
//
//    /*write the date and the message to the file*/
//    fwrite(theTime, sizeof (theTime[0]), strlen(theTime), outfile);
//    
//    /*write the name of the thread to a file*/
//    char thread_name[16];
//    prctl(PR_GET_NAME, &thread_name, 0, 0, 0);
//    
//    //fwrite(' ', 1, 1, outfile);
//    fwrite(merged, sizeof (merged[0]), strlen(merged), outfile);
//
//    /* we are done with file, close it */
//    fclose(outfile);
//
//}