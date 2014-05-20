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
int hlpUplink(Packet* p){
    Node* np;
    np = lookup(hlpHashTable, np, p->subtype);       //Lookup corresponding function in table
    int status = (*(np->func))(p);        //Call control function
    return status;
}

/*determines control string for power packets*/
int hlpPower(Packet* p){
    Node* np;
    /*construct control string*/
    char control[4];
    control[0] = p->type[0];
    strcpy(control + 1, p->subtype);
    
    np = lookup(hlpHashTable, np, control);     //Lookup corresponding function in table
    int status = (*(np->func))(p);        //Call control function
    return status;
}

/*Uplink control functions*/
int uDataStart(Packet* p){
    puts("Received data start Uplink");
    //Insert uplink handling code here
    Packet* newp = constructPacket(UPLINK_S, DATASTART, NULL);
    enqueue(&hkdownQueue, newp);
    return GOOD_PACKET;
}
int uDataStop(Packet* p){
    puts("Received data stop Uplink");
    //Insert uplink handling code here
    Packet* newp = constructPacket(UPLINK_S, DATASTOP, NULL);
    enqueue(&hkdownQueue, newp);
    return GOOD_PACKET;
}
int uDark1(){
    puts("Received Dark1 Uplink");
    //Insert uplink handling code here
    Packet* newp = constructPacket(newp, UPLINK_S, DARK1, NULL);
    enqueue(&hkdownQueue, newp);
    return GOOD_PACKET;
}
int uDark2(Packet* p){
    puts("Received Dark2 Uplink");
    //Insert uplink handling code here
    Packet* newp = constructPacket(newp, UPLINK_S, DARK2, NULL);
    enqueue(&hkdownQueue, newp);
    return GOOD_PACKET;
}
int uDark3(Packet* p){
    puts("Received Dark3 Uplink");
    //Insert uplink handling code here
    Packet* newp = constructPacket(newp, UPLINK_S, DARK3, NULL);
    enqueue(&hkdownQueue, newp);
    return GOOD_PACKET;
}
int uDark4(Packet* p){
    puts("Received Dark4 Uplink");
    //Insert uplink handling code here
    Packet* newp = constructPacket(newp, UPLINK_S, DARK4, NULL);
    enqueue(&hkdownQueue, newp);
    return GOOD_PACKET;
}
int uSleep(Packet* p){
    puts("Received shutdown Uplink");
    //Insert uplink handling code here
    Packet* newp = constructPacket(newp, UPLINK_S, SLEEP, NULL);
    enqueue(&hkdownQueue, newp);
    return GOOD_PACKET;
}
int uWake(Packet* p){
    puts("Received shutdown pending Uplink");
    //Insert uplink handling code here
    Packet* newp = constructPacket(newp, UPLINK_S, WAKE, NULL);
    enqueue(&hkdownQueue, newp);
}
int uTest(Packet* p){
    puts("Received test Uplink");
    //Insert uplink handling code here
    Packet* newp = constructPacket(newp, UPLINK_S, TEST, NULL);
    enqueue(&hkdownQueue, newp);
    return GOOD_PACKET;
}

/*Timer control functions*/
int tDataStart(Packet* p){
    puts("Received data start Timer");
    //Insert timer handling code here
    Packet* newp = constructPacket(newp, TIMER, DATASTART, NULL);
    enqueue(&hkdownQueue, newp);
    return GOOD_PACKET;
}
int tDataStop(Packet* p){
    puts("Received data stop Timer");
    //Insert timer handling code here
    Packet* newp = constructPacket(newp, TIMER, DATASTOP, NULL);
    enqueue(&hkdownQueue, newp);
    return GOOD_PACKET;
}
int tDark2(Packet* p){
    puts("Received Dark2 Timer");
    //Insert timer handling code here
    Packet* newp = constructPacket(newp, TIMER, DARK2, NULL);
    enqueue(&hkdownQueue, newp);
    return GOOD_PACKET;
}
int tDark4(Packet* p){
    puts("Received Dark4 Timer");
    //Insert timer handling code here
    Packet* newp = constructPacket(newp, TIMER, DARK4, NULL);
    enqueue(&hkdownQueue, newp);
    return GOOD_PACKET;
}
int tSleep(Packet* p){
    puts("Received shutdown Timer");
    //Insert timer handling code here
    Packet* newp = constructPacket(newp, TIMER, SLEEP, NULL);
    enqueue(&hkdownQueue, newp);
    return GOOD_PACKET;
}
/*
 *Mission Data Acquisition (MDAQ) control functions 
 */

