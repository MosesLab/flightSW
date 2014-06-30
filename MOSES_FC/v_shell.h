/* 
 * File:   v_shell.h
 * Author: Roy Smart
 * Montana State University
 *
 * Created on June 30, 2014, 4:07 PM
 */

#ifndef V_SHELL_H
#define	V_SHELL_H

#include "system.h"

//#define VSHELL_OUTPUT "vshell_output"

int stdin_pipe[2];
int stdout_pipe[2];

int vshell_init();



#endif	/* V_SHELL_H */

