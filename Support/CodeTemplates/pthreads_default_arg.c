//Code template to show how to pass different default arguments to pthread routines

/*
    Three functions 
    1) pthread_create
    2) pthread_join 
    3) pthread_exit
*/

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

pthread_t *t; 

//Integer argument
void *sample_int(void *int_arg) {

    //int *x = 100;
    // (int *)int_arg --> x 
    // *x
    printf("The integer argument passed is %d \n", *(int *)int_arg);

    pthread_exit(NULL);

}

//Character argument 
void *sample_char(void *char_arg) {

    printf("The char argument passed is %c \n", *(char *)char_arg);

    pthread_exit(NULL);

}

//String argument passings? 
void *sample_string(void *string_arg) {


    printf("The char argument passed is %s \n", (char *)string_arg);

    pthread_exit(NULL);

}


//Argument as an array of integers
void *sample_arr(void *int_arr_arg) {

    //Be mindful of the type conversion and the precedence (note the brackets)
    for(int i = 0; i < 10; i++) {
        printf("Array element %d = %d\n", i, ((int *)int_arr_arg)[i]);
    }

    pthread_exit(NULL);

}

int main() {

    int n = 12204;
    char c = 'a';
    char s[10] = "abcdefghij";
    //char *s;
    //s = (char*)(malloc(sizeof(char) * 10))
    // Code to write "abcdefghij" to s[0],s[1],...

    int a[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    t = (pthread_t *)malloc(sizeof(pthread_t));

    //Creating a thread passing an integer argument to the routine
    pthread_create(t, NULL, sample_int, (void *)&n);
    pthread_join(*t, NULL);

    //Creating a thread passing a char argument
    pthread_create(t, NULL, sample_char, (void *)&c);
    pthread_join(*t, NULL);

    //Creating a thread passing a string (char array) argument
    pthread_create(t, NULL, sample_string, (void *)s);
    pthread_join(*t, NULL);

    //Creating a thread passing an integer array as an argument
    pthread_create(t, NULL, sample_arr, (void *)a);
    pthread_join(*t, NULL);

    return 0;

}