/* 
 * File:   ScienceTimeline.h
 * Author: david
 *
 * Created on May 28, 2014, 11:34 AM
 */

#include <signal.h>
#include <pthread.h>
#include "defs.h"
//#include "system.h"

extern int seq_map_size;
extern sequence_t *sequenceMap;
extern sequence_t tempSequence;
extern sequence_t * currentSequence;

sequence_t constructSequence(char *filepath);
void loadSequences();
void reload(char* file);
int findAndJump_seq(double);
const char * findAndReplace_seq();
void jump(int frame);
void save();
void saveAs();
void scale();
void translate(double);
void reset_seq();
void setNum(int n);
int getNum();
int getExposureCount();
double getCurrentExposure();
const char * toString();

/* Functions present in flightSW_2005 but not implemented
 * hasMoreExposures(): not really relevant since we will 
 *                     be going through every exposure 
 * 
 * getNextExposure(): duration is the value of the current
 *                    exposures time,
 *
 * 
 *  
 *   
 * */




