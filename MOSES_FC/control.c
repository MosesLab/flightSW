#include "control.h"
#include "roe_image.h"

/*global variable delcaration*/
moses_ops_t ops;
gpio_out_uni gpio_power_state;
pid_t vshell_pid;

/* hlp_control is a thread that reads packets from the housekeeping uplink and 
 * executes the commands contained within the packets 
 */
void * hlp_control(void * arg) {
    int f_up = 0;
    int stdin_des;
    char msg[255];

    prctl(PR_SET_NAME, "CONTROL", 0, 0, 0);
    record("-->HLP control thread started : %.4x\n\n");



    /*initialize virtual shell input*/
    stdin_des = open(STDIN_PIPE, O_WRONLY);

    /*build lookup table for encoding and decoding packets*/
    buildLookupTable();

    /*initialize hash table to match packet strings to control functions*/
    hlpHashInit();

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

        int control_type = control_wait(f_up, &lqueue[gpio_in]);

        if (control_type == HLP_PACKET) {

            /*read next packet from HKUP*/
            readPacket(f_up, p);
//            recordPacket(p);

            if (ts_alive) {
                if (p->status == GOOD_PACKET) {
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

                /*create data to send with ACK*/
                char* data;
                data = concat(2, p->type, p->subtype);

                /*respond to control packet with ACK*/
                char* ackType;
                if (p->status == GOOD_PACKET) {

                    ackType = GDPKT;
                } else {
                    ackType = BDPKT;
                    record("BAD PACKET EXECUTION\n");
                }
                packet_t* nextp = constructPacket(ackType, ACK, data);
                enqueue(&lqueue[hkdown], nextp);
            }

        } else if (control_type == GPIO_INP) {

            /*dequeue next packet from gpio input queue*/
            gpio_in_uni * gpio_control = (gpio_in_uni*) dequeue(&lqueue[gpio_in]);

            sprintf(msg, "GPIO value: %d\n", (U32) (gpio_control->val));
            record(msg);

            /*execute control sequence based off of read gpio value*/
            exec_gpio(gpio_control);

            free(gpio_control);

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
    unsigned int fdown = 0;

    prctl(PR_SET_NAME, "HLP_DOWN", 0, 0, 0);

    //    sleep(2); //sleep to give control a chance to initialize queue
    record("-->HLP Down thread started....\n\n");

    /*Open housekeeping downlink using configuration file*/
    if (*(int*) arg == 1) { //Open real housekeeping downlink
        fdown = init_serial_connection(HKDOWN, HKDOWN_REAL);
    } else if (*(int*) arg == 2) { //Open simulated housekeeping downlink
        fdown = init_serial_connection(HKDOWN, HKDOWN_SIM);
    } else {
        record("HK down serial connection not configured\n");
    }

    while (ts_alive) {

        packet_t * p = (packet_t *) dequeue(&lqueue[hkdown]); //dequeue the next packet once it becomes available

        if (!ts_alive) break; //If the program has terminated, break out of the loop
        if (p->status) {
            sendPacket(p, fdown);
//            recordPacket(p); //save packet to logfile for debugging   
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
        data = input_timeout(stdout_des, 10000, 0); //Shouldn't need to timeout

        if (data) {

            /*read from stdout pipe*/
            if ((read(stdout_des, buf, 255)) == -1) {
                record("read failed in HLP shell out");
            }

            /*push onto hk down queue*/
            packet_t * sr = constructPacket(SHELL_S, OUTPUT, buf);
            enqueue(&lqueue[hkdown], sr);
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
        enqueue(&lqueue[hkdown], p);
        sleep(1);
    }
    return NULL;
}




