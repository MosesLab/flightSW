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
    struct timeval expstop, expstart, expmid, shutdiff, expdiff;
    int dur = (int) (duration * 1000000); //duration is the exposure length in microseconds'
    int actual; // computer recorded time interval between opening and closing the shutter

    int i;
    for(i = 0; i < 5; i++)
    {
        //flush(); //Flush ROE 5 times
    }
    if (sig == 1) // use the shutter for Data sequence 
    {
        /*exposure duration should be lengthened to accout for shutter openeing*/
        dur = dur + PULSE;
        
        /*start measuring exposure duration*/
        gettimeofday(&expstart, NULL);

        //send open shutter signal to DIO
        open_shutter();
        
        /*take another time measurement to determine remaining time to wait */
        gettimeofday(&expmid, NULL);
        
        /*calculate remaining time to wait, including pulse*/
        timeval_subtract(&shutdiff, expstart, expmid);
        int open_time = shutdiff.tv_sec * 1000000 + shutdiff.tv_usec;
        int time_wait = dur - open_time;

//        //wait for exposure duration, calculate with the pulse
//        actual = wait_exposure(time_wait) + open_time - PULSE;
        
        /*sleep for remaining exposure duration*/
        usleep(time_wait);      

        gettimeofday(&expstop, NULL);
        // send close shutter signal to DIO
        close_shutter();


        // wait for interval to close shutter

        //clear the pin

        timeval_subtract(&expdiff, expstart, expstop);
        sprintf(msg, "Computer Time: %lu:%06lu sec\n", expdiff.tv_sec, expdiff.tv_usec);
        record(msg);
        actual = expdiff.tv_sec * 1e6 + expdiff.tv_usec - PULSE;
    } else // performing dark exposure, just wait
    {
        gettimeofday(&expstart, NULL);
        usleep(dur);
        gettimeofday(&expstop, NULL);
        
        timeval_subtract(&expdiff, expstart, expstop);
        sprintf(msg, "Computer Time: %lu:%06lu sec\n", expdiff.tv_sec, expdiff.tv_usec);
        record(msg);
        actual = expdiff.tv_sec * 1e6 + expdiff.tv_usec;
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

    long t = (end.tv_sec * 1e6 + end.tv_usec) - (start.tv_sec * 1e6 + start.tv_usec);

    result->tv_sec = t / 1e6;
    result->tv_usec = t - result->tv_sec * 1e6;

}
