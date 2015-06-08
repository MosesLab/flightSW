/* 
 * File:   logger.h
 * Author: byrdie
 *
 * Created on May 21, 2014, 6:06 PM
 */

#ifndef LOGGER_H
#define	LOGGER_H

#include <stdio.h>

//#define splice(a, b, c) splice(a, 0, b, 0, c, 0)

#define LOG_PATH "/moses/ramdisk/moses_log.txt"

void record(const char* message);
void copy_log_to_disk();

#endif	/* LOGGER_H */

