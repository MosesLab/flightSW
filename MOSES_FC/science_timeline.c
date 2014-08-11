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
    //sleep(1);
    prctl(PR_SET_NAME,"SCI_TIMELINE",0,0,0);
    record("-->Science Timeline thread started....\n\n");
    init_signal_handler_stl();
    char* msg = (char *) malloc(200 * sizeof (char));

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
        if (ops.seq_run == FALSE) {
            record("Sequence stopped, wait for signal to start\n");

            pthread_sigmask(SIG_BLOCK, &maskstl, &oldmaskstl);
            sigwait(&maskstl, &caught_signal);
            pthread_sigmask(SIG_UNBLOCK, &maskstl, &oldmaskstl);

            ops.seq_run = TRUE;

            record("SIGUSR1 received, starting sequence\n");
        }
                
        /* if ROE active, set to known state (exit default, reset, exit default) */
        //Add code here
        //exitDefault();
        //reset();
        //exitDefault();
        

        /*establish current sequence */
        currentSequence = sequenceMap[ops.sequence];
        sprintf(msg, "Current sequence: %s, Sequence number:%d\n", currentSequence.sequenceName, ops.sequence);
        record(msg);

        /* push packets w/info about current sequence */
        packet_t* a = (packet_t*) constructPacket(MDAQ_RSP, BEGIN_SEQ, (char *) NULL);
        packet_t* b = (packet_t*) constructPacket(MDAQ_RSP, GT_CUR_SEQ, currentSequence.sequenceName);
        enqueue(&hkdownQueue, a);
        enqueue(&hkdownQueue, b);

        /* for each exposure in the sequence */
        int i;
        for (i = 0; i < currentSequence.numFrames; i++) {
            sprintf(msg, "Starting exposure for duration: %3.3f seconds (%d out of %d)\n", currentSequence.exposureTimes[i], i + 1, currentSequence.numFrames);
            record(msg);
            /* check for running, roe active.... */
             
            //If ROE not active?
            
            if (ops.seq_run == FALSE) {
                sprintf(msg, "Sequence not running, breaking out of sequence.\n");
                record(msg);
                break;
            }

            sprintf(msg, "Taking exposure for duration: %3.3f seconds.\n", currentSequence.exposureTimes[i]);
            record(msg);
            int duration = takeExposure(currentSequence.exposureTimes[i], currentSequence.seq_type);
            image.duration = duration;
            /*push packets with information about frame(index and exposure length) */
            sprintf(sindex, "%d", i);
            sprintf(sframe, "%6.3f", currentSequence.exposureTimes[i]);

            a = (packet_t*)constructPacket("MDAQ_RSP", GT_CUR_FRMI, sindex);
            b = (packet_t*)constructPacket("MDAQ_RSP", GT_CUR_FRML, sframe);
            enqueue(&hkdownQueue, a);
            enqueue(&hkdownQueue, b);

            /*initialize index( these will start at -1 and be incremented by DMA*/

            record("Done with exposure, perform data collection.\n");

            /* Command ROE to Readout*/
            //readOut(...);
            
            /* push packet w/info about end read out */
            a = (packet_t*)constructPacket("MDAQ_RSP", GT_CUR_FRMI, sindex);
            enqueue(&hkdownQueue, a);
            
            /* write buffer data to disk  and telemetry*/
            record("Signal disk write.\n");

            //wait 4 seconds for response from ROE
            sleep(4);
            //poll for response?
            if (ops.dma_write == 1 && threads[image_writer_thread])
            { 
                pthread_kill(threads[image_writer_thread], SIGUSR2); //tell image_writer to start dma transfer
            }

            sprintf(msg, "Exposure of %3.3lf seconds complete.\n\n", currentSequence.exposureTimes[i]);
            record(msg);
        }/* end for each exposure */

        /* done with sequence, push packet with info */
        sprintf(msg, "Done with sequence %s\n\n\n", currentSequence.sequenceName);
        record(msg);

        a = (packet_t*)constructPacket(MDAQ_RSP,END_SEQ,(char *)NULL);
        enqueue(&hkdownQueue, a);    
        record("Done witps.seq_run = FAh sequences\n");
        ops.seq_run = FALSE;

    }//end while ts_alive

    record("Done with scienceTimeline\n");
    return NULL;
}

/*this function sets up the signal handler to run
  runsig when a signal is received from the experiment manager*/
void init_signal_handler_stl() {

    sigfillset(&oldmaskstl); //save the old mask
    sigemptyset(&maskstl); //create a blank new mask
    sigaddset(&maskstl, SIGUSR1); //add SIGUSR1 to mask

    /*no signal dispositions for signals between threads*/
    run_action.sa_mask = oldmaskstl;
    run_action.sa_flags = 0;
    
    sigaction(SIGUSR1, &run_action, NULL);
    record("Signal handler initiated.\n");
}
