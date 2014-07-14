/*
 * File: gpio.c
 * Author: Roy Smart
 * MOSES LAB
 * Montana State University
 * 
 */

#include "gpio.h"

/*Uses PLX API to write to memory locations on FPGA*/
int poke_gpio(U32 offset, U32 data){
    
    /*variable declaration*/
    RETURN_CODE rc;     //return code
    U8 bar_index = GPIO_BAR_INDEX;
    U32 sz_buffer = PLX_BUFFER_WIDTH;
    U32 return_val = TRUE;
    
    
    /*Get and open PLX FPGA device*/
    rc = GetAndOpenDevice(&device, 0x9056);
    
    /*check if device opened successfully*/
    if(rc != ApiSuccess){
        record("*ERROR - API failed, unable to open PLX device*");
        PlxSdkErrorDisplay(rc);
    }
    else{
        
        /*Read current BAR properties*/
        rc = PlxPci_PciBarProperties(&device, bar_index, &bar_properties);
        
        /*Check if bar properties were read successfully*/
        if (rc != ApiSuccess){
            record("*ERROR - API failed, unable to read BAR properties*");
            PlxSdkErrorDisplay(rc);
        }
        else{
            
            /*write to device*/
            rc = PlxPci_PciBarSpaceWrite(&device, bar_index, offset, &data, sz_buffer, type_bit, FALSE);
            
            /*Check if write was successful*/
            if(rc != ApiSuccess){
                record("*ERROR* - API failed to write to device");
                PlxSdkErrorDisplay(rc);
            }
            else{
                return_val = FALSE;
            }
        }
    }
    
    return return_val;
}

/*uses gpio_write to open the shutter*/
int open_shutter(){
    return poke_gpio(SHUTTER_OFFSET, SHUTTER_OPEN_SIM);
}

/*Uses gpio_write to close shutter*/
int close_shutter(){
    return poke_gpio(SHUTTER_OFFSET, SHUTTER_CLOSE_SIM);
}
