//Example to showcase use to generic queue

/*
    Five functions:
    1) queue_create
    2) queue_init
    3) queue_new_node
    4) queue_insert_tail
    5) queue_pop_head
*/

#include "../../queue.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>

struct student {
    int no;
    char name[10];
};

int main() {
    
    int x = 1;
    int y = 2;
    int z = 3;

    //Queue --->
        // Head -------------- Tail
    
    /* Note: it is important that you do NOT move/copy this value
     * after the list has been initialized. Always refer to it by
     * reference/through a pointer indirection */
    //Allocate memory for the queue q
    struct queue q = queue_create();
    
    //Initialize queue
    queue_init(&q);
    
    //Inserting your elements into the queue

    //Create a node that represents your element
    struct queue_entry *node = queue_new_node(&x);
    //Now insert the node into the queue
    queue_insert_tail(&q, node);
    
    struct queue_entry *node2 = queue_new_node(&y);
    queue_insert_tail(&q, node2);
    
    struct queue_entry *node3 = queue_new_node(&z);
    queue_insert_tail(&q, node3);

    //Popping elements from the queue

    //Create a queue entry note to store the element popped by the queue
    //pop from the head given how we inserted elements at the tail => FIFO / FCFS
    struct queue_entry *ptr = queue_pop_head(&q);
    //Now reference the content by accessing "data"
    //Make sure to convert it to the appropriate type
   
    //while(ptr) {
	printf("popped %d\n", *(int*)(ptr->data));

	queue_insert_tail(&q, ptr);
	usleep(1000 * 1000);
	
	ptr = queue_pop_head(&q);
    //}

    queue_pop_head(&q); //pop 2
    queue_pop_head(&q); //pop 3

    //Now queue should be empty
    // queue_pop_head(&q) --> This will return NULL


    //Note that the queue is generic - you can store any data in it
    struct student s1, s2; 

    s1.no = 10;
    strcpy(s1.name, "Akshay");

    s2.no = 12;
    strcpy(s2.name, "Ash");

    struct queue_entry *std_node1 = queue_new_node(&s1);
    queue_insert_tail(&q, std_node1);

    struct queue_entry *std_node2 = queue_new_node(&s2);
    queue_insert_tail(&q, std_node2);

    //Now pop from this queue the structs inserted
    struct queue_entry *pop = queue_pop_head(&q);
    printf("Student name = %s and Roll = %d\n", ((struct student *)(pop->data))->name, ((struct student *)(pop->data))->no);

    struct queue_entry *pop1 = queue_pop_head(&q);
    printf("Student name = %s and Roll = %d\n", ((struct student *)(pop1->data))->name, ((struct student *)(pop1->data))->no);


    return 0;
}
