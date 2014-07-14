/******************************************************************************
 *
 * File Name:
 *
 *      ConsFunc.c
 *
 * Description:
 *
 *      Provides a common layer to work with the console
 *
 * Revision History:
 *
 *      04-01-07 : PLX SDK v5.00
 *
 ******************************************************************************/


#include <stdarg.h>
#include "ConsFunc.h"
#include "PlxTypes.h"




/*************************************
 *            Globals
 ************************************/
static unsigned char  _Gbl_bThrottleOutput = FALSE;
static unsigned char  _Gbl_bOutputDisable  = FALSE;
static unsigned short _Gbl_LineCount       = 0;
static unsigned short _Gbl_LineCountMax    = DEFAULT_SCREEN_SIZE - SCREEN_THROTTLE_OFFSET;



/******************************************************************
 *
 * Function   :  ConsoleInitialize
 *
 * Description:  Initialize the console
 *
 *****************************************************************/
void
ConsoleInitialize(
    void
    )
{
#if defined(PLX_LINUX)

    WINDOW *pActiveWin;


    // Initialize console
    pActiveWin = initscr();

    // Allow insert/delete of lines (for scrolling)
    idlok(
        pActiveWin,
        TRUE
        );

    // Allow console to scroll
    scrollok(
        pActiveWin,
        TRUE
        );

    // Disable buffering to provide input immediately to app
    cbreak();

    // getch will echo characters
    echo();

    // Enable blocking mode (for getch & scanw)
    nodelay(
        stdscr,
        FALSE
        );

    // Set the max line count based on current screen size
    _Gbl_LineCountMax = getmaxy(stdscr) - SCREEN_THROTTLE_OFFSET;

#elif defined(_WIN32) || defined(_WIN64)

    CONSOLE_SCREEN_BUFFER_INFO csbi;


    // Get the current console information
    GetConsoleScreenBufferInfo(
        GetStdHandle(STD_OUTPUT_HANDLE),
        &csbi
        );

    // Set the max line count based on current window size
    _Gbl_LineCountMax = ((csbi.srWindow.Bottom - csbi.srWindow.Top) + 1) - SCREEN_THROTTLE_OFFSET;

#elif defined(PLX_DOS)

    struct text_info screen_info;


    // Get the current console information
    gettextinfo(
        &screen_info
        );

    // Set the max line count based on current screen size
    _Gbl_LineCountMax = screen_info.screenheight - SCREEN_THROTTLE_OFFSET;

#endif
}




/******************************************************************
 *
 * Function   :  ConsoleEnd
 *
 * Description:  Restore the console
 *
 *****************************************************************/
void
ConsoleEnd(
    void
    )
{
#if defined(PLX_LINUX)

    // Restore console
    endwin();

#endif
}




/******************************************************************
 *
 * Function   :  ConsoleScreenHeightSet
 *
 * Description:  Sets the size of the console in number of lines
 *
 *****************************************************************/
unsigned short
ConsoleScreenHeightSet(
    unsigned short NumLines
    )
{
#if defined(PLX_LINUX)

    return -1;

#elif defined(_WIN32) || defined(_WIN64)

    SMALL_RECT WindowSize;


    // Set the new window size
    WindowSize.Left   = 0;
    WindowSize.Right  = 80 - 1;
    WindowSize.Top    = 0;
    WindowSize.Bottom = NumLines - 1;

    // Re-size window
    if (SetConsoleWindowInfo(
            GetStdHandle(STD_OUTPUT_HANDLE),
            TRUE,      // Treat coordinates as absolute
            &WindowSize
            ) == 0)
    {
        return -1;
    }

    // Update internal limit
    _Gbl_LineCountMax = NumLines - SCREEN_THROTTLE_OFFSET;

    return 0;

#elif defined(PLX_DOS)

    // Verify valid setting
    if ((NumLines != 25) &&
        (NumLines != 43) &&
        (NumLines != 50))
    {
        return -1;
    }

    // Set new screen size
    _set_screen_lines(
        NumLines
        );

    // Update internal limit
    _Gbl_LineCountMax = NumLines - SCREEN_THROTTLE_OFFSET;

    return 0;

#endif
}




/******************************************************************
 *
 * Function   :  ConsoleScreenHeightGet
 *
 * Description:  Returns the height of the current screen size
 *
 *****************************************************************/
unsigned short
ConsoleScreenHeightGet(
    void
    )
{
    return _Gbl_LineCountMax + SCREEN_THROTTLE_OFFSET;
}




/******************************************************************
 *
 * Function   :  ConsoleIoThrottle
 *
 * Description:  Toggle throttling of the console output
 *
 *****************************************************************/
void
ConsoleIoThrottle(
    unsigned char bEnable
    )
{
    _Gbl_bThrottleOutput = bEnable;

    // Reset if disabled
    if (!bEnable)
    {
        _Gbl_LineCount      = 0;
        _Gbl_bOutputDisable = FALSE;
    }
}




