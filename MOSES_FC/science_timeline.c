/**************************************************
 * Author: David Keltgen                            *
 * Company: Montana State University: MOSES LAB     *
 * File Name: scienceTimeline.c                     *
 * Date:  May 28 2014                               *
 * Description: ScienceTimeline will be responsible *
 *              for providng the functionality of   *
 *              the Science Timeline Thread.        *
 **************************************************/

#include "science_timeline.h"

void * science_timeline(void * arg) {
    char* msg = (char *) malloc(200 * sizeof (char));
    char sindex[2];
    char sframe[10];

    /*Set thread name*/
    prctl(PR_SET_NAME, "SCI_TIMELINE", 0, 0, 0);

    //sleep(1);

    record("-->Science Timeline thread started....\n\n");


//        void init_shutter();      //Would still like to do this -- causes segfault


    /* wait for ROE to become active */
    //record("Waiting for ROE to become active...\n");
    //while(!roe_struct.active)
    //    usleep(20000);
    //record("ROE Active\n");

    /* if ROE active, set to known state (exit default, reset, exit default) */
    //exitDefault();
    //reset();
    //exitDefault();

    while (ts_alive) {

        /*wait until sequence is enqueued*/
        sequence_t * currentSequence = (sequence_t *) dequeue(&lqueue[sequence]);

        ops.seq_run = TRUE;

        sprintf(msg, "Current sequence: %s, Sequence number:%d\n", currentSequence->sequenceName, ops.sequence);
        record(msg);

        /* push packets w/info about current sequence */
        packet_t* a = (packet_t*) constructPacket(MDAQ_RSP, BEGIN_SEQ, (char *) NULL);
        packet_t* b = (packet_t*) constructPacket(MDAQ_RSP, GT_CUR_SEQ, currentSequence->sequenceName);
        enqueue(&lqueue[hkdown], a);
        enqueue(&lqueue[hkdown], b);

        /* for each exposure in the sequence */
        int i;
        for (i = 0; i < currentSequence->numFrames; i++) {
            sprintf(msg, "Starting exposure for duration: %3.3f seconds (%d out of %d)\n", currentSequence->exposureTimes[i], i + 1, currentSequence->numFrames);
            record(msg);
            /* check for running, roe active.... */

            //If ROE not active?

            if (ops.seq_run == FALSE) {
                sprintf(msg, "Sequence not running, breaking out of sequence.\n");
                record(msg);
                break;
            }

            /*construct new image to read data into*/
            roeimage_t * image = malloc(sizeof(roeimage_t));
            int index[4] = {0, 0, 0, 0};        //incremented when the data is sorted
            char channels = ops.channels;
            constructImage(image, index, channels, 16);

            sprintf(msg, "Taking exposure for duration: %3.3f seconds.\n", currentSequence->exposureTimes[i]);
            record(msg);
            int duration = takeExposure(currentSequence->exposureTimes[i], currentSequence->seq_type);

            image->duration = duration;
            image->seq_name = currentSequence->sequenceName;
            image->num_exp = i+1;       //Index of exposure in sequence
            image->num_frames = currentSequence->numFrames; //Number of exposures this sequence

            /*push packets with information about frame(index and exposure length) */
            sprintf(sindex, "%d", i);
            sprintf(sframe, "%6.3f", currentSequence->exposureTimes[i]);

            a = (packet_t*) constructPacket("MDAQ_RSP", GT_CUR_FRMI, sindex);
            b = (packet_t*) constructPacket("MDAQ_RSP", GT_CUR_FRML, sframe);
            enqueue(&lqueue[hkdown], a);
            enqueue(&lqueue[hkdown], b);

            /*initialize index( these will start at -1 and be incremented by DMA*/

            record("Done with exposure. Wait for readout...\n");

            /* Command ROE to Readout*/
            //readOut(...);

            //wait 4 seconds for response from ROE that readout is complete
            sleep(4);

            /* push packet w/info about end read out */
            a = (packet_t*) constructPacket("MDAQ_RSP", GT_CUR_FRMI, sindex);
            enqueue(&lqueue[hkdown], a);

            /*Enqueue image to image writer thread*/
            record("Queue image for writing.\n");
            enqueue(&lqueue[scit_image], image);

            sprintf(msg, "Exposure of %3.3lf seconds complete.\n\n", currentSequence->exposureTimes[i]);
            record(msg);
        }/* end for each exposure */

        /* done with sequence, push packet with info */
        sprintf(msg, "Done with sequence %s\n\n\n", currentSequence->sequenceName);
        record(msg);

        a = (packet_t*) constructPacket(MDAQ_RSP, END_SEQ, (char *) NULL);
        enqueue(&lqueue[hkdown], a);
        record("Done witps.seq_run = FAh sequences\n");
        ops.seq_run = FALSE;

    }//end while ts_alive

    record("Done with scienceTimeline\n");
    return NULL;
}