/*Associates a sequence name with a signal number*/
int setSequence(Packet* p){
    puts("Get sequence command received");
    return GOOD_PACKET;
}
/*Uses stem for an output file.*/
int setOutputName(Packet* p){
    puts("Set output filename command received");
    return GOOD_PACKET;
}
/*Get sequence name that corresponds to the signal number in the data field*/
int getSeqName(Packet* p){
    puts("Get sequence name command received");
    return GOOD_PACKET;
}
/*Generates a string representation of the sequence file*/
int getSeqInfo(Packet* p){
    puts("Get sequence info command received");
    return GOOD_PACKET;
}
/*Commands the flight software to return the currently running sequence*/
int getCurSeqName(Packet* p){
    puts("Get current sequence name command received");
    return GOOD_PACKET;
}
/*Commands the flight software to return the current frame's exposure length*/
int getCurFrameLen(Packet* p){
    puts("Get current frame length command received");
    return GOOD_PACKET;
}
/*Command the flight software to return the current frame's index number*/
int getCurFrameIndex(Packet* p){
    puts("Get current frame index command received");
    return GOOD_PACKET;
}
/*Commands the flight software to return the filename of the output file*/
int getOutputName(Packet* p){
    puts("Get output filename command received");
    return GOOD_PACKET;
}
/*Commands the flight software to determine if he ROE is in self-test mode*/
int getSelftestStatus(Packet* p){
    puts("Get self-test mode status command received");
    return GOOD_PACKET;
}
/*Commands the flight software to determine if the ROE is in STIMS mode*/
int getStimsStatus(Packet* p){
    puts("Get STIMS mode status command received");
    return GOOD_PACKET;
}
/*Commands the flight software to determine if Telemetry is enabled*/
int getTelemStatus(Packet* p){
    puts("Get Telemetry mode status command received");
    return GOOD_PACKET;
}
/*Commands the flight software to determine if Channel 0 data will be recorded*/
int getCh0Status(Packet* p){
    puts("Get status of channel 0 record command received");
    return GOOD_PACKET;
}
/*Commands the flight software to determine if positive channel data will be recorded*/
int getPosOnlyStatus(Packet* p){
    puts("Get status of positive channel record command received");
    return GOOD_PACKET;
}
/*Command the flight software to scale the current sequence, multiplying each frame by the
 floating point value contained within the Data field*/
int scaleSequence(Packet* p){
    puts("Command to scale sequence received");
    return GOOD_PACKET;
}
/*Commands the flight software to translate the current sequence, adding the 
 floating point value in the Data field to each frame in the sequence*/
int translateSeq(Packet* p){
    puts("Command to translate sequence received");
    return GOOD_PACKET;
}
/*Commands the flight software to interrupt the current exposure, find an exposure
 of the length given in the Data field, and jump to the first exposure matching
 this criterion*/
int findAndJump(Packet* p){
    puts("Command to Find and Jump received");
    return GOOD_PACKET;
}
/*Commands the flight software to jump to the exposure with index number given 
 in the data field*/
int jumpToExp(Packet* p){
    puts("Command to Jump to exposure received");
    return GOOD_PACKET;
}
/*CONFLICTING INTERFACES*/
/*HLP Document: Commands the flight software to save the current sequence to the 
 filename defined in the Data field (possibly "SVC") */
