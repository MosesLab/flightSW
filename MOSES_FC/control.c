#include "control.h"
#include "roe_image.h"

/* hlp_control is a thread that reads packets from the housekeeping uplink and 
 * executes the commands contained within the packets 
 */
void * hlp_control(void * arg) {
    prctl(PR_SET_NAME, "HLP_CONTROL", 0, 0, 0);
    record("-->HLP control thread started : %.4x\n\n");

    int f_up = 0;
    int stdin_des;
    char msg[255];

    /*initialize virtual shell*/
    vshell_init();

    /*initialize virtual shell input*/
    stdin_des = open(STDIN_PIPE, O_WRONLY);

    /*build lookup table for encoding and decoding packets*/
    buildLookupTable();

    /*initialize hash table to match packet strings to control functions*/
    hlpHashInit();

    /*initialize gpio input control queue*/
    lockingQueue_init(&gpio_in_queue);

    /*set up global GPIO output state*/
    gpio_power_state.val = 0x0;

    /*Open housekeeping downlink using configuration file*/
    if (*(int*) arg == 1) { //Open real housekeeping downlink
        f_up = init_serial_connection(HKUP, HKUP_REAL);
    } else if (*(int*) arg == 2) { //Open simulated housekeeping downlink
        f_up = init_serial_connection(HKUP, HKUP_SIM);
    } else {
        record("HK up serial connection not configured");
    }

    /*all below should be changed to make it more organized*/
    ops.seq_run = FALSE;
    ops.channels = CH1 | CH2 | CH3;
    ops.dma_write = TRUE;

    /*Load the Sequence Map*/
    loadSequences();

    seq_map_size = 1; //only for testing, this needs to better integrated into data structure

    /*main control loop*/
    while (ts_alive) {

        /*allocate space for packet*/
        packet_t* p;
        if ((p = (packet_t*) calloc(1, sizeof (packet_t))) == NULL) {
            record("malloc failed to allocate packet\n");
        }

        int control_type = control_wait(f_up, &gpio_in_queue);

        if (control_type == HLP_PACKET) {

            /*read next packet from HKUP*/
            readPacket(f_up, p);
            recordPacket(p);

            if (ts_alive) {
                if (p->status == GOOD_PACKET) { //check checksum
                    switch (p->type[0]) {
                        case SHELL:
                            hlp_shell(stdin_des, p);
                            break;
                        case MDAQ_RQS:
                        case UPLINK:
                        case PWR:
                            p->control = concat(2, p->type, p->subtype);
                            p->status = execPacket(p);
                            break;
                        case HK_RQS:
                            p->control = concat(3, p->type, p->subtype, p->data);
                            p->status = execPacket(p);
                            break;
                        default:
                            p->status = BAD_PACKET;
                            break;
                    }
                }

                char* data;
                data = concat(2, p->type, p->subtype);

                char* ackType;
                if (p->status == GOOD_PACKET) {

                    ackType = GDPKT;
                } else {
                    ackType = BDPKT;
                    record("BAD PACKET EXECUTION\n");
                }
                packet_t* nextp = constructPacket(ackType, ACK, data);
                enqueue(&hkdownQueue, nextp);


            }


        } else if (control_type == GPIO_INP) {

            /*dequeue next packet from gpio input queue*/
            gpio_in_uni * gpio_control = (gpio_in_uni*) dequeue(&gpio_in_queue);

            sprintf(msg, "GPIO value: %d\n", (U32) (gpio_control->val));
            record(msg);

            free(gpio_control); //double free??????????????????????????????????????????


        } else {
            record("Waiting for input failed\n");
        }
        free(p);
    }
    /*need to clean up properly but these don't allow the program to terminate correctly*/
    //close(fup);  
    //lockingQueue_destroy(&hkupQueue);
    return NULL;
}

/*
 * hlp_down is a thread that waits on a packet from a queue and sends it over 
 * the housekeeping downlink
 */
void * hlp_down(void * arg) {
    prctl(PR_SET_NAME, "HLP_DOWN", 0, 0, 0);

    unsigned int fdown = 0;

    sleep(2); //sleep to give control a chance to initialize queue
    record("-->HLP Down thread started....\n\n");

    /*Open housekeeping downlink using configuration file*/
    if (*(int*) arg == 1) { //Open real housekeeping downlink
        fdown = init_serial_connection(HKDOWN, HKDOWN_REAL);
    } else if (*(int*) arg == 2) { //Open simulated housekeeping downlink
        fdown = init_serial_connection(HKDOWN, HKDOWN_SIM);
    } else {
        record("HK down serial connection not configured\n");
    }

    /*initialize locking queue for hk down packets*/
    lockingQueue_init(&hkdownQueue);

    while (ts_alive) {

        packet_t * p = (packet_t *) dequeue(&hkdownQueue); //dequeue the next packet once it becomes available

        if (!ts_alive) break; //If the program has terminated, break out of the loop
        if (p->status) {
            sendPacket(p, fdown);
            recordPacket(p); //save packet to logfile for debugging   
            free(p); //Clean up after packet is sent
        } else {
            record("Bad hlp_down packet\n");
        }

    }
    return NULL;
}

/*
 * reads data from stdout into hlp packets pushed onto hkdown queue
 */
