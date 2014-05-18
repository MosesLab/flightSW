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
#include "system.h"
#include "nodeType.h"

#define HASHSIZE 101


/*functions*/
unsigned hash(char*);
Node* lookup(Node**, char*);
Node* install(Node**, char*, char*);
char* strduplicate(char*);

/*hash: form hash value for string s*/
unsigned hash(char* s){
    unsigned hashval;
    for(hashval = 0; *s != '\0'; s++){
        hashval = *s + 31 * hashval;
    }
    return hashval % HASHSIZE;
}

/*lookup: look for s in hashtable*/
Node* lookup(Node** hTable, char* s){
    Node* np;
    for(np = hTable[hash(s)]; np != NULL; np = np->next){
        if(strcmp(s, np->name) == 0){
            return np;  //Found
        }
    }
    return NULL;        //Not Found
}

/*install: put name and definition in hashtable*/
Node* install(Node** hTable, char* name, char* defn){
    Node* np;
    unsigned hashval;
    if((np = lookup(hTable, name)) == NULL){    //Not found
        np = (Node*) malloc(sizeof(*np));
        if(np == NULL || (np->name = strduplicate(name)) == NULL){
            return NULL;
        }
        hashval = hash(name);
        np->next = hTable[hashval];
        hTable[hashval] = np;
    }
    else{       //Already in hashtable
        free((void*) np->defn); //Free previous definition
    }
    if((np->defn = strduplicate(defn)) == NULL){
        return NULL;
    }
    return np;  
}

/*Make a duplicate of string s*/
char* strduplicate(char* s){
    char* p;
    p = (char*)malloc(strlen(s)+1);
    if(p != NULL){
        strcpy(p,s);
    }
    return p;
}

