//Code template to showcase how to use contexts and their different methods 

/* 
    Four functions: 
    1) getcontext
    2) makecontext
    3) setcontext 
    4) swapcontext
*/

#include<stdio.h>
#include<stdlib.h>

//The header file that contains all stuff related to user contexts 
#include<ucontext.h>

int x = 0;

//Beware, if you get segfaults, it is most probably because of the stack size not being enough for the context 
#define STACK_SIZE 1024*1024

//The user context variables 
ucontext_t *m, *uc1;


void sample_func() {
    
    printf("Hi thee, I have switched to this context \n");
    x++;
    setcontext(m);
}

int main() {

    //Allocate memory for context variables
    m = (ucontext_t *)malloc(sizeof(ucontext_t));
    uc1 = (ucontext_t *)malloc(sizeof(ucontext_t));
   
    //Now to initialize the context we want to set for a function with some pre-defined values: use getcontext
    if( getcontext(uc1) == -1)  {
        printf("error\n");
    }

    //getcontext(uc1) --> saves state of the execution here 
    
    //TO MAKE OUR OWN CONTEXT-------------------------------------------------------------------
    //Each context needs to be specifically given some new memory space for storing an execution stack 
    char *uc1_s;
    uc1_s = (char *)malloc(sizeof(char) * (STACK_SIZE));
    
    //Now for each context, set the starting stack address 
    uc1->uc_stack.ss_sp = uc1_s;
    //Set also the size 
    uc1->uc_stack.ss_size = sizeof(char) * (STACK_SIZE);
    //Set the stack size flags to 0 - You do not need to know what this does.
    uc1->uc_stack.ss_flags = 0;

    //We could also specify which context must execute once the current context terminates
    uc1->uc_link = 0;
    //Set uc_link to m
    //Do not call setcontext(m) in sample_func 
    //now run the code below and see what happens

    
    //Now make your contexts 
    makecontext(uc1, sample_func, 0);
    //int x = 0;
    //makecontext(uc1, sample_func, 2, &x, &c...)

    //---------------------------------------------------------------------------------------------

    //USING OUR MADE CONTEXTS IN DIFFERENT WAYS 

    //--------------------------------------------------------------------------------------------

    //How getcontext and setcontext work 
    //getcontext(m); //save state here
    /*
    if(!x) {

        printf("%d\n", x);
        //Run user context uc1
        setcontext(uc1); 
        //Whichever thread is running the current thread (here it is main), this thread will run the user-thread defined by uc1

    }
    else {
        printf("We have run the user context uc1\n");
    }
    */

    //------------------------------------------------------------------------------------------------

    //How swapcontext works 

    //Save the current context (or state in m)
    //set the current context to uc1
    swapcontext(m, uc1);

    printf("Came back from user context invoking sample_func \n");
    

    return 0;

}