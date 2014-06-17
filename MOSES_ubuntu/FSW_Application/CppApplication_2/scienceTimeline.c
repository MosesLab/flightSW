/* 
 * File:   ScienceTimeline.c
 * Author: David Keltgen
 *
 * Created on May 28, 2014, 11:35 AM
 * 
 * ScienceTimeline will be responsible for providing the functionality of
 * the Science Timeline thread.
 */

#include "defs.h"


/**************************************************
 * Author: David Keltgen                            *
 * Company: Montana State University: MOSES LAB     *
 * File Name: scienceTimeline.c                     *
 * Date:  May 28 2014                               *
 * Description: ScienceTimeline will be responsible *
 *              for providng the functionality of   *
 *              the Science Timeline Thread.        *
 **************************************************/

#include <signal.h>
#include "scienceTimeline.h"
#include "sequence.h"

#define TRUE  1
#define FALSE 0

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
    sigaddset(&oldmaskstl, SIGUSR1); //add SIGUSR1 to mask
    run_action.sa_handler = runsig;
    run_action.sa_mask = oldmaskstl;
    run_action.sa_flags = 0;

    sigaction(SIGUSR1, &run_action, NULL);
    record("Signal handler initiated.\n");
}

int ScienceTimeline(void) {
    //do
    //{
      // printf("time = 0\n"); 
    //}
    //while(1);
    record("ScienceTimeline thread started.\n");

    char msg[1000]; // for writing strings
    int i, j, k;
    short *BUFFER[4]; //[2200000];  

    pthread_sigmask(SIGUSR1, &maskstl, &oldmaskstl);


    //sigsuspend(&oldmaskstl); 

    ops.sequence = sequence1; // start off with the first sequence;

    printf("sizeofshort %d\n", sizeof(short));
    /*create pixel buffers */
    for (i = 0; i < 4; i++) {
        BUFFER[i] = (short *) malloc(2200000 * sizeof (short));
    }

    /* wait for ROE to become active */
    //Add code here

    /* ROE initialization: ROE exitdefault, reset, exitdefault */
    //Add code here

    while (ts_alive) {

        if (ops.seq_pause == TRUE) {
            record("Sequence paused\n");
            //sigsuspend(&oldmaskstl);   // wait here until signal is sent to unpause
            record("After SIGUSR1\n");
        }
        printf("flag\n");
        /* if ROE active, set to known state (exit default, reset, exit default) */
        //Add code here

        /*establish current sequence */
        printf("sequence %d\n", ops.sequence);
        printf("%d\n", sequenceMap[ops.sequence].numFrames);
        currentSequence = sequenceMap[ops.sequence];
        printf("Current sequence: %s \n", currentSequence.sequenceName);

        for(i = 0; i < 5; i++)
        {
           printf("sequence name %s\n", sequenceMap[i].sequenceName);
        }

        /* push packets w/info about current sequence */
        Packet* a = constructPacket("MDAQ_RSP", "BEGIN_SEQ", 0, (char *) NULL);
        Packet* r = constructPacket("MDAQ_RSP", "GT_CUR_SEQ", currentSequence.sequenceName);
        enqueue(&hkdownQueue, a);
        enqueue(&hkdownQueue, r);

        /* for each exposure in the sequence */
        for (i = 0; i < currentSequence.numFrames; i++) {
            printf("exposuretime %3.3d\n", currentSequence.exposureTimes[i]);
            /* check for running, paused, roe active.... */
            while (ops.seq_pause == TRUE) //pause if necessary
            {
                if (ops.seq_run == FALSE) //break out of sequence if necessary
                    break;
            }

            if (ops.seq_run == FALSE) break; //"		"

            /* */
            //Add roe active code here
            //sprintf(msg, "Taking exposure for duration: %3.3f    %d\n", currentSequence.exposureTimes[i], i);
           //record(msg);
            /*get the exposure index */

            /*push packets with information about frame(index and exposure length) */
            sprintf(sindex, "%d", i);
            sprintf(sframe, "%6.3f", currentSequence.exposureTimes[i]);

            Packet* a = constructPacket("MDAQ_RSP", "GT_CUR_FRMI", sindex);
            Packet* b = constructPacket("MDAQ_RSP", "GT_CUR_FRML", sframe);
            enqueue(&hkdownQueue, a);
            enqueue(&hkdownQueue, b);
            /* clear the pixel buffers */
            //short number = 0;
            //for (j = 0; j < 4; j++) {
            //    for (k = 0; k < 1; k++) {
            //        BUFFER[j][k] = number;
            //    }
            //}
            
            /*initialize index( these will start at -1 and be incremented by DMA*/  
            int index[4] = {2200000, 2200000, 2200000, 2200000};
            /* setup the data frame info and take exposure */
            int duration = takeExposure(currentSequence.exposureTimes[i], currentSequence.seq_type);
            //int duration = 1;
            int num = currentSequence.currentFrame;
            record("Done with exposure, perform data collection\n");

            /* initialize the DMA */

            /* command FPGA to start reading  and send packet*/

            /* command ROE to readout, then dmaWait, and finish and sort? */

            /* push packet w/info about end read out */
            /* write buffer data to disk  and telemetry*/
            record("Writing data to disk.\n");
           // if(ops.dma_write == 1)
                //write_data(duration, num, BUFFER, index);
            printf("Exposure of %3.3lf seconds complete %d.\n", currentSequence.exposureTimes[i], i);
            //record(msg);
        }/* end for each exposure */
        /* done with sequence, push packet with info */
        sprintf(msg,"Done with sequence %s\n", currentSequence.sequenceName);
        record(msg);
        
        //Packet* a = constructPacket("MDAQ_RSP","END_SEQ",0,(char *)NULL);
        //enqueue(&hkdownQueue, a);

        /*Reset the flags*/
        //ops.seq_run = FALSE;
        //ops.seq_pause = TRUE;
        
        
        
        /*Move to next sequence*/
        ops.sequence++;
        printf("%d     %d\n",ops.sequence, (sizeof (sequenceMap) / sizeof (sequence)) - 1);
        if (ops.sequence > (sizeof (sequenceMap) / sizeof (sequence)) - 1) {
            /* We have gone through all of the sequences, exit the program*/
            printf("Done with sequences (exit thread)\n");
            record("Done with sequences (exit thread)\n");
            //ts_alive = 0;
            break;
        }
        /* delete pixel buffers */
        //free(BUFFER);

    }

    record("Done with scienceTimeline\n");
    return 0;
}

/*signal handler*/

