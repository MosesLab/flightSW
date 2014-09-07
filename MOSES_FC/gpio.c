/*
 * File: gpio.c
 * Author: Roy Smart
 * MOSES LAB
 * Montana State University
 * 
 */

#include "gpio.h"

/*global variable declaration*/
PLX_PCI_BAR_PROP bar_properties;
PLX_ACCESS_TYPE type_bit;
U8 bar_index;
U32 bar_sz_buffer;
U32 power_subsystem_mask[NUM_SUBSYSTEM];
U32 gpio_control_mask[NUM_CONTROL_GPIO];

/*Uses PLX API to write to memory locations on FPGA*/
int poke_gpio(U32 offset, U32 data) {
    RETURN_CODE rc; //return code
    U32 return_val = TRUE;

    /*write data to device*/
    rc = PlxPci_PciBarSpaceWrite(&fpga_dev, bar_index, offset, &data, bar_sz_buffer, type_bit, FALSE);

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
    rc = PlxPci_PciBarSpaceRead(&fpga_dev, bar_index, offset, data_buf, bar_sz_buffer, type_bit, FALSE);

    /*Check if write was successful*/
    if (rc != ApiSuccess) {
        record("*ERROR* - API failed to read from device \n");
        PlxSdkErrorDisplay(rc);
        return_val = FALSE;
    }

    return return_val;
}

/**
 * Upon receiving a GPIO input interrupt the FPGA server calls this function to
 * take the correct action based off which pin is asserted
 * @return 
 */
int handle_gpio_in() {
    int rc;
    //    char msg[255];
    U32 gpio_state = 0;

    /*allocate dynamic space for gpio value*/
    gpio_in_uni * gpio_in_state = malloc(sizeof (gpio_in_uni));

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
    gpio_in_state->val = gpio_state;
    enqueue(&lqueue[gpio_in], gpio_in_state);

    return TRUE;
}

/*GPIO pin on VDX controls shutter*/
void open_shutter() {
    record("Opening shutter\n");

    /*activate VDX GPIO with driver call*/
    iopl(3);

    /*initialize pins for writing*/
    outb((SHUTTER_OPEN), SHUTTER_ENABLE);

    /*assert pin*/
    outb(SHUTTER_OPEN, SHUTTER_OFFSET);

    /*wait for pulse length*/
    usleep(SHUTTER_PULSE);

    /*deassert pin*/
    outb(0x00, SHUTTER_OFFSET);

    iopl(0);
}

/*VDX FPIO*/
void close_shutter() {
    record("Closing Shutter\n");

    /*activate VDX GPIO with driver call*/
    iopl(3);

    /*initialize pins for writing*/
    outb((SHUTTER_CLOSE), SHUTTER_ENABLE);

    /*assert pin*/
    outb(SHUTTER_CLOSE, SHUTTER_OFFSET);

    /*wait for pulse length*/
    usleep(SHUTTER_PULSE);

    /*deassert pin*/
    outb(0x00, SHUTTER_OFFSET);

    iopl(0);
}

/*sets up the memory used in powering the instrument*/
int init_gpio() {
    int rc;

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

    /*initialize bar properties*/
    bar_index = (U8) GPIO_BAR_INDEX;
    bar_sz_buffer = PLX_BUFFER_WIDTH;
    type_bit = BitSize32;

    /*Read current BAR properties*/
    rc = PlxPci_PciBarProperties(&fpga_dev, bar_index, &bar_properties);

    /*Check if bar properties were read successfully*/
    if (rc != ApiSuccess) {
        record("*ERROR* - API failed, unable to read BAR properties \n");
        PlxSdkErrorDisplay(rc);
        return FALSE;
    }

    /*initialize acknowledge register*/
    poke_gpio(GPIO_I_INT_ACK, 0xFFFFFFFF);

    /*enable GPIO pins on the FPGA*/
        poke_gpio(GPIO_I_INT_ENABLE, 0xFFFFFFFF);

    /*Initialize all power GPIO into write mode*/
    //    int i;
    //    for (i = 0; i < NUM_SUBSYSTEM; i++) {
    //        write_gpio(POWER_DIRECTION_OFFSET, power_subsystem_arr[i], ON);
    //    }

    return TRUE;

}

/**
 * makes driver call for shutter gpuo
 */
void init_shutter() {
    /*activate VDX GPIO with driver call*/
    iopl(3);

    /*initialize pins for writing*/
    outb((SHUTTER_OPEN | SHUTTER_CLOSE), SHUTTER_ENABLE);
}