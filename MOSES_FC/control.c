#include "control.h"

void * controlThread(void * arg) {

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
                    printf("Uplink packet\n");
                    break;
                case PWR:
                    printf("Power packet\n");
                    break;
                case HK_RQS:
                    printf("HK Request Packet\n");
                    break;
                default:
                    printf("Bad Packet\n");
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
    /*need to clean up properly but these don't work*/
    //close(fup);  
    //lockingQueue_destroy(&hkupQueue);

    return 0;

}
