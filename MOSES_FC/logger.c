/* 
 * File:     ProgramLogger.c
 * Author: David Keltgen
 * creates a log file that can be used to track the programs behavior 
 * Created on May 19, 2014, 11:58 AM
 */

#include "system.h"
#include "logger.h"

unsigned int cur_sz = 0;
unsigned int tot_sz = 0;

void record(const char* message) {

    FILE *outfile; // for writing
    outfile = fopen(LOG_PATH, "a"); // write to this file
    time_t now;
    time(&now);
    char theTime[255];
    char * no_color = "\x1b[0m";

    /*micro time*/
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday(&tv, &tz);
    tm = localtime(&tv.tv_sec);
    sprintf(theTime, "%d:%02d:%02d.%03d", tm->tm_hour, tm->tm_min, tm->tm_sec, (int) (tv.tv_usec / 1000));


    /* Get data for image details*/
    char ddate[255];
    time_t curTime = time(NULL);
    struct tm *broken = localtime(&curTime);
    strftime(ddate, 20, "%y-%m-%d", broken); //get date


    cur_sz += fwrite("[", 1, 1, outfile);

    /*write the date and the message to the file*/
    cur_sz += fwrite(theTime, sizeof (theTime[0]), strlen(theTime), outfile);

    cur_sz += fwrite("] ", 1, 2, outfile);

    /*write the name of the thread to a file*/
    char thread_name[16];
    prctl(PR_GET_NAME, &thread_name, 0, 0, 0);
    cur_sz += fwrite(&thread_name, sizeof (thread_name[0]), strlen(thread_name), outfile);
    cur_sz += fwrite(no_color, sizeof (no_color[0]), strlen(no_color), outfile);




    char * delim = " : ";
    cur_sz += fwrite(delim, sizeof (char), strlen(delim), outfile);

    //fwrite(' ', 1, 1, outfile);
    cur_sz += fwrite(message, sizeof (message[0]), strlen(message), outfile);

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
    sprintf(new_path, "/moses/log_backups/moseslog_%d-%d-%d.txt", tm->tm_mon + 1, tm->tm_mday, 1900 + tm->tm_year);


    int pipefd[2];
    int result;
//    FILE *in_file;
//    FILE *out_file;

    result = pipe(pipefd);

//    in_file = fopen(LOG_PATH, "r");
//    out_file = fopen(new_path, "w");
    
    int in_fd = open(LOG_PATH, O_RDONLY);
    int out_fd = open(new_path, O_WRONLY);

    /*seek to the appropriate position in the log*/
    off64_t in_offset = lseek64(in_fd, cur_sz, SEEK_END);
    printf("In offset is: %d\n", (int) in_offset);
    if (result == -1)
        printf("%d - %s\n", errno, strerror(errno));

    /* Move the cursor to the end of the backup log*/
    off64_t out_offset = lseek64(out_fd, 0, SEEK_END);
    printf("Out offset is: %d\n", (int) out_offset);
    if (out_offset == -1)
        printf("%d - %s\n", errno, strerror(errno));

    //    result = splice(fileno(in_file), 0, pipefd[1], NULL, 4096, SPLICE_F_MORE | SPLICE_F_MOVE);
    result = splice(in_fd, NULL, pipefd[1], NULL, cur_sz, SPLICE_F_MOVE);
    printf("%d\n", result);

    //    result = splice(pipefd[0], NULL, fileno(out_file), 0, 4096, SPLICE_F_MORE | SPLICE_F_MOVE);
    result = splice(pipefd[0], NULL, out_fd, &out_offset, result, SPLICE_F_MOVE);
    printf("%d\n", result);

    if (result == -1)
        printf("%d - %s\n", errno, strerror(errno));

    close(pipefd[0]);
    close(pipefd[1]);
    close(in_fd);
    close(out_fd);

    sprintf(msg, "Wrote %d bytes to backup log %s\n", result, new_path);
    record(msg);

    tot_sz += cur_sz;
    cur_sz = 0;


}
