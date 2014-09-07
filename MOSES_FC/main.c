/* 
 * File:   main.c
 * Author: Roy Smart
 *
 * Created on January 22, 2014, 5:21 PM
 */

#include "main.h"

volatile sig_atomic_t ts_alive = 1; //variable modified by signal handler, setting this to false will end the threads

unsigned int num_threads = NUM_RROBIN + NUM_FIFO;
thread_func tfuncs[NUM_RROBIN + NUM_FIFO];
void * targs[NUM_RROBIN + NUM_FIFO];
pthread_t threads[NUM_RROBIN + NUM_FIFO]; //array of running threads

pid_t main_pid;
int quit_sig = 0;
struct sigaction quit_action, init_action, start_action; //action to be taken when ^C (SIGINT) is entered
sigset_t mask, oldmask; //masks for SIGINT signal

int config_values[NUM_RROBIN + NUM_FIFO + NUM_IO]; //array of values holding moses program configurations  
node_t** config_hash_table;

LockingQueue lqueue[QUEUE_NUM];

/**
 * Main program entry point. Reads configuration file to configure program.
 * Starts flight software threads based off of configs.
 * 
 * @return 0 upon successful exit
 */
int main(void) {


    record("*****************************************************\n");
    record("MOSES FLIGHT SOFTWARE\n");
    record("*****************************************************\n");

    /*init configuration stings*/
    main_init();

    /*read in configuration values*/
    read_moses_config();

    /*record this thread's PID*/
    main_pid = getpid();

    /*Use signals to inform the program to quit*/
    init_quit_signal_handler();

    /*initialize virtual shell*/
    vshell_pid = vshell_init();

    /*start threads indicated by configuration file*/
    start_threads();



    /*Upon program termiation (^c) attempt to join the threads*/
    //    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    //    while (ts_alive) {
    //        sigsuspend(&oldmask); // wait here until the program is killed
    //    }
    //    sigprocmask(SIG_UNBLOCK, &mask, &oldmask);

//    while (ts_alive) {

                pthread_sigmask(SIG_BLOCK, &mask, &oldmask);
                sigwait(&mask, &quit_sig);
                pthread_sigmask(SIG_UNBLOCK, &mask, &oldmask);
//        wait(0);
//    }

    record("exited wait\n");
    
    /*SIGINT caught, ending program*/
    join_threads();

    record("FLIGHT SOFTWARE EXITED\n\n\n");

    return 0;
}

