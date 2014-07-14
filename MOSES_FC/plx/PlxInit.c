/*******************************************************************************
Based on PLX SDK v5.00
********************************************************************************
Project:		FreeForm/PCI-104
Module:			PlxInit.c
Description:	Plx Initialization functions, plus some common FPGA register
				access routines
********************************************************************************
Date		Author	Modifications
--------------------------------------------------------------------------------
2009-02-26	MF		Move Direct Master & Interrupt functions to another module
2009-03-11	MF		Moved include dependencies to .h
2012-02-13	MF		Minor correction to sleep functions
2012-02-14	MF		Add xgetch(), yesno_loop
2012-02-16	MF		Comment out error prints in GetAndOpen
2012-02-17	MF		Add xgetche
*******************************************************************************/

#include "PlxInit.h"

#ifdef METRICS
LARGE_INTEGER tTotalWaitBit;
U32 numWait = 0;
U32 waitBitIterations = 0;
#endif

/**********************************************
*               Globals
**********************************************/
API_ERRORS ApiErrors[] =
{
    { ApiSuccess,                   "ApiSuccess"                   },
    { ApiFailed,                    "ApiFailed"                    },
    { ApiNullParam,                 "ApiNullParam"                 },
    { ApiUnsupportedFunction,       "ApiUnsupportedFunction"       },
    { ApiNoActiveDriver,            "ApiNoActiveDriver"            },
    { ApiConfigAccessFailed,        "ApiConfigAccessFailed"        },
    { ApiInvalidDeviceInfo,         "ApiInvalidDeviceInfo"         },
    { ApiInvalidDriverVersion,      "ApiInvalidDriverVersion"      },
    { ApiInvalidOffset,             "ApiInvalidOffset"             },
    { ApiInvalidData,               "ApiInvalidData"               },
    { ApiInvalidSize,               "ApiInvalidSize"               },
    { ApiInvalidAddress,            "ApiInvalidAddress"            },
    { ApiInvalidAccessType,         "ApiInvalidAccessType"         },
    { ApiInvalidIndex,              "ApiInvalidIndex"              },
    { ApiInvalidPowerState,         "ApiInvalidPowerState"         },
    { ApiInvalidIopSpace,           "ApiInvalidIopSpace"           },
    { ApiInvalidHandle,             "ApiInvalidHandle"             },
    { ApiInvalidPciSpace,           "ApiInvalidPciSpace"           },
    { ApiInvalidBusIndex,           "ApiInvalidBusIndex"           },
    { ApiInsufficientResources,     "ApiInsufficientResources"     },
    { ApiWaitTimeout,               "ApiWaitTimeout"               },
    { ApiWaitCanceled,              "ApiWaitCanceled"              },
    { ApiDmaChannelUnavailable,     "ApiDmaChannelUnavailable"     },
    { ApiDmaChannelInvalid,         "ApiDmaChannelInvalid"         },
    { ApiDmaDone,                   "ApiDmaDone"                   },
    { ApiDmaPaused,                 "ApiDmaPaused"                 },
    { ApiDmaInProgress,             "ApiDmaInProgress"             },
    { ApiDmaCommandInvalid,         "ApiDmaCommandInvalid"         },
    { ApiDmaInvalidChannelPriority, "ApiDmaInvalidChannelPriority" },
    { ApiDmaSglPagesGetError,       "ApiDmaSglPagesGetError"       },
    { ApiDmaSglPagesLockError,      "ApiDmaSglPagesLockError"      },
    { ApiMuFifoEmpty,               "ApiMuFifoEmpty"               },
    { ApiMuFifoFull,                "ApiMuFifoFull"                },
    { ApiPowerDown,                 "ApiPowerDown"                 },
    { ApiHSNotSupported,            "ApiHSNotSupported"            },
    { ApiVPDNotSupported,           "ApiVPDNotSupported"           },
    { ApiLastError,                 "Unknown"                      }
};


/*********************************************************************
 * Function   :  PlxSdkErrorText
 * Description:  Returns the text string associated with a RETURN_CODE
 ********************************************************************/
char* PlxSdkErrorText(RETURN_CODE code)
{
    U16 i;
	
    i = 0;

    while (ApiErrors[i].code != ApiLastError)
    {
        if (ApiErrors[i].code == code)
        {
            return ApiErrors[i].text;
        }

        i++;
    }

    return ApiErrors[i].text;
}


/*********************************************************************
 * Function   :  PlxSdkErrorDisplay
 * Description:  Displays the API error code and corresponding text
 ********************************************************************/
void PlxSdkErrorDisplay(RETURN_CODE code )
{
    printf("\tAPI Error: %s (%03Xh)\n",PlxSdkErrorText(code),code);
}

