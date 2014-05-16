/* 
 * File:   controlFuncs.h
 * Author: Roy Smart
 * 
 * controlFuncs contains the functions that define what the flight computer 
 * does upon receiving a timer, uplink, or HLP packet
 *
 * Created on May 14, 2014, 7:30 PM
 */

#ifndef CONTROLFUNCS_H
#define	CONTROLFUNCS_H

#include "system.h"
#include "hlp.h"
#include "packetType.h"

/*
 * Array containing uplink subtypes. Used in a map from uplink functions to 
 * their corresponding uplink subtype
 */ 
char* uplinkMap[] = {DATASTART, DATASTOP, DARK1, DARK2, DARK3, DARK4, SLEEP, WAKE, TEST};

/*typedef for array of function pointers for HLP uplinks*/

/*hlp control functions*/
int hlpUplink(Packet*);

/*Timer control functions*/
void tDataStart();
void tDataStop();
void tDark2();
void tDark4();
void tSleep();

/*uplink control functions*/
void uplinkInit();


void uDataStart();
void uDataStop();
void uDark1();
void uDark2();
void uDark3();
void uDark4();
void uSleep();
void uWake();
void uTest();



#endif	/* CONTROLFUNCS_H */

