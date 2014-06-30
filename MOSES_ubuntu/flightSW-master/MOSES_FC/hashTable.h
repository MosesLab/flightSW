/* 
 * File:   hashTable.h
 * Author: birdie
 *
 * Created on May 20, 2014, 8:45 AM
 */

#ifndef HASHTABLE_H
#define	HASHTABLE_H

#include "system.h"


/*functions*/
unsigned hash(char*, int);
node_t* lookup(node_t**, char* s, int);
node_t* installNode(node_t**, char*, void *, int);

char* concat(int, ...);

#endif	/* HASHTABLE_H */

