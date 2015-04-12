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

#define STDIN_PIPE PATH "fifo/moses_stdin_fifo"
#define STDOUT_PIPE PATH "fifo/dev/moses_stdout_fifo"

#define P_READ 0
#define P_WRITE 1

extern int stdin_copy;
extern int stdout_copy;

int vshell_init();



#endif	/* V_SHELL_H */

