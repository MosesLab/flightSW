/* 
 * File:   main.c
 * Author: Roy Smart
 *
 * Created on January 22, 2014, 5:21 PM
 */

#include "main.h"

volatile sig_atomic_t ts_alive = 1; //variable modified by signal handler, setting this to false will end the threads
volatile sig_atomic_t img_wr_alive = 1;

unsigned int num_threads;
thread_func tfuncs[NUM_RROBIN + NUM_FIFO];
void * targs[NUM_RROBIN + NUM_FIFO];
pthread_t threads[NUM_RROBIN + NUM_FIFO]; //array of running threads

pid_t main_pid;
int quit_sig = 0;
struct sigaction quit_action, init_action, start_action, reset_action; //action to be taken when ^C (SIGINT) is entered
sigset_t mask, oldmask; //masks for SIGINT signal

int config_values[NUM_RROBIN + NUM_FIFO + NUM_IO]; //array of values holding moses program configurations  
node_t** config_hash_table;

LockingQueue lqueue[QUEUE_NUM];

/**
 * Main program entry point. Loop to see if restart is necessary
 * 
 * @return 0 upon successful exit
 */
int main(int argc, char **argv) {
    num_threads = NUM_RROBIN + NUM_FIFO;
    ops.sleep = 0;
    init_logger();

    /*initialize virtual shell*/
    vshell_pid = vshell_init();


    while (moses());

    return 0;

}

/**
 * Call this method for program start. Allows restarting the program.
 * Reads configuration file to configure program.
 * Starts flight software threads based off of configs.
 * 
 * @return 
 */
int moses() {
    char msg[255];

    record("*****************************************************\n");
    record("*            MOSES FLIGHT SOFTWARE                  *\n");
    record("*****************************************************\n");

    printf("*****************************************************\n");
    printf("*            MOSES FLIGHT SOFTWARE                  *\n");
    printf("*****************************************************\n");

    /*init configuration stings*/
    main_init();

    /*read in configuration values*/
    read_moses_config();

    /*record this thread's PID*/
    main_pid = getpid();

    /*Use signals to inform the program to quit*/
    init_quit_signal_handler();

    /*start threads indicated by configuration file*/
    start_threads();

    /*Upon program termination (^c) attempt to join the threads*/
    pthread_sigmask(SIG_BLOCK, &mask, &oldmask);
    sigwait(&mask, &quit_sig);
    pthread_sigmask(SIG_UNBLOCK, &mask, &oldmask);

    record("exited wait\n");

    /*SIGINT or SIGHUP caught, ending program*/
    join_threads();

    /*clean up memory and open devices*/
    cleanup();

    sprintf(msg, "quit_sig: %d\n", quit_sig);
    record(msg);

    /* if SIGUSR2, a reset command was received. */
    if (quit_sig == SIGUSR2) {

        sleep(2);
        record("Flight software rebooting...\n");

        return TRUE;
    }

    record("FLIGHT SOFTWARE EXITED\n\n\n");

    return FALSE;
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
    void * returns;
    //   char msg[256];
    /*sleep to give threads a chance to clean up a little*/
    sleep(1);

    /*Check to see if this function was called because of sleep T/U */
    if (ops.sleep) {
        record("in ops.sleep\n");

        /* Turn off subsytems*/
        set_power(tcs1, OFF);
        set_power(tcs2, OFF);
        set_power(tcs3, OFF);
        set_power(shutter, OFF);
        set_power(roe, OFF);
        set_power(halpha, OFF);
        set_power(premod, OFF);
        set_power(ps5v, OFF);
        set_power(psdual12v, OFF);

        record("All Subsystems turned off\n");



        img_wr_alive = 0;

        /* Wait until image writer is done saving images*/
        pthread_cond_broadcast(&lqueue[fpga_image].cond);        
        pthread_join(threads[image_writer_thread], &returns);

        set_power(11, ON); // hit cc_power
        sleep(1);

        ts_alive = 0;
        
        pthread_cond_broadcast(&lqueue[telem_image].cond);
        pthread_cond_broadcast(&lqueue[sequence].cond);
        pthread_cond_broadcast(&lqueue[scit_image].cond);

        /*Gracefully close down sci_ti(making sure the shutter is closed)*/
        pthread_join(threads[sci_timeline_thread], NULL);

        /* Cancel the threads that dont need to be joined*/
        int i;
        for (i = 0; i < num_threads; i++) {
            if (threads[i] != 0) {
                if (i != sci_timeline_thread && i != image_writer_thread) {
                    pthread_cancel(threads[i]);
                }
            }
        }

        /* Goodnight MOSES */
        execlp("shutdown", "shutdown", "-h", "now", (char *) 0);

        return;

    } // end if sleep

    int i;
    for (i = 0; i < num_threads; i++) {
        if (threads[i] != 0) {
            pthread_cancel(threads[i]);
        }
    }

    kill(vshell_pid, SIGKILL);

    record("killed bash\n");

}

/*initialize signal handler to listen for quit signal*/
void init_quit_signal_handler() {
    sigfillset(&oldmask); //save the old mask
    sigemptyset(&mask); //create a blank new mask

    /*quit signal handling*/
    sigaddset(&mask, SIGINT); //add SIGINT (^C) to mask
    quit_action.sa_handler = quit_signal;
    quit_action.sa_mask = oldmask;
    quit_action.sa_flags = SA_RESTART; //SA_RESTART;
    sigaction(SIGINT, &quit_action, NULL);

    /*experiment data start signal handling*/
    start_action.sa_handler = start_signal;
    start_action.sa_mask = oldmask;
    start_action.sa_flags = 0;
    sigaction(SIGUSR1, &start_action, NULL);

    /*reset flight software signal handling*/
    sigaddset(&mask, SIGUSR2);
    start_action.sa_handler = reset_signal;
    start_action.sa_mask = oldmask;
    start_action.sa_flags = 0;
    sigaction(SIGUSR2, &reset_action, NULL);

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

/*Signal flight software to reset*/
void reset_signal(int sig) {

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
    config_strings[image_sim_interface] = IMAGE_SIM_CONF;
    config_strings[synclink_interface] = SYNCLINK_CONF;

    /*initialize memory for configuration hash table*/
    if ((config_hash_table = calloc(config_size, sizeof (node_t))) == NULL) {
        record("calloc failed to allocate hash table\n");
    }

    /*fill hash table with array of strings matching indices for configuration values*/
    uint i;
    for (i = 0; i < config_size; i++) {
        int * int_def = malloc(sizeof (int));
        *int_def = i;

        /*insert node into hash table*/
        installNode(config_hash_table, config_strings[i], int_def, config_size);
    }

    /*fill array of function pointers for pthread call*/
    tfuncs[hlp_control_thread] = hlp_control;
    tfuncs[hlp_down_thread] = hlp_down;
    tfuncs[gpio_control_thread] = gpio_control;
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
    char * config_path = PATH "moses.conf";


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

void cleanup() {
    record("Release page-locked contiguous buffer\n");
    dmaClearBlock();

    record("Close DMA channel\n");
    dmaClose();
    close_fpga();

    /*grab time for backup file naming*/
    time_t now;
    time(&now);
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday(&tv, &tz);
    tm = localtime(&tv.tv_sec);

    char cmd[255];
    sprintf(cmd, "cat %s >> /moses/log_backups/moseslog_%02d-%02d-%04d", LOG_PATH, tm->tm_mon + 1, tm->tm_mday, tm->tm_year + 1900);

    record("Saving backup log to filesystem\n");
    if (system(cmd) == -1) {
        record("Saving backup log failed!\n");
    }

    free(config_hash_table);

}
