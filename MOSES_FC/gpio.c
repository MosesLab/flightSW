/*
 * File: gpio.c
 * Author: Roy Smart
 * MOSES LAB
 * Montana State University
 * 
 */

#include "gpio.h"

/*write gpio uses peek() and poke() to read from register and assign new bit*/
//int write_gpio(U32 offset, U32 mask, U32 state) {
//    int rc; //return code for API calls
//
//    /*bitwise AND state with pin mask*/
//    U32 masked_state = mask & state;
//
//    /*read current contents of PCI BAR memory space*/
//    U32 current_state;
//    rc = peek_gpio(offset, &current_state);
//
//    /*apply new value*/
//    U32 new_state = (current_state & ~mask) | masked_state;
//
//    //    /*check that the previous pins read correctly*/
//    //    if (rc != TRUE) {
//    //        record("Failed to read previous state of GPIO pins\n");
//    //        return FALSE;
//    //    } else {
//    //        /*Assert output*/
//    //        return poke_gpio(offset, new_state);
//    //    }
//}

/*Uses PLX API to write to memory locations on FPGA*/
int poke_gpio(U32 offset, U32 data) {

    /*variable declaration*/
    RETURN_CODE rc; //return code
    //    U8 bar_index = (U8) GPIO_BAR_INDEX;
    //    U32 sz_buffer = PLX_BUFFER_WIDTH;
    U32 return_val = TRUE;

    //    /*Read current BAR properties*/
    //    rc = PlxPci_PciBarProperties(&fpga_dev, bar_index, &bar_properties);
    //
    //    /*Check if bar properties were read successfully*/
    //    if (rc != ApiSuccess) {
    //        record("*ERROR* - API failed, unable to read BAR properties \n");
    //        PlxSdkErrorDisplay(rc);
    //        return_val = FALSE;
    //    } else {

    /*write data to device*/
    rc = PlxPci_PciBarSpaceWrite(&fpga_dev, bar_index, offset, &data, sz_buffer, type_bit, FALSE);

    /*Check if write was successful*/
    if (rc != ApiSuccess) {
        record("*ERROR* - API failed to write to device \n");
        PlxSdkErrorDisplay(rc);
        return_val = FALSE;
    }
    //    }
    return return_val;
}

/*peeks into PCI BAR memory space and places it in data_buf*/
int peek_gpio(U32 offset, U32 * data_buf) {
    /*variable declaration*/
    RETURN_CODE rc; //return code
    //    U8 bar_index = (U8) GPIO_BAR_INDEX;
    //    U32 sz_buffer = PLX_BUFFER_WIDTH;
    U32 return_val = TRUE;

    //    /*Read current BAR properties*/
    //    rc = PlxPci_PciBarProperties(&fpga_dev, bar_index, &bar_properties);
    //
    //    /*Check if bar properties were read successfully*/
    //    if (rc != ApiSuccess) {
    //        record("*ERROR* - API failed, unable to read BAR properties \n");
    //        PlxSdkErrorDisplay(rc);
    //        return_val = FALSE;
    //    } else {
    /*read data from device*/
    rc = PlxPci_PciBarSpaceRead(&fpga_dev, bar_index, offset, data_buf, sz_buffer, type_bit, FALSE);

    /*Check if write was successful*/
    if (rc != ApiSuccess) {
        record("*ERROR* - API failed to read from device \n");
        PlxSdkErrorDisplay(rc);
        return_val = FALSE;
    }
    //    }

    return return_val;
}

int handle_gpio_in() {
    int rc;
    char msg[255];
    U32 gpio_state = 0;

    /*allocate dynamic space for gpio value*/
    gpio_in_uni * gpio_in = malloc(sizeof (gpio_in_uni));

    /*read pins that initiated interrupt from fpga*/
    rc = peek_gpio(GPIO_I_INT_REG, &gpio_state);
    if (rc == FALSE) {
        record("Error reading GPIO input interrupt\n");
        return FALSE;
    }

    /*send acknowledge read gpio value to fpga*/
    rc = poke_gpio(GPIO_I_INT_ACK, gpio_state);
    if (rc == FALSE) {
        record("Error acknowledging GPIO input interrupt\n");
        return FALSE;
    }

    sprintf(msg, "GPIO value: %d\n", gpio_state);
    record(msg);

    /*enqueue value to send to gpio control*/
    gpio_in->in_val = gpio_state;
    enqueue(&gpio_in_queue, &gpio_in);

    return TRUE;
}

///*uses gpio_write to open the shutter*/
//int open_shutter() {
//    record("Opening shutter\n");
//    return write_gpio(SHUTTER_OFFSET, SHUTTER_OPEN_SIM, ON);
//    //    return poke_gpio(SHUTTER_OFFSET, SHUTTER_OPEN_SIM);
//}
//
///*Uses gpio_write to close shutter*/
//int close_shutter() {
//    record("Closing Shutter\n");
//    return write_gpio(SHUTTER_OFFSET, SHUTTER_CLOSE_SIM, OFF);
//    //    return poke_gpio(SHUTTER_OFFSET, 0);
//}

/*sets up the memory used in powering the instrument*/
void init_gpio() {
    power_subsystem_arr[shutter_driver] = SHUTTER_SIM;
    power_subsystem_arr[roe] = ROE_SIM;
    power_subsystem_arr[pre_mod] = PREMOD_SIM;
    power_subsystem_arr[tcs_1] = TCS_1_SIM;
    power_subsystem_arr[tcs_3] = TCS_3_SIM;
    power_subsystem_arr[tcs_2] = TCS_2_SIM;
    power_subsystem_arr[tcs] = TCS_SIM;
    power_subsystem_arr[reg_5V] = REG_5V_SIM;
    power_subsystem_arr[reg_12V] = REG_12V_SIM;
    power_subsystem_arr[h_alpha] = H_ALPHA_SIM;

    /*initialize acknowledge register*/
    poke_gpio(GPIO_I_INT_ACK, 0xFFFFFFFF);

    /*enable GPIO pins on the FPGA*/
    poke_gpio(GPIO_I_INT_ENABLE, 0xFFFFFFFF);




    /*Initialize all power GPIO into write mode*/
    //    int i;
    //    for (i = 0; i < NUM_SUBSYSTEM; i++) {
    //        write_gpio(POWER_DIRECTION_OFFSET, power_subsystem_arr[i], ON);
    //    }
}