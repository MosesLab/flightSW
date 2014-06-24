#include "control.h"

/* hlp_control is a thread that reads packets from the housekeeping uplink and 
 * executes the commands contained within the packets 
 */
void * hlp_control(void * arg) {
    record("-->HLP control thread started....\n\n");
    
    lockingQueue_init(&hkdownQueue);
    int fup = init_hkup_serial_connection();
    buildLookupTable();
    hlpHashInit();
    
    /*all below should be changed to make it more organized*/
    ops.seq_pause = TRUE;
    ops.seq_run = FALSE;
    ops.channels = CH1 | CH2 | CH3;
    ops.dma_write = TRUE;
    
     /*Load the Sequence Map*/
    char _sequence1[21]	= "sequence/dark1demo";
    char _sequence2[21]	= "sequence/dark2demo";
    //char _sequence3[21]	= "sequence/datademo";
    char _sequence4[21]	= "sequence/dark3demo";
    char _sequence5[21]	= "sequence/dark4demo";
    
    sequenceMap[0] = constructSequence(_sequence1);
//    sequenceMap[1] = constructSequence(_sequence2);
//   // sequenceMap[2] = constructSequence(_sequence3);
//    sequenceMap[3] = constructSequence(_sequence4);
//    sequenceMap[4] = constructSequence(_sequence5);
    
    seq_map_size = 1;   //only for testing, this needs to better integrated into data structure

    while (ts_alive) {
        /*allocate space for packet*/
        Packet* p;
        if ((p = (Packet*) malloc(sizeof (Packet))) == NULL) {
            record("malloc failed to allocate packet\n");
        }
        
        /*test starting data with SIGUSR1*/
//        Packet * t = constructPacket(UPLINK_S, DATASTART, NULL);
//        sleep(3);
//        uDataStart(t);
//        
//        
//        sleep(15);
//        sequenceMap[0] = constructSequence(_sequence2);
//        uDataStart(t);
        /*end SIGUSR1 testing*/

        readPacket(fup, p);
        
        if (ts_alive) {
        /*case statement not necessary here, can get away with just one call
         * to execpacket
         */
        switch (p->type[0]) {
            case SHELL:
                printf("Shell packet\n");
                break;
            case MDAQ_RQS:
                printf("DAQ packet\n");
                p->control = concat(2, p->type, p->subtype);
                p->status = execPacket(p);
                break;
            case UPLINK:
                printf("HLP Uplink packet\n");
                p->control = concat(2, p->type, p->subtype);
                p->status = execPacket(p);
                break;
            case PWR:
                printf("Power packet\n");
                p->control = concat(2, p->type, p->subtype);
                p->status = execPacket(p);
                break;
            case HK_RQS:
                printf("HK Request Packet\n");
                p->control = concat(3, p->type, p->subtype, p->data);
                p->status = execPacket(p);
                break;
            default:
                printf("Bad Packet type\n");
                p->status = BAD_PACKET;
                break;
        }

        


            recordPacket(p);

            char* data;
            data = concat(2, p->type, p->subtype);

            char* ackType;
            if (p->status == GOOD_PACKET) {
                ackType = GDPKT;
            } else {
                ackType = BDPKT;
                record("BAD PACKET EXECUTION\n");
            }
            Packet* nextp = constructPacket(ackType, ACK, data); //cast gets rid of compiler warning but unclear why the compiler is giving a warning, return type should be Packet*
            enqueue(&hkdownQueue, nextp);
            //free(p);    //Can't do this
            printf("\n");
        }

    }
    /*need to clean up properly but these don't allow the program to terminate correctly*/
    //close(fup);  
    //lockingQueue_destroy(&hkupQueue);
}

/*
 * hlp_down is a thread that waits on a packet from a queue and sends it over 
 * the housekeeping downlink
 */
void * hlp_down(void * arg) {
    record("-->HLP Down thread started....\n\n");
    fdown = init_hkdown_serial_connection(); //Open housekeeping downlink
    while (ts_alive) {

        Packet * p = dequeue(&hkdownQueue); //dequeue the next packet once it becomes available
        if (!ts_alive) break;   //If the program has terminated, break out of the loop
        if (p->status) {
            sendPacket(p, fdown);
            recordPacket(p);    //save packet to logfile for debugging   
            free(p);    //Clean up after packet is sent
        } else {
            record("Bad hlp_down packet\n");
        }

    }
}

/*
 * hlp_housekeeping loops to send periodic updates of temperatures across
 * experiment
 */
void * hlp_housekeeping(void * arg){
    record("-->HLP Housekeeping thread started....\n\n");
    while(ts_alive){
        Packet * p = constructPacket(GDPKT, ACK, NULL);
        //recordPacket(p);
        enqueue(&hkdownQueue, p);
        sleep(1);
    }
}

/*High speed telemetry thread for use with synclink USB adapter*/
void * telem(void * arg){
    record("-->High-speed Telemetry thread started....\n\n");
    /*jackson: dequeue and call sendTM from here*/
}
