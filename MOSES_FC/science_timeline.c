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
    int rc;
    char* msg = (char *) malloc(200 * sizeof (char));
    char sindex[5];
    char sframe[1200];


    /*Set thread name*/
    prctl(PR_SET_NAME, "SCI_TIMELINE", 0, 0, 0);

    ops.read_block = READBLK_DEFAULT;

    record("-->Science Timeline thread started....\n");

    /* wait for ROE to become active */
    record("Waiting for ROE to become active...\n");

//    /*main loop*/
    while (ts_alive) {

        /*wait until sequence is enqueued*/
        record("Wait for new sequence\n");
        currentSequence = (sequence_t *) dequeue(&lqueue[sequence]);

        if (ops.roe == ON) { // Check if the ROE is still present
            if (currentSequence != NULL) {
                ops.seq_run = TRUE;

                sprintf(msg, "Current sequence: %s, Sequence number:%d\n", currentSequence->sequenceName, ops.sequence);
                record(msg);

                record("New sequence received, starting sequence\n");

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
                    a = (packet_t*) constructPacket(MDAQ_RSP, BEGIN_EXP, (char *) NULL);
                    enqueue(&lqueue[hkdown], a);
                    if (ops.seq_run == FALSE) {
                        sprintf(msg, "Sequence not running, breaking out of sequence.\n");
                        record(msg);
                        break;
                    }

                    /*construct new image to read data into*/
                    roeimage_t * image = malloc(sizeof (roeimage_t));
                    int index[4] = {0, 0, 0, 0}; //incremented when the data is sorted
                    char channels = ops.channels;
                    constructImage(image, index, channels, 16);

                    sprintf(msg, "Taking exposure for duration: %3.3f seconds.\n", currentSequence->exposureTimes[i]);
                    record(msg);
                    int duration = takeExposure(currentSequence->exposureTimes[i], currentSequence->seq_type);

                    /*copy name of sequence to the image struct*/
                    unsigned int name_size = strlen(currentSequence->sequenceName) + 1;
                    image->name = malloc(sizeof (char) * name_size); // add one for null terminated character
                    memcpy(image->name, currentSequence->sequenceName, name_size);

                    /*copy the rest of the values necessary for this image struct*/
                    image->duration = duration;
                    image->num_exp = i + 1; //Index of exposure in sequence
                    image->num_frames = currentSequence->numFrames; //Number of exposures this sequence

                    /*push packets with information about frame(index and exposure length) */
                    sprintf(sindex, "%d", i);
                    sprintf(sframe, "%6.3f", currentSequence->exposureTimes[i]);

                    a = (packet_t*) constructPacket(MDAQ_RSP, GT_CUR_FRMI, sindex);
                    b = (packet_t*) constructPacket(MDAQ_RSP, GT_CUR_FRML, sframe);
                    enqueue(&lqueue[hkdown], a);
                    enqueue(&lqueue[hkdown], b);

                    /*initialize index( these will start at -1 and be incremented by DMA*/

                    record("Done with exposure. Wait for readout...\n");

                    /*Enqueue image buffer to fpga server thread for DMA transfer*/
                    record("Queue image buffer for DMA transfer.\n");
                    enqueue(&lqueue[scit_image], image);

                    /*Wait until FPGA has entered buffer mode*/
                    rc = wait_on_sem(&dma_control_sem, 2);
                    if (rc != TRUE) {
                        record("Failed to set FPGA to buffer mode, trying next exposure\n");
                        continue;
                    }


                    /* Command ROE to Readout*/
                    if (roe_struct.active) {
                        record("Sending command to ROE to readout\n");


                        readOut(ops.read_block, 100000);
                        a = (packet_t*) constructPacket(MDAQ_RSP, BEGIN_RD_OUT, (char *) NULL);
                        enqueue(&lqueue[hkdown], a);

                    }

                    record("waiting on DMA completion\n");

                    /*Wait until DMA is complete before proceeding*/
                    wait_on_sem(&dma_control_sem, 15);


                    /* push packet w/info about end read out */
                    a = (packet_t*) constructPacket(MDAQ_RSP, END_RD_OUT, (char *) NULL);
                    b = (packet_t*) constructPacket(MDAQ_RSP, GT_CUR_FRMI, sindex);
                    enqueue(&lqueue[hkdown], a);
                    enqueue(&lqueue[hkdown], b);


                    sprintf(msg, "Exposure of %3.3lf seconds complete.\n\n", currentSequence->exposureTimes[i]);
                    record(msg);
                }/* end for each exposure */

                /* done with sequence, push packet with info */
                sprintf(msg, "Done with sequence %s\n\n\n", currentSequence->sequenceName);
                record(msg);

                a = (packet_t*) constructPacket(MDAQ_RSP, END_SEQ, (char *) NULL);
                enqueue(&lqueue[hkdown], a);
                ops.seq_run = FALSE;
            }
        } else { // If the ROE is present but not on, pause science timeline to reactivate
            record("ROE not active!\n");
            sleep(1);
        }
    }//end while ts_alive

    record("Done with scienceTimeline\n");

    return NULL;
}