/* write_data will be part of a new thread that will be in
 * waiting mode until it is called to write an image to a file. 
   A signal, SIGUSR2, will be sent when DMA is ready to transfer
   data to memory and will initialize the writing to disk*/
void * write_data(void * arg) {
    char msg[100];

    /*Set thread name*/
    prctl(PR_SET_NAME, "IMAGE_WRITER", 0, 0, 0);

    record("-->Image Writer thread started....\n\n");

    while (ts_alive) {

//        short *BUFFER[4];
//        /*create pixel buffers */
//        int i;
//        for (i = 0; i < 4; i++) {
//            BUFFER[i] = (short *) calloc(2200000, sizeof (short));
//        }


        char filename[80];
        char ftimedate[80];
        char dtime[100];
        char ddate[100];

        

        /*Wait for image to be enqueued*/
        record("Waiting for new image...\n");
        roeimage_t * image = dequeue(&lqueue[fpga_image]);
        record("Dequeued new image\n");
        
        /* Get data for image details*/
        time_t curTime = time(NULL);
        struct tm *broken = localtime(&curTime);
        strftime(dtime, 20, "%H:%M:%S", broken);
        strftime(ddate, 20, "%y-%m-%d", broken); //get date
        strftime(ftimedate, 40, "%d%m%y%H%M%S", broken);
        //construct a unique filename	
        sprintf(filename, "%s.roe", ftimedate);
        sprintf(filename, "%s/%s.roe", DATADIR, ftimedate);

        image->filename = filename;
        image->name = image->seq_name; //Add the information to the image
        image->date = ddate;
        image->time = dtime;
        //image.duration = duration;
        image->width = 2048;
        image->height = 1024;
        

        record("Image Opened\n");

        /*write the image and metadata to disk*/
        writeToFile(image);

        sprintf(msg, "File %s successfully written to disk. (%d out of %d)\n", filename, image->num_exp, image->num_frames);
        record(msg);

        /*push the filename onto the telemetry queue*/
        if (ops.tm_write == 1) {
            imgPtr_t newPtr;
            newPtr.filePath = filename;
            newPtr.next = NULL;
            enqueue(&lqueue[telem_image], &newPtr); //enqueues the path for telem
            record("Filename pushed to telemetry queue\n");
        }


        /*need to free allocated image to prevent memory leak --RTS*/
        free(image->data[0]);
        free(image->data[1]);
        free(image->data[2]);
        free(image->data[3]);
        free(image);
    }//end while ts_alive

    return 0;
}

/*High speed telemetry thread for use with synclink USB adapter*/
void * telem(void * arg) {
    prctl(PR_SET_NAME, "TELEM", 0, 0, 0);
    record("-->High-speed Telemetry thread started....\n\n");
    FILE *fp;
    int synclink_fd = synclink_init(SYNCLINK_START);
    int xmlTrigger = 1;

    lockingQueue_init(&lqueue[telem_image]);

    while (ts_alive) {
        //        if (roeQueue.count != 0) {
        //dequeue imgPtr_t here

        imgPtr_t * curr_image = (imgPtr_t *) dequeue(&lqueue[telem_image]); //RTS
        char * curr_path = curr_image->filePath;

        //            fp = fopen(roeQueue.first->filePath, "r");  //Open file
        fp = fopen(curr_path, "r");

        if (fp == NULL) { //Error opening file
            //                printf("fopen(%s) error=%d %s\n", roeQueue.first->filePath, errno, strerror(errno));
            printf("fopen(%s) error=%d %s\n", curr_path, errno, strerror(errno));
        } else fclose(fp);
        if ((&lqueue[telem_image])->first != NULL) {

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