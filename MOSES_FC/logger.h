/* 
 * File:   logger.h
 * Author: byrdie
 *
 * Created on May 21, 2014, 6:06 PM
 */

#ifndef LOGGER_H
#define	LOGGER_H




//#define splice(a, b, c) splice(a, 0, b, 0, c, 0)

#define LOG_PATH "/moses/ramdisk/moses_log.txt"

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define CYAN    "\x1b[36m"
#define NO_COLOR        "\x1b[0m"

void record(const char* message);
void copy_log_to_disk();
void init_logger();





#endif	/* LOGGER_H */

