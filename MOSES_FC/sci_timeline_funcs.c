/**************************************************
* Author: David Keltgen                           *
* Company: Montana State University: MOSES LAB    *
* File Name: scienceTimelineFuncs.c               *
* Date:  June 4 2014                              *
* Description: Functions used in the              *
*              Science Timeline.                  *
**************************************************/

#include "sci_timeline_funcs.h"


int takeExposure(double duration, int sig)
{
    char msg[100];
//    char dtime[32];
    struct timeval expstop, expstart;
    //duration is the exposure length in microseconds'
    int dur = (int) (duration*1000000);// - PULSE;
    /*actual is the physical amount of time the shutter is open*/
    int actual; 
    
    //int i;
    //for(i = 0; i < 5; i++)
        //roe->flush(); //Flush ROE 5 times
        
    if(sig == 1) // use the shutter for Data sequence 
    {
        //send open shutter signal to DIO
        gettimeofday(&expstart, NULL);
        
        //wait for interval to open shutter
        
        //clear pin to avoid excess current
        
        //wait for exposure duration, calculate with the pulse
        actual = wait_exposure(dur) + PULSE;
        // send close shutter signal to DIO
        
        // wait for interval to close shutter
        
        //clear the pin
        gettimeofday(&expstop, NULL);
        sprintf(msg,"Computer Time: %lu seconds, %lu microseconds\n", expstop.tv_sec - expstart.tv_sec, expstop.tv_usec - expstart.tv_usec);
        record(msg);
    }
    else // performing dark exposure, just wait
    {
        gettimeofday(&expstart, NULL);
        actual = wait_exposure(dur);//+PULSE);
        gettimeofday(&expstop, NULL);
        sprintf(msg,"Computer Time: %lu seconds, %lu microseconds\n", expstop.tv_sec - expstart.tv_sec, expstop.tv_usec - expstart.tv_usec);
        record(msg);
    }
    return actual;    
}

int write_data(int duration, int num, short **BUFFER, int *index)
{
    char msg[100];
//    char name[100];
    char filename[80];
//    char filepath[80];
    char ftimedate[80];
    char dtime[100];
    char ddate[100];
    
    //char dataDir[10] = "/mdata";
    char channels = ops.channels;
    
    /*Initialize the image*/
    constructImage(BUFFER,index, channels, 16);
    /* Get data for image details*/
    time_t curTime = time(NULL);
    struct tm *broken = localtime(&curTime);
    strftime(dtime,20, "%H:%M:%S", broken);
    strftime(ddate,20,"%y-%m-%d",broken);	//get date
    strftime(ftimedate,40,"%d%m%y%H%M%S",broken);
    //construct a unique filename	
    sprintf(filename,"%s.roe",ftimedate);
    sprintf(filename,"%s/%s.roe",DATADIR,ftimedate);
    
    record("Image Opened\n");
    image.filename = filename;
    image.name = currentSequence.sequenceName;		//Add the information to the image
    image.date = ddate;			
    image.time = dtime;			
    image.duration = duration;			
    image.width = 2048;				
    image.height = 1024;
    sprintf(msg,"Writing image file %s\n", filename);
    record(msg);
    /*write the image and metadata to disk*/
    writeToFile(filename); //write the image and metadata to disk
    
    /*push the filename onto the telemetry queue*/
    if(ops.tm_write == 1)
    {
        imgPtr_t newPtr;
        newPtr.filePath = filename;
        newPtr.next = NULL;
        enqueue(&roeQueue, &newPtr);                       //enqueues the path for telem
    }
   
    sprintf(msg,"File %s successfully written to disk.\n",filename);
    record(msg);
    
    /*need to free allocated image to prevent memory leak --RTS*/
    free(image.data[0]);
    free(image.data[1]);
    free(image.data[2]);
    free(image.data[3]);
    
    return 0;
}

int wait_exposure(int microsec)
{
    /*This function doesnt work*/
    struct timeval start, end;
    //int sec, usec;
    //sec = microsec/1000000;
    //usec = microsec%1000000;
    gettimeofday(&start,NULL);
    
    /*this is incorrect, using busy wait*/
//    while((end.tv_sec*1000000+end.tv_usec) < 
//	      (start.tv_sec*1000000+start.tv_usec+microsec))
//    {
//            gettimeofday(&end, NULL);
//    }
    
    usleep(microsec);
    gettimeofday(&end, NULL);
    
    return ((end.tv_sec-start.tv_sec)*1000000 + 
                (end.tv_usec-start.tv_usec));
}

