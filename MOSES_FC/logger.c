/* 
 * File:     ProgramLogger.c
 * Author: David Keltgen
 * creates a log file that can be used to track the programs behavior 
 * Created on May 19, 2014, 11:58 AM
 */

#include "system.h"
#include "logger.h"

unsigned int log_sz = 0;

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


    /* Get data for image details*/
    char ddate[255];
    time_t curTime = time(NULL);
    struct tm *broken = localtime(&curTime);
    strftime(ddate, 20, "%y-%m-%d", broken); //get date


    log_sz += fwrite("[", 1, 1, outfile);

    /*write the date and the message to the file*/
    log_sz += fwrite(theTime, sizeof (theTime[0]), strlen(theTime), outfile);

    log_sz += fwrite("] ", 1, 2, outfile);

    /*write the name of the thread to a file*/
    char thread_name[16];
    prctl(PR_GET_NAME, &thread_name, 0, 0, 0);
    log_sz += fwrite(&thread_name, sizeof (thread_name[0]), strlen(thread_name), outfile);



    char * delim = " : ";
    log_sz += fwrite(delim, sizeof (char), strlen(delim), outfile);

    //fwrite(' ', 1, 1, outfile);
    log_sz += fwrite(message, sizeof (message[0]), strlen(message), outfile);

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
    sprintf(new_path, "/moses/log_backups/moseslog_%d_%d_%d.txt", tm->tm_mon + 1, tm->tm_mday, tm->tm_year);


    int pipefd[2];
    int result;
    FILE *in_file;
    FILE *out_file;

    result = pipe(pipefd);

    in_file = fopen(LOG_PATH, "rb");
    out_file = fopen(new_path, "wb");

    //    result = splice(fileno(in_file), 0, pipefd[1], NULL, 4096, SPLICE_F_MORE | SPLICE_F_MOVE);
    result = splice(fileno(in_file), 0, pipefd[1], NULL, log_sz, SPLICE_F_MOVE);
    printf("%d\n", result);

    //    result = splice(pipefd[0], NULL, fileno(out_file), 0, 4096, SPLICE_F_MORE | SPLICE_F_MOVE);
    result = splice(pipefd[0], NULL, fileno(out_file), 0, log_sz, SPLICE_F_MOVE);
    printf("%d\n", result);

    if (result == -1)
        printf("%d - %s\n", errno, strerror(errno));

    close(pipefd[0]);
    close(pipefd[1]);
    fclose(in_file);
    fclose(out_file);

    sprintf(msg, "Backup log %s successfully written to disk\n", new_path);
    record(msg);
}