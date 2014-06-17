/**************************************************
* Author: David Keltgen                           *
* Company: Montana State University: MOSES LAB    *
* File Name: roeSim.c             *
* Date:  June 4 2014                              *
* Description: Functions used for the purpose of               *
*              testing the timing of addressing
 *             the ROE.                   *
**************************************************/
#include "roe.h"

int startupROESim()
{
    record("Connection established. DEFAULT MODE");
    return 0;
}

//Exit Roe default mode and enter manual mode
int exitDefaultSim()
{
	record("Exiting Default Mode");
    	return 0;
}

//Put Roe into Selftest Mode
int selftestModeSim()
{
	record("Entering Self Test Mode");
    	return 0;
}

//Turn on the Stims
int stimOnSim()
{
	record("Entering Stims Mode");			
    	return 0;
}

//Turn off the Stims
int stimOffSim()
{
	record("Exiting Stims Mode");
    	return 0;
}

//Reset Roe back to default mode
int resetSim()
{
	record("Reseting to Default Mode");
    	return 0;
}

//Flush the ROE to readout the CCD
int readOutSim(char* block,int delay)
{
	record("Reading Out CCD's");
    	return 0;
}

//flush the ccd's
int flushSim()
{
	record("Flushing CCD's");
    	return 0;
}

//Request House Keeping data from roe;
int getHKSim(char* hkparam)
{
    	return 0xFF;
}

//Request the Analogue Electronics
char* getAESim()
{
	//char aTable[17] = "0123456789ABCDEF";
	//unsigned char params[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	char* result = "";
	//for(int i = 0; i < 8; i++)
	//{
	//	result += aTable[params[i]/16];
	//	result += aTable[params[i]%16];
	//	//result += aTable[params[i]/16];
	//}
	return result;
}

//Set the Analogue Electronics
int setAESim(char* paramstring)
{
	record("Setting AE Params");
    	return 0;
}

//write string to roe
int manualWriteSim(char* msg, int size)
{
    	return 0;
}