/* 
 * File:   gpio.h
 * Author: Roy Smart
 *
 * Created on July 11, 2014, 1:52 PM
 */

#ifndef GPIO_H
#define	GPIO_H

#include "system.h"
#include "plx/PlxApi.h"
#include "plx/PlxInit.h"

#define GPIO_BAR_INDEX 2
#define SHUTTER_OFFSET 0x2C
#define SHUTTER_OPEN_SIM 0xF
#define SHUTTER_CLOSE_SIM 0x1
#define PLX_BUFFER_WIDTH 4
/*GPIO write variables*/

PLX_DEVICE_OBJECT device;
PLX_PCI_BAR_PROP bar_properties;
PLX_ACCESS_TYPE type_bit;




/*poke gpio pins*/
int poke_gpio(U32, U32);

/*use write gpio to open/close shutter*/
int open_shutter();
int close_shutter();

#endif	/* GPIO_H */

