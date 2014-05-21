#include "control.h"

/* hlp_control is a thread that reads packets from the housekeeping uplink and 
 * executes the commands contained within the packets 
 */
void * hlp_control(void * arg) {
    lockingQueue_init(&hkdownQueue);
    int fup = init_hkup_serial_connection();
    buildLookupTable();
    hlpHashInit();

    while (ts_alive) {
        //int status = GOOD_PACKET; //used by control functions to store validity of packet
        Packet new_packet;
        Packet * p = &new_packet;
        readPacket(fup, p);
        /*the Type of packet determines how */
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

        if (ts_alive) {


            printf("Received:   %s%s%s%s%s%s\n", p->timeStamp, p->type, p->subtype, p->dataLength, p->data, p->checksum);
            //enqueue(&hkdownQueue, p);

            char* data;
            data = concat(2, p->type, p->subtype);
            
            char* ackType;
            if (p->status == GOOD_PACKET) {
                ackType = GDPKT;
            } else {
                ackType = BDPKT;
                puts("BAD PACKET EXECUTION");
            }
            Packet* rp = constructPacket(ackType, ACK, data); //cast gets rid of compiler warning but unclear why the compiler is giving a warning, return type should be Packet*
            enqueue(&hkdownQueue, rp);
            
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
    fdown = init_hkdown_serial_connection(); //Open housekeeping downlink
    while (ts_alive) {

        Packet * p = dequeue(&hkdownQueue); //dequeue the next packet once it becomes available

        if (p->status) {
            sendPacket(p, fdown);
            printf("Sent:       %s%s%s%s%s%s\n", p->timeStamp, p->type, p->subtype, p->dataLength, p->data, p->checksum);            
            free(p);    //Clean up after packet is sent
        } else {
            printf("Bad send Packet\n");
        }

    }
}

/*
 * 
 */
