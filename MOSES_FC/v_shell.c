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

int vshell_init(){
    /*initialize pipes*/
    if(pipe(stdin_pipe)){
        fprintf (stderr, "Pipe failed.\n");
           return EXIT_FAILURE;
    }
    if(pipe(stdout_pipe)){
        fprintf (stderr, "Pipe failed.\n");
           return EXIT_FAILURE;
    }
    
    int result = fork();
    
    if(result == 0){
        
    }
    
    
    return 0;
}