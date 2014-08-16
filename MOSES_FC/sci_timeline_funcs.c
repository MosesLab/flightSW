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
        timeval_subtract(&expdiff, expstart, expstop);
        sprintf(msg, "Computer Time: %lu seconds, %lu microseconds\n", expdiff.tv_sec, expdiff.tv_usec);
        record(msg);
    } else // performing dark exposure, just wait
    {
        gettimeofday(&expstart, NULL);
        actual = wait_exposure(dur); //+PULSE);
        gettimeofday(&expstop, NULL);
        timeval_subtract(&expdiff, expstart, expstop);
        sprintf(msg, "Computer Time: %lu seconds, %lu microseconds\n", expdiff.tv_sec, expdiff.tv_usec);
        record(msg);
    }
    return actual;
}



int wait_exposure(int microsec) {
    /*This function doesnt work*/
    struct timeval start, end;

    gettimeofday(&start, NULL);

    usleep(microsec);
    
    gettimeofday(&end, NULL);

    return ((end.tv_sec - start.tv_sec)*1000000 +
            (end.tv_usec - start.tv_usec));
}

void timeval_subtract(struct timeval * result, struct timeval start, struct timeval end) {

    long t = (end.tv_sec*1e6 + end.tv_usec) - (start.tv_sec*1e6 + start.tv_usec);
    
    result->tv_sec = t / 1e6;
    result->tv_usec = t - result->tv_sec * 1e6;
    
}