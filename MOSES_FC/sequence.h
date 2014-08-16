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

sequence_t constructSequence(char *filepath);
void loadSequences();

