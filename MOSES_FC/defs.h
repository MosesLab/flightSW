/* 
 * File:   defs.h
 * Author: birdie
 *
 * Created on May 15, 2014, 1:50 PM
 */

#ifndef DEFS_H
#define	DEFS_H

#define FALSE 0
#define TRUE 1

enum tmu{
    UDataStart,
    UDataStop,
    UDark1,
    UDark2,
    UDark3,
    UDark4,
    USleep,
    UWake,
    UTest,    
    TDataStart,
    TDataStop,
    TDark2,
    TDark4,
    TSleep,
    NoFunc      //Used by the map to return the absence of a matching function
};


#endif	/* DEFS_H */

