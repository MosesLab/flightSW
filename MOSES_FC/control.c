#include "control.h"

/* hlp_control is a thread that reads packets from the housekeeping uplink and 
 * executes the commands contained within the packets 
 */
void * hlp_control(void * arg) {
    lockingQueue_init(&hkdownQueue);
    int fup = init_hkup_serial_connection();
    buildLookupTable();

    while (ts_alive) {
        int status = GOOD_PACKET;
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
                    break;
                case PWR:
                    printf("Power packet\n");
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
        if (ts_alive) enqueue(&hkdownQueue, p);

        char data[16];
        data[0] = p->type[0];
        strcpy(data + 1, p->subtype);
        if (p->status == GOOD_PACKET) {
            Packet* nextp = constructPacket(GDPKT, ACK, data);
            enqueue(&hkdownQueue, nextp);
        } else {
            Packet* nextp = constructPacket(BDPKT, ACK, data);
            enqueue(&hkdownQueue, nextp);
        }


    }
    /*need to clean up properly but these don't allow the program to terminate correctly*/
    //close(fup);  
    //lockingQueue_destroy(&hkupQueue);
}

/* hlp_down is a thread that waits on a packet from a queue and sends it over 
 * the housekeeping downlink
 */
void * hlp_down(void * arg){
    fdown = init_hkdown_serial_connection();    //Open housekeeping downlink
    while(ts_alive){
        
        Packet p = dequeue(&hkdownQueue);       //dequeue the next packet once it becomes available
        
//        if(p.status){
                printf("%s%s%s%s%s%s%d\n",p.timeStamp, p.type, p.subtype, p.dataLength, p.data, p.checksum, p.status);
                sendPacket(&p, fdown);
//        }
//        else{
//            printf("Bad send Packet\n");
//        }
        printf("\n");
    }    
}
