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

#ifndef SEQUENCE_H
#define	SEQUENCE_H

sequence sequenceMap[5];

sequence constructSequence(char *filepath);
sequence tempSequence;
sequence currentSequence;

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* SEQUENCE_H */


