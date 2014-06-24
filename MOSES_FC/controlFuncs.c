/*
 * controlFuncs.c
 * 
 * Author: Roy Smart
 * Created: May 15, 2014
 * 
 * contains the functions that direct the flight computer what to do in the 
 * event of telemetry input, e.g. timers, uplinks and hlp packets.
 */
#include "controlFuncs.h"

/*
 * Determines control string for uplink packets and finds the corresponding 
 * function by looking up the control string in the hash table
 */
int execPacket(Packet* p) {
    node_t* np =lookup(hlpHashTable, p->control, hlp_hash_size); //Lookup corresponding function in table
    if (np == NULL) {
        return BAD_PACKET; //bad acknowlege if unsuccessful in finding control string in hash table
    }
    int status = (*((hlpControl)np->def))(NULL); //Call control function, cast from void pointer
    return status;
}

/*Uplink control functions*/
int uDataStart(Packet* p) {
    record("Received data start Uplink\n");
    
    /*send signal to science timeline to start data*/
    pthread_kill(threads[sci_timeline_thread], SIGUSR1);
    
    Packet* r = constructPacket(UPLINK_S, DATASTART, NULL);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

int uDataStop(Packet* p) {
    record("Received data stop Uplink\n");
    //Insert uplink handling code here
    Packet* r = constructPacket(UPLINK_S, DATASTOP, NULL);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

int uDark1() {
    record("Received Dark1 Uplink\n");
    //Insert uplink handling code here
    Packet* r = constructPacket(UPLINK_S, DARK1, NULL);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

int uDark2(Packet* p) {
    record("Received Dark2 Uplink");
    //Insert uplink handling code here
    Packet* r = constructPacket(UPLINK_S, DARK2, NULL);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

int uDark3(Packet* p) {
    record("Received Dark3 Uplink\n");
    //Insert uplink handling code here
    Packet* r = constructPacket(UPLINK_S, DARK3, NULL);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

int uDark4(Packet* p) {
    record("Received Dark4 Uplink\n");
    //Insert uplink handling code here
    Packet* r = constructPacket(UPLINK_S, DARK4, NULL);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

int uSleep(Packet* p) {
    record("Received shutdown Uplink\n");
    //Insert uplink handling code here
    Packet* r = constructPacket(UPLINK_S, SLEEP, NULL);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

int uWake(Packet* p) {
    record("Received shutdown pending Uplink\n");
    //Insert uplink handling code here
    Packet* newp = constructPacket(UPLINK_S, WAKE, NULL);
    enqueue(&hkdownQueue, newp);
}

int uTest(Packet* p) {
    record("Received test Uplink\n");
    //Insert uplink handling code here
    Packet* r = constructPacket(UPLINK_S, TEST, NULL);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Timer control functions*/
int tDataStart(Packet* p) {
    record("Received data start Timer\n");
    //Insert timer handling code here
    Packet* r = constructPacket(TIMER_S, DATASTART, NULL);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

int tDataStop(Packet* p) {
    record("Received data stop Timer\n");
    //Insert timer handling code here
    Packet* r = constructPacket(TIMER_S, DATASTOP, NULL);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

int tDark2(Packet* p) {
    record("Received Dark2 Timer\n");
    //Insert timer handling code here
    Packet* r = constructPacket(TIMER_S, DARK2, NULL);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

int tDark4(Packet* p) {
    record("Received Dark4 Timer\n");
    //Insert timer handling code here
    Packet* r = constructPacket(TIMER_S, DARK4, NULL);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

int tSleep(Packet* p) {
    record("Received shutdown Timer\n");
    //Insert timer handling code here
    Packet* r = constructPacket(TIMER_S, SLEEP, NULL);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}
/*
 *Mission Data Acquisition (MDAQ) control functions 
 */

/*Associates a sequence name with a signal number*/
int setSequence(Packet* p) {
    record("Set sequence command received\n");
    //Insert set sequence code here
    return GOOD_PACKET;
}

/*Uses stem for an output file.*/
int setOutputName(Packet* p) {
    record("Set output filename command received\n");
    //insert control code here
    return GOOD_PACKET;
}

/*Get sequence name that corresponds to the signal number in the data field*/
int getSeqName(Packet* p) {
    record("Get sequence name command received\n");
    //Insert control code here
    char* name = "testname"; //test EGSE
    Packet* r = constructPacket(MDAQ_RSP, GT_SEQ_NM, name);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Generates a string representation of the sequence file*/
int getSeqInfo(Packet* p) {
    record("Get sequence info command received\n");
    //Insert control code here
    char* info = "testinfo"; //test EGSE
    Packet* r = constructPacket(MDAQ_RSP, GT_SEQ_INFO, info);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Commands the flight software to return the currently running sequence*/
int getCurSeqName(Packet* p) {
    record("Get current sequence name command received\n");
    //Insert control code here
    char* name = "testname"; //test EGSE
    Packet* r = constructPacket(MDAQ_RSP, GT_CUR_SEQ, name);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Commands the flight software to return the current frame's exposure length*/
int getCurFrameLen(Packet* p) {
    record("Get current frame length command received\n");
    //Insert control code here  
    char* response = "0.0"; //test EGSE
    Packet* r = constructPacket(MDAQ_RSP, GT_CUR_FRML, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Command the flight software to return the current frame's index number*/
int getCurFrameIndex(Packet* p) {
    record("Get current frame index command received\n");
    //Insert control code here  
    char* response = "0"; //test EGSE
    Packet* r = constructPacket(MDAQ_RSP, GT_CUR_FRMI, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Commands the flight software to return the filename of the output file*/
int getOutputName(Packet* p) {
    record("Get output filename command received\n");
    //Insert control code here  
    char* response = "testOuputFilename"; //test EGSE
    Packet* r = constructPacket(MDAQ_RSP, GT_OFN, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Commands the flight software to determine if he ROE is in self-test mode*/
int getSelftestStatus(Packet* p) {
    record("Get self-test mode status command received\n");
    //Insert control code here  
    char* response = "test"; //test EGSE
    Packet* r = constructPacket(MDAQ_RSP, GT_SLFT_STS, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Commands the flight software to determine if the ROE is in STIMS mode*/
int getStimsStatus(Packet* p) {
    record("Get STIMS mode status command received\n");
    //Insert control code here  
    char* response = "test"; //test EGSE
    Packet* r = constructPacket(MDAQ_RSP, GT_STM_STS, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Commands the flight software to determine if Telemetry is enabled*/
int getTelemStatus(Packet* p) {
    record("Get Telemetry mode status command received\n");
    //Insert control code here  
    char* response = "test"; //test EGSE
    Packet* r = constructPacket(MDAQ_RSP, GT_TLM_STS, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Commands the flight software to determine if Channel 0 data will be recorded*/
int getCh0Status(Packet* p) {
    record("Get status of channel 0 record command received\n");
    //Insert control code here  
    char* response = "test"; //test EGSE
    Packet* r = constructPacket(MDAQ_RSP, GT_CH0_STS, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Commands the flight software to determine if positive channel data will be recorded*/
int getPosOnlyStatus(Packet* p) {
    record("Get status of positive channel record command received\n");
    //Insert control code here  
    char* response = "test"; //test EGSE
    Packet* r = constructPacket(MDAQ_RSP, GT_POS_STS, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Command the flight software to scale the current sequence, multiplying each frame by the
 floating point value contained within the Data field*/
int scaleSequence(Packet* p) {
    record("Command to scale sequence received\n");
    return GOOD_PACKET;
}

/*Commands the flight software to translate the current sequence, adding the 
 floating point value in the Data field to each frame in the sequence*/
int translateSeq(Packet* p) {
    record("Command to translate sequence received\n");
    return GOOD_PACKET;
}

/*Commands the flight software to interrupt the current exposure, find an exposure
 of the length given in the Data field, and jump to the first exposure matching
 this criterion*/
int findAndJump(Packet* p) {
    record("Command to Find and Jump received\n");
    //Insert control code here  
    char* response = "0"; //test EGSE
    Packet* r = constructPacket(MDAQ_RSP, FIND_N_JUMP, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Commands the flight software to jump to the exposure with index number given 
 in the data field*/
int jumpToExp(Packet* p) {
    record("Command to Jump to exposure received\n");
    //Insert control code here  
    char* response = "0"; //test EGSE
    Packet* r = constructPacket(MDAQ_RSP, JUMP, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}
/*CONFLICTING INTERFACES*/
/*HLP Document: Commands the flight software to save the current sequence to the 
 filename defined in the Data field (possibly "SVC") */

/*flightSW_2005: Saves the sequence defined in the Data field*/
int saveSequence(Packet* p) {
    record("Command to save sequence received\n");
    return GOOD_PACKET;
}

/*Commands the flight software to find exposures with lengths given by the first
 floating point value in the data field and replace them with the second value 
 the Data field*/
int findAndReplace(Packet* p) {
    record("Command to find and replace received\n");
    //Insert control code here  
    char* response = "0,0"; //test EGSE
    Packet* r = constructPacket(MDAQ_RSP, FIND_N_RPLC, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Commands the flight software to start the currently loaded sequence*/
int beginSequence(Packet* p) {
    record("Command to begin sequence received\n");
    return GOOD_PACKET;
}

/*Commands the flight software to stop the currently running sequence*/
int endSequence(Packet* p) {
    record("Command to stop sequence received\n");
    return GOOD_PACKET;
}

/*Commands the flight software to terminate*/
int exitSW(Packet* p) {
    record("Command to terminate received\n");
    return GOOD_PACKET;
}

/*commands the flight software to turn telemetry ON*/
int telemEnable(Packet* p) {
    record("Command to enable telemetry received\n");
    return GOOD_PACKET;
}

/*Commands the flight software to turn telemetry OFF*/
int telemDisable(Packet* p) {
    record("Command to disable telemetry received\n");
    return GOOD_PACKET;
}

/*Commands the flight software to turn channel 0 ON*/
int ch0Enable(Packet* p) {
    record("Command to enable Channel 0 received\n");
    return GOOD_PACKET;
}

/*Commands the flight software to turn Channel 0 OFF*/
int ch0Disable(Packet* p) {
    record("Command to disable Channel 0 received\n");
    return GOOD_PACKET;
}

/* Commands the flight software to enable only positive channel */
int posOnlyEnable(Packet* p) {
    record("Command to enable only positive channel received\n");
    return GOOD_PACKET;
}

/* Commands the flight software to disable only positive channel */
int posOnlyDisable(Packet* p) {
    record("Command to disable only positive channel received\n");
    return GOOD_PACKET;
}

/*Commands the flight software to turn STIMS ON*/
int stimsEnable(Packet* p) {
    record("Command to enable STIMS mode received\n");
    return GOOD_PACKET;
}

/*Commands the flight software to turn STIMS OFF*/
int stimsDisable(Packet* p) {
    record("Command to disable STIMS mode received\n");
    return GOOD_PACKET;
}

/*Commands the flight software to reset the ROE*/
int resetROE(Packet* p) {
    record("Command to reset ROE received\n");
    return GOOD_PACKET;
}

/*Exit to default mode*/
int disableDefaultROE(Packet* p) {
    record("Command to exit to default mode received\n");
    return GOOD_PACKET;
}

/*Commands the flight software to set the ROE to self-test mode*/
int enableSelftestROE(Packet* p) {
    record("Command to set ROE to self-test mode received\n");
    return GOOD_PACKET;
}

/*Command to reset the flight software*/
int resetSW(Packet* p) {
    record("Command to reset flight software received\n");
    return GOOD_PACKET;
}

/*
 * Power control functions
 */

/*Command the payload subsystem to power on*/
int enablePower(Packet* p) {
    record("Command to enable subsystem power received\n");
    //Insert control code here  
    Packet* r = constructPacket(PWR_S, STATUS_ON, p->data);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Command the payload subsystem to power off*/
int disablePower(Packet* p) {
    record("Command to disable subsystem power received\n");
    //Insert control code here  
    Packet* r = constructPacket(PWR_S, STATUS_OFF, p->data);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Query the power status of the payload subsystem*/
int queryPower(Packet* p) {
    record("Command to query subsystem power received\n");
    //Insert control code here  
    Packet* r = constructPacket(PWR_S, STATUS_OFF, p->data);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*
 * Housekeeping control functions
 */

/*Request for flight computer +2.0V voltage*/
int FC_2V_V(Packet* p) {
    record("Request for FC +2.0V voltage received\n");
    //Insert control code here  
    char* response = "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS2_0V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for flight computer +2.5V voltage*/
int FC_2_5V_V(Packet* p) {
    record("Request for FC +2.5V voltage received\n");
    //Insert control code here  
    char* response = FC_P2_5V_V "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS2_5V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE +2.5V voltage*/
int ROE_2_5V_V(Packet* p) {
    record("Request for ROE +2.5V voltage received\n");
    //Insert control code here  
    char* response = ROE_P2_5VD_V "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS2_5V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE +2.5V current*/
int ROE_2_5V_I(Packet* p) {
    record("Request for ROE +2.5V current received\n");
    //Insert control code here  
    char* response = ROE_P2_5VD_C "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS2_5V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for flight computer +3.3V voltage*/
int FC_3_3V_V(Packet* p) {
    record("Request for FC +3.3V voltage received\n");
    //Insert control code here  
    char* response = "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS3_3V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for flight computer +5.0V voltage*/
int FC_POS_5V_V(Packet* p) {
    record("Request for FC +5.0V voltage received\n");
    //Insert control code here  
    char* response = FC_P5V_V "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS5V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE +5.0V A Channel voltage*/
int ROE_POS_5V_VA(Packet* p) {
    record("Request for ROE +5.0V A Channel voltage received\n");
    //Insert control code here  
    char* response = ROE_P5VAA_V "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS5V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE +5.0V B Channel voltage*/
int ROE_POS_5V_VB(Packet* p) {
    record("Request for ROE +5.0V B Channel voltage received\n");
    //Insert control code here  
    char* response = ROE_P5VAB_V "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS5V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE +5.0V D Channel voltage*/
int ROE_POS_5V_VD(Packet* p) {
    record("Request for ROE +5.0V D Channel voltage received\n");
    //Insert control code here  
    char* response = ROE_P5VD_V "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS5V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE +5.0V A Channel current*/
int ROE_POS_5V_IA(Packet* p) {
    record("Request for ROE +5.0V A Channel current received\n");
    //Insert control code here  
    char* response = ROE_P5VAA_C "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS5V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE +5.0V B Channel current*/
int ROE_POS_5V_IB(Packet* p) {
    record("Request for ROE +5.0V B Channel current received\n");
    //Insert control code here  
    char* response = ROE_P5VAB_C "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS5V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE +5.0V D Channel current*/
int ROE_POS_5V_ID(Packet* p) {
    record("Request for ROE +5.0V D Channel current received\n");
    //Insert control code here  
    char* response = ROE_P5VD_C "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS5V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE -5.0V A Channel voltage*/
int ROE_NEG_5V_VA(Packet* p) {
    record("Request for ROE -5.0V A Channel voltage received\n");
    //Insert control code here  
    char* response = ROE_N5VAA_V "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, NEG5V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE -5.0V B Channel voltage*/
int ROE_NEG_5V_VB(Packet* p) {
    record("Request for ROE -5.0V B Channel voltage received\n");
    //Insert control code here  
    char* response = ROE_N5VAB_V "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, NEG5V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE -5.0V A Channel current*/
int ROE_NEG_5V_IA(Packet* p) {
    record("Request for ROE -5.0V A Channel current received\n");
    //Insert control code here  
    char* response = ROE_N5VAA_C "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, NEG5V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE -5.0V B Channel current*/
int ROE_NEG_5V_IB(Packet* p) {
    record("Request for ROE -5.0V B Channel current received\n");
    //Insert control code here  
    char* response = ROE_N5VAB_C "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, NEG5V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for flight computer +12V voltage*/
int FC_12V_V(Packet* p) {
    record("Request for FC +12V voltage received\n");
    //Insert control code here  
    char* response = FC_P12V_V "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS12V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE +12V A Channel voltage*/
int ROE_12V_VA(Packet* p) {
    record("Request for ROE +12V A Channel voltage received\n");
    //Insert control code here  
    char* response = ROE_P12VA_V "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS12V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE +12V B Channel voltage*/
int ROE_12V_VB(Packet* p) {
    record("Request for ROE +12V B Channel voltage received\n");
    //Insert control code here  
    char* response = ROE_P12VB_V "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS12V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE +12V A Channel current*/
int ROE_12V_IA(Packet* p) {
    record("Request for ROE +12V A Channel current received\n");
    //Insert control code here  
    char* response = ROE_P12VA_C "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS12V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE +12V B Channel current*/
int ROE_12V_IB(Packet* p) {
    record("Request for ROE +12V B Channel current received\n");
    //Insert control code here  
    char* response = ROE_P12VB_C "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS12V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE +36V A Channel voltage*/
int ROE_36V_VA(Packet* p) {
    record("Request for ROE +36V A Channel voltage received\n");
    //Insert control code here  
    char* response = ROE_P36VA_V "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS36V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE +36V B Channel voltage*/
int ROE_36V_VB(Packet* p) {
    record("Request for ROE +36V B Channel voltage received\n");
    //Insert control code here  
    char* response = ROE_P36VB_V "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS36V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE +36V A Channel current*/
int ROE_36V_IA(Packet* p) {
    record("Request for ROE +36V A Channel current received\n");
    //Insert control code here  
    char* response = ROE_P36VA_C "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS36V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE +36V B Channel current*/
int ROE_36V_IB(Packet* p) {
    record("Request for ROE +36V B Channel current received\n");
    //Insert control code here  
    char* response = ROE_P36VB_C "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, POS36V, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for flight computer temperature 1*/
int FC_TEMP_1(Packet* p) {
    record("Request for FC temp 1 received\n");
    //Insert control code here  
    char* response = TEMP1 "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, TEMP, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for flight computer temperature 2*/
int FC_TEMP_2(Packet* p) {
    record("Request for FC temp 2 received\n");
    //Insert control code here  
    char* response = TEMP2 "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, TEMP, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for flight computer temperature 3*/
int FC_TEMP_3(Packet* p) {
    record("Request for FC temp 3 received\n");
    //Insert control code here  
    char* response = TEMP3 "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, TEMP, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE upper temperature*/
int ROE_TEMP_UPPER(Packet* p) {
    record("Request for ROE upper temperature received\n");
    //Insert control code here  
    char* response = UPPER "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, TEMP, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for ROE lower temperature*/
int ROE_TEMP_LOWER(Packet* p) {
    record("Request for ROE lower temperature received\n");
    //Insert control code here  
    char* response = LOWER "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, TEMP, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for CCDA_VOD current from the ROE*/
int ROE_CCDA_VOD(Packet* p) {
    record("Request for ROE CCDA_VOD current received\n");
    //Insert control code here  
    char* response = "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, CCDA_VOD, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for CCDA_VRD current from the ROE*/
int ROE_CCDA_VRD(Packet* p) {
    record("Request for ROE CCDA_VRD current received\n");
    //Insert control code here  
    char* response = "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, CCDA_VRD, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for CCDA_VSS current from the ROE*/
int ROE_CCDA_VSS(Packet* p) {
    record("Request for ROE CCDA_VSS current received\n");
    //Insert control code here  
    char* response = "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, CCDA_VSS, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for CCDS_VOD current from the ROE*/
int ROE_CCDS_VOD(Packet* p) {
    record("Request for ROE CCDS_VOD current received\n");
    //Insert control code here  
    char* response = "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, CCDB_VOD, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for CCDS_VRD current from the ROE*/
int ROE_CCDS_VRD(Packet* p) {
    record("Request for ROE CCDS_VRD current received\n");
    //Insert control code here  
    char* response = "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, CCDB_VRD, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Request for CCDS_VSS current from the ROE*/
int ROE_CCDS_VSS(Packet* p) {
    record("Request for ROE CCDS_VSS current received\n");
    //Insert control code here  
    char* response = "0.0"; //test EGSE
    Packet* r = constructPacket(HK_RSP, CCDB_VSS, response);
    enqueue(&hkdownQueue, r);
    return GOOD_PACKET;
}

/*Uses a hash table to match packet strings to function pointers*/
void hlpHashInit() {
    hlp_hash_size = funcNumber;

    /*allocate space for control strings*/
    char** stringTable;
    if ((stringTable = (char**) malloc(sizeof (char*) * funcNumber)) == NULL) {
        record("malloc failed to allocate control string array\n");
    }

    /*Uplink control string-function matching*/
    stringTable[UDataStart] = UPLINK_S DATASTART;
    stringTable[UDataStop] = UPLINK_S DATASTOP;
    stringTable[UDark1] = UPLINK_S DARK1;
    stringTable[UDark2] = UPLINK_S DARK2;
    stringTable[UDark3] = UPLINK_S DARK3;
    stringTable[UDark4] = UPLINK_S DARK4;
    stringTable[USleep] = UPLINK_S SLEEP;
    stringTable[UWake] = UPLINK_S WAKE;
    stringTable[UTest] = UPLINK_S TEST;
    stringTable[SetSeq] = MDAQ_RQS_S ST_SEQUENCE;
    stringTable[SetOut] = MDAQ_RQS_S ST_OFN;
    stringTable[GetSeqName] = MDAQ_RQS_S GT_SEQ_NM;
    stringTable[GetSeqInfo] = MDAQ_RQS_S GT_SEQ_INFO;
    stringTable[GetCurSeq] = MDAQ_RQS_S GT_CUR_SEQ;
    stringTable[GetFrameLen] = MDAQ_RQS_S GT_CUR_FRML;
    stringTable[GetFrameInd] = MDAQ_RQS_S GT_CUR_FRMI;
    stringTable[GetOut] = MDAQ_RQS_S GT_OFN;
    stringTable[GetSelf] = MDAQ_RQS_S GT_SLFT_STS;
    stringTable[GetStims] = MDAQ_RQS_S GT_STM_STS;
    stringTable[GetTelem] = MDAQ_RQS_S GT_TLM_STS;
    stringTable[GetCh0] = MDAQ_RQS_S GT_CH0_STS;
    stringTable[GetPosOnly] = MDAQ_RQS_S GT_POS_STS;
    stringTable[ScaleSeq] = MDAQ_RQS_S SCALE_SEQ;
    stringTable[TransSeq] = MDAQ_RQS_S TRANS_SEQ;
    stringTable[FindJump] = MDAQ_RQS_S FIND_N_JUMP;
    stringTable[JumpSeq] = MDAQ_RQS_S JUMP;
    stringTable[SaveSeq] = MDAQ_RQS_S SAV_SEQ;
    //stringTable[SaveCurSeq] = SAV_CUR_SEQ;
    stringTable[FindReplace] = MDAQ_RQS_S FIND_N_RPLC;
    stringTable[BeginSeq] = MDAQ_RQS_S BEGIN_SEQ;
    stringTable[EndSeq] = MDAQ_RQS_S END_SEQ;
    stringTable[ExitSW] = MDAQ_RQS_S EXT;
    stringTable[TelemOn] = MDAQ_RQS_S TELEM_ON;
    stringTable[TelemOff] = MDAQ_RQS_S TELEM_OFF;
    stringTable[ZeroOn] = MDAQ_RQS_S CHAN0_ON;
    stringTable[ZeroOff] = MDAQ_RQS_S CHAN0_OFF;
    stringTable[PosOnlyOn] = MDAQ_RQS_S POSONLY_ON;
    stringTable[PosOnlyOff] = MDAQ_RQS_S POSONLY_OFF;
    stringTable[StimsOn] = MDAQ_RQS_S STIM_ON;
    stringTable[StimsOff] = MDAQ_RQS_S STIM_OFF;
    stringTable[ResetROE] = MDAQ_RQS_S RST_ROE;
    stringTable[RoeDefaultOff] = MDAQ_RQS_S EXIT_DFLT;
    stringTable[RoeSelfOn] = MDAQ_RQS_S SET_SLFTST;
    stringTable[ResetSW] = MDAQ_RQS_S RESET_MDAQ;
    stringTable[PowerOn] = PWR_S TURN_ON;
    stringTable[PowerOff] = PWR_S TURN_OFF;
    stringTable[PowerQuery] = PWR_S QUERY;
    stringTable[FC_2V] = HK_RQS_S POS2_0V;
    stringTable[FC_2_5V] = HK_RQS_S POS2_5V FC_P2_5V_V;
    stringTable[ROE_2_5V] = HK_RQS_S POS2_5V ROE_P2_5VD_V;
    stringTable[ROE_2_5I] = HK_RQS_S POS2_5V ROE_P2_5VD_C;
    stringTable[FC_3_3V] = HK_RQS_S POS3_3V;
    stringTable[FC_P5_5V] = HK_RQS_S POS5V FC_P5V_V;
    stringTable[ROE_P5_5VA] = HK_RQS_S POS5V ROE_P5VAA_V;
    stringTable[ROE_P5_5VB] = HK_RQS_S POS5V ROE_P5VAB_V;
    stringTable[ROE_P5_5VD] = HK_RQS_S POS5V ROE_P5VD_V;
    stringTable[ROE_P5_5IA] = HK_RQS_S POS5V ROE_P5VAA_C;
    stringTable[ROE_P5_5IB] = HK_RQS_S POS5V ROE_P5VAB_C;
    stringTable[ROE_P5_5ID] = HK_RQS_S POS5V ROE_P5VD_C;
    stringTable[ROE_N5_5VA] = HK_RQS_S NEG5V ROE_N5VAA_V;
    stringTable[ROE_N5_5VB] = HK_RQS_S NEG5V ROE_N5VAB_V;
    stringTable[ROE_N5_5IA] = HK_RQS_S NEG5V ROE_N5VAA_C;
    stringTable[ROE_N5_5IB] = HK_RQS_S NEG5V ROE_N5VAB_C;
    stringTable[FC_12V] = HK_RQS_S POS12V FC_P12V_V;
    stringTable[ROE_12VA] = HK_RQS_S POS12V ROE_P12VA_V;
    stringTable[ROE_12VB] = HK_RQS_S POS12V ROE_P12VB_V;
    stringTable[ROE_12IA] = HK_RQS_S POS12V ROE_P12VA_C;
    stringTable[ROE_12IB] = HK_RQS_S POS12V ROE_P12VB_C;
    stringTable[ROE_36VA] = HK_RQS_S POS36V ROE_P36VA_V;
    stringTable[ROE_36VB] = HK_RQS_S POS36V ROE_P36VB_V;
    stringTable[ROE_36IA] = HK_RQS_S POS36V ROE_P36VA_C;
    stringTable[ROE_36IB] = HK_RQS_S POS36V ROE_P36VB_C;
    stringTable[FC_T1] = HK_RQS_S TEMP TEMP1;
    stringTable[FC_T2] = HK_RQS_S TEMP TEMP2;
    stringTable[FC_T3] = HK_RQS_S TEMP TEMP3;
    stringTable[ROE_TU] = HK_RQS_S TEMP UPPER;
    stringTable[ROE_TL] = HK_RQS_S TEMP LOWER;
    stringTable[ROE_CA_VOD] = HK_RQS_S CCDA_VOD;
    stringTable[ROE_CA_VRD] = HK_RQS_S CCDA_VRD;
    stringTable[ROE_CA_VSS] = HK_RQS_S CCDA_VSS;
    stringTable[ROE_CS_VOD] = HK_RQS_S CCDB_VOD;
    stringTable[ROE_CS_VRD] = HK_RQS_S CCDB_VRD;
    stringTable[ROE_CS_VSS] = HK_RQS_S CCDB_VSS;

    /*allocate space for function pointers*/
    hlpControl* functionTable;
    if ((functionTable = calloc(funcNumber, sizeof (hlpControl))) == NULL) {
        record("malloc failed to allocate control function array\n");
    }

    /*array of function pointers to match with control strings*/
    functionTable[UDataStart] = &uDataStart;
    functionTable[UDataStop] = &uDataStop;
    functionTable[UDark1] = &uDark1;
    functionTable[UDark2] = &uDark2;
    functionTable[UDark3] = &uDark3;
    functionTable[UDark4] = &uDark4;
    functionTable[USleep] = &uSleep;
    functionTable[UWake] = &uWake;
    functionTable[UTest] = &uTest;
    functionTable[SetSeq] = &setSequence;
    functionTable[SetOut] = &setOutputName;
    functionTable[GetSeqName] = &getSeqName;
    functionTable[GetSeqInfo] = &getSeqInfo;
    functionTable[GetCurSeq] = &getCurSeqName;
    functionTable[GetFrameLen] = &getCurFrameLen;
    functionTable[GetFrameInd] = &getCurFrameIndex;
    functionTable[GetOut] = &getOutputName;
    functionTable[GetSelf] = &getSelftestStatus;
    functionTable[GetStims] = &getStimsStatus;
    functionTable[GetTelem] = &getTelemStatus;
    functionTable[GetCh0] = &getCh0Status;
    functionTable[GetPosOnly] = &getPosOnlyStatus;
    functionTable[ScaleSeq] = &scaleSequence;
    functionTable[TransSeq] = &translateSeq;
    functionTable[FindJump] = &findAndJump;
    functionTable[JumpSeq] = &jumpToExp;
    functionTable[SaveSeq] = &saveSequence;
    //functionTable[SaveCurSeq] = saveCurSeqAs;
    functionTable[FindReplace] = &findAndReplace;
    functionTable[BeginSeq] = &beginSequence;
    functionTable[EndSeq] = &endSequence;
    functionTable[ExitSW] = &exitSW;
    functionTable[TelemOn] = &telemEnable;
    functionTable[TelemOff] = &telemDisable;
    functionTable[ZeroOn] = &ch0Enable;
    functionTable[ZeroOff] = &ch0Disable;
    functionTable[PosOnlyOn] = &posOnlyEnable;
    functionTable[PosOnlyOff] = &posOnlyDisable;
    functionTable[StimsOn] = &stimsEnable;
    functionTable[StimsOff] = &stimsDisable;
    functionTable[ResetROE] = &resetROE;
    functionTable[RoeDefaultOff] = &disableDefaultROE;
    functionTable[RoeSelfOn] = &enableSelftestROE;
    functionTable[ResetSW] = &resetSW;
    functionTable[PowerOn] = &enablePower;
    functionTable[PowerOff] = &disablePower;
    functionTable[PowerQuery] = &queryPower;
    functionTable[FC_2V] = &FC_2V_V;
    functionTable[FC_2_5V] = &FC_2_5V_V;
    functionTable[ROE_2_5V] = &ROE_2_5V_V;
    functionTable[ROE_2_5I] = &ROE_2_5V_I;
    functionTable[FC_3_3V] = &FC_3_3V_V;
    functionTable[FC_P5_5V] = &FC_POS_5V_V;
    functionTable[ROE_P5_5VA] = &ROE_POS_5V_VA;
    functionTable[ROE_P5_5VB] = &ROE_POS_5V_VB;
    functionTable[ROE_P5_5VD] = &ROE_POS_5V_VD;
    functionTable[ROE_P5_5IA] = &ROE_POS_5V_IA;
    functionTable[ROE_P5_5IB] = &ROE_POS_5V_IB;
    functionTable[ROE_P5_5ID] = &ROE_POS_5V_ID;
    functionTable[ROE_N5_5VA] = &ROE_NEG_5V_VA;
    functionTable[ROE_N5_5VB] = &ROE_NEG_5V_VB;
    functionTable[ROE_N5_5IA] = &ROE_NEG_5V_IA;
    functionTable[ROE_N5_5IB] = &ROE_NEG_5V_IB;
    functionTable[FC_12V] = &FC_12V_V;
    functionTable[ROE_12VA] = &ROE_12V_VA;
    functionTable[ROE_12VB] = &ROE_12V_VB;
    functionTable[ROE_12IA] = &ROE_12V_IA;
    functionTable[ROE_12IB] = &ROE_12V_IB;
    functionTable[ROE_36VA] = &ROE_36V_VA;
    functionTable[ROE_36VB] = &ROE_36V_VB;
    functionTable[ROE_36IA] = &ROE_36V_IA;
    functionTable[ROE_36IB] = &ROE_36V_IB;
    functionTable[FC_T1] = &FC_TEMP_1;
    functionTable[FC_T2] = &FC_TEMP_2;
    functionTable[FC_T3] = &FC_TEMP_3;
    functionTable[ROE_TU] = &ROE_TEMP_UPPER;
    functionTable[ROE_TL] = &ROE_TEMP_LOWER;
    functionTable[ROE_CA_VOD] = &ROE_CCDA_VOD;
    functionTable[ROE_CA_VRD] = &ROE_CCDA_VRD;
    functionTable[ROE_CA_VSS] = &ROE_CCDA_VSS;
    functionTable[ROE_CS_VOD] = &ROE_CCDS_VOD;
    functionTable[ROE_CS_VRD] = &ROE_CCDS_VRD;
    functionTable[ROE_CS_VSS] = &ROE_CCDS_VSS;

    /*initialize memory for function hash table*/
    if ((hlpHashTable = malloc(sizeof(node_t) * hlp_hash_size)) == NULL) {
        record("malloc failed to allocate hash table\n");
    }

    /*fill hash table with array of strings matching function pointers*/
    int i;
    for (i = 0; i < funcNumber; i++) {
        installNode(hlpHashTable, stringTable[i], functionTable[i], hlp_hash_size);
    }
}