/*flightSW_2005: Saves the sequence defined in the Data field*/
int saveSequence(Packet* p){
    puts("Command to save sequence received");
    return GOOD_PACKET;
}
/*Commands the flight software to find exposures with lengths given by the first
 floating point value in the data field and replace them with the second value 
 the Data field*/
int findAndReplace(Packet* p){
    puts("Command to find and replace received");
    return GOOD_PACKET;
}
/*Commands the flight software to start the currently loaded sequence*/
int beginSequence(Packet* p){
    puts("Command to begin sequence received");
    return GOOD_PACKET;
}
/*Commands the flight software to stop the currently running sequence*/
int endSequence(Packet* p){
    puts("Command to stop sequence received");
    return GOOD_PACKET;
}
/*Commands the flight software to terminate*/
int exitSW(Packet* p){
    puts("Command to terminate received");
    return GOOD_PACKET;
}
/*commands the flight software to turn telemetry ON*/
int telemEnable(Packet* p){
    puts("Command to enable telemetry received");
    return GOOD_PACKET;
}
/*Commands the flight software to turn telemetry OFF*/
int telemDisable(Packet* p){
    puts("Command to disable telemetry received");
    return GOOD_PACKET;
}
/*Commands the flight software to turn channel 0 ON*/
int ch0Enable(Packet* p){
    puts("Command to enable Channel 0 received");
    return GOOD_PACKET;
}
/*Commands the flight software to turn Channel 0 OFF*/
int ch0Disable(Packet* p){
    puts("Command to disable Channel 0 received");
    return GOOD_PACKET;
}
/*Commands the flight software to enable only positive channel*/
int posOnlyEnable(Packet* p){
    puts("Command to enable only positive channel received");
    return GOOD_PACKET;
}
/*Commands the flight software to disable only positive channel*/
int posOnlyDisable(Packet* p){
    puts("Command to disable only positive channel received");
    return GOOD_PACKET;
}
/*Commands the flight software to turn STIMS ON*/
int stimsEnable(Packet* p){
    puts("Command to enable STIMS mode received");
    return GOOD_PACKET;
}
/*Commands the flight software to turn STIMS OFF*/
int stimsDisable(Packet* p){
    puts("Command to disable STIMS mode received");
    return GOOD_PACKET;
}
/*Commands the flight software to reset the ROE*/
int resetROE(Packet* p){
    puts("Command to reset ROE received");
    return GOOD_PACKET;
}
/*Exit to default mode*/
int disableDefaultROE(Packet* p){
    puts("Command to exit to default mode received");
    return GOOD_PACKET;
}
/*Commands the flight software to set the ROE to self-test mode*/
int enableSelftestROE(Packet* p){
    puts("Command to set ROE to self-test mode received");
    return GOOD_PACKET;
}
/*Command to reset the flight software*/
int resetSW(Packet* p){
    puts("Command to reset flight software received");
    return GOOD_PACKET;
}

/*
 * Power control functions
 */ 

/*Command the payload subsystem to power on*/
int enablePower(Packet* p){
    puts("Command to enable subsystem power received");
    return GOOD_PACKET;
}
/*Command the payload subsystem to power off*/
int disablePower(Packet* p){
    puts("Command to disable subsystem power received");
    return GOOD_PACKET;
}
/*Query the power status of the payload subsystem*/
int queryPower(Packet* p){
    puts("Command to query subsystem power received");
    return GOOD_PACKET;
}

/*
 * Housekeeping control functions
 */ 

