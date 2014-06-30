/* 
 * File:   main.c
 * Author: Roy Smart
 *
 * Created on January 22, 2014, 5:21 PM
 */

#include "main.h"

/*
 * 
 */
int main(void) {

    /*init configuration stings*/
    config_strings_init();

    record("*****************************************************\n");
    record("MOSES FLIGHT SOFTWARE\n");
    record("*****************************************************\n");

    /*read in configuration values*/
    read_moses_config();


    /*start threads indicated by configuration file*/
    start_threads();

    /*Upon program termiation (^c) attempt to join the threads*/
    init_quit_signal_handler();
    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    while (ts_alive) {
        sigsuspend(&oldmask); // wait here until the program is killed
    }
    sigprocmask(SIG_UNBLOCK, &mask, &oldmask);

    /*SIGINT caught, ending program*/
    join_threads();

    record("FLIGHT SOFTWARE EXITED\n\n\n");

    return 0;
}

/*signal all threads to exit*/
void quit_signal(int sig) {
    ts_alive = 0;
}

/*this method takes a function pointer and starts it as a new thread*/
void start_threads() {


    pthread_attr_init(&attrs);
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);
    if (config_values[hlp_down_thread] == 1)
        pthread_create(&threads[hlp_down_thread], &attrs, (void * (*)(void *))hlp_down, &config_values[NUM_THREADS + hlp_down_interface]);
    if (config_values[hlp_control_thread] == 1)
        pthread_create(&threads[hlp_control_thread], &attrs, (void * (*)(void *))hlp_control, &config_values[NUM_THREADS + hlp_up_interface]);
    if (config_values[hlp_hk_thread] == 1)
        pthread_create(&threads[hlp_hk_thread], &attrs, (void * (*)(void*))hlp_housekeeping, NULL);
    if (config_values[sci_timeline_thread] == 1)
        pthread_create(&threads[sci_timeline_thread], &attrs, (void * (*)(void*))science_timeline, NULL);
    if (config_values[telem_thread] == 1)
        pthread_create(&threads[telem_thread], &attrs, (void * (*)(void*))telem, NULL);


}

/*more like canceling threads at the moment, not sure if need to clean up properly*/
void join_threads() {
//    void * returns;

    int i;
    for (i = 0; i < NUM_THREADS; i++) {
        if (threads[i] != 0){
//            pthread_join(threads[i], &returns);
        pthread_cancel(threads[i]);
        }
    }
}

/*initialize signal handler to listen for quit signal*/
void init_quit_signal_handler() {
    sigfillset(&oldmask); //save the old mask
    sigemptyset(&mask); //create a blank new mask
    sigaddset(&mask, SIGINT); //add SIGINT (^C) to mask
    quit_action.sa_handler = quit_signal;
    quit_action.sa_mask = oldmask;
    quit_action.sa_flags = 0;

    sigaction(SIGINT, &quit_action, NULL);
}

/*set up hash table with configuration strings to match values in moses.conf*/
void config_strings_init() {
    config_size = NUM_THREADS + NUM_IO;

    /*allocate strings to match with configuration file*/
    config_strings[hlp_control_thread] = CONTROL_CONF;
    config_strings[hlp_down_thread] = DOWN_CONF;
    config_strings[hlp_hk_thread] = HK_CONF;
    config_strings[sci_timeline_thread] = SCIENCE_CONF;
    config_strings[telem_thread] = TELEM_CONF;

    /*must offset by NUM_THREADS to be enumerated correctly*/
    config_strings[NUM_THREADS + hlp_up_interface] = HKUP_CONF;
    config_strings[NUM_THREADS + hlp_down_interface] = HKDOWN_CONF;
    config_strings[NUM_THREADS + roe_interface] = ROE_CONF;
    config_strings[NUM_THREADS + synclink_interface] = SYNCLINK_CONF;

    /*initialize memory for configuration hash table*/
    if ((config_hash_table = calloc(sizeof (node_t) * config_size, 1)) == NULL) {
        record("malloc failed to allocate hash table\n");
    }

    /*fill hash table with array of strings matching indices for configuration values*/
    int i;
    for (i = 0; i < config_size; i++) {
        int * int_def = malloc(sizeof (int));
        *int_def = i;

        /*insert nod into hash table*/
        installNode(config_hash_table, config_strings[i], int_def, config_size);
    }
}

/*read in configuration file for thread and I/O attributes*/
void read_moses_config() {
    record("Reading in configuration file.....");

    /*read configuration file*/
    FILE * config_fp = fopen(config_path, "r");

    int rc = 0;
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


