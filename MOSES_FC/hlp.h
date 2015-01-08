/* 
 * File:   hlp.h
 * Author: Roy Smart
 *
 * Created on February 15, 2014, 2:15 PM
 */

#ifndef HLP_H
#define	HLP_H

//Packet Types --SENT
//#define CONTROL  'C'
//#define TIMER    'T'
//#define HK_RSP   'K'
//#define MDAQ_RSP 'Q' 

//Both


//Recieved
#define UPLINK   'U'
#define MDAQ_RQS 'M'
#define PWR      'P'
#define HK_RQS   'H'
#define SHELL    'S'


/*Sent*/
#define UPLINK_S        "U"
#define TIMER_S         "T"
#define PWR_S           "P"
#define GDPKT           "G"
#define BDPKT           "B"
#define MDAQ_RQS_S      "M"
#define HK_RQS_S        "H"
#define HK_RSP         "K"
#define MDAQ_RSP        "Q" 
#define SHELL_S         "S"
#define PWR_S             "P"

//Control
#define QUIT	   "QIT"

//TIMER/UPLINK Subtypes
#define DARK1      "DK1"
#define DARK2      "DK2"
#define DARK3      "DK3"
#define DARK4      "DK4"
#define DATASTART  "DST"
#define DATASTOP   "DSP"
#define SLEEP	   "SLP"
#define WAKE	   "WAK"
#define TEST	   "TST"
#define ACK        "ACK"

//SHELL Subtypes 
#define INPUT	   "INP"
#define OUTPUT	   "OUT"
#define INTERRUPT  "INT"

//MDAQ Subtypes
#define ST_SEQUENCE "SSQ"
#define ST_OFN      "SOF"
#define GT_SEQ_NM   "GSN" //New ?
#define GT_SEQ_INFO "GSI"
#define GT_CUR_SEQ  "GCS"
#define GT_CUR_FRML "GFL"
#define GT_CUR_FRMI "GFI" 
//#define GT_CUR_SEQ_INFO "GCI" //New ?
//#define GT_CUR_SEQ_NUM  "GCN" //New ?
#define GT_OFN      "GOF"
#define GT_SLFT_STS "GST"
#define GT_STM_STS  "GSM"
#define GT_TLM_STS  "GTM"
#define GT_CH0_STS  "GC0"
#define GT_POS_STS  "GPO"
#define SCALE_SEQ   "SCL"
#define TRANS_SEQ   "TRN"
#define FIND_N_JUMP "FNJ"
#define JUMP        "JMP"
//#define SAV_CUR_SEQ "SVC" //New
#define SAV_SEQ     "SAV" //New
//#define SAV_CUR_SEQ_AS "SVA" //New	
#define LOAD_SEQ    "LDS" //New
#define UNLOAD_SEQ  "ULD" //New
#define FIND_N_RPLC "FNR"
#define BEGIN_RD_OUT "BRO" //NEW !
#define END_RD_OUT  "ERO"  //NEW !
#define BEGIN_SEQ   "BSQ"
#define END_SEQ     "ESQ"
#define EXT         "XIT" //Not Implemented
#define TELEM_ON    "TMN"
#define TELEM_OFF   "TMF"
#define CHAN0_ON    "C0N"
#define CHAN0_OFF   "C0F"
#define POSONLY_ON  "PON"
#define POSONLY_OFF "POF"
//#define READOUT_CCD "ROC" //New
#define STIM_ON     "STN"
#define STIM_OFF    "STF"
#define RST_ROE     "RRR"
#define EXIT_DFLT   "XDF"
#define SET_SLFTST  "SLF"
#define RESET_MDAQ  "RST" //Not Implemented
#define BEGIN_EXP   "EST"
#define END_EXP     "ESP"
#define ROE_GACK    "RCK"
#define ROE_BACK    "BCK"
//#define FIRESIG     "COM"
//#define STOP_EXP    "EXP"
//#define SHUTTER_OPN "SHO"
//#define SHUTTER_CLS "SHC"
//#define GET_AE      "RAE"
//#define SET_AE      "SAE"
//#define GT_SHUTTER  "GSH"

//Power Subtypes
#define TURN_ON	   "TON"
#define TURN_OFF   "TOF"
#define QUERY	   "QRY"
#define STATUS_ON  "SON"
#define STATUS_OFF "SOF"

//HK Subtypes
#define POS2_0V  "2.0"
#define POS3_3V  "3.3"
#define POS2_5V  "2.5"
#define POS5V    "+5V"
#define NEG5V    "-5V"
#define NEG10V   "10V"
#define POS12V   "12V"
#define POS36V   "36V"
#define TEMP     "TMP"
#define CCDA_VOD "AVO"
#define CCDA_VRD "AVR"
#define CCDA_VSS "AVS"
#define CCDB_VOD "BVO"
#define CCDB_VRD "BVR"
#define CCDB_VSS "BVS"

//HK Arguments
#define FC_P2_5V_V   "VC"
#define ROE_P2_5VD_V "VD"
#define ROE_P2_5VD_I "ID"

#define ROE_P5VAA_V  "VA"
#define ROE_P5VAB_V  "VB"
#define FC_P5V_V     "VC"
#define ROE_P5VD_V   "VD"
#define ROE_P5VAA_I  "IA"
#define ROE_P5VAB_I  "IB"
#define ROE_P5VD_I   "IC"       //wrong according to hlp document, should be "ID" EGSE software is incorrect

#define ROE_N5VAA_V  "VA"
#define ROE_N5VAB_V  "VB"
#define ROE_N5VAA_I  "IA"
#define ROE_N5VAB_I  "IB"

#define ROE_N10VA_V  "VA"
#define ROE_N10VB_V  "VB"

#define ROE_P12VA_V  "VA"
#define ROE_P12VB_V  "VB"
#define FC_P12V_V    "VC"
#define ROE_P12VA_I  "IA"
#define ROE_P12VB_I  "IB"

#define ROE_P36VA_V  "VA"
#define ROE_P36VB_V  "VB"
#define ROE_P36VA_I  "IA"
#define ROE_P36VB_I  "IB"

#define TEMP1 "1"
#define TEMP2 "2"
#define TEMP3 "3"
#define UPPER "U"
#define LOWER "L"


//#define BAD_PACKET	-1
//#define GOOD_PACKET	0
#define BAD_PACKET	0
#define GOOD_PACKET	1


#endif	/* HLP_H */