/*Request for flight computer +2.0V voltage*/
int FC_2V_V(Packet* p){
    puts("Request for FC +2.0V voltage received");
    return GOOD_PACKET;
}
/*Request for flight computer +2.5V voltage*/
int FC_2_5V_V(Packet* p){
    puts("Request for FC +2.5V voltage received");
    return GOOD_PACKET;
}
/*Request for ROE +2.5V voltage*/
int ROE_2_5V_V(Packet* p){
    puts("Request for ROE +2.5V voltage received");
    return GOOD_PACKET;
}
/*Request for ROE +2.5V current*/
int ROE_2_5V_I(Packet* p){
    puts("Request for ROE +2.5V current received");
    return GOOD_PACKET;
}
/*Request for flight computer +3.3V voltage*/
int FC_3_3V_V(Packet* p){
    puts("Request for FC +3.3V voltage received");
    return GOOD_PACKET;
}
/*Request for flight computer +5.0V voltage*/
int FC_POS_5V_V(Packet* p){
    puts("Request for FC +5.0V voltage received");
    return GOOD_PACKET;
}
/*Request for ROE +5.0V A Channel voltage*/
int ROE_POS_5V_VA(Packet* p){
    puts("Request for ROE +5.0V A Channel voltage received");
    return GOOD_PACKET;
}
/*Request for ROE +5.0V B Channel voltage*/
int ROE_POS_5V_VB(Packet* p){
    puts("Request for ROE +5.0V B Channel voltage received");
    return GOOD_PACKET;
}
/*Request for ROE +5.0V D Channel voltage*/
int ROE_POS_5V_VD(Packet* p){
    puts("Request for ROE +5.0V D Channel voltage received");
    return GOOD_PACKET;
}
/*Request for ROE +5.0V A Channel current*/
int ROE_POS_5V_IA(Packet* p){
    puts("Request for ROE +5.0V A Channel current received");
    return GOOD_PACKET;
}
/*Request for ROE +5.0V B Channel current*/
int ROE_POS_5V_IB(Packet* p){
    puts("Request for ROE +5.0V B Channel current received");
    return GOOD_PACKET;
}
/*Request for ROE +5.0V D Channel current*/
int ROE_POS_5V_ID(Packet* p){
    puts("Request for ROE +5.0V D Channel current received");
    return GOOD_PACKET;
}
/*Request for ROE -5.0V A Channel voltage*/
int ROE_NEG_5V_VA(Packet* p){
    puts("Request for ROE -5.0V A Channel voltage received");
    return GOOD_PACKET;
}
/*Request for ROE -5.0V B Channel voltage*/
int ROE_NEG_5V_VB(Packet* p){
    puts("Request for ROE -5.0V B Channel voltage received");
    return GOOD_PACKET;
}
/*Request for ROE -5.0V A Channel current*/
int ROE_NEG_5V_IA(Packet* p){
    puts("Request for ROE -5.0V A Channel current received");
    return GOOD_PACKET;
}
/*Request for ROE -5.0V B Channel current*/
int ROE_NEG_5V_IB(Packet* p){
    puts("Request for ROE -5.0V B Channel current received");
    return GOOD_PACKET;
}
/*Request for flight computer +12V voltage*/
int FC_12V_V(Packet* p){
    puts("Request for FC +12V voltage received");
    return GOOD_PACKET;
}
/*Request for ROE +12V A Channel voltage*/
int ROE_12V_VA(Packet* p){
    puts("Request for ROE +12V A Channel voltage received");
    return GOOD_PACKET;
}
/*Request for ROE +12V B Channel voltage*/
int ROE_12V_VB(Packet* p){
    puts("Request for ROE +12V B Channel voltage received");
    return GOOD_PACKET;
}
/*Request for ROE +12V A Channel current*/
int ROE_12V_IA(Packet* p){
    puts("Request for ROE +12V A Channel current received");
    return GOOD_PACKET;
}
/*Request for ROE +12V B Channel current*/
int ROE_12V_IB(Packet* p){
    puts("Request for ROE +12V B Channel current received");
    return GOOD_PACKET;
}
/*Request for ROE +36V A Channel voltage*/
int ROE_36V_VA(Packet* p){
    puts("Request for ROE +36V A Channel voltage received");
    return GOOD_PACKET;
}
/*Request for ROE +36V B Channel voltage*/
int ROE_36V_VB(Packet* p){
    puts("Request for ROE +36V B Channel voltage received");
    return GOOD_PACKET;
}
/*Request for ROE +36V A Channel current*/
int ROE_36V_IA(Packet* p){
    puts("Request for ROE +36V A Channel current received");
    return GOOD_PACKET;
}
/*Request for ROE +36V B Channel current*/
int ROE_36V_IB(Packet* p){
    puts("Request for ROE +36V B Channel current received");
    return GOOD_PACKET;
}
/*Request for flight computer temperature 1*/
int FC_TEMP_1(Packet* p){
    puts("Request for FC temp 1 received");
    return GOOD_PACKET;
}
/*Request for flight computer temperature 2*/
int FC_TEMP_2(Packet* p){
    puts("Request for FC temp 2 received");
    return GOOD_PACKET;
}
/*Request for flight computer temperature 3*/
int FC_TEMP_3(Packet* p){
    puts("Request for FC temp 3 received");
    return GOOD_PACKET;
}
/*Request for ROE upper temperature*/
int ROE_TEMP_UPPER(Packet* p){
    puts("Request for ROE upper temperature received");
    return GOOD_PACKET;
}
/*Request for ROE lower temperature*/
int ROE_TEMP_LOWER(Packet* p){
    puts("Request for ROE lower temperature received");
    return GOOD_PACKET;
}
/*Request for CCDA_VOD current from the ROE*/
int ROE_CCDA_VOD(Packet* p){
    puts("Request for ROE CCDA_VOD current received");
    return GOOD_PACKET;
}
/*Request for CCDA_VRD current from the ROE*/
int ROE_CCDA_VRD(Packet* p){
    puts("Request for ROE CCDA_VRD current received");
    return GOOD_PACKET;
}
/*Request for CCDA_VSS current from the ROE*/
int ROE_CCDA_VSS(Packet* p){
    puts("Request for ROE CCDA_VSS current received");
    return GOOD_PACKET;
}
/*Request for CCDS_VOD current from the ROE*/
int ROE_CCDS_VOD(Packet* p){
    puts("Request for ROE CCDS_VOD current received");
    return GOOD_PACKET;
}
/*Request for CCDS_VRD current from the ROE*/
int ROE_CCDS_VRD(Packet* p){
    puts("Request for ROE CCDS_VRD current received");
    return GOOD_PACKET;
}
/*Request for CCDS_VSS current from the ROE*/
int ROE_CCDS_VSS(Packet* p){
    puts("Request for ROE CCDS_VSS current received");
    return GOOD_PACKET;
}