/******************************************************************
 *
 * Function   :  ConsoleIoThrottleReset
 *
 * Description:  Resets console line count
 *
 *****************************************************************/
void
ConsoleIoThrottleReset(
    void
    )
{
    _Gbl_LineCount = 0;
}




/******************************************************************
 *
 * Function   :  ConsoleIoOutputDisable
 *
 * Description:  Toggle console output
 *
 *****************************************************************/
void
ConsoleIoOutputDisable(
    unsigned char bEnable
    )
{
    _Gbl_bOutputDisable = bEnable;
}




/*********************************************************************
 *
 * Function   :  PlxPrintf
 *
 * Description:  Outputs a formatted string
 *
 ********************************************************************/
void
PlxPrintf(
    const char *format,
    ...
    )
{
    int          i;
    int          Width;
    int          Precision;
    char         toggle;
    char         pFormatSpec[16];
    char         pOutput[MAX_DECIMAL_BUFFER_SIZE];
    va_list      pArgs;
    const char  *pFormat;
    signed char  Size;
    signed char  Flag_Directive;
    signed char  Flag_LeadingZero;


    // Exit if console output disabled
    if (_Gbl_bOutputDisable)
        return;

    // Initialize the optional arguments pointer
    va_start(
        pArgs,
        format
        );

    pFormat = format;

    while (*pFormat != '\0')
    {
        if (*pFormat != '%')
        {
            // Non-field, just print it
            Cons_putchar(
                *pFormat
                );

            // Wrap to next line on return
            if (*pFormat == '\n')
            {
                // Check if need to pause output
                if (_Gbl_bThrottleOutput)
                {
                    // Increment line count
                    _Gbl_LineCount++;

                    if (_Gbl_LineCount >= _Gbl_LineCountMax)
                    {
                        Cons_printf(
                            "-- More (Press any to continue, 'C' for continuous, or 'Q' to quit) --"
                            );

                        // Get user input
                        toggle = Cons_getch();

                        // Clear 'More' message
                        Cons_printf(
                            "\r                                                                      \r"
                            );

                        if ((toggle == 'C') || (toggle == 'c'))
                        {
                            // Disable throttle output
                            ConsoleIoThrottle(
                                FALSE
                                );
                        }
                        else if ((toggle == 'Q') || (toggle == 'q'))
                        {
                            // Disable any further output
                            ConsoleIoOutputDisable(
                                TRUE
                                );

                            goto _Exit_PlxPrintf;
                        }
                        else
                        {
                            // Reset the line count
                            ConsoleIoThrottleReset();
                        }
                    }
                }
            }
        }
        else
        {
            // Reached '%' character
            Size             = -1;
            Width            = -1;
            Precision        = -1;
            Flag_Directive   = -1;
            Flag_LeadingZero = -1;

            pFormat++;

            // Check for any flag directives
            if ( (*pFormat == '-') || (*pFormat == '+') ||
                 (*pFormat == '#') || (*pFormat == ' ') )
            {
                Flag_Directive = *pFormat;

                pFormat++;
            }

            // Check for a width specification
            while ((*pFormat >= '0') && (*pFormat <= '9'))
            {
                // Check for leading 0
                if (Flag_LeadingZero == -1)
                {
                    if (*pFormat == '0')
                        Flag_LeadingZero = 1;
                    else
                        Flag_LeadingZero = 0;
                }

                if (Width == -1)
                    Width = 0;

                // Build the width
                Width = (Width * 10) + (*pFormat - '0');

                pFormat++;
            }

            // Check for a precision specification
            if (*pFormat == '.')
            {
                pFormat++;

                Precision = 0;

                // Get precision
                while ((*pFormat >= '0') && (*pFormat <= '9'))
                {
                    // Build the Precision
                    Precision = (signed char)(Precision * 10) + (*pFormat - '0');

                    pFormat++;
                }
            }

            // Check for size specification
            if ( (*pFormat == 'l') || (*pFormat == 'L') ||
                 (*pFormat == 'h') || (*pFormat == 'H') )
            {
                Size = *pFormat;

                pFormat++;
            }

            if (*pFormat == '\0')
                break;


            // Build the format specification
            i = 1;
            pFormatSpec[0] = '%';

            if (Flag_Directive != -1)
            {
                pFormatSpec[i] = Flag_Directive;
                i++;
            }

            if (Flag_LeadingZero == 1)
            {
                pFormatSpec[i] = '0';
                i++;
            }

            if ((Width != -1) && (Width != 0))
            {
                sprintf(
                    &(pFormatSpec[i]),
                    "%d",
                    Width
                    );

                if (Width < 10)
                    i += 1;
                else if (Width < 100)
                    i += 2;
            }

            if ((Precision != -1) && (Precision != 0))
            {
                pFormatSpec[i] = '.';
                i++;

                sprintf(
                    &(pFormatSpec[i]),
                    "%d",
                    Precision
                    );

                if (Precision < 10)
                    i += 1;
                else if (Precision < 100)
                    i += 2;
            }

            if (Size != -1)
            {
                pFormatSpec[i] = Size;
                i++;
            }

            // Check type
            switch (*pFormat)
            {
                case 'd':
                case 'i':
                    pFormatSpec[i] = *pFormat;
                    i++;
                    pFormatSpec[i] = '\0';

                    // Convert value to string
                    sprintf(
                        pOutput,
                        pFormatSpec,
                        va_arg(pArgs, int)
                        );

                    // Display output
                    Cons_printf(
                        pOutput
                        );
                    break;

                case 'x':
                case 'X':
                    pFormatSpec[i] = *pFormat;
                    i++;

                    pFormatSpec[i] = '\0';

                    // Prepare output string
                    if (Size == -1)
                    {
                        sprintf(
                            pOutput,
                            pFormatSpec,
                            va_arg(pArgs, unsigned int)
                            );
                    }
                    else
                    {
                        sprintf(
                            pOutput,
                            pFormatSpec,
                            va_arg(pArgs, long int)
                            );
                    }

                    // Display value
                    Cons_printf(
                        pOutput
                        );
                    break;

                case 'p':
                    pFormatSpec[i] = 'p';
                    i++;

                    pFormatSpec[i] = '\0';

                    sprintf(
                        pOutput,
                        pFormatSpec,
                        va_arg(pArgs, VOID*)
                        );

                    // Display pointer
                    Cons_printf(
                        pOutput
                        );
                    break;

                case 'f':
                    pFormatSpec[i] = 'f';
                    i++;

                    pFormatSpec[i] = '\0';

                    sprintf(
                        pOutput,
                        pFormatSpec,
                        va_arg(pArgs, double)
                        );

                    // Display pointer
                    Cons_printf(
                        pOutput
                        );
                    break;

                case 'c':
                    // Display the character
                    Cons_putchar(
                        (char) va_arg(pArgs, int)
                        );
                    break;

                case 'o':
                case 'u':
                    pFormatSpec[i] = *pFormat;
                    i++;
                    pFormatSpec[i] = '\0';

                    // Convert value to string
                    sprintf(
                        pOutput,
                        pFormatSpec,
                        va_arg(pArgs, unsigned int)
                        );

                    // Display output
                    Cons_printf(
                        pOutput
                        );
                    break;

                case 's':
                    Cons_printf(
                        va_arg(pArgs, char *)
                        );
                    break;

                default:
                    // Display the character
                    Cons_putchar(
                        *pFormat
                        );
                    break;
            }
        }

        pFormat++;
    }

_Exit_PlxPrintf:
    va_end(
        pArgs
        );
}




