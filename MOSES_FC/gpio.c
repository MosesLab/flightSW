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
U32 output_ddr2_ctrl = 0;
U32 output_ddr2_addr = 0;
U32 input_gpio_int_ack = 0;
U32 output_gpio = 0;

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
 * @return returns 0 if failure
 *      returns 1 if GPIO input was handled appropriately
 *      returns 2 if DMA input is available
 */
int handle_fpga_input() {
    int rc;
    int data_manager_state = 0;
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

    data_manager_state = ((gpio_state & 0x80000000) >> 31); //Check bit 31 to see if DMA is available
    if (data_manager_state == 0x01) {
        return DMA_AVAILABLE;
    } else {
        /*enqueue value to send to gpio control*/
        gpio_in_state->val = gpio_state;
        enqueue(&lqueue[gpio_in], gpio_in_state);

        return TRUE;
    }


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

    /*reset FPGA*/
    output_gpio |= (1 << 26);
    WriteDword(&fpga_dev, 2, OUTPUT_GPIO_ADDR, output_gpio);
    output_gpio = 0;
    WriteDword(&fpga_dev, 2, OUTPUT_GPIO_ADDR, output_gpio);

    /*enable GPIO pins on the FPGA*/
    poke_gpio(GPIO_I_INT_ENABLE, 0x87FFFFFF); // Enable all input gpio interrupts

    /*initialize acknowledge register*/
    poke_gpio(GPIO_I_INT_ACK, 0xFFFFFFFF);

    // Initialize the system
    //    WriteDword(&fpga_dev, 2, GPIO_I_INT_ENABLE, input_gpio_int_en); // Disable all the input gpio interrupts
    //    input_gpio_int_ack = 0xFFFFFFFF;
    //    WriteDword(&fpga_dev, 2, GPIO_I_INT_ACK, input_gpio_int_ack); // Acknowledge all active interrupts, if any
    input_gpio_int_ack = 0x00000000;
    WriteDword(&fpga_dev, 2, GPIO_I_INT_ACK, input_gpio_int_ack);
    output_gpio |= 0x00003000; // Set output_gpio value to display LED value 3
    WriteDword(&fpga_dev, 2, 0x14, output_gpio);

    output_ddr2_addr = 0x003FFFFC;
    WriteDword(&fpga_dev, 2, OUTPUT_DDR2_ADDRESS_ADDR, output_ddr2_addr); // Sets the DDR2 address to zero (currently unused)
    output_ddr2_ctrl = 0x00000000;
    WriteDword(&fpga_dev, 2, OUTPUT_DDR2_CTRL_ADDR, output_ddr2_ctrl); // Set the Data Manager control signal to zero


    return TRUE;

}

/**
 * makes driver call for shutter gpio
 */
void init_shutter() {
    /*activate VDX GPIO with driver call*/
    iopl(3);

    /*initialize pins for writing*/
    outb((SHUTTER_OPEN | SHUTTER_CLOSE), SHUTTER_ENABLE);
}