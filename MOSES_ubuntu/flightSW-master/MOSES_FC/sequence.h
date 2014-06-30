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

int seq_map_size;

sequence_t *sequenceMap;

sequence_t constructSequence(char *filepath);
sequence_t tempSequence;
sequence_t currentSequence;

