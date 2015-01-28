#include "fpga.h"

/**
 * FPGA server thread facilitates communications between the flight software 
 * and the FPGA. The thread waits on a interrupt for DMA or GPIO input. The wait
 * times out periodically to check if there is any GPIO output to poke the 
 * FPGA
 * 
 * @param arg (not used)
 * @return  (not used)
 */

/*replace conditional variable with semaphore since pthread_cond_timedwait() is broken */
sem_t dma_done_sem;

void * fpga_server(void * arg) {
    int rc;
    gpio_out_uni * temp_state = NULL; //If this variable is set, the server will deassert the output latch

    prctl(PR_SET_NAME, "FPGA_SERVER", 0, 0, 0);

    record("-->FPGA Server thread started....\n");

    /*initialize semaphore*/
    sem_init(&dma_done_sem, 0, 0); //initialize semaphore to zero

    /*open fpga device*/
    open_fpga();

    /*Allocate buffer for image fragments*/
    uint i;
    for (i = 0; i < NUM_FRAGMENT; i++) {
        allocate_buffer(&dma_params[i], &pci_buffer[i], &virt_buf[i]);
    }

    /*initialize GPIO pins*/
    rc = init_gpio();
    if (rc == FALSE) {
        record("Error initializing GPIO pins\n");
    }

    /*initialize DMA pipeline*/
    initializeDMA();

    set_buffer_mode();

    /*FPGA server main control loop*/
    while (ts_alive) {
        U32 interrupt = 0;

        /*wait on interrupt for DMA and GPIO input*/
        rc = interrupt_wait(&interrupt);
        if (rc == FALSE) {
            record("Error during wait\n");
        }

        /**
         * Unlatch the power latch if it was latched before timeout
         * 
         * This statement requires that the timeout on the interrupt wait must be 
         * similar to the latch pulse length (66ms).
         */
        if (temp_state) {

            /*deassert latch*/
            temp_state->bf.latch = 0;

            /*Poke unlatched value to FPGA*/
            rc = poke_gpio(OUTPUT_GPIO_ADDR, temp_state->val);
            if (rc == FALSE) {
                record("Error writing GPIO output\n");
            }

            /*free gpio value*/
            free(temp_state); //Free GPIO output struct from assert
            temp_state = NULL;
        }

        /*take action based off what type of interrupt*/
        if (interrupt == INP_INT) {

            /*Call function to handle input interrupt from FPGA*/
            record("Interrupt received\n");

            rc = handle_fpga_input();
            if (rc == FALSE) {
                record("Error handling FPGA input\n");
            } else if (rc == DMA_AVAILABLE) { // DMA available in buffer, begin transfer


                record("Perform DMA transfer from FPGA\n");

                for (i = 0; i < NUM_FRAGMENT; i++) {
                    dmaRead(dma_params[i], DMA_TIMEOUT);
                }

                /*Wake up science timeline waiting for DMA completion*/
                sem_post(&dma_done_sem);

                // Return to IDLE state
                record("DMA Complete, Returning to IDLE state\n");
                output_ddr2_ctrl &= 0x00FFFFFF;
                output_ddr2_ctrl |= (0x02 << 24);
                WriteDword(&fpga_dev, 2, OUTPUT_DDR2_CTRL_ADDR, output_ddr2_ctrl);
                output_ddr2_ctrl &= 0x00FFFFFF;
                output_ddr2_ctrl |= (0x00000000 << 24);
                WriteDword(&fpga_dev, 2, OUTPUT_DDR2_CTRL_ADDR, output_ddr2_ctrl);

                /*close DMA channel*/
                //                dmaClose();

                record("Sort image\n");
                unsort(dma_image);


                record("Enqueue image to writer\n");
                enqueue(&lqueue[fpga_image], dma_image);

                set_buffer_mode();
            }


        } else if (interrupt == TIMEOUT_INT) {

            /*check if new GPIO output is available*/
            if (occupied(&lqueue[gpio_out])) {
                gpio_out_uni * gpio_out_val = dequeue(&lqueue[gpio_out]);

                /*Check if request or assert*/
                if (gpio_out_val->val == REQ_POWER) {

                    /*request pin values*/
                    rc = peek_gpio(OUTPUT_GPIO_ADDR, &(gpio_out_val->val));
                    if (rc == FALSE) {
                        record("Error reading GPIO request\n");
                    }

                    enqueue(&lqueue[gpio_req], gpio_out_val);
                } else {
                    /*apply output if not request*/
                    rc = poke_gpio(OUTPUT_GPIO_ADDR, gpio_out_val->val);
                    if (rc == FALSE) {
                        record("Error writing GPIO output\n");
                    }

                    usleep(1000); //sleep for 1ms before applying latch

                    /*apply latch*/
                    gpio_out_val->bf.latch = 1;

                    /*apply latched output*/
                    rc = poke_gpio(OUTPUT_GPIO_ADDR, gpio_out_val->val);
                    if (rc == FALSE) {
                        record("Error writing GPIO output\n");
                    }

                    temp_state = gpio_out_val; //write to this variable so we know to delatch next timeout
                }
            }

            /*check if image input is available*/
            /*TESTING!!!!!!! Do not use in real life!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11*/
            if (occupied(&lqueue[scit_image])) {

                /*try resetting to prevent buffer overflow, DONT USE IN REAL LIFE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
                //                reset_fpga();

                //                PlxPci_DeviceReset(&fpga_dev);

                /*open DMA channel*/
                //                initializeDMA();

                //                record("Set FPGA to buffer state\n");
                //                // Set Data Manager State to BUFFER
                //                output_ddr2_ctrl &= 0x00FFFFFF;
                //                output_ddr2_ctrl |= (0x01 << 24);
                //                WriteDword(&fpga_dev, 2, OUTPUT_DDR2_CTRL_ADDR, output_ddr2_ctrl);

                record("Dequeue new image\n");
                dma_image = dequeue(&lqueue[scit_image]);

                //Trigger a frame
                record("Trigger simulated frame\n");
                gpio_out_state.bf.frame_trigger = 1;
                poke_gpio(OUTPUT_GPIO_ADDR, gpio_out_state.val);
                gpio_out_state.bf.frame_trigger = 0;
                poke_gpio(OUTPUT_GPIO_ADDR, gpio_out_state.val);

            }

        }

    }

    return NULL;
}

