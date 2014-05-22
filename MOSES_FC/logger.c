/* 
 * File:     ProgramLogger.c
 * Author: David Keltgen
 * creates a log file that can be used to track the programs behavior 
 * Created on May 19, 2014, 11:58 AM
 */

#include "ProgramLogger.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>



void record(const char* message) {

	FILE *outfile;     // for writing
    outfile = fopen("programLog.txt", "a");                       // write to this file
    time_t now;
    time(&now);
    char* theTime;    
	
    theTime = ctime(&now);
    theTime[ strlen(theTime) - 1 ] = ' ';  //Replace the '\n' with a space
   
    /*write the date and the message to the file*/
    fwrite(theTime, sizeof(theTime[0]), strlen(theTime), outfile);
	//fwrite(' ', 1, 1, outfile);
    fwrite( message, sizeof(message[0]), strlen(message), outfile);
   
    /* we are done with file, close it */
    fclose(outfile);
}