/*************************************************
 *
 *         Windows-specific functions
 *
 ************************************************/

#if defined(_WIN32) || defined(_WIN64)

/******************************************************************
 *
 * Function   :  Plx_clrscr
 *
 * Description:  Clears the console window
 *
 *****************************************************************/
void
Plx_clrscr(
    void
    )
{
    HANDLE                     hConsole;
    COORD                      CoordScreen;
    DWORD                      cCharsWritten;
    DWORD                      dwWinSize;
    CONSOLE_SCREEN_BUFFER_INFO csbi;


    // Get handle to the console
    hConsole =
        GetStdHandle(
            STD_OUTPUT_HANDLE
            );

    // Get the current console information
    GetConsoleScreenBufferInfo(
        hConsole,
        &csbi
        );

    dwWinSize = csbi.dwSize.X * csbi.dwSize.Y;

    // Set screen coordinates
    CoordScreen.X = 0;
    CoordScreen.Y = 0;

    // Fill the entire screen with blanks
    FillConsoleOutputCharacter(
        hConsole,
        (TCHAR) ' ',
        dwWinSize,
        CoordScreen,
        &cCharsWritten
        );

    // Put the cursor at origin
    SetConsoleCursorPosition(
        hConsole,
        CoordScreen
        );
}

#endif // _WIN32 || _WIN64




/*************************************************
 *
 *         Linux-specific functions
 *
 ************************************************/

#if defined(PLX_LINUX)

/******************************************************************
 *
 * Function   :  Plx_kbhit
 *
 * Description:  Determines if input is pending
 *
 *****************************************************************/
int
Plx_kbhit(
    void
    )
{
    char ch;


    // Put getch into non-blocking mode
    nodelay(
        stdscr,
        TRUE
        );

    // getch will not echo characters
    noecho();

    // Get character
    ch = getch();

    // Restore getch into blocking mode
    nodelay(
        stdscr,
        FALSE
        );

    // Restore getch echo of characters
    echo();

    if (ch == ERR)
        return 0;

    // Put character back into input queue
    ungetch(
        ch
        );

    return 1;
}




/******************************************************************
 *
 * Function   :  Plx_getch
 *
 * Description:  Gets a character from the keyboard (with blocking)
 *
 *****************************************************************/
char
Plx_getch(
    void
    )
{
    char ch;


    // getch will not echo characters
    noecho();

    // Get character
    ch = getch();

    // Restore getch echo of characters
    echo();

    return ch;
}

#endif // PLX_LINUX
