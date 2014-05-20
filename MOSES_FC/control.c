#include "control.h"

/* hlp_control is a thread that reads packets from the housekeeping uplink and 
 * executes the commands contained within the packets 
 */
void * hlp_control(void * arg) {
    lockingQueue_init(&hkdownQueue);
    int fup = init_hkup_serial_connection();
    buildLookupTable();
    //uplinkInit();     obsufucated tmu map
    hlpHashInit();
    puts("\n");
    while (ts_alive) {
        int status = GOOD_PACKET;       //used by control functions to store validity of packet
        Packet new_packet;
        Packet * p = &new_packet;
        readPacket(fup, p);
        if (!p->status) {
            status = BAD_PACKET;
        } else {
            switch (p->type[0]) {
                case SHELL:
                    printf("Shell packet\n");
                    break;
                case MDAQ_RQS:
                    printf("DAQ packet\n");
                    break;
                case UPLINK:
                    printf("HLP Uplink packet\n");
                    status = hlpUplink(p);
                    break;
                case PWR:
                    printf("Power packet\n");
                    status = hlpPower(p);
                    break;
                case HK_RQS:
                    printf("HK Request Packet\n");
                    break;
                default:
                    printf("Bad Packet type\n");
                    status = BAD_PACKET;
                    break;
            }
        }
        if (ts_alive) {
            

            printf("Received:   %s%s%s%s%s%s\n", p->timeStamp, p->type, p->subtype, p->dataLength, p->data, p->checksum);
            //enqueue(&hkdownQueue, p);

            char data[16];
            data[0] = p->type[0];
            strcpy(data + 1, p->subtype);
            char* ackType;
            if (p->status == GOOD_PACKET) {
                ackType = GDPKT;
            } else {
                ackType = BDPKT;
            }
            Packet* nextp = constructPacket(ackType, ACK, data); //cast gets rid of compiler warning but unclear why the compiler is giving a warning, return type should be Packet*
            enqueue(&hkdownQueue, nextp);
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
            printf("Sent:       %s%s%s%s%s%s\n", p->timeStamp, p->type, p->subtype, p->dataLength, p->data, p->checksum);
            sendPacket(p, fdown);
            free(p);
        } else {
            printf("Bad send Packet\n");
        }

    }
}

/*
 * 
 */ 
