/**************************************************
* Author: David Keltgen                           *
* Company: Montana State University: MOSES LAB    *
* File Name: roe.h                                *
* Date: June 4 2014                               *
* Description: Software representation of the     *
*              roe.                               *
**************************************************/

#ifndef ROE_H
#define	ROE_H


#include "system.h"

struct ReadOutElectronics
{
    int isActive;
    int activate;
    int deactivate;
    int exitDefault;
    int selfTestMode;
    int stimOn;
    int stimOff;
    int readOut;
    int flush;
    int getHK;
    int getMode;
    int reset;
    char* getAE;
    int manualWrite;
    
    int active;
    int state;
    int atoh; //ascii to hex
    int readRoe;
    int recieveAck;
    int roeLink;
    int mode; //default or manual
    pthread_mutex_t mx;
};

struct ReadOutElectronics roe;

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* ROE_H */

