#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

pthread_t *t; 

//Integer argument
void *sample_int(void *int_arg) {

    printf("The integer argument passed is %d \n", *(int *)int_arg);

    pthread_exit((void **)int_arg);

}

int main() {

    int n = 1028975;

    t = (pthread_t *)malloc(sizeof(pthread_t));

    void **ret_val;

    //Creating a thread passing an integer argument to the routine
    pthread_create(t, NULL, sample_int, (void *)&n);
    pthread_join(*t, ret_val);

    //Please take care of this carefully
    //We need the return value sent from the routine.
    //But now the type is void **
    //So first, we deference it as void * by doing (*ret_val)
    //Now we do the type conversion.
    //void *return_value = *ret_val
    printf("Integer value returned by the thread is %d\n", *(int *)(*ret_val));

    return 0;

}