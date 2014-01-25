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
    
    init_signal_handler();
    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    while(ts_alive){
        sigsuspend(&oldmask);   // wait here until the program is killed
    }
    sigprocmask(SIG_UNBLOCK, &mask, &oldmask);
    
    /*SIGINT caught, ending program*/
    join_threads();
    
    return (EXIT_SUCCESS);
}

void quit_signal(int sig){
    ts_alive = 0;
}

/*this method takes a function pointer and starts it as a new thread*/
void start_threads(){
    pthread_create(&threads[hkupThread], NULL, (void * (*)(void*))HkupThread(), NULL);
    pthread_create(&threads[controlThread], NULL, (void * (*)(void*))ControlThread(), NULL);

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
