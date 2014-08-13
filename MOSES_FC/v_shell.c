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
int vshell_init() {
    int rc;
    FILE * rf;

    /*initialize pipes*/
    //    mknod(STDIN_PIPE, S_IFIFO | 0666, 0);
    //    mknod(STDOUT_PIPE, S_IFIFO | 0666, 0);
    mkfifo(STDIN_PIPE, 0666);
    mkfifo(STDOUT_PIPE, 0666);

    pid_t result = fork();

    if (result == 0) { //this is the child
        prctl(PR_SET_NAME, "SHELL_INPUT", 0, 0, 0); //set name of the child process

        /*set thread priority*/
        int ret;
        struct sched_param params;
        params.sched_priority = sched_get_priority_max(SCHED_RR) - 10;
        ret = pthread_setschedparam(pthread_self(), SCHED_RR, &params);
        if (ret != 0) {
            // Print the error
            record("Unsuccessful in setting thread realtime prio\n");
            return 0;
        }

        /*redirect standard input and output*/
        record("Redirecting stdin and stdout\n");

        rf = fopen(STDIN_PIPE, "r");
        if (rf == NULL) record("Error opening named pipe\n");
        rf = fopen(STDOUT_PIPE, "w");
        if (rf == NULL) record("Error opening named pipe\n");

        /*Close stdin and stdout to make sure*/
        rc = fclose(stdout);
        if (rc == EOF) record("Failed to close stdout\n");
        fclose(stdin);
        if (rc == EOF) record("Failed to close stdin\n");

        /*Copy stdin and stdout to named pipes*/
        rf = freopen(STDIN_PIPE, "r", stdin); //Redirect standard input
        if (rf == NULL) record("Failed to redirect stdin\n");
        rf = freopen(STDOUT_PIPE, "w", stdout); //Redirect standard output for new process
        if (rf == NULL) record("Failed to redirect stdout\n");
        rf = freopen(STDOUT_PIPE, "w", stderr); //Redirect standard error for new process
        if (rf == NULL) record("Failed to redirect stderr\n");

        record("Starting shell...\n");
        //Start shelld --  this one uses bash. the ./bashrc file should be used
        //        if (execlp("bash", "bash", "--noprofile", "--rcfile", "bashrc", "-i", (char *) 0) == -1) {
        if (execlp("bash", "bash", "--rcfile", "bashrc", "-i", "-s", (char *) 0) == -1) {
            record("ERROR in starting virtual shell!\n");
        }
        return -1; //shouldn't return if it worked correctly
    }

    return 0;
}

///*executes commands carried by shell packets in virtual shell*/
//void execute(int * in_pipe, packet_t * p) {
//    FILE * in_stream;
//
//    //    close(in_pipe[P_READ]);    //close other end first
//    in_stream = fdopen(in_pipe[P_WRITE], "w"); //Open as file pointer
//    if (in_stream == NULL) {
//        fprintf(stderr, "Opening stdin pipe failed: %s \n", (char*) strerror(errno));
//        return;
//    }
//
//    //    write(in_stream, p->data, p->dataLength);
//    fwrite(p->data, sizeof (char), p->dataSize, in_stream);
//
//    fclose(in_stream);
//}
//
///*provides interface for reading from piped stdout of virtual shell 
// returns number of characters read*/
//int shell_read(int * out_pipe, char * buf) {
//    FILE * out_stream;
//    int result;
//
//    //    close(out_pipe[P_WRITE]);  //close other end first
//    out_stream = fdopen(out_pipe[P_READ], "r");
//    if (out_stream == NULL) {
//        fprintf(stderr, "Opening stdout pipe failed.\n");
//        return EXIT_FAILURE;
//    }
//
//    result = fread(buf, sizeof (char), 255, out_stream);
//    fclose(out_stream);
//    return result;
//}