/*this method takes a function pointer and starts it as a new thread*/
void start_threads() {
    pthread_attr_t attrs[num_threads]; //FIFO and round robin scheduling attributes
    struct sched_param param;
    int max_fifo = sched_get_priority_max(SCHED_FIFO);
    int max_rr = sched_get_priority_max(SCHED_RR);

    /*fill array of arguments for pthread call*/
    targs[hlp_control_thread] = &config_values[hlp_up_interface];
    targs[hlp_down_thread] = &config_values[hlp_down_interface];
    targs[gpio_control_thread] = NULL;
    targs[hlp_shell_thread] = NULL;
    targs[sci_timeline_thread] = NULL;
    targs[telem_thread] = NULL;
    targs[image_writer_thread] = NULL;
    targs[fpga_server_thread] = NULL;

    /**
     * FIFO thread attribute loop
     * Threads get lower priority as the loop progresses
     */
    unsigned int i;
    for (i = 0; i < num_threads; i++) {
        pthread_attr_init(&attrs[i]);
        pthread_attr_setinheritsched(&attrs[i], PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setdetachstate(&attrs[i], PTHREAD_CREATE_JOINABLE);

        if (i < NUM_FIFO) { // Set thread policy as FIFO for the first threads
            param.sched_priority = max_fifo - i;
            pthread_attr_setschedpolicy(&attrs[i], SCHED_FIFO);
        } else { //Set scheduling policy as round-robin
            param.sched_priority = max_rr - i;
            pthread_attr_setschedpolicy(&attrs[i], SCHED_RR);
        }
        pthread_attr_setschedparam(&attrs[i], &param);

        /*if configured, start thread*/
        if (config_values[i] == 1) {
            pthread_create(&threads[i], &attrs[i], tfuncs[i], targs[i]);
        }
    }
}

/*more like canceling threads at the moment, not sure if need to clean up properly*/
void join_threads() {
//    void * returns;

    /*sleep to give threads a chance to clean up a little*/
    sleep(1);
    
    kill(vshell_pid, SIGTERM);
    
    record("killed bash\n");

    int i;
    for (i = 0; i < num_threads; i++) {
        if (threads[i] != 0) {
//            pthread_join(threads[i], &returns);
                        pthread_cancel(threads[i]);
        }
    }
}

/*initialize signal handler to listen for quit signal*/
void init_quit_signal_handler() {
    sigfillset(&oldmask); //save the old mask
    sigemptyset(&mask); //create a blank new mask

    /*quit signal handling*/
    sigaddset(&mask, SIGINT); //add SIGINT (^C) to mask
    quit_action.sa_handler = quit_signal;
    quit_action.sa_mask = oldmask;
    quit_action.sa_flags = SA_RESTART;
    sigaction(SIGINT, &quit_action, NULL);

    /*experiment data start signal handling*/
//    sigaddset(&mask, SIGUSR1);
    start_action.sa_handler = start_signal;
    start_action.sa_mask = oldmask;
    start_action.sa_flags = 0;
    sigaction(SIGUSR1, &start_action, NULL);

}

/*signal all threads to exit*/
void quit_signal(int sig) {
    record("SIGINT received\n");
    ts_alive = 0;
}

/*Signal experiment to start gathering data*/
void start_signal(int sig) {
    record("Flight computer signaled Data Start\n");
    uDataStart();
}

/*set up hash table with configuration strings to match values in moses.conf*/
void main_init() {
    uint config_size = num_threads + NUM_IO;
    char * config_strings[num_threads + NUM_IO];

    /*allocate strings to match with configuration file*/
    config_strings[hlp_control_thread] = HLP_CONTROL_CONF;
    config_strings[hlp_down_thread] = DOWN_CONF;
    config_strings[gpio_control_thread] = GPIO_CONTROL_CONF;
    config_strings[hlp_shell_thread] = SHELL_CONF;
    config_strings[sci_timeline_thread] = SCIENCE_CONF;
    config_strings[telem_thread] = TELEM_CONF;
    config_strings[image_writer_thread] = IMAGE_WRITER_CONF;
    config_strings[fpga_server_thread] = FPGA_SERVER_CONF;

    config_strings[hlp_up_interface] = HKUP_CONF;
    config_strings[hlp_down_interface] = HKDOWN_CONF;
    config_strings[roe_interface] = ROE_CONF;
    config_strings[synclink_interface] = SYNCLINK_CONF;

    /*initialize memory for configuration hash table*/
    if ((config_hash_table = calloc(config_size, sizeof (node_t))) == NULL) {
        record("malloc failed to allocate hash table\n");
    }

    /*fill hash table with array of strings matching indices for configuration values*/
    uint i;
    for (i = 0; i < config_size; i++) {
        int * int_def = malloc(sizeof (int));
        *int_def = i;

        /*insert node into hash table*/
        installNode(config_hash_table, config_strings[i], int_def, config_size);
    }

    /*fill array of function pointer for pthread call*/
    tfuncs[hlp_control_thread] = hlp_control;
    tfuncs[hlp_down_thread] = hlp_down;
    tfuncs[gpio_control_thread] = hlp_housekeeping;
    tfuncs[hlp_shell_thread] = hlp_shell_out;
    tfuncs[sci_timeline_thread] = science_timeline;
    tfuncs[telem_thread] = telem;
    tfuncs[image_writer_thread] = write_data;
    tfuncs[fpga_server_thread] = fpga_server;

    /*initialize locking queues*/
    for (i = 0; i < QUEUE_NUM; i++) {
        lockingQueue_init(&lqueue[i]);
    }

}

/*read in configuration file for thread and I/O attributes*/
void read_moses_config() {
    int rc = 0;
    unsigned int config_size = num_threads + NUM_IO;
    char * config_path = "moses.conf";


    record("Reading in configuration file.....\n");

    /*read configuration file*/
    FILE * config_fp = fopen(config_path, "r");

    while (rc != EOF) {
        char new_char; //first char of each line
        int next_value, string_len = 0;
        /*check to see if comment  and rewind file pointer to beginning of line*/
        rc = fscanf(config_fp, "%c", &new_char);
        if (rc == EOF) break;
        rc = ungetc(new_char, config_fp); //put checked char back to read line

        char *line = NULL, *remaining_line = NULL; //pointer to insert read line
        size_t line_len = 0, rem_line_len = 0; //size of read data

        if (new_char == '#' || new_char == '\n' || new_char == ' ') { //This is a commented line, don't read data
            rc = getline(&line, &line_len, config_fp);
        } else { //Uncommented line, value into correct place in array
            string_len = getdelim(&line, &line_len, (int) '=', config_fp); //read until "=" is reached 
            rc = fscanf(config_fp, "%i", &next_value); //read configuration value
            rc = getline(&remaining_line, &rem_line_len, config_fp); //read remainder of line

            /*record configurations to output*/
            char* confString = (char *) malloc(200 * sizeof (char));
            sprintf(confString, "%s%d\n", line, next_value);
            record(confString);
            free(confString);

            line[string_len - 1] = '\0'; //strip trailing "=" sign

            node_t * np = lookup(config_hash_table, line, config_size); //Lookup corresponding string in hash table
            if (np == NULL) {
                record(concat(3, "Bad configuration string:", line, "\n"));
            } else {
                config_values[*((int*) np->def)] = next_value; //enter values into array of configurations
            }
        }
    }
}


