/*******************************************************************************
Based on PLX SDK v5.00
********************************************************************************
Project:		FreeForm/PCI-104
Module:			PlxInit.h
Description:	IPlx Initialization functions, plus some common FPGA register
				access routines
********************************************************************************
Date		Author	Modifications
--------------------------------------------------------------------------------
2009-03-11	MF		Added include dependencies from .c
2009-08-20	MF		Add board type FCG005,FCG006
2009-10-27	MF		Add more board types FCG007-FCG012
*******************************************************************************/

#ifndef PLXINIT_H
#define PLXINIT_H

#include <time.h>
#include <stdio.h>
#include "Plx.h"
#include "PlxApi.h"
#include "Reg9056.h"

#if defined(PLX_MSWINDOWS)
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <WinBase.h>

#pragma warning( disable : 4996 )  // disable depractation warning.
#elif defined(PLX_LINUX) || defined(PLX_QNX6)
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

//#define _CRT_SECURE_NO_DEPRECATE

#ifndef __PLX_ERROR_H
//#pragma message("Creating definition for RETURN_CODE")
typedef PLX_STATUS        RETURN_CODE;  // to support use with older api
#endif

typedef struct _boardInfo
{
	char	boardRev;
	U8		boardType;
	U8		boardIdx;
	char	serialNum[5];
	char	boardName[7];
	U16		plxDeviceType;

} boardInfo;

#define BOARD_TYPE_FCG001	'A'
#define BOARD_TYPE_FCG002	'B'
#define BOARD_TYPE_FCG003	'C'
#define BOARD_TYPE_FCG004	'D'
#define BOARD_TYPE_FCG005	'E'
#define BOARD_TYPE_FCG006	'F'
#define BOARD_TYPE_FCG007	'G'
//#define BOARD_TYPE_FCG008	'H'
//#define BOARD_TYPE_FCG009	'I'

#define BOARD_TYPE_FCG021	'H'
#define BOARD_TYPE_FCG022	'I'
#define BOARD_TYPE_FCG023	'J'
#define BOARD_TYPE_FCG024	'K'
#define BOARD_TYPE_FCG025	'L'
#define BOARD_TYPE_FCG026	'M'

#define BOARD_TYPE_FCG010	'N'
#define BOARD_TYPE_FCG011	'O'
#define BOARD_TYPE_FCG012	'P'
#define BOARD_TYPE_FCG014	'Q'

/******************************
*        Definitions
******************************/
#define MAX_DEVICES_TO_LIST        50

typedef struct _API_ERRORS
{
    RETURN_CODE  code;
    char        *text;
} API_ERRORS;

#ifndef PLX_QNX6
typedef struct _PLX_PTR
{
	PLX_DEVICE_OBJECT*	pDevice;
	PLX_INTERRUPT*		pInterrupt;
	PLX_NOTIFY_OBJECT*	pEvent;
} PLX_PTR;
#endif

/********************************************************************************
*        Functions
*********************************************************************************/
S8 SelectDevice(PLX_DEVICE_KEY *pKey);
char* PlxSdkErrorText(RETURN_CODE code);
void PlxSdkErrorDisplay(RETURN_CODE code);
RETURN_CODE GetAndOpenDevice (PLX_DEVICE_OBJECT* pDevice, U16 plxDeviceID  );
RETURN_CODE WriteDword(PLX_DEVICE_OBJECT* pDevice, U8 bar, U32 offset, U32 value );
RETURN_CODE WriteWord(PLX_DEVICE_OBJECT* pDevice, U8 bar, U32 offset, U16 value );
RETURN_CODE WriteByte(PLX_DEVICE_OBJECT* pDevice, U8 bar, U32 offset, U8 value );
RETURN_CODE ReadDword(PLX_DEVICE_OBJECT* pDevice, U8 bar, U32 offset, U32* pValue );
RETURN_CODE ReadWord(PLX_DEVICE_OBJECT* pDevice, U8 bar, U32 offset, U16* pValue );
RETURN_CODE ReadByte(PLX_DEVICE_OBJECT* pDevice, U8 bar, U32 offset, U8* pValue );
#ifndef PLX_QNX6
U32 atoh( char *string ) ;
#endif
char get_digit( char c ) ;
RETURN_CODE waitBitSet(PLX_DEVICE_OBJECT* pDevice, U8 bar, U32 offset, U32 bitMask);
RETURN_CODE waitBitClr(PLX_DEVICE_OBJECT* pDevice, U8 bar, U32 offset, U32 bitMask);
U16 CTISleep(U16 ms);
U16 CTISleepUS(U16 us);
void CTIPause();
int xgetch(void);
int xgetche(void);
char* xgets(char* buf, int sz );
char yesno_loop(char* prompt);

#endif
