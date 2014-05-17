/* 
 * File:   hashTable.h
 * Author: Roy Smart
 *
 * Created on May 17, 2014, 4:56 PM
 */

#ifndef NODETYPE_H
#define	NODETYPE_H

typedef struct{
    void* next; //next entry in the chain
    char* name; //defined name
    char* defn; //replacement text
}Node;


#endif	/* HASHTABLE_H */

