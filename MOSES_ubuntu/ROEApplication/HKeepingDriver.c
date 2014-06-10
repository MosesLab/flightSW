/**************************************************
* Author: David Keltgen                           *
* Company: Montana State University: MOSES LAB    *
* File Name: HKeepingDriver.c                          *
* Date: 5 June 2014                               *
* Description: Function calls to test the         *
 *             interface between the flight       *
 *             computer and the ROE.              *
**************************************************/

#include "HKeepingDriver.h"
#include "roe.h"
#include "roehk.h"

int SendHKCmds(void)
{
    char buffer[30];
    record("HK Thread started\n");
    
    sprintf(buffer,"%s : %d\n", "VPOS5VD",      getHKReal(VPOS5VD));
    record(buffer); //ROE +5VD Voltage
    
    /*usleep(10000);
    sprintf(buffer,"%s : %d\n", "VPOS2_5VD",    getHKReal(VPOS2_5VD));
    record(buffer); //ROE +2.5VD Voltage
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "VPOS5VA_A",   getHKReal(VPOS5VA_A));
    record(buffer); //ROE +5VAA Voltage
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "VPOS5VA_B",    getHKReal(VPOS5VA_B));
    record(buffer); //ROE +5VAB Voltage
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "VNEG5VA_A",    getHKReal(VNEG5VA_A));
    record(buffer); //ROE -5VAA Voltage
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "VNEG5VA_B",    getHKReal(VNEG5VA_B));
    record(buffer); //ROE -5VAB Voltage
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "VPOS36V_A",    getHKReal(VPOS36V_A));
    record(buffer); //ROE +36VA Voltage
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "VPOS36V_B",    getHKReal(VPOS36V_B));
    record(buffer); //ROE +36VB Voltage
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "VPOS12V_A",    getHKReal(VPOS12V_A));
    record(buffer); //ROE +12VA Voltage
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "VPOS12V_B",    getHKReal(VPOS12V_B));
    record(buffer); //ROE +12VB Voltage
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "CCDA_VODC",    getHKReal(CCDA_VODC));
    record(buffer); //Roe CCDA_VOD Current
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "CCDA_VRDC",    getHKReal(CCDA_VRDC));
    record(buffer); //Roe CCDA_VRD Current
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "CCDA_VSSC",    getHKReal(CCDA_VSSC ));
    record(buffer); //Roe CCDA_VSS Current
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "CPOS5VD",      getHKReal(CPOS5VD));
    record(buffer); //ROE +5VD Current
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "CPOS2_5VD",    getHKReal(CPOS2_5VD));
    record(buffer); //ROE +2.5VD Current
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "CPOS5VA_A",    getHKReal(CPOS5VA_A));
    record(buffer); //ROE +5VAA Current
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "CPOS5VA_B",    getHKReal(CPOS5VA_B));
    record(buffer); //ROE +5VAB Current
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "CNEG5VA_A",    getHKReal(CNEG5VA_A));
    record(buffer); //ROE -5VAA Current
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "CNEG5VA_B",    getHKReal(CNEG5VA_B));
    record(buffer); //ROE -5VAB Current
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "CPOS36V_A",   getHKReal(CPOS36V_A));
    record(buffer); //ROE +36VA Current
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "CPOS36V_B",    getHKReal(CPOS36V_B));
    record(buffer); //ROE +36VB Current
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "CPOS12V_A",    getHKReal(CPOS12V_A));
    record(buffer); //ROE +12VA Current
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "CPOS12V_B",    getHKReal(CPOS12V_B));
    record(buffer); //ROE +12VB Current
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "UPPER_TEMP",   getHKReal(UPPER_TEMP));
    record(buffer); //ROE Upper Temp
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "LOWER_TEMP",  getHKReal(LOWER_TEMP));
    record(buffer); //ROE Upper Temp
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "VNEG10V_A",    getHKReal(VNEG10V_A));
    record(buffer); //ROE -10VA Voltage
    
    usleep(10000);
    
    sprintf(buffer,"%s : %d\n", "VNEG10V_B",    getHKReal(VNEG10V_B));
    record(buffer); //ROE -10VB Voltage
    
    
    //sprintf(buffer,"%s : %s", END_OF_FRAME getHKReal(END_OF_FRAME));
    //record(buffer); //ROE +5VAB Voltage
    */
    
    record("Done sending HK commands\n");
    return 0;
}