void * hlp_shell_out(void * arg) {
    prctl(PR_SET_NAME, "HLP_SHELL_OUT", 0, 0, 0);

    unsigned int data = FALSE;
    char * buf;

    /*sleep to allow time for pipe to be initialized */
    sleep(1);

    record("-->HLP shell output listener thread started...\n\n");

    /*open pipe to virtual shell stdout*/
    unsigned int stdout_des = open(STDOUT_PIPE, O_RDONLY);

    while (ts_alive) {

        /*initialize buffer*/
        buf = calloc(sizeof (char), 256);

        /*use select() to monitor output pipe*/
        data = input_timeout(stdout_des, 10000, 0);

        if (data) {

            /*read from stdout pipe*/
            if ((read(stdout_des, buf, 255)) == -1) {
                record("read failed in HLP shell out");
            }

            /*push onto hk down queue*/
            packet_t * sr = constructPacket(SHELL_S, OUTPUT, buf);
            enqueue(&hkdownQueue, sr);
        }
        free(buf);
    }

    return NULL;
}

/*
 * hlp_housekeeping loops to send periodic updates of temperatures across
 * experiment
 */
void * hlp_housekeeping(void * arg) {
    prctl(PR_SET_NAME, "HLP_HK", 0, 0, 0);
    record("-->HLP Housekeeping thread started....\n\n");
    while (ts_alive) {
        packet_t * p = constructPacket(GDPKT, ACK, NULL);
        enqueue(&hkdownQueue, p);
        sleep(1);
    }
    return NULL;
}

/**
 * FPGA server thread facilitates communications between the flight software 
 * and the FPGA. The thread waits on a interrupt for DMA or GPIO input. The wait
 * times out periodically to check if there is any GPIO output to poke the 
 * FPGA
 * 
 * @param arg (not used)
 * @return  (not used)
 */
void * fpga_server(void * arg) {
    int rc;

    prctl(PR_SET_NAME, "FPGA_SERVER", 0, 0, 0);

    record("-->FPGA Server thread started\n");

    /*initialize DMA pipeline*/
    initializeDMA();

    /*initialize GPIO pins*/
    init_gpio();

    /*Initialize locking queues for thread sync*/
    lockingQueue_init(&scit_image_queue);
    lockingQueue_init(&gpio_out_queue);

    /*main server control loop*/
    while (ts_alive) {
        U32 interrupt = 0;

        /*wait on interrupt for DMA and GPIO input*/
        rc = interrupt_wait(&interrupt);
        if (rc == FALSE) {
            record("Error during wait\n");
        }

        /*take action based off what type of interrupt*/
        if (interrupt == INP_INT) {

            // need to check if DMA or GPIO input here by peeking at fpga register

            rc = handle_gpio_in();
            if (rc == FALSE) {
                record("Error handling GPIO\n");
            }
        } else if (interrupt == TIMEOUT_INT) {

            /*check if new GPIO output is available*/
            if (occupied(&gpio_out_queue)) {
                gpio_out_uni * gpio_out = dequeue(&gpio_out_queue);

                /*Check if request or assert*/
                if (gpio_out->val == REQ_POWER) {
                    /*request pin values*/
                    rc = peek_gpio(GPIO_OUT_REG, &(gpio_out->val));
                    if (rc == FALSE) {
                        record("Error reading GPIO request\n");
                    }
                    enqueue(&gpio_req_queue, gpio_out);
                } else {
                    /*apply output if not request*/
                    rc = poke_gpio(GPIO_OUT_REG, gpio_out->val);
                    if (rc == FALSE) {
                        record("Error writing GPIO output\n");
                    }
                    free(gpio_out);
                }
            }

            /*check if image input is available*/
            /*TESTING!!!!!!! Do not use in real life*/
            //            if(occupied(&scit_image_queue)){             
            //                roeimage_t * dma_image = dequeue(&scit_image_queue);
            //                
            //                
            //            }

        }

    }

    return NULL;
}

/*High speed telemetry thread for use with synclink USB adapter*/
void * telem(void * arg) {
    prctl(PR_SET_NAME, "TELEM", 0, 0, 0);
    record("-->High-speed Telemetry thread started....\n\n");
    FILE *fp;
    int synclink_fd = synclink_init(SYNCLINK_START);
    int xmlTrigger = 1;

    lockingQueue_init(&telem_image_queue);

    while (ts_alive) {
        //        if (roeQueue.count != 0) {
        //dequeue imgPtr_t here

        imgPtr_t * curr_image = (imgPtr_t *) dequeue(&telem_image_queue); //RTS
        char * curr_path = curr_image->filePath;

        //            fp = fopen(roeQueue.first->filePath, "r");  //Open file
        fp = fopen(curr_path, "r");

        if (fp == NULL) { //Error opening file
            //                printf("fopen(%s) error=%d %s\n", roeQueue.first->filePath, errno, strerror(errno));
            printf("fopen(%s) error=%d %s\n", curr_path, errno, strerror(errno));
        } else fclose(fp);
        if ((&telem_image_queue)->first != NULL) {

            fseek(fp, 0, SEEK_END); // seek to end of file
            fseek(fp, 0, SEEK_SET);

            int check = send_image(curr_image, xmlTrigger, synclink_fd); //Send actual Image

            if (xmlTrigger == 1) {
                xmlTrigger = 0;
            } else if (xmlTrigger == 0) {
                xmlTrigger = 1;
            }

            if (check == 0) {
                //                    tm_dequeue(&roeQueue);                  //dequeue the next packet once it becomes available
            }
        }
        //        }
        //        else {
        //            struct timespec timeToWait;
        //            struct timeval now;
        //
        //            gettimeofday(&now, NULL);
        //            timeToWait.tv_sec = now.tv_sec + 1;
        //            timeToWait.tv_nsec = 0;
        //
        //        }
    }
    return NULL;
}
