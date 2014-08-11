/**************************************************
 * Author: David Keltgen                           *
 * Company: Montana State University: MOSES LAB    *
 * File Name: scienceTimelineFuncs.c               *
 * Date:  June 4 2014                              *
 * Description: Functions used in the              *
 *              Science Timeline.                  *
 **************************************************/

#include "sci_timeline_funcs.h"

int takeExposure(double duration, int sig) {
    char msg[100];
    struct timeval expstop, expstart, expdiff;
    int dur = (int) (duration * 1000000); // - PULSE; //duration is the exposure length in microseconds'
    int actual; // computer recorded time interval between opening and closing the shutter
    
    //int i;
    //for(i = 0; i < 5; i++)
    //flush(); //Flush ROE 5 times

    if (sig == 1) // use the shutter for Data sequence 
    {
        //send open shutter signal to DIO
        open_shutter();
        
        gettimeofday(&expstart, NULL);

        //wait for interval to open shutter

        //clear pin to avoid excess current

        //wait for exposure duration, calculate with the pulse
        actual = wait_exposure(dur) + PULSE;
        
        // send close shutter signal to DIO
        close_shutter();
        

        // wait for interval to close shutter

        //clear the pin
        gettimeofday(&expstop, NULL);
        timeval_subtract(&expdiff, &expstart, &expstop);
        sprintf(msg, "Computer Time: %lu seconds, %lu microseconds\n", expdiff.tv_sec, expdiff.tv_usec);
        record(msg);
    } else // performing dark exposure, just wait
    {
        gettimeofday(&expstart, NULL);
        actual = wait_exposure(dur); //+PULSE);
        gettimeofday(&expstop, NULL);
        sprintf(msg, "Computer Time: %lu seconds, %lu microseconds\n", expstop.tv_sec - expstart.tv_sec, expstop.tv_usec - expstart.tv_usec);
        record(msg);
    }
    return actual;
}

/* write_data will be part of a new thread that will be in
 * waiting mode until it is called to write an image to a file. 
   A signal, SIGUSR2, will be sent when DMA is ready to transfer
   data to memory and will initialize the writing to disk*/
