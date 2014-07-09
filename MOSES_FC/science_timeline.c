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
    sleep(1);
    prctl(PR_SET_NAME,"science_timeline",0,0,0);
    record("-->Science Timeline thread started....\n\n");

    init_signal_handler_stl();

    char* msg = (char *) malloc(200 * sizeof (char));
    int i;
    short *BUFFER[4]; //[2200000];  



    //sigsuspend(&oldmaskstl); 

    ops.sequence = 1; // start off with the first sequence TESTING;
    /*create pixel buffers */
    for (i = 0; i < 4; i++) {
        BUFFER[i] = (short *) malloc(2200000 * sizeof (short));
    }

    /* wait for ROE to become active */
    //Add code here

    /* ROE initialization: ROE exitdefault, reset, exitdefault */
    //Add code here

    while (ts_alive) {

        if (ops.seq_run == FALSE) {
            record("Sequence stopped, wait for signal to start\n");

            //pthread_sigmask(SIG_BLOCK, &maskstl, &oldmaskstl);
            //sigwait(&maskstl, &caught_signal);
            //pthread_sigmask(SIG_UNBLOCK, &maskstl, &oldmaskstl);
            
            //ps.sequence = 1; // start off with the first sequence TESTING!!!!

            ops.seq_run = TRUE;
            ops.seq_pause = FALSE;

            /*running signal handler out of context, should probably be changed*/
            //runsig();

            record("SIGUSR1 received, starting sequence\n");
        }
                
        /* if ROE active, set to known state (exit default, reset, exit default) */
        //Add code here

        /*establish current sequence */
        currentSequence = sequenceMap[ops.sequence];
        sprintf(msg, "Current sequence: %s %d\n", currentSequence.sequenceName, ops.sequence);
        record(msg);

        /* push packets w/info about current sequence */
        packet_t* a = (packet_t*) constructPacket(MDAQ_RSP, BEGIN_SEQ, (char *) NULL);
        packet_t* r = (packet_t*) constructPacket(MDAQ_RSP, GT_CUR_SEQ, currentSequence.sequenceName);
                enqueue(&hkdownQueue, a);
                enqueue(&hkdownQueue, r);

        /* for each exposure in the sequence */
                printf("Number of Frames:%d\n", currentSequence.numFrames);
        for (i = 0; i < currentSequence.numFrames; i++) {
            sprintf(msg, "Starting exposure for duration: %3.3f seconds (%d out of %d)\n", currentSequence.exposureTimes[i], i + 1, currentSequence.numFrames);
            record(msg);
            /* check for running, paused, roe active.... */
            while (ops.seq_pause == TRUE) //pause if necessary
            {
                sprintf(msg, "PAUSE = TRUE\n");
                record(msg);
                if (ops.seq_run == FALSE) //break out of sequence if necessary
                {
                    sprintf(msg, "Sequence paused and not running, breaking out of sequence.\n");
                    record(msg);
                    break;
                }
            }

            if (ops.seq_run == FALSE) {
                sprintf(msg, "Sequence not running, breaking out of sequence.\n");
                record(msg);
                break;
            }


            /* */
            //Add roe active code here
            sprintf(msg, "Taking exposure for duration: %3.3f seconds.\n", currentSequence.exposureTimes[i]);
            record(msg);
            int duration = takeExposure(currentSequence.exposureTimes[i], currentSequence.seq_type);
            /*get the exposure index */

            /*push packets with information about frame(index and exposure length) */
            sprintf(sindex, "%d", i);
            sprintf(sframe, "%6.3f", currentSequence.exposureTimes[i]);

            //Packet* a = (Packet*)constructPacket("MDAQ_RSP", "GT_CUR_FRMI", sindex);
            //Packet* b = (Packet*)constructPacket("MDAQ_RSP", "GT_CUR_FRML", sframe);
            //enqueue(&hkdownQueue, a);
            //enqueue(&hkdownQueue, b);
            /* clear the pixel buffers */
            //short number = 0;
            //for (j = 0; j < 4; j++) {
            //    for (k = 0; k < 1; k++) {
            //        BUFFER[j][k] = number;
            //    }
            //}

            /*initialize index( these will start at -1 and be incremented by DMA*/
            int index[4] = {2200000, 2200000, 2200000, 2200000};

            //int duration = 1;
            int num = currentSequence.currentFrame;
            record("Done with exposure, perform data collection.\n");

            /* initialize the DMA */

            /* command FPGA to start reading  and send packet*/

            /* command ROE to readout, then dmaWait, and finish and sort? */

            /* push packet w/info about end read out */
            /* write buffer data to disk  and telemetry*/
            record("Writing data to disk.\n");
            if (ops.dma_write == 1)
                write_data(duration, num, BUFFER, index);

            sprintf(msg, "Exposure of %3.3lf seconds complete %d.\n\n", currentSequence.exposureTimes[i], i);
            record(msg);
        }/* end for each exposure */

        /* done with sequence, push packet with info */
        sprintf(msg, "Done with sequence %s\n\n\n", currentSequence.sequenceName);
        record(msg);

        //a = (Packet*)constructPacket("MDAQ_RSP","END_SEQ",0,(char *)NULL);
        //enqueue(&hkdownQueue, a);

        /*Reset the flags*/
        //ops.seq_run = FALSE;
        //ops.seq_pause = TRUE;

        /*Move to next sequence*/

        ops.sequence++;
        sprintf(msg,"ops.sequence: %d, seq_map_size:%d", ops.sequence, seq_map_size);
        record(msg);
//        if (ops.sequence > (sizeof (sequenceMap) / sizeof (sequence_t)) - 1) {
            if(ops.sequence > seq_map_size - 1){
            /* We have gone through all of the sequences, exit the program*/
            record("Done with sequences\n");
            ops.seq_run = FALSE;
            ops.seq_pause = TRUE;
            ts_alive = 0;
        }
    }
    /* delete pixel buffers */
    free(BUFFER[0]);
    free(BUFFER[1]);
    free(BUFFER[2]);
    free(BUFFER[3]);




    record("Done with scienceTimeline\n");
    return NULL;
}

void runsig() {
    char msg[100]; // for writing strings
    printf(msg, "seq_run: %d, seq_pause: %d\n", ops.seq_run, ops.seq_pause);
    ops.seq_run = TRUE;
    ops.seq_pause = FALSE;
    printf(msg, "seq_run: %d, seq_pause: %d\n", ops.seq_run, ops.seq_pause);
}

/*this function sets up the signal handler to run
  runsig when a signal is received from the experiment manager*/
void init_signal_handler_stl() {

    sigfillset(&oldmaskstl); //save the old mask
    sigemptyset(&maskstl); //create a blank new mask
    sigaddset(&maskstl, SIGUSR1); //add SIGUSR1 to mask

    /*no signal dispositions for signals between threads*/
        run_action.sa_handler = runsig;
        run_action.sa_mask = oldmaskstl;
        run_action.sa_flags = 0;
    
        sigaction(SIGUSR1, &run_action, NULL);
    record("Signal handler initiated.\n");

}
