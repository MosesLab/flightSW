/* 
 * File:     ProgramLogger.c
 * Author: David Keltgen
 * creates a log file that can be used to track the programs behavior 
 * Created on May 19, 2014, 11:58 AM
 */

#include "system.h"
#include "logger.h"

void record(const char* message) {

    FILE *outfile; // for writing
    outfile = fopen(LOG_PATH, "a"); // write to this file
    time_t now;
    time(&now);
    char theTime[255];

    /*micro time*/
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday(&tv, &tz);
    tm = localtime(&tv.tv_sec);
    sprintf(theTime, "%d:%02d:%02d:%03d", tm->tm_hour, tm->tm_min, tm->tm_sec, (int) (tv.tv_usec / 1000));


    fwrite("[", 1, 1, outfile);

    /*write the date and the message to the file*/
    fwrite(theTime, sizeof (theTime[0]), strlen(theTime), outfile);

    fwrite("] ", 1, 2, outfile);

    /*write the name of the thread to a file*/
    char thread_name[16];
    prctl(PR_GET_NAME, &thread_name, 0, 0, 0);
    fwrite(&thread_name, sizeof (thread_name[0]), strlen(thread_name), outfile);



    char * delim = " : ";
    fwrite(delim, sizeof (char), strlen(delim), outfile);

    //fwrite(' ', 1, 1, outfile);
    fwrite(message, sizeof (message[0]), strlen(message), outfile);

    /* we are done with file, close it */
    fclose(outfile);
}

void copy_log_to_disk() {
    time_t now;
    time(&now);
    char msg[255];
    char new_path[255];

    /*micro time*/
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday(&tv, &tz);
    tm = localtime(&tv.tv_sec);
    sprintf(new_path, "/moses/log_backups/moses_log_%d_%d_%02d.txt", tm->tm_mon + 1, tm->tm_mday, tm->tm_year);

    /*open pipe*/
    int p[2];
    int rc = pipe(p);
    if (rc != 0) {
        record("*ERROR* Failed to open pipe for log backup!\n");
    }

    /*open both log and backup log */
    int out = open(new_path, O_WRONLY);
    int in = open(LOG_PATH, O_RDONLY);
    while (splice(p[0], out, splice(in, p[1], 4096)) > 0);

    sprintf(msg, "Backup log %s successfully written to disk\n", new_path);
}