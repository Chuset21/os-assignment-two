
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<pthread.h>

pthread_t *t;

struct student {
    int roll;
    char name[20];
    int class_no;
};



void *print_student(void *arg) {

    //Note how the type conversion is done here
    struct student *p = (struct student *)arg;

    printf("Roll number = %d \n", p->roll);
    printf("Name = %s\n", p->name);
    printf("Class number = %d \n", p->class_no);

    pthread_exit(NULL);
}


int main() {

    t = (pthread_t *)malloc(sizeof(pthread_t));

    struct student s1;
    s1.roll = 20;
    strcpy(s1.name, "Akshay G - COMP 310");
    s1.class_no = 310;

    pthread_create(t, NULL, print_student, (void *) &s1);

    pthread_join(*t, NULL);

}