#include "control.h"
#include "roe_image.h"

/* hlp_control is a thread that reads packets from the housekeeping uplink and 
 * executes the commands contained within the packets 
 */
void * hlp_control(void * arg) {
    prctl(PR_SET_NAME, "hlp_control", 0, 0, 0);
    record("-->HLP control thread started : %.4x\n\n");


    int f_up = 0;
    int stdin_des;

    /*initialize virtual shell*/
    vshell_init(shell_in_pipe, shell_out_pipe);

/*initialize virtual shell input*/
    stdin_des = open(STDIN_PIPE, O_WRONLY);

    /*initialize locking queue for hk down packets*/
    lockingQueue_init(&hkdownQueue);

    /*Open housekeeping downlink using configuartion file*/
    if (*(int*) arg == 1) { //Open real housekeeping downlink
        f_up = init_serial_connection(HKUP, HKUP_REAL);
    } else if (*(int*) arg == 2) { //Open simulated housekeeping downlink
        f_up = init_serial_connection(HKUP, HKUP_SIM);
    } else {
        record("HK down serial connection not configured");
    }

    /*build lookup table for encoding and decoding packets*/
    buildLookupTable();

    /*initialize hash table to match packet strings to control functions*/
    hlpHashInit();

    /*all below should be changed to make it more organized*/
    ops.seq_pause = TRUE;
    ops.seq_run = FALSE;
    ops.channels = CH1 | CH2 | CH3;
    ops.dma_write = TRUE;

    /*Load the Sequence Map*/
    loadSequences();

    seq_map_size = 1; //only for testing, this needs to better integrated into data structure

    while (ts_alive) {
        /*allocate space for packet*/
        packet_t* p;
        if ((p = (packet_t*) calloc(sizeof (packet_t), 1)) == NULL) {
            record("malloc failed to allocate packet\n");
        }

        readPacket(f_up, p);
        recordPacket(p);

        if (ts_alive) {
            /*case statement not necessary here, can get away with just one call
             * to execpacket
             */
            switch (p->type[0]) {
                case SHELL:
//                    printf("Shell packet\n");
                    /*write to input of virtual shell*/
                    hlp_shell(stdin_des, p);

                    break;
                case MDAQ_RQS:
//                    printf("DAQ packet\n");
                    p->control = concat(2, p->type, p->subtype);
                    p->status = execPacket(p);
                    break;
                case UPLINK:
//                    printf("HLP Uplink packet\n");
                    p->control = concat(2, p->type, p->subtype);
                    p->status = execPacket(p);
                    break;
                case PWR:
//                    printf("Power packet\n");
                    p->control = concat(2, p->type, p->subtype);
                    p->status = execPacket(p);
                    break;
                case HK_RQS:
//                    printf("HK Request Packet\n");
                    p->control = concat(3, p->type, p->subtype, p->data);
                    p->status = execPacket(p);
                    break;
                default:
//                    printf("Bad Packet type\n");
                    p->status = BAD_PACKET;
                    break;
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
            packet_t* nextp = constructPacket(ackType, ACK, data); //cast gets rid of compiler warning but unclear why the compiler is giving a warning, return type should be Packet*
            enqueue(&hkdownQueue, nextp);

        }
        //        free(p);    //Why doesn't this work????
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
    prctl(PR_SET_NAME, "hlp_down", 0, 0, 0);
    sleep(2); //sleep to give control a chance to initialize queue
    record("-->HLP Down thread started....\n\n");



    /*Open housekeeping downlink using configuartion file*/
    if (*(int*) arg == 1) { //Open real housekeeping downlink
        fdown = init_serial_connection(HKDOWN, HKDOWN_REAL);
    } else if (*(int*) arg == 2) { //Open simulated housekeeping downlink
        fdown = init_serial_connection(HKDOWN, HKDOWN_SIM);
    } else {
        record("HK down serial connection not configured\n");
    }

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
 * reads data from stdout into hlp packets pushed ont hkdown queue
 */
void * hlp_shell_out(void * arg) {
    prctl(PR_SET_NAME, "hlp_shell_output", 0, 0, 0);
    int data = FALSE;
    //    int readData, i;

    char * buf;

    /*sleep to allow time for pipe to be initialized */
    sleep(1);

    record("-->HLP shell output listener thread started...\n\n");

    //        FILE * stdout_ptr = fopen(STDOUT_PIPE, "r");
    int stdout_des = open(STDOUT_PIPE, O_RDONLY);
//    fcntl(stdout_des, F_SETFL, fcntl(stdout_des, F_GETFL) + ~O_NONBLOCK);


    while (ts_alive) {
        //        char buf[255];
         buf = calloc(sizeof (char),256);
        /*use select() to monitor output pipe*/
        data = input_timeout(stdout_des, 2);

        if (data > 0) {
         
            /*initialize buffer*/
//            int i;
//            for(i = 0; i < 256; i++){
//                buf[i] = '\0';
//            }
//           

            read(stdout_des, buf, 255);
            
//            record(buf);

            //        for (i = 0; i < readData; i++) {
            //            if (buf[i] < 0x20 || buf[i] > 0x7E)
            //                buf[i] = 0x20;
            //        }
            //
            /*push onto hk down queue*/
            packet_t * sr = constructPacket(SHELL_S, OUTPUT, buf);
            enqueue(&hkdownQueue, sr);
        }
//                        free(buf);
    }

    return NULL;
}

/*
 * hlp_housekeeping loops to send periodic updates of temperatures across
 * experiment
 */
void * hlp_housekeeping(void * arg) {
    prctl(PR_SET_NAME, "hlp_housekeeping", 0, 0, 0);
    record("-->HLP Housekeeping thread started....\n\n");
    while (ts_alive) {
        packet_t * p = constructPacket(GDPKT, ACK, NULL);
        enqueue(&hkdownQueue, p);
        sleep(1);
    }
    return NULL;
}

/*High speed telemetry thread for use with synclink USB adapter*/
void * telem(void * arg) {
    prctl(PR_SET_NAME, "telemetry", 0, 0, 0);
    record("-->High-speed Telemetry thread started....\n\n");
    FILE *fp;
    int synclink_fd = synclink_init(SYNCLINK_START);
    int xmlTrigger = 1;

    lockingQueue_init(&roeQueue);

    while (ts_alive) {
        //        if (roeQueue.count != 0) {
        //dequeue imgPtr_t here

        imgPtr_t * curr_image = (imgPtr_t *) dequeue(&roeQueue); //RTS
        char * curr_path = curr_image->filePath;

        //            fp = fopen(roeQueue.first->filePath, "r");  //Open file
        fp = fopen(curr_path, "r");

        if (fp == NULL) { //Error opening file
            //                printf("fopen(%s) error=%d %s\n", roeQueue.first->filePath, errno, strerror(errno));
            printf("fopen(%s) error=%d %s\n", curr_path, errno, strerror(errno));
        } else fclose(fp);
        if ((&roeQueue)->first != NULL) {

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
