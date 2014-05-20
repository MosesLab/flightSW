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
unsigned hash(char* s){
    unsigned hashval;
    for(hashval = 0; *s != '\0'; s++){
        hashval = *s + 31 * hashval;
    }
    return hashval % hashsize;
}

/*lookup: look for s in hashtable*/
Node* lookup(Node** hTable, Node* np, char* s){
    np = hTable[hash(s)];
    while(np != NULL){        
        if(strcmp(s, np->name) == 0){
            return np;  //Found
        }
        np = np->next;
    }
    return NULL;        //Not Found
//    for(np = hTable[hash(s)]; np != NULL; np = np->next){
//        if(strcmp(s, np->name) == 0){
//            return np;  //Found
//        }
//    }
//    return NULL;        //Not Found
}

/*install: put name and definition in hashtable*/
Node* installNode(Node** hTable, char* name, hlpControl defn){
    Node* np;
    unsigned hashval;
    if((np = lookup(hTable, np, name)) == NULL){    //Not found
        np = (Node*) malloc(sizeof(*np));
        if(np == NULL || (np->name = strdup(name)) == NULL){
            return NULL;
        }
        hashval = hash(name);
        np->next = hTable[hashval];
        hTable[hashval] = np;
    }
    else{       //Already in hashtable
        free((void*) np->func); //Free previous definition
    }
    if((np->func = defn) == NULL){
        return NULL;
    }
    return np;  
}