/*********************************************************************
Grabs first device
*********************************************************************/
RETURN_CODE GetAndOpenDevice (PLX_DEVICE_OBJECT* pDevice, U16 plxDeviceID )
{
	PLX_DEVICE_KEY    	DevKey;
	RETURN_CODE			rc;

	DevKey.bus         = PCI_FIELD_IGNORE;
	DevKey.slot        = PCI_FIELD_IGNORE;
	DevKey.function    = PCI_FIELD_IGNORE;
	DevKey.DeviceId    = plxDeviceID; //0x9056;
	DevKey.VendorId    = 0x10B5;
	DevKey.SubDeviceId = PCI_FIELD_IGNORE;
	DevKey.SubVendorId = PCI_FIELD_IGNORE;
	DevKey.Revision    = PCI_FIELD_IGNORE;

	// Check if device exists
	rc = PlxPci_DeviceFind(&DevKey,0);

	if (rc == ApiSuccess)
	{
		printf("Device Info: \n\tDevice ID=%04x\n\tVendor ID=%04x\n"
					"\tSub-Device ID=%04x\n\tSub-Vendor ID=%04x\n"
					"\tBus=%02x\n\tSlot=%02x\n\tFunction=%02x\n",
				DevKey.DeviceId,
				DevKey.VendorId,
				DevKey.SubDeviceId,
				DevKey.SubVendorId,
				DevKey.bus,
				DevKey.slot,
				DevKey.function
				);
	}
	else
    {
        //printf("\n   ERROR: Unable to find or select a PLX device\n");
		//PlxSdkErrorDisplay(rc);
		return(rc);
    }

	// Open the device
    rc = PlxPci_DeviceOpen(&DevKey,pDevice );

    if (rc != ApiSuccess)
    {
		//printf("\n   ERROR: Unable to open PLX device\n");
		//PlxSdkErrorDisplay(rc);
		return(rc);
    }
	
	return(rc);
}

RETURN_CODE WriteDword(PLX_DEVICE_OBJECT* pDevice, U8 bar, U32 offset, U32 value )
{
	return ( PlxPci_PciBarSpaceWrite(pDevice,bar,offset,&value,4,BitSize32,FALSE) );
}

RETURN_CODE WriteWord(PLX_DEVICE_OBJECT* pDevice, U8 bar, U32 offset, U16 value )
{
	return ( PlxPci_PciBarSpaceWrite(pDevice,bar,offset,&value,2,BitSize16,FALSE) );
}

RETURN_CODE WriteByte(PLX_DEVICE_OBJECT* pDevice, U8 bar, U32 offset, U8 value )
{
	return ( PlxPci_PciBarSpaceWrite(pDevice,bar,offset,&value,1,BitSize8,FALSE) );
}

RETURN_CODE ReadDword(PLX_DEVICE_OBJECT* pDevice, U8 bar, U32 offset, U32* pValue )
{
	return ( PlxPci_PciBarSpaceRead(pDevice,bar,offset,pValue,4,BitSize32,FALSE) );
}

RETURN_CODE ReadWord(PLX_DEVICE_OBJECT* pDevice, U8 bar, U32 offset, U16* pValue )
{
	return ( PlxPci_PciBarSpaceRead(pDevice,bar,offset,pValue,2,BitSize16,FALSE) );
}

RETURN_CODE ReadByte(PLX_DEVICE_OBJECT* pDevice, U8 bar, U32 offset, U8* pValue )
{
	return ( PlxPci_PciBarSpaceRead(pDevice,bar,offset,pValue,1,BitSize8,FALSE) );
}

/********************************************************************** 
* Function:		atoh 
* Description:	convert an ascii string to hex number 
**********************************************************************/ 
#ifndef PLX_QNX6 
U32		atoh( char *string ) 
{ 
	int	digit, result; 
 
	result = 0; 
	while( ( digit = get_digit( *string++ ) ) != -1 ) 
		result = result * 16 + digit; 
	return( result ); 
} 
#endif
 
/********************************************************************** 
* Function:		get_digit 
* Description:	convert a single ascii digit to hex 
**********************************************************************/ 
 
char 	get_digit( char c ) 
{ 
	if ( ( c >= '0' ) && ( c <= '9' ) ) 
		return( c - '0' ); 
	if ( ( c >= 'a' ) && ( c <= 'f' ) ) 
		return( c - 'a' + 10 ); 
	if ( ( c >= 'A' ) && ( c <= 'F' ) ) 
		return( c - 'A' + 10 ); 
	return( -1 ); 
} 



#define SLEEP_DELAY 2

RETURN_CODE waitBitSet(PLX_DEVICE_OBJECT* pDevice, U8 bar, U32 offset, U32 bitMask)
{
	U32 rVal;
	RETURN_CODE rc;

	do
	{
		rc = ReadDword (pDevice,bar,offset,&rVal);
//		printf("%x\n",rVal);
//		CTISleep(SLEEP_DELAY);
		CTISleepUS(150);

	} while (((rVal & bitMask) != bitMask)  && rc == ApiSuccess);

	return (rc);
}



