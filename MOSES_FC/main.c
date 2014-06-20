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
    
    /*read in configuration values*/
    //read_moses_config();     //not implemented yet

    record("*****************************************************\n");
    record("MOSES FLIGHT SOFTWARE\n");
    record("*****************************************************\n");
    
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
    pthread_create(&threads[hlp_down_thread], &attrs, (void * (*)(void*))hlp_down, NULL); //shouldn't be enabled for debug
    pthread_create(&threads[hlp_control_thread], &attrs, (void * (*)(void*))hlp_control, NULL);
    //pthread_create(&threads[hlp_hk_thread], &attrs, (void * (*)(void*))hlp_housekeeping, NULL);       
    pthread_create(&threads[sci_timeline_thread], &attrs, (void * (*)(void*))science_timeline, NULL);
    
    
}

void join_threads() {
    void * returns;

    int i;
    for (i = 0; i < NUM_THREADS; i++) {
        //pthread_join(threads[i], &returns);
        pthread_cancel(threads[i]);
    }
}

void init_quit_signal_handler() {
    sigfillset(&oldmask); //save the old mask
    sigemptyset(&mask); //create a blank new mask
    sigaddset(&mask, SIGINT); //add SIGINT (^C) to mask
    quit_action.sa_handler = quit_signal;
    quit_action.sa_mask = oldmask;
    quit_action.sa_flags = 0;

    sigaction(SIGINT, &quit_action, NULL);
}

void config_strings_init() {
    config_strings[hlp_control_thread] = "HLP_CONTROL_THREAD";
    config_strings[hlp_down_thread] = "HLP_DOWN_THREAD";
    config_strings[hlp_hk_thread] = "HLP_HK_THREAD";
    config_strings[sci_timeline_thread] = "SCIENCE_THREAD";
    config_strings[telem_thread] = "TELEM_THREAD";

    /*must offset by NUM_THREADS to be enumerated correctly*/
    config_strings[NUM_THREADS + hlp_up_interface] = "HLP_UP";
    config_strings[NUM_THREADS + hlp_down_interface] = "HLP_DOWN";
    config_strings[NUM_THREADS + roe_interface] = "ROE";
    config_strings[NUM_THREADS + synclink_interface] = "SYNCLINK";
}

/*read in configuration file for thread and I/O attributes*/
void read_moses_config(){
    /*read configuration file*/
    FILE * config_fp = fopen(config_path, "r");
    
    int rc = 0;
    while (rc != EOF) {
        char new_char;
        int next_value;
        /*check to see if comment and rewind file pointer to beginning of line*/
        rc = fscanf(config_fp, "%c", &new_char);
        if(rc == EOF) break;
        rc = ungetc(new_char, config_fp);
        
        
        
        char *line = NULL, *remaining_line = NULL; //pointer to insert read line
        size_t line_len = 0; //size of read data

        if (new_char == '#' || new_char == '\n' || new_char == ' ') { //This is a commented line, don't read data
            rc = getline(&line, &line_len, config_fp);
        } else {        //Uncommented line, value into correct place in array
            rc = getdelim(&line, &line_len, (int)'=', config_fp);
            rc = fscanf(config_fp, "%i", &next_value);
            rc = getline(&remaining_line, &line_len, config_fp);
            printf("%s%d\n", line, next_value);
        }
    }
}

