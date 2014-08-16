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
#include <sys/io.h>


#define ON 0xFFFFFFFF
#define OFF 0x0

#define GPIO_BAR_INDEX 2
#define PLX_BUFFER_WIDTH 4

/*registers for interrupt-driven gpio*/
#define GPIO_I_STATE_REG 0x00000000
#define GPIO_I_INT_ENABLE 0x00000004
#define GPIO_I_INT_REG 0x0000000C
#define GPIO_I_INT_ACK 0x00000008

/*Control GPIO input macros*/
#define NUM_CONTROL_GPIO 14

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
#define SHUTTER_ENABLE 0x98
#define SHUTTER_OFFSET 0x78
#define SHUTTER_OPEN 0x1
#define SHUTTER_CLOSE 0x2
#define SHUTTER_PULSE 200000 //usec

/*Power is controlled by FPGA GPIO pins*/
//#define SHUTTER_SIM     0x00000001
//#define ROE_SIM         0x00000002
//#define PREMOD_SIM      0x00000004
//#define TCS_1_SIM       0x00000008
//#define TCS_3_SIM       0x00000010
//#define TCS_2_SIM       0x00000020
//#define TCS_SIM         0x00000040
//#define REG_5V_SIM      0x00000080
//#define REG_12V_SIM     0x00000100
//#define H_ALPHA_SIM     0x00000200
//#define LATCH           0x00000400


/*requests for data are inicated by writina all ones to the gpio out queue*/
#define REQ_POWER       0xFFFFFFFF

/*GPIO write variables*/
extern PLX_DEVICE_OBJECT fpga_dev;
extern PLX_PCI_BAR_PROP bar_properties;
extern PLX_ACCESS_TYPE  type_bit;
extern U8               bar_index;
extern U32              sz_buffer;


/*array to hold power pin macros*/
U32 power_subsystem_mask[NUM_SUBSYSTEM];
U32 gpio_control_mask[NUM_CONTROL_GPIO];

LockingQueue gpio_out_queue;    // Pass gpio values from producers to fpga server
LockingQueue gpio_in_queue;     //Pass GPIO values from fpga server to gpio control

/*poke gpio pins*/
int poke_gpio(U32, U32);
int peek_gpio(U32, U32*);

int handle_gpio_in();

/*use write gpio to open/close shutter*/
void open_shutter();
void close_shutter();

/*initialize gpio for reading and writing*/
void init_gpio();
void init_shutter();

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

