/*
 * File: gpio.c
 * Author: Roy Smart
 * MOSES LAB
 * Montana State University
 * 
 */

#include "gpio.h"

/*Uses PLX API to write to memory locations on FPGA*/
int poke_gpio(U32 offset, U32 data) {
    RETURN_CODE rc; //return code
    U32 return_val = TRUE;

    /*write data to device*/
    rc = PlxPci_PciBarSpaceWrite(&fpga_dev, bar_index, offset, &data, sz_buffer, type_bit, FALSE);

    /*Check if write was successful*/
    if (rc != ApiSuccess) {
        record("*ERROR* - API failed to write to device \n");
        PlxSdkErrorDisplay(rc);
        return_val = FALSE;
    }
    return return_val;
}

/*peeks into PCI BAR memory space and places it in data_buf*/
int peek_gpio(U32 offset, U32 * data_buf) {
    RETURN_CODE rc; //return code
    U32 return_val = TRUE;

    /*read data from device*/
    rc = PlxPci_PciBarSpaceRead(&fpga_dev, bar_index, offset, data_buf, sz_buffer, type_bit, FALSE);

    /*Check if write was successful*/
    if (rc != ApiSuccess) {
        record("*ERROR* - API failed to read from device \n");
        PlxSdkErrorDisplay(rc);
        return_val = FALSE;
    }

    return return_val;
}

int handle_gpio_in() {
    int rc;
    //    char msg[255];
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

    /*enqueue value to send to gpio control*/
    gpio_in->val = gpio_state;
    enqueue(&gpio_in_queue, gpio_in);

    return TRUE;
}

/*GPIO pin on VDX controls shutter*/
void open_shutter() {
    record("Opening shutter\n");
    
    /*activate VDX GPIO with driver call*/
    iopl(3);

    /*initialize pins for writing*/
    outb(0x03, SHUTTER_ENABLE);

    /*assert pin*/
    outb(SHUTTER_OFFSET, SHUTTER_OPEN);

    /*wait for pulse length*/
    usleep(SHUTTER_PULSE);

    /*deassert pin*/
    outb(SHUTTER_OFFSET, 0x00);
}

/*VDX FPIO*/
void close_shutter() {
    record("Closing Shutter\n");
    
    /*activate VDX GPIO with driver call*/
    iopl(3);

    /*initialize pins for writing*/
    outb(0x03, SHUTTER_ENABLE);

    /*assert pin*/
    outb(SHUTTER_OFFSET, SHUTTER_CLOSE);

    /*wait for pulse length*/
    usleep(SHUTTER_PULSE);

    /*deassert pin*/
    outb(SHUTTER_OFFSET, 0x00);
}

/*sets up the memory used in powering the instrument*/
void init_gpio() {
    /*initialize acknowledge register*/
    poke_gpio(GPIO_I_INT_ACK, 0xFFFFFFFF);

    /*enable GPIO pins on the FPGA*/
    //    poke_gpio(GPIO_I_INT_ENABLE, 0xFFFFFFFF);

    U32 mask = 0x00000001;
    unsigned int i;
    for (i = 0; i < NUM_SUBSYSTEM; i++) {
        power_subsystem_mask[i] = mask;
        mask = mask << 1;
    }

    mask = 0x00000001;
    for (i = 0; i < NUM_CONTROL_GPIO; i++) {
        gpio_control_mask[i] = mask;
        mask = mask << 1;
    }

    /*Initialize all power GPIO into write mode*/
    //    int i;
    //    for (i = 0; i < NUM_SUBSYSTEM; i++) {
    //        write_gpio(POWER_DIRECTION_OFFSET, power_subsystem_arr[i], ON);
    //    }
}

/**
 * makes driver call for shutter gpuo
 */
void init_shutter() {
    /*activate VDX GPIO with driver call*/
    iopl(3);

    /*initialize pins for writing*/
    outb(0x03, SHUTTER_ENABLE);
}