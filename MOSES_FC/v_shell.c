/*
 * File: v_shell.c
 * Author: Roy Smart
 * Montana State University
 * 
 * Created on June 30, 2014, 4:07 PM
 * 
 * Executes shell packets
 */
#include "v_shell.h"

int vshell_init(int * stdin_pipe, int * stdout_pipe) {
    /*initialize pipes*/
    if (pipe(stdin_pipe)) {
        fprintf(stderr, "Pipe failed.\n");
        return EXIT_FAILURE;
    }
    if (pipe(stdout_pipe)) {
        fprintf(stderr, "Pipe failed.\n");
        return EXIT_FAILURE;
    }

    pid_t result = fork();

    if (result == 0) {
        
        /*redirect standard input and output*/
        record("Redirecting stdin and stdout");
        dup2(stdin_pipe[0], STDIN_FILENO);
        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stdout_pipe[1], STDERR_FILENO);

        record("starting shell");
        //Start shelld --  this one uses bash. the ./bashrc file should be used
        if (execlp("bash", "bash", "--noprofile", "--rcfile", "bashrc", "-i", (char *) 0) == -1) {
            record("ERROR in starting virtual shell!");
        }
        return -1;      //shouldn't return if it worked correctly
    }


    return 0;
}