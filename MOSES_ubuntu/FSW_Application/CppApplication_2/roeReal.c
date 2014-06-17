/**************************************************
* Author: David Keltgen                           *
* Company: Montana State University: MOSES LAB    *
* File Name: roeSim.c             *
* Date:  June 4 2014                              *
* Description: Software representation of the     *
*              Read Out Electronics(ROE).         *
**************************************************/
#include "roe.h"

int ActivateROEReal()
{
    pthread_mutex_lock(&roe.mx);
    if(roe.active == FALSE)
    {
        //do activate stuff
    }
    
    roe.active = TRUE;
    pthread_mutex_unlock(&roe.mx);
    record("ROE Active\n");
    return 0;
}

int deactivateReal()
{
  pthread_mutex_lock(&roe.mx);
  if(roe.active == TRUE)
  {
    //close the link   
  }
  roe.active = FALSE;
  pthread_mutex_lock(&roe.mx);
  record("ROE Deactivated\n");
  }
  



//Exit Roe default mode and enter manual mode
int exitDefaultReal()
{
	record("Exiting Default Mode");
    	return 0;
}

//Put Roe into Selftest Mode
int selftestModeReal()
{
	record("Entering Self Test Mode");
    	return 0;
}

//Turn on the Stims
int stimOnReal()
{
	record("Entering Stims Mode");			
    	return 0;
}

//Turn off the Stims
int stimOffReal()
{
	record("Exiting Stims Mode");
    	return 0;
}

//Reset Roe back to default mode
int resetReal()
{
	record("Reseting to Default Mode");
    	return 0;
}

//Flush the ROE to readout the CCD
int readOutReal(char* block,int delay)
{
	record("Reading Out CCD's");
    	return 0;
}

//flush the ccd's
int flushReal()
{
	record("Flushing CCD's");
    	return 0;
}

//Request House Keeping data from roe;
int getHKReal(char* hkparam)
{
    	return 0xFF;
}

//Request the Analogue Electronics
char* getAEReal()
{
	/*char aTable[17] = "0123456789ABCDEF";
	unsigned char params[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	char* result = "";
	for(int i = 0; i < 8; i++)
	{
		result += aTable[params[i]/16];
		result += aTable[params[i]%16];
		//result += aTable[params[i]/16];
	}
	return result;*/
}

//Set the Analogue Electronics
int setAEReal(char* paramstring)
{
	record("Setting AE Params");
    	return 0;
}

//write string to roe
int manualWriteReal(char* msg, int size)
{
    	return 0;
}
