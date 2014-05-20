/* 
 * File:   hashTable.h
 * Author: birdie
 *
 * Created on May 20, 2014, 8:45 AM
 */

#ifndef HASHTABLE_H
#define	HASHTABLE_H

#include "system.h"

extern int hashsize;

/*functions*/
unsigned hash(char*);
Node* lookup(Node**, char*);
Node* installNode(Node**, char*, hlpControl);

char* concat(int, ...)

#endif	/* HASHTABLE_H */

