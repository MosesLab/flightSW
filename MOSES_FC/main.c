/* 
 * File:   main.c
 * Author: byrdie
 *
 * Created on January 22, 2014, 5:21 PM
 */

#include "main.h"
/*
 * 
 */
int main(void) {
    start_threads();
    
    /*Upon program termiation (^c) attempt to join the threads*/
    init_signal_handler();
    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    while(ts_alive){
        sigsuspend(&oldmask);   // wait here until the program is killed
    }
    sigprocmask(SIG_UNBLOCK, &mask, &oldmask);
    
    /*SIGINT caught, ending program*/
    join_threads();
    
    return 0;
}

/*signal all threads to exit*/
void quit_signal(int sig){
    ts_alive = 0;
}

/*this method takes a function pointer and starts it as a new thread*/
void start_threads(){
    
    pthread_attr_init(&attrs);
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);
    pthread_create(&threads[hlp_down_thread], &attrs, (void * (*)(void*))hlp_down, NULL);
    pthread_create(&threads[hlp_control_thread], &attrs, (void * (*)(void*))hlp_control, NULL);

}

void join_threads(){
    void * returns;
    
    int i;
    for(i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], &returns);
    }
}


void init_signal_handler(){
    sigfillset(&oldmask);       //save the old mask
    sigemptyset(&mask);         //create a blank new mask
    sigaddset(&mask, SIGINT);   //add SIGINT (^C) to mask
    quit_action.sa_handler = quit_signal;
    quit_action.sa_mask = oldmask;
    quit_action.sa_flags = 0;
    
    sigaction(SIGINT, &quit_action, NULL);
}
