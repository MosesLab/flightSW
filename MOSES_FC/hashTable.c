/*
 * File: hashTable.c
 * Author: Roy Smart
 * 
 * Created May, 15th 2014
 * 
 * Used to hash map strings provided by HLP packets and function
 * pointers that execute the appropriate control sequence based off of  
 * information contained within the packets 
 */ 
#include "hashTable.h"

/*hash: form hash value for string s*/
unsigned hash(char* s, int size){
    unsigned hashval;
    for(hashval = 0; *s != '\0'; s++){
        hashval = *s + 31 * hashval;
    }
    return hashval % size;
}

/*lookup: look for s in hashtable*/
node_t* lookup(node_t** hTable, char* s, int size){
    node_t* np = hTable[hash(s, size)];
//    while(np != NULL){        
//        if(strcmp(s, np->name) == 0){
//            return np;  //Found
//        }
//        np = np->next;
//    }
//    return NULL;        //Not Found
    for(np = hTable[hash(s, size)]; np != NULL; np = np->next){
        if(strcmp(s, np->name) == 0){
            return np;  //Found
        }
    }
    return NULL;        //Not Found
}

/*install: put name and definition in hashtable*/
node_t* installNode(node_t** hTable, char* name, void * defn, int size){
    node_t* np;
    unsigned hashval;
    if((np = lookup(hTable, name, size)) == NULL){    //Not found
        np = (node_t*) malloc(sizeof(*np));
        if(np == NULL || (np->name = strdup(name)) == NULL){
            return NULL;
        }
        hashval = hash(name, size);
        np->next = hTable[hashval];
        hTable[hashval] = np;
    }
    else{       //Already in hashtable
        free((void*) np->def); //Free previous definition
    }
    if((np->def = defn) == NULL){
        return NULL;
    }
    return np;  
}

char* concat(int count, ...)
{
    va_list ap;
    int i;

    // Find required length to store merged string
    int len = 1; // room for NULL
    va_start(ap, count);
    for(i=0 ; i<count ; i++)
        len += strlen(va_arg(ap, char*));
    va_end(ap);

    // Allocate memory to concat strings
    char *merged = calloc(sizeof(char),len);
    int null_pos = 0;

    // Actually concatenate strings
    va_start(ap, count);
    for(i=0 ; i<count ; i++)
    {
        char *s = va_arg(ap, char*);
        strcpy(merged+null_pos, s);
        null_pos += strlen(s);
    }
    va_end(ap);

    return merged;
}
