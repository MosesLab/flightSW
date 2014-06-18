
#include "roe_image.h"

roeImage_t* constructImage(char* name, int ** data, int size){
    int dataSize;
    
    //itoah(dataSize, dataLength, 2);  //convert length from int to string
    
    /*allocate space for packet*/
    roeImage_t* r;
    if((r = (roeImage_t*) malloc(sizeof(roeImage_t))) == NULL){
        record("malloc failed to allocate image");
    }
    r->name = name;
    //getCurrentTime(r->timeStamp);
    //memcpy(r->name, name, size);
    r->dataSize = size;
    if(&data != NULL) memcpy(r->data, data, dataSize + 1);
    r->status = GOOD_PACKET;    
    r->next = NULL;
    
    return r;
}

/*records a packet to a log file*/
void recordImage(roeImage_t* r){
    char* pString;
    asprintf(&pString, "%s%s%s\n", r->name, r->dataSize);
    record(pString);
    free(pString);      //clean up string after recording
}

/*needs to be integrated with sendTM.c*/
void sendImage(roeImage_t * r, int fd){
    
    char start = STARTBYTE;
    char end = ENDBYTE;
    char eof = 0x04;
    
    sendData(&start, 1, fd);
    sendData(r->name, 1, fd);
    sendData((char*)r->data, r->dataSize, fd);
    sendData(&end, 1, fd);
    sendData(&eof, 1, fd);
}