/* write_data will be part of a new thread that will be in
 * waiting mode until it is called to write an image to a file. 
 */
void * write_data(void * arg) {
    char msg[100];

    /*Set thread name*/
    prctl(PR_SET_NAME, "IMAGE_WRITER", 0, 0, 0);

    record("-->Image Writer thread started....\n");

    while (ts_alive) {

        /*dynamically allocate fields for image metadata. David, you screwed me over here, damn you and your statically allocated shit*/
        char ftimedate[80];
        char * filename = malloc(sizeof (char) * 80);
        char * dtime = malloc(sizeof (char) * 100);
        char * ddate = malloc(sizeof (char) * 100);


        /*Wait for image to be enqueued*/
        record("Waiting for new image...\n");
        roeimage_t * image = dequeue(&lqueue[fpga_image]);
        if (image != NULL) {
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
            image->date = ddate;
            image->time = dtime;
            image->width = 2048;
            image->height = 1024;

            image->xml_cur_index = xml_index; // update current index of xml snippet array


            record("Image Opened\n");

            /*write the image and metadata to disk*/
            writeToFile(image);

            sprintf(msg, "File %s successfully written to disk. (%d out of %d)\n", filename, image->num_exp, image->num_frames);
            record(msg);

            /*push the filename onto the telemetry queue*/
            if (ops.tm_write == 1) {
                enqueue(&lqueue[telem_image], image); //enqueues the path for telem
                record("Filename pushed to telemetry queue\n");
            } else {
                /*need to free allocated image to prevent memory leak --RTS*/
                free(image->name);
                free(image->filename);
                free(image->date);
                free(image->time);
                free(image->data[0]);
                free(image->data[1]);
                free(image->data[2]);
                free(image->data[3]);
                free(image);
            }

            /*copy a backup of the log to disk after each image write*/
            copy_log_to_disk();
        }

    }//end while ts_alive
    record("Done Writing Images\n");
    return 0;
}

/*High speed telemetry thread for use with synclink USB adapter*/
void * telem(void * arg) {
    prctl(PR_SET_NAME, "TELEM", 0, 0, 0);
    record("--->High-speed Telemetry thread started....\n");
    int synclink_fd = synclink_init(SYNCLINK_START);
    int rc;
    int sequence_itr = 1;
    char msg[255];
    roeimage_t * new_image = NULL;

    /*Main telemetry loop*/
    while (ts_alive) {

        /*dequeue new image from image writer thread*/
        new_image = (roeimage_t*) dequeue(&lqueue[telem_image]);
        if (!ts_alive) return NULL;
        sprintf(msg, "Dequeued new image %s\n", new_image->filename);
        record(msg);


        rc = send_image(new_image, synclink_fd); //Send actual Image
        if (rc < 0) {
            record("Failed to send image over telemetry!");
        }
        else {
            sprintf(msg, "File %s successfully sent via high-speed telemetry. (%d out of %d)\n", new_image->filename, new_image->num_exp, new_image->num_frames);
            record(msg);
        }
        sequence_itr++;
        
        /*free all dynamically allocated memory associated with image*/
        free(new_image->name);
        free(new_image->filename);
        free(new_image->date);
        free(new_image->time);
        free(new_image->data[0]);
        free(new_image->data[1]);
        free(new_image->data[2]);
        free(new_image->data[3]);
        free(new_image);

        new_image = NULL;
    }

    return NULL;
}