/*Uses a hash table to match packet strings to function pointers*/
void hlpHashInit(){
     hashsize = funcNumber *2; 
  
    /*allocate space for control strings*/
    char** stringTable;
    if((stringTable = (char**) malloc(sizeof(char*) * funcNumber)) == NULL){
        puts("malloc failed to allocate control string array");
    }
    
    /*Uplink control string-function matching*/
    stringTable[UDataStart] = DATASTART; 
    stringTable[UDataStop] = DATASTOP; 
    stringTable[UDark1] = DARK1;
    stringTable[UDark2] = DARK2;
    stringTable[UDark3] = DARK3;
    stringTable[UDark4] = DARK4;
    stringTable[USleep] = SLEEP;
    stringTable[UWake] = WAKE;
    stringTable[UTest] = TEST;
    stringTable[SetSeq] = ST_SEQUENCE;
    stringTable[SetOut] = ST_OFN;
    stringTable[GetSeqName] = GT_SEQ_NM;
    stringTable[GetSeqInfo] = GT_SEQ_INFO;
    stringTable[GetCurSeq] = GT_CUR_SEQ;
    stringTable[GetFrameLen] = GT_CUR_FRML;
    stringTable[GetFrameInd] = GT_CUR_FRMI;
    stringTable[GetOut] = GT_OFN;
    stringTable[GetSelf] = GT_SLFT_STS;
    stringTable[GetStims] = GT_STM_STS;
    stringTable[GetTelem] = GT_TLM_STS;
    stringTable[GetCh0] = GT_CH0_STS;
    stringTable[GetPosOnly] = GT_POS_STS;
    stringTable[ScaleSeq] = SCALE_SEQ;
    stringTable[TransSeq] = TRANS_SEQ;
    stringTable[FindJump] = FIND_N_JUMP;
    stringTable[JumpSeq] = JUMP;
    stringTable[SaveSeq] = SAV_SEQ;
    //stringTable[SaveCurSeq] = SAV_CUR_SEQ;
    stringTable[FindReplace] = FIND_N_RPLC;
    stringTable[BeginSeq] = BEGIN_SEQ;
    stringTable[EndSeq] = END_SEQ;
    stringTable[ExitSW] = EXT;
    stringTable[TelemOn] = TELEM_ON;
    stringTable[TelemOff] = TELEM_OFF;
    stringTable[ZeroOn] = CHAN0_ON;
    stringTable[ZeroOff] = CHAN0_OFF;
    stringTable[PosOnlyOn] = POSONLY_ON;
    stringTable[PosOnlyOff] = POSONLY_OFF;
    stringTable[StimsOn] = STIM_ON;
    stringTable[StimsOff] = STIM_OFF;
    stringTable[ResetROE] = RST_ROE;
    stringTable[RoeDefaultOff] = EXIT_DFLT;
    stringTable[RoeSelfOn] = SET_SLFTST;
    stringTable[ResetSW] = RESET_MDAQ;
    stringTable[PowerOn] = PWR_S TURN_ON;
    stringTable[PowerOff] = PWR_S TURN_OFF;
    stringTable[PowerQuery] = PWR_S QUERY;
    stringTable[FC_2V] = FC_2V_V_STR;
    stringTable[FC_2_5V] = FC_2_5V_V_STR;
    stringTable[ROE_2_5V] = ROE_2_5V_V_STR;
    stringTable[ROE_2_5I] = ROE_2_5V_I_STR;
    stringTable[FC_3_3V] = FC_3_3V_V_STR;
    stringTable[FC_P5_5V] = FC_POS_5_5V_V_STR;
    stringTable[ROE_P5_5VA] = ROE_POS_5_5V_VA_STR;
    stringTable[ROE_P5_5VB] = ROE_POS_5_5V_VB_STR;
    stringTable[ROE_P5_5VD] = ROE_POS_5_5V_VD_STR;
    stringTable[ROE_P5_5IA] = ROE_POS_5_5V_IA_STR;
    stringTable[ROE_P5_5IB] = ROE_POS_5_5V_IB_STR;
    stringTable[ROE_P5_5ID] = ROE_POS_5_5V_ID_STR;
    stringTable[ROE_N5_5VA] = ROE_NEG_5_5V_VA_STR;
    stringTable[ROE_N5_5VB] = ROE_NEG_5_5V_VB_STR;
    stringTable[ROE_N5_5IA] = ROE_NEG_5_5V_IA_STR;
    stringTable[ROE_N5_5IB] = ROE_NEG_5_5V_IB_STR;
    stringTable[FC_12V] = FC_12V_V_STR;
    stringTable[ROE_12VA] = ROE_12V_VA_STR;
    stringTable[ROE_12VB] = ROE_12V_VB_STR;
    stringTable[ROE_12IA] = ROE_12V_IA_STR;
    stringTable[ROE_12IB] = ROE_12V_IB_STR;
    stringTable[ROE_36VA] = ROE_36V_VA_STR;
    stringTable[ROE_36VB] = ROE_36V_VB_STR;
    stringTable[ROE_36IA] = ROE_36V_IA_STR;
    stringTable[ROE_36IB] = ROE_36V_IB_STR;
    stringTable[FC_T1] = FC_TEMP_1_STR;
    stringTable[FC_T2] = FC_TEMP_2_STR;
    stringTable[FC_T3] = FC_TEMP_3_STR;
    stringTable[ROE_TU] = ROE_TEMP_UPPER_STR;
    stringTable[ROE_TL] = ROE_TEMP_LOWER_STR;
    stringTable[ROE_CA_VOD] = ROE_CCDA_VOD_STR;
    stringTable[ROE_CA_VRD] = ROE_CCDA_VRD_STR;
    stringTable[ROE_CA_VSS] = ROE_CCDA_VSS_STR;
    stringTable[ROE_CS_VOD] = ROE_CCDS_VOD_STR;
    stringTable[ROE_CS_VRD] = ROE_CCDS_VRD_STR;
    stringTable[ROE_CS_VSS] = ROE_CCDS_VSS_STR;
    
     /*allocate space for function pointers*/
    hlpControl* functionTable;
    if((functionTable = calloc(funcNumber, sizeof(hlpControl))) == NULL){
        puts("malloc failed to allocate control function array");
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
    if((hlpHashTable = (Node**) malloc(sizeof(Node*) * hashsize)) == NULL){
        puts("malloc failed to allocate hash table");
    }
    
    /*fill hash table with array of strings matching function pointers*/
    int i;
    for(i = 0; i < funcNumber; i++){
        installNode(hlpHashTable, stringTable[i], functionTable[i]);
    }   
}

/*Determines how to execute HLP uplink packets*/
//int hlpUplink (Packet * p){
//    char* target = p->subtype;
//    
//    /*use uplink map*/
//    enum tmu funcResult = findUplink(target);
//    if(funcResult == NoFunc){
//        return BAD_PACKET;
//    }
//    else{
//        (*tmuFuncs[funcResult])();
//        return GOOD_PACKET;
//    }
//    
//}

///*matches a packet subtype with the corresponding index of tmufuncs pointer*/
//enum tmu findUplink(char * target){
//    int size = sizeof(uplinkMap) / sizeof(uplinkMap[0]);     
//    
//    /*loop to find matching string*/
//    int i = 0;
//    while(i < size){
//        if(strcmp(uplinkMap[i], target) == 0){
//            return i;
//        }
//        else{
//            i++;
//        }
//    }
//    
//    return NoFunc;  //return -1 if no matching string is found
//}

/*initiates function pointers and maps them to corresponding uplink packet subtypes*/
//void uplinkInit(){
//    uplinkMap[UDataStart] = DATASTART;
//    uplinkMap[UDataStop] = DATASTOP;
//    uplinkMap[UDark1] = DARK1;
//    uplinkMap[UDark2] = DARK2;
//    uplinkMap[UDark3] = DARK3;
//    uplinkMap[UDark4] = DARK4;
//    uplinkMap[USleep] = SLEEP;
//    uplinkMap[UWake] = WAKE;
//    uplinkMap[UTest] = TEST;
//    
//    tmuFuncs[UDataStart] = uDataStart;
//    tmuFuncs[UDataStop] = uDataStop;
//    tmuFuncs[UDark1] = uDark1;
//    tmuFuncs[UDark2] = uDark2;
//    tmuFuncs[UDark3] = uDark3;
//    tmuFuncs[UDark4] = uDark4;
//    tmuFuncs[USleep] = uSleep;
//    tmuFuncs[UWake] = uWake;
//    tmuFuncs[UTest] = uTest;
//    tmuFuncs[TDataStart] = tDataStart;
//    tmuFuncs[TDataStop] = tDataStop;
//    tmuFuncs[TDark2] = tDark2;
//    tmuFuncs[TDark4] = tDark4;
//    tmuFuncs[TSleep] = tSleep;
//}
