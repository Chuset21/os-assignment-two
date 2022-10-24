//Sample code to showcase pthread usage and showcasing shared memory

//1. Example showcasing shared memory accesses by multiple threads.
//2. Example showing incorrect write/read values if shared memory accesses are not properly syncrhonized
//3. Example showcasing how using mutexes, memory accesses can be properly synchronized

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

//Mutex
    //It is a MUTual EXclusion-device
    //Typically useful for protecting shared data from concurrent modifications. 
    //Has mainly two possible states:
        //1. Locked - Owned by some thread
        //2. Unlocked - Not owned by any thread
    //The main property of a mutex is "No two threads can own the same mutex at once".
    //A thread trying to Lock (own) a mutex that is already locked will be 
        // Suspended until the owning thread unlocks it first.

/*
    Three functions 
    1) pthread_mutex_init
    2) pthread_mutex_lock
    3) pthread_mutex_unlock
*/


pthread_mutex_t lck;

int counter = 10000;

void *thread_one_dec_cntr() {

    for(int i = 0; i < 5000; i++) 
        counter --;

}

void *thread_two_dec_cntr() {

    for(int i = 0; i < 5000; i++) 
        counter --;

}


void * thread_one_mutex_dec() {
    for(int i = 0; i < 5000; i++) {
           
        pthread_mutex_lock(&lck);
        //Critical section
        counter = counter - 1;
        pthread_mutex_unlock(&lck);
      
    }

    pthread_exit(NULL);

}


void * thread_two_mutex_dec() {
    for(int i = 0; i < 5000; i++) {

        pthread_mutex_lock(&lck); //waiting here / put to sleep --- suspended
        counter = counter - 1;
        pthread_mutex_unlock(&lck);
      
    }

    pthread_exit(NULL);

}

void main() {

    //Create a pthread variable to define a thread
    pthread_t *t1, *t2;

    //Initialize a mutex to default kind (fast)
        //Other kinds:
            //1.Error 
            //2.Recursive

    pthread_mutex_init(&lck, PTHREAD_MUTEX_DEFAULT);

    //Allocate memory for thread variable
    t1 = (pthread_t*)malloc(sizeof(pthread_t));
    t2 = (pthread_t*)malloc(sizeof(pthread_t));


    //Creating threads to work on routines that modify counter without mutexes
    //pthread_create(t1, NULL, thread_one_dec_cntr, NULL);
    //pthread_create(t2, NULL, thread_two_dec_cntr, NULL);

    
    //Creating threads to work on routines that modify counter with owning a mutex 
    pthread_create(t1, NULL, thread_one_mutex_dec, NULL);
    pthread_create(t2, NULL, thread_two_mutex_dec, NULL);

    pthread_join(*t1, NULL);
    pthread_join(*t2, NULL);
    
    printf("Counter value is %d \n", counter);


}