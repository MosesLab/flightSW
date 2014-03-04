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
#define GDPKT    'G'
#define BDPKT    'B'
#define HK_RSP   'K'
#define MDAQ_RSP 'Q' 

//Both
#define SHELL    'S'

//Recieved
#define MDAQ_RQS 'M'
#define PWR      'P'
#define HK_RQS   'H'

#define BAD_PACKET	-1
#define GOOD_PACKET	0

#endif	/* HLP_H */

