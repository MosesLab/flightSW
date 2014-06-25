/* 
 * File:   controlFuncs.h
 * Author: Roy Smart
 * 
 * controlFuncs contains the functions that define what the flight computer 
 * does upon receiving a timer, uplink, or HLP packet
 *
 * Created on May 14, 2014, 7:30 PM
 */

#ifndef CONTROLFUNCS_H
#define	CONTROLFUNCS_H

#include "system.h"
#include "moses_threads.h"
#include "hlp.h"
#include "lockingQueue.h"
#include "packet.h"
#include "hashTable.h"

extern LockingQueue hkdownQueue;
/*
 * Array containing uplink subtypes. Used in a map from uplink functions to 
 * their corresponding uplink subtype
 */ 
//char* uplinkMap[9];
//void (*tmuFuncs[14])(void);   //part of timer and uplink map, obsfucated

/*initialize hash table to find functions based on packet string*/
int funcNumber; // number of control functions  
int hlp_hash_size;
node_t** hlpHashTable;

void hlpHashInit();
int execPacket(Packet*);

/*HLP uplink control functions*/
int uDataStart();
int uDataStop();
int uDark1();
int uDark2();
int uDark3();
int uDark4();
int uSleep();
int uWake();
int uTest();

/*Timer control functions*/
int tDataStart();
int tDataStop();
int tDark2();
int tDark4();
int tSleep();

/*HLP shell control functions*/

/*HLP MDAQ control functions*/
int setSequence(Packet*);       //Set Sequence command
int setOutputName(Packet*);     //Set output filename command
int getSeqName(Packet*);       //Get sequence name command
int getSeqInfo(Packet*);   //Get Sequence info command
//int getCurSeqNum(Packet*);      //Get current sequence number
int getCurSeqName(Packet*);     //Get current sequence name
int getCurFrameLen(Packet*);    //Get current frame length
int getCurFrameIndex(Packet*);  //Get current frame index
int getOutputName(Packet*);     //Get current output filename
int getSelftestStatus(Packet*);       //Get current selftest Mode status
int getStimsStatus(Packet*);    //Get STIMS mode status
int getTelemStatus(Packet*);    //Get telemetry mode status
int getCh0Status(Packet*);    //Get channel zero status
int getPosOnlyStatus(Packet*);      //Get positive channel only status
int scaleSequence(Packet*);     //Scale sequence command
int translateSeq(Packet*);      //Translate sequence command
int findAndJump(Packet*);       //Find and jump command
int jumpToExp(Packet*);              //Jump command
int saveSequence(Packet*);      //Save sequence command
//int saveCurSequence(Packet*);   //Save current sequence command
//int saveCurSeqAs(Packet*);      //Save current sequence as command
//int loadSequence(Packet*);      //Load sequence
//int unloadSequence(Packet*);  //Unload sequence
int findAndReplace(Packet*);    //Find and replace command
int beginSequence(Packet*);     //Begin Sequence command
int endSequence(Packet*);       //End Sequence command
int exitSW(Packet*);            //Exit flight software command
int telemEnable(Packet*);       //Enable telemetry command
int telemDisable(Packet*);      //Disable telemetry command
int ch0Enable(Packet*);         //Enable channel zero command
int ch0Disable(Packet*);        //Disable channel zero command
int posOnlyEnable(Packet*);     //Enable positive channel only
int posOnlyDisable(Packet*);    //Disable positive channel only
int stimsEnable(Packet*);       //Enable STIMS mode command
int stimsDisable(Packet*);       //Disable STIMS mode command
int resetROE(Packet*);          //ROE reset command
int disableDefaultROE(Packet*);    //ROE exit default mode command
int enableSelftestROE(Packet*); //ROE enter selftest mode command
//int readoutCCD(Packet*);      //readout ccd command
int resetSW(Packet*);           //Reset MDAQ and flight software
//int fireSignal(Packet*);        //Fire internal signal command
//int stopExposure(Packet*);      //Stops the current exposure
//int openShutter(Packet*);       //Open the shutter
//int closeShutter(Packet*);      //Close the shutter
//int getAEParams(Packet*);     //Get analogue electronics parameters
//int setAEParams(Packet*);     //Set analogue electronics parameters
//int getShutter(Packet*);      //Get the last shutter signal

/*HLP power control functions*/
int enablePower(Packet*);       //Turn power On for specified subsystems
int disablePower(Packet*);      //Turn power Off for specified subsystems
int queryPower(Packet*);        //Query power for specified subsystems