RETURN_CODE waitBitClr(PLX_DEVICE_OBJECT* pDevice, U8 bar, U32 offset, U32 bitMask)
{
	U32 rVal;
	RETURN_CODE rc;

	#ifdef METRICS
	LARGE_INTEGER tStart, tEnd;
	numWait++;
	QueryPerformanceCounter(&tStart);
	#endif

	do
	{
		rc = ReadDword (pDevice,bar,offset,&rVal);
//		printf("%x\n",rVal);
//		CTISleep(SLEEP_DELAY);
		#ifdef METRICS
		waitBitIterations++;
		#endif

	} while (((rVal | ~bitMask) != ~bitMask)  && rc == ApiSuccess);

	#ifdef METRICS
	QueryPerformanceCounter(&tEnd);
	tTotalWaitBit.QuadPart = tTotalWaitBit.QuadPart+(tEnd.QuadPart-tStart.QuadPart);
	#endif

	return (rc);
}



U16 CTISleep(U16 ms)
{

	#if defined(PLX_MSWINDOWS)
		Sleep(ms);
		return(0);
	#elif defined(PLX_LINUX) || defined(PLX_QNX6)
		struct timespec ts;

		ts.tv_sec = ms/1000;
		ts.tv_nsec = (ms - (ts.tv_sec*1000) )* 1000000;
		//printf("\nCTISleep: ts.tv_sec = %d, ts.tv_nsec = %ld\n",ts.tv_sec ,ts.tv_nsec);  
		return ( nanosleep(&ts, NULL) );	
	#endif
	
}

U16 CTISleepUS(U16 us)
{

	#if defined(PLX_MSWINDOWS)
		//usleep(us);
		__int64 timeEllapsed;
		__int64 timeStart;
		__int64 timeDelta;
		__int64 timeToWait;

		QueryPerformanceFrequency( (LARGE_INTEGER*)(&timeDelta ) );

		timeToWait = (__int64)((double)timeDelta * (double)us / 1000000.0f);

		QueryPerformanceCounter ( (LARGE_INTEGER*)(&timeStart ) );

		timeEllapsed = timeStart;

		while( ( timeEllapsed - timeStart ) < timeToWait )
		{
			QueryPerformanceCounter( (LARGE_INTEGER*)(&timeEllapsed ) ); 
		}

		return(0);
	#else
		struct timespec ts;

		ts.tv_sec = us/1000000;
		ts.tv_nsec = (us - (ts.tv_sec*1000000) )* 1000;
		//printf("\nCTISleep: ts.tv_sec = %d, ts.tv_nsec = %ld\n",ts.tv_sec ,ts.tv_nsec);  
		return ( nanosleep(&ts, NULL) );
	#endif
}

/*
U8 charToHex(S8 c)
{
	switch(c)
	{
		case '0' : 
			return 0x0;
		case '1' : 
			return 0x1;
		case '2' : 
			return 0x2;
		case '3' : 
			return 0x3;
		case '4' : 
			return 0x4;
		case '5' : 
			return 0x5;
		case '6' : 
			return 0x6;
		case '7' : 
			return 0x7;
		case '8' : 
			return 0x8;
		case '9' : 
			return 0x9;
		case 'A':
		case 'a': 
			return 0xA;
		case 'B' :
		case 'b' : 
			return 0xB;
		case 'C' :
		case 'c' : 
			return 0xC;
		case 'D' :
		case 'd' : 
			return 0xD;
		case 'E' :
		case 'e' : 
			return 0xE;
		case 'F' :
		case 'f' : 
			return 0xF;
		default : 
			return 0x0;
	}
}*/
void CTIPause()
{

	#if defined(PLX_MSWINDOWS)
	fflush( stdin ); 
	printf("\n\n[Press any key to continue]\n");
	getchar();

	#elif defined(PLX_LINUX)
	printf("\n >> [Press any key to continue]\n");
	xgetch();
	#endif
}

/*******************************************************************************
Function:		yesno_loop
Description:
*******************************************************************************/

char yesno_loop(char* prompt)
{
	char c;

	#if defined(PLX_MSWINDOWS)
	fflush( stdin ); 
	#endif
	
	do
	{
		printf("\n\n >> %s Press (y) or (n):  ", prompt);
		c = xgetche();
	} while (c != 'y' && c != 'n');
	printf("\n");
	return(c);
}

/*******************************************************************************
Function:		xgetch
Description:	getch emulation
*******************************************************************************/
int xgetch(void)
{
	#if defined(PLX_MSWINDOWS)
	return getch();

	#elif defined(PLX_LINUX)
	struct termios oldt, newt;
	int ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;

	#endif
}

int xgetche(void)
{
	#if defined(PLX_MSWINDOWS)
	return getche();

	#elif defined(PLX_LINUX)
	struct termios oldt, newt;
	int ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;

	#endif
}

/*******************************************************************************
Function:		xgets
Description:	fgets sort of, without enter
*******************************************************************************/
char* xgets(char* buf, int sz )
{
	#if defined(PLX_MSWINDOWS)
	int i;
	for(i=0;i<sz;i++)
	{
		buf[i] = getche();
	}
	return buf;

	#elif defined(PLX_LINUX)
	struct termios oldt, newt;
	int i;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON  );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	for(i=0;i<sz;i++)
	{
		buf[i] = getchar();
	}
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return buf;
	#endif
}
