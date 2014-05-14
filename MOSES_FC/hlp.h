/* 
 * File:   hlp.h
 * Author: byrdie
 *
 * Created on February 15, 2014, 2:15 PM
 */

#ifndef HLP_H
#define	HLP_H

//Packet Types --SENT
#define CONTROL  'C'
#define TIMER    'T'
#define UPLINK   'U'
#define GDPKT    "G"
#define BDPKT    "B"
#define HK_RSP   'K'
#define MDAQ_RSP 'Q' 

//Both
#define SHELL    'S'

//Recieved
#define MDAQ_RQS 'M'
#define PWR      'P'
#define HK_RQS   'H'


//TIMER/UPLINK Subtypes
#define DARK1      "DK1"
#define DARK2      "DK2"
#define DARK3      "DK3"
#define DARK4      "DK4"
#define DATASTART  "DST"
#define DATASTOP   "DSP"
#define SLEEP	   "SLP"
#define WAKE	   "WAK"
#define TEST	   "TST"
#define ACK        "ACK"

//#define BAD_PACKET	-1
//#define GOOD_PACKET	0
#define BAD_PACKET	0
#define GOOD_PACKET	1

#endif	/* HLP_H */