int write_data() {

    prctl(PR_SET_NAME,"IMAGE_WRITER",0,0,0);
//    struct timeval expstop, expstart;
    
    while (ts_alive) {

        short *BUFFER[4];
        /*create pixel buffers */
        int i;
        for (i = 0; i < 4; i++) {
            BUFFER[i] = (short *) calloc(2200000, sizeof (short));
        }
        /*initialize index( these will start at -1 and be incremented by DMA*/
        int index[4] = {2200000,2200000,2200000,2200000};

        char msg[100];
        char filename[80];
        char ftimedate[80];
        char dtime[100];
        char ddate[100];

        char channels = ops.channels;

        init_signal_handler_image();
        /* Wait for SIGUSR2 (When received response from ROE readout)*/
        pthread_sigmask(SIG_BLOCK, &maskimage, &oldmaskimage);
        record("Waiting for signal...\n");
        sigwait(&maskimage, &caught_image_signal);
        pthread_sigmask(SIG_UNBLOCK, &maskimage, &oldmaskimage);
        record("SIGUSR2 Received, reading DMA and writing to disk now\n");

//        gettimeofday(&expstart, NULL);
//        //initializeDMA();
//        
//        /*DMA Channel is open, now send GPIO*/
//        write_gpio(POWER_OFFSET, 0x0F);
//        sleep(1);
//        write_gpio(POWER_OFFSET, 0x00);
//        /*Buffer updated here*/
//        //dmaRead();
//        gettimeofday(&expstop, NULL);
        
//        sprintf(msg, "Time from initialize to Interrupt received: %lu seconds, %lu microseconds\n", expstop.tv_sec - expstart.tv_sec, expstop.tv_usec - expstart.tv_usec);
        
        /*Clear DMA block*/

        /*Initialize the image*/
        constructImage(BUFFER, index, channels, 16);
        /* Get data for image details*/
        time_t curTime = time(NULL);
        struct tm *broken = localtime(&curTime);
        strftime(dtime, 20, "%H:%M:%S", broken);
        strftime(ddate, 20, "%y-%m-%d", broken); //get date
        strftime(ftimedate, 40, "%d%m%y%H%M%S", broken);
        //construct a unique filename	
        sprintf(filename, "%s.roe", ftimedate);
        sprintf(filename, "%s/%s.roe", DATADIR, ftimedate);

        record("Image Opened\n");
        image.filename = filename;
        image.name = currentSequence.sequenceName; //Add the information to the image
        image.date = ddate;
        image.time = dtime;
        //image.duration = duration;
        image.width = 2048;
        image.height = 1024;
        sprintf(msg, "Writing image file %s\n", filename);
        record(msg);

        /* Copy original image to temporary struct for image_writer_thread access.
         * This is to ensure that the image can be written to disk without being overwritten
         * by the next exposure */

        memcpy(&tempimage, &image, sizeof (tempimage));

        tempimage.filename = strdup(image.filename);
        tempimage.name = strdup(image.name); //string
        tempimage.bitpix = image.bitpix;
        tempimage.width = image.width;
        tempimage.height = image.height;
        tempimage.date = strdup(image.date); //string
        tempimage.time = strdup(image.time); //string
        tempimage.origin = strdup(image.origin); //string
        tempimage.instrument = strdup(image.instrument); //string
        tempimage.observer = strdup(image.observer); //string
        tempimage.object = strdup(image.object); //string 
        tempimage.duration = image.duration;
        tempimage.channels = image.channels;
        tempimage.size[0] = image.size[0];
        tempimage.size[1] = image.size[1];
        tempimage.size[2] = image.size[2];
        tempimage.size[3] = image.size[3];
        for (i = 0; i < 4; i++)
            memcpy((char *) tempimage.data[i], (char *) image.data[i], image.size[i]); //copy data

        /*write the image and metadata to disk*/
        writeToFile(); 

        /*push the filename onto the telemetry queue*/
        if (ops.tm_write == 1) {
            imgPtr_t newPtr;
            newPtr.filePath = filename;
            newPtr.next = NULL;
            enqueue(&roeQueue, &newPtr); //enqueues the path for telem
            record("Filename pushed to telemetry queue\n");
        }

        sprintf(msg, "File %s successfully written to disk.\n", filename);
        record(msg);

        /*need to free allocated image to prevent memory leak --RTS*/
        free(image.data[0]);
        free(image.data[1]);
        free(image.data[2]);
        free(image.data[3]);
        free(BUFFER[0]);
        free(BUFFER[1]);
        free(BUFFER[2]);
        free(BUFFER[3]);
    }//end while ts_alive
    
    return 0;
}

int wait_exposure(int microsec) {
    /*This function doesnt work*/
    struct timeval start, end;
    //int sec, usec;
    //sec = microsec/1000000;
    //usec = microsec%1000000;
    gettimeofday(&start, NULL);

    /*this is incorrect, using busy wait*/
    //    while((end.tv_sec*1000000+end.tv_usec) < 
    //	      (start.tv_sec*1000000+start.tv_usec+microsec))
    //    {
    //            gettimeofday(&end, NULL);
    //    }

    usleep(microsec);
    gettimeofday(&end, NULL);

    return ((end.tv_sec - start.tv_sec)*1000000 +
            (end.tv_usec - start.tv_usec));
}

/*this function sets up the signal handler to run
  runsig when a signal is received from the experiment manager*/
void init_signal_handler_image() {

    sigfillset(&oldmaskimage); //save the old mask
    sigemptyset(&maskimage); //create a blank new mask
    sigaddset(&maskimage, SIGUSR2); //add SIGUSR1 to mask

    /*no signal dispositions for signals between threads*/
    run_action_image.sa_mask = oldmaskimage;
    run_action_image.sa_flags = 0;

    sigaction(SIGUSR2, &run_action_image, NULL);
    record("Signal handler initiated.\n");
}


int timeval_subtract (struct timeval * result, struct timeval * x, struct timeval * y)
{
       /* Perform the carry for the later subtraction by updating y. */
       if (x->tv_usec < y->tv_usec) {
         int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
         y->tv_usec -= 1000000 * nsec;
         y->tv_sec += nsec;
       }
       if (x->tv_usec - y->tv_usec > 1000000) {
         int nsec = (x->tv_usec - y->tv_usec) / 1000000;
         y->tv_usec += 1000000 * nsec;
         y->tv_sec -= nsec;
       }
     
       /* Compute the time remaining to wait.
          tv_usec is certainly positive. */
       result->tv_sec = x->tv_sec - y->tv_sec;
       result->tv_usec = x->tv_usec - y->tv_usec;
     
       /* Return 1 if result is negative. */
       return x->tv_sec < y->tv_sec;
     }