/*HLP housekeeping control functions*/
int FC_2V_V(Packet*);         //Flight Computer +2.0 voltage measurement
int FC_2_5V_V(Packet*);   //Flight Computer +2.5V voltage measurement
int ROE_2_5V_V(Packet*);   //ROE +2.5V voltage measurement
int ROE_2_5V_I(Packet*);  //ROE +2.5V current measurement
int FC_3_3V_V(Packet*);   //Flight Computer +3.3V voltage measurement
int FC_POS_5V_V(Packet*);     //Flight Computer +5.5V voltage measurement
int ROE_POS_5V_VA(Packet*);   //ROE +5.5V A channel voltage measurement
int ROE_POS_5V_VB(Packet*);   //ROE +5.5V B channel voltage measurement
int ROE_POS_5V_VD(Packet*);   //ROE +5.5V D channel voltage measurement
int ROE_POS_5V_IA(Packet*);   //ROE +5.5V A channel current measurement
int ROE_POS_5V_IB(Packet*);   //ROE +5.5V B channel current measurement
int ROE_POS_5V_ID(Packet*);   //ROE +5.5V D channel current measurement
int ROE_NEG_5V_VA(Packet*);   //ROE -5.5V A channel voltage measurement
int ROE_NEG_5V_VB(Packet*);   //ROE -5.5V B channel voltage measurement
int ROE_NEG_5V_IA(Packet*);   //ROE -5.5V A channel current measurement
int ROE_NEG_5V_IB(Packet*);   //ROE -5.5V B channel current measurement
int FC_12V_V(Packet*);          //Flight Computer +12V voltage measurement
int ROE_12V_VA(Packet*);        //ROE +12V A channel voltage measurement
int ROE_12V_VB(Packet*);        //ROE +12V B channel voltage measurement
int ROE_12V_IA(Packet*);        //ROE +12V A channel current measurement
int ROE_12V_IB(Packet*);        //ROE +12V B channel current measurement
int ROE_36V_VA(Packet*);        //ROE +36V A channel voltage measurement
int ROE_36V_VB(Packet*);        //ROE +36V B channel voltage measurement
int ROE_36V_IA(Packet*);        //ROE +36V A channel current measurement
int ROE_36V_IB(Packet*);        //ROE +36V B channel current measurement
int FC_TEMP_1(Packet*);         //Flight Computer temperature 1
int FC_TEMP_2(Packet*);         //Flight Computer temperature 2
int FC_TEMP_3(Packet*);         //Flight Computer temperature 3
int ROE_TEMP_UPPER(Packet*);    //ROE upper temperature
int ROE_TEMP_LOWER(Packet*);    //ROE lower temperature
int ROE_CCDA_VOD(Packet*);      //ROE CCDA_VOD current
int ROE_CCDA_VRD(Packet*);      //ROE CCDA_VRD current
int ROE_CCDA_VSS(Packet*);      //ROE CCDA_VRD current
int ROE_CCDS_VOD(Packet*);      //ROE CCDS_VOD current
int ROE_CCDS_VRD(Packet*);      //ROE CCDS_VRD current
int ROE_CCDS_VSS(Packet*);      //ROE CCDS_VRD current

enum control{
    UDataStart,
    UDataStop,
    UDark1,
    UDark2,
    UDark3,
    UDark4,
    USleep,
    UWake,
    UTest,
    
//    TDataStart,
//    TDataStop,
//    TDark2,
//    TDark4,
//    TSleep,
    
    SetSeq,     //Set Sequence Command
    SetOut,      //Set Ouput Filename Command
    GetSeqName,     //Get Sequence Name Command
    GetSeqInfo,         //Get Sequence Info Command
    GetCurSeq,          //Get Current Sequence Name
    GetFrameLen,        //Get Current Frame Length
    GetFrameInd,        //Get Current Frame Index
    GetOut,             //Get Output Filename
    GetSelf,            //Get Selftest Mode Status
    GetStims,           //Get Stims Mode Status
    GetTelem,           //Get Telemetry Enabled Status
    GetCh0,             //Get Channel Zero Enabled Status
    GetPosOnly,         //Get Positive Channel Only Status
    ScaleSeq,           //Scale Sequence Command
    TransSeq,           //Translate Sequence Command
    FindJump,           //Find and Jump Command
    JumpSeq,            //Jump Command
    SaveSeq,            //Save Sequence Command
    //SaveCurSeq,         //Save current Sequence Command
    FindReplace,        //Find and Replace Command
    BeginSeq,
    EndSeq,
    ExitSW,
    TelemOn,
    TelemOff,
    ZeroOn,
    ZeroOff,
    PosOnlyOn,
    PosOnlyOff,
    StimsOn,
    StimsOff,
    ResetROE,
    RoeDefaultOff,
    RoeSelfOn,
    ResetSW,
    
    PowerOn,
    PowerOff,
    PowerQuery,
    
    FC_2V,
    FC_2_5V,
    ROE_2_5V,
    ROE_2_5I,
    FC_3_3V,
    FC_P5_5V,
    ROE_P5_5VA,
    ROE_P5_5VB,
    ROE_P5_5VD,
    ROE_P5_5IA,
    ROE_P5_5IB,
    ROE_P5_5ID,
    ROE_N5_5VA,
    ROE_N5_5VB,
    ROE_N5_5IA,
    ROE_N5_5IB,
    FC_12V,
    ROE_12VA,
    ROE_12VB,
    ROE_12IA,
    ROE_12IB,
    ROE_36VA,
    ROE_36VB,
    ROE_36IA,
    ROE_36IB,
    FC_T1,
    FC_T2,
    FC_T3,
    ROE_TU,
    ROE_TL,
    ROE_CA_VOD,
    ROE_CA_VRD,
    ROE_CA_VSS,
    ROE_CS_VOD,
    ROE_CS_VRD,
    ROE_CS_VSS,
};

#endif	/* CONTROLFUNCS_H */

