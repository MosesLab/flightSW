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


#define ON 0xFFFFFFFF
#define OFF 0x0

#define GPIO_BAR_INDEX 2
#define PLX_BUFFER_WIDTH 4

/*Shutter macros*/
#define SHUTTER_OFFSET 0x2C
#define SHUTTER_OPEN_SIM 0x1
#define SHUTTER_CLOSE_SIM 0x1


/*Power macros*/
#define NUM_SUBSYSTEMS 10
#define POWER_OFFSET 0x2C
#define SHUTTER_SIM 0x2
#define ROE_SIM 0x4
#define PREMOD_SIM 0x8
#define TCS_1_SIM 0x10
#define TCS_3_SIM 0x20
#define TCS_2_SIM 0x40
#define TCS_SIM 0x80
#define REG_5V_SIM 0x100
#define REG_12V_SIM 0x200
#define H_ALPHA_SIM 0x400

/*GPIO write variables*/
PLX_DEVICE_OBJECT fpga_dev;
PLX_PCI_BAR_PROP bar_properties;
PLX_ACCESS_TYPE type_bit;


U32 current_power_state;

/*array to hold power pin macros*/
int power_subsystem_arr[NUM_SUBSYSTEMS];

/*poke gpio pins*/
int poke_gpio(U32, U32);
int peek_gpio(U32, U32*);

/*use write gpio to open/close shutter*/
int open_shutter();
int close_shutter();

/*power control functions*/
int set_power(U32, U32);
int get_power(U32, U32*);
void init_power();

/*this enumeration should be changed when we decide on power formats!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
enum power_subsystem_pins{
    shutter_driver,
    roe,
    pre_mod,
    tcs_1,
    tcs_3,
    tcs_2,
    tcs,
    reg_5V,
    reg_12V,
    h_alpha,
};
#endif	/* GPIO_H */

