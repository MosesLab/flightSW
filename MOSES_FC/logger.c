/* 
 * File:     ProgramLogger.c
 * Author: David Keltgen
 * creates a log file that can be used to track the programs behavior 
 * Created on May 19, 2014, 11:58 AM
 */

#define _GNU_SOURCE 

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sched.h>
#include "moses_threads.h"
#include "hashTable.h"
#include "logger.h"
#include "defs.h"




unsigned int cur_sz = 0;
unsigned int tot_sz = 0;

node_t** thread_strings_hash_table;
int ts_hash_sz;

char backup_path[255];

void record(const char* message) {

    FILE *outfile; // for writing
    outfile = fopen(LOG_PATH, "a"); // write to this file
//    FILE *backup; // for writing
//    backup = fopen(backup_path, "a"); // write to this file


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

    cur_sz += fwrite("[", 1, 1, outfile);
//    cur_sz += fwrite("[", 1, 1, backup);


    /*write the date and the message to the file*/
    cur_sz += fwrite(theTime, sizeof (theTime[0]), strlen(theTime), outfile);
//    cur_sz += fwrite(theTime, sizeof (theTime[0]), strlen(theTime), backup);

    cur_sz += fwrite("] ", 1, 2, outfile);
//    cur_sz += fwrite("] ", 1, 2, backup);

    /*write the name of the thread to a file*/
    char thread_name[16];
    prctl(PR_GET_NAME, &thread_name, 0, 0, 0);

    /*Use thread name to find appropriate color in hash table*/
    char * color;
    node_t* np = lookup(thread_strings_hash_table, thread_name, ts_hash_sz); //Lookup corresponding function in table
    if(np == NULL){
        color = NO_COLOR;
    } else {
        color = (char *) np->def;
    }

    cur_sz += fwrite(color, sizeof (color[0]), strlen(color), outfile);
    
    cur_sz += fwrite(thread_name, sizeof (thread_name[0]), strlen(thread_name), outfile);
//    cur_sz += fwrite(thread_name, sizeof (thread_name[0]), strlen(thread_name), backup);
    
    cur_sz += fwrite(no_color, sizeof (no_color[0]), strlen(no_color), outfile);

    char * delim = " : ";
    cur_sz += fwrite(delim, sizeof (char), strlen(delim), outfile);
//    cur_sz += fwrite(delim, sizeof (char), strlen(delim), backup);

    //fwrite(' ', 1, 1, outfile);
    cur_sz += fwrite(message, sizeof (message[0]), strlen(message), outfile);
//    cur_sz += fwrite(message, sizeof (message[0]), strlen(message), backup);

    /* we are done with file, close it */
    fclose(outfile);
//    fclose(backup);
}


void init_logger() {
    /*micro time*/
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday(&tv, &tz);
    tm = localtime(&tv.tv_sec);
    sprintf(backup_path, "/moses/log_backups/moseslog_%d-%d-%d.txt", tm->tm_mon + 1, tm->tm_mday, 1900 + tm->tm_year);

    char ** thread_strings = malloc(sizeof (char*) * num_threads);
    char ** thread_colors = malloc(sizeof (char*) * 10);
    ts_hash_sz = num_threads * 2;

    /* Allocate strings to denote threads in log file*/
    thread_strings[hlp_control_thread] = HLP_CNTL;
    thread_strings[hlp_down_thread] = HLP_DOWN;
    thread_strings[gpio_control_thread] = GPIO_CNTL;
    thread_strings[hlp_shell_thread] = HLP_SHELL;
    thread_strings[sci_timeline_thread] = SCI_TIMELINE;
    thread_strings[telem_thread] = TELEM;
    thread_strings[image_writer_thread] = IMG_WRITER;
    thread_strings[fpga_server_thread] = FPGA_SERVER;

    thread_colors[hlp_control_thread] = GREEN;
    thread_colors[hlp_down_thread] = GREEN;
    thread_colors[gpio_control_thread] = GREEN;
    thread_colors[hlp_shell_thread] = GREEN;
    thread_colors[sci_timeline_thread] = CYAN;
    thread_colors[telem_thread] = YELLOW;
    thread_colors[image_writer_thread] = YELLOW;
    thread_colors[fpga_server_thread] = BLUE;

    /*init memory for log strings hash table*/
    /*initialize memory for configuration hash table*/
    if ((thread_strings_hash_table = calloc(ts_hash_sz, sizeof(node_t))) == NULL) {
        printf("calloc failed to allocate hash table\n");
    }

    /*fill hash table with array of strings matching indices for configuration values*/
    uint i;
    for (i = 0; i < num_threads; i++) {

        /*insert node into hash table*/
        installNode(thread_strings_hash_table, thread_strings[i], thread_colors[i], ts_hash_sz);
    }
}
