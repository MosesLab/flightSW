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

/*executes bash and attaches stdin and stdout to pipes*/
int vshell_init(int * stdin_pipe, int * stdout_pipe) {
    /*initialize pipes*/
    mknod(STDIN_PIPE, S_IFIFO | 0666, 0);
    mknod(STDOUT_PIPE, S_IFIFO | 0666, 0);
    //    if (pipe(stdin_pipe)) {
    //        fprintf(stderr, "Pipe failed.\n");
    //        return EXIT_FAILURE;
    //    }
    //    if (pipe(stdout_pipe)) {
    //        fprintf(stderr, "Pipe failed.\n");
    //        return EXIT_FAILURE;
    //    }

    pid_t result = fork();

    if (result == 0) { //this is the child

        /*redirect standard input and output*/
        record("Redirecting stdin and stdout\n");
        fopen(STDIN_PIPE, "r");
        fopen(STDOUT_PIPE, "w");
    
        //        fclose(stdout);
        //        fclose(stdin);
        freopen(STDIN_PIPE, "r", stdin); //Redirect standard input
        freopen(STDOUT_PIPE, "w", stdout); //Redirect standard output for new process
        freopen(STDOUT_PIPE, "w", stderr); //Redirect standard error for new process


        //        close(STDIN_FILENO);
        //        dup2(stdin_pipe[P_READ], STDIN_FILENO);
        //        close(stdin_pipe[P_WRITE]);
        //        
        //        close(STDOUT_FILENO);
        //        dup2(stdout_pipe[P_WRITE], STDOUT_FILENO);
        //        dup2(stdout_pipe[P_WRITE], STDERR_FILENO);
        //        close(stdout_pipe[P_READ]);

        record("Starting shell...\n");
        //Start shelld --  this one uses bash. the ./bashrc file should be used
        //        if (execlp("bash", "bash", "--noprofile", "--rcfile", "bashrc", "-i", (char *) 0) == -1) {
        if (execlp("bash", "bash", "--rcfile", "bashrc", "-i", "-s",  (char *) 0) == -1) {
            record("ERROR in starting virtual shell!\n");
        }
        return -1; //shouldn't return if it worked correctly
    }

    //    close(stdin_pipe[P_READ]);

    return 0;
}

/*executes commands carried by shell packets in virtual shell*/
void execute(int * in_pipe, packet_t * p) {
    FILE * in_stream;

    //    close(in_pipe[P_READ]);    //close other end first
    in_stream = fdopen(in_pipe[P_WRITE], "w"); //Open as file pointer
    if (in_stream == NULL) {
        fprintf(stderr, "Opening stdin pipe failed: %s \n", (char*) strerror(errno));
        return;
    }

    //    write(in_stream, p->data, p->dataLength);
    fwrite(p->data, sizeof (char), p->dataSize, in_stream);

    fclose(in_stream);
}

/*provides interface for reading from piped stdout of virtual shell 
 returns number of characters read*/
int shell_read(int * out_pipe, char * buf) {
    FILE * out_stream;
    int result;

    //    close(out_pipe[P_WRITE]);  //close other end first
    out_stream = fdopen(out_pipe[P_READ], "r");
    if (out_stream == NULL) {
        fprintf(stderr, "Opening stdout pipe failed.\n");
        return EXIT_FAILURE;
    }

    result = fread(buf, sizeof (char), 255, out_stream);
    fclose(out_stream);
    return result;
}
