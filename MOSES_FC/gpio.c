/*
 * File: gpio.c
 * Author: Roy Smart
 * MOSES LAB
 * Montana State University
 * 
 */

#include "gpio.h"

int write_gpio(U32 offset, U32 data){
    RETURN_CODE rc;     //return code
    
    /*Get and open PLX FPGA device*/
    rc = GetAndOpenDevice(&device, 0x9056);
    
    return TRUE;
}