/**
 * interrupt_wait() will wait on the PCI interrupt line and determine if GPIO 
 * input or DMA requested the interrupt
 * 
 * @return 1 if success 0 if failure
 */
int interrupt_wait(U32 * interrupt) {
    int rc;

    /*clear and reset interrupt structure*/
    memset(&plx_intr, 0, sizeof (PLX_INTERRUPT));

    /*set interrupt structure*/
    plx_intr.LocalToPci = 1; //set bit 11
    //    plx_intr.PciMain = 1;		// Bit 8 -- should already been on

    /*enable interrupt on PLX bridge*/
    rc = PlxPci_InterruptEnable(&fpga_dev, &plx_intr); // sets PCI9056_INT_CTRL_STAT
    if (rc != ApiSuccess) PlxSdkErrorDisplay(rc);

    /*register for interrupt with kernel*/
    rc = PlxPci_NotificationRegisterFor(&fpga_dev, &plx_intr, &plx_event);
    if (rc != ApiSuccess) PlxSdkErrorDisplay(rc);

    /*wait for interrupt*/
    int waitrc = PlxPci_NotificationWait(&fpga_dev, &plx_event, FPGA_TIMEOUT);

    /*disable interrupt*/
    rc = PlxPci_InterruptDisable(&fpga_dev, &plx_intr); // sets PCI9056_INT_CTRL_STAT
    if (rc != ApiSuccess) PlxSdkErrorDisplay(rc);

    /*handle return code of wait function*/
    if (waitrc == ApiWaitTimeout) {
        *interrupt = TIMEOUT_INT;
        return TRUE;
    } else if (waitrc == ApiSuccess) {
        *interrupt = INP_INT;
        return TRUE;
    } else if (waitrc == ApiWaitCanceled) {
        record("Wait canceled\n");
        return FALSE;
    } else {
        record("Wait returned an unknown value\n");
        return FALSE;
    }


}