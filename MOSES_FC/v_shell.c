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

/*save file pointers for redirected std in/out*/
FILE * stdin_filep;
FILE * stdout_filep;

/*executes bash and attaches stdin and stdout to pipes*/
int vshell_init() {
    int rc;
    FILE * rf;

    /*bash signal masks*/
    sigset_t set;
    struct sigaction sa;

    /*initialize pipes*/
    mkfifo(STDIN_PIPE, 0666);
    mkfifo(STDOUT_PIPE, 0666);

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
    fclose(stderr);
    if (rc == EOF) record("Failed to close stdin\n");

    /*Copy stdin and stdout to named pipes*/
    stdin_filep = freopen(STDIN_PIPE, "r", stdin); //Redirect standard input
    if (stdin_filep == NULL) record("Failed to redirect stdin\n");
    stdout_filep = freopen(STDOUT_PIPE, "w", stdout); //Redirect standard output for new process
    if (stdout_filep == NULL) record("Failed to redirect stdout\n");
    stdout_filep = freopen(STDOUT_PIPE, "w", stderr); //Redirect standard error for new process
    if (stdout_filep == NULL) record("Failed to redirect stderr\n");

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

        /* Set up the structure to specify the new action. */
        memset(&sa, 0, sizeof (struct sigaction));
        sa.sa_handler = SIG_DFL;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGINT, &sa, NULL);
        sigaction(SIGTERM, &sa, NULL);
        sigaction(SIGPIPE, &sa, NULL);
        sigaction(SIGCHLD, &sa, NULL);
        sigaction(SIGHUP, &sa, NULL);
        sigaction(SIGUSR1, &sa, NULL);
        sigaction(SIGUSR2, &sa, NULL);

        /* unblock all signals */
        sigfillset(&set);
        pthread_sigmask(SIG_UNBLOCK, &set, NULL);

        record("Starting shell...\n");
        //Start shelld --  this one uses bash. the ./bashrc file should be used
        //        if (execlp("bash", "bash", "--noprofile", "--rcfile", "bashrc", "-i", (char *) 0) == -1) {
        if (execlp("bash", "bash", "--rcfile", "bashrc", "-i", "-s", (char *) 0) == -1) {
            record("ERROR in starting virtual shell!\n");
        }
        return -1; //shouldn't return if it worked correctly
    }

    return result;
}

