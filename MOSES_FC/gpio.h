/* 
 * File:   gpio.h
 * Author: Roy Smart
 *
 * Created on July 11, 2014, 1:52 PM
 */

#ifndef GPIO_H
#define	GPIO_H

#include "system.h"
#include "lockingQueue.h"


#define ON 0xFFFFFFFF
#define OFF 0x0

#define GPIO_BAR_INDEX 2
#define PLX_BUFFER_WIDTH 4

/*registers for interrupt-driven gpio*/
#define GPIO_I_STATE_REG 0x00000000
#define GPIO_I_INT_ENABLE 0x00000004
#define GPIO_I_INT_REG 0x0000000C
#define GPIO_I_INT_ACK 0x00000008

/*register for output gpio*/
#define GPIO_OUT_REG 0x14
/*Shutter macros*/
//#define SHUTTER_OFFSET 0x14


/*Power macros*/
#define NUM_SUBSYSTEM 10
//#define POWER_OFFSET 0x2C
//#define POWER_OFFSET 0x10
//#define POWER_DIRECTION_OFFSET 0x14

/*Shutter is controlled by VDX GPIO pins*/
#define SHUTTER_OPEN_SIM 0x1
#define SHUTTER_CLOSE_SIM 0x2

/*Power is controlled by FPGA GPIO pins*/
#define LATCH 0x4
#define SHUTTER_SIM 0x8
#define ROE_SIM 0x800000
#define PREMOD_SIM 0x1000000
#define TCS_1_SIM 0x2000000
#define TCS_3_SIM 0x4000000
#define TCS_2_SIM 0x8000000
#define TCS_SIM 0x10000000
#define REG_5V_SIM 0x20000000
#define REG_12V_SIM 0x40000000
#define H_ALPHA_SIM 0x80000000


/*GPIO write variables*/
extern PLX_DEVICE_OBJECT fpga_dev;
extern PLX_PCI_BAR_PROP bar_properties;
extern PLX_ACCESS_TYPE type_bit;



/*array to hold power pin macros*/
U32 power_subsystem_arr[NUM_SUBSYSTEM];

LockingQueue gpio_out_queue;    // Pass gpio values from producers to fpga server
LockingQueue gpio_in_queue;     //Pass GPIO values from fpga server to gpio control

/*poke gpio pins*/
//int write_gpio(U32,U32,U32);
int poke_gpio(U32, U32);
int peek_gpio(U32, U32*);

int handle_gpio_in();

/*use write gpio to open/close shutter*/
int open_shutter();
int close_shutter();


void init_gpio();

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

