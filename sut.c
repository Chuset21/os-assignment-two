#include <pthread.h>
#include <ucontext.h>
#include <unistd.h>
#include "sut.h"
#include "queue.h"

pthread_t *c_exec;
pthread_t *i_exec;
struct queue q;
ucontext_t *c_exec_execute_context, *c_exec_task_context;

#define STACK_SIZE (1024*1024)

_Noreturn void *c_exec_execute(__attribute__((unused)) void *arg) {
    while (true) {
        const struct queue_entry *const pop = queue_pop_head(&q);
        if (pop == NULL) {
            usleep(100);
        } else {
            const sut_task_f task = (sut_task_f) pop;
            makecontext(c_exec_task_context, task, 0);
            swapcontext(c_exec_execute_context, c_exec_task_context);
        }
    }
}

void sut_init() {
    q = queue_create();
    queue_init(&q);
    c_exec_execute_context = (ucontext_t *) malloc(sizeof(ucontext_t));
    c_exec_task_context = (ucontext_t *) malloc(sizeof(ucontext_t));

    char *const c_exec_s = (char *) malloc(sizeof(char) * (STACK_SIZE));
    c_exec_task_context->uc_stack.ss_sp = c_exec_s;
    c_exec_task_context->uc_stack.ss_size = sizeof(char) * (STACK_SIZE);
    c_exec_task_context->uc_stack.ss_flags = 0;
    c_exec_task_context->uc_link = c_exec_execute_context;

    c_exec = (pthread_t *) malloc(sizeof(pthread_t));
    i_exec = (pthread_t *) malloc(sizeof(pthread_t));

    pthread_create(c_exec, NULL, c_exec_execute, NULL);
}

bool sut_create(sut_task_f fn) {
    struct queue_entry *const node = queue_new_node(fn);
    if (node == NULL) {
        return false;
    }

    queue_insert_tail(&q, node);

    return true;
}

void sut_yield() {

}

void sut_exit() {

}

int sut_open(char *file_name) {
    return -1;
}

void sut_write(int fd, char *buf, int size) {

}

void sut_close(int fd) {

}

char *sut_read(int fd, char *buf, int size) {
    return NULL;
}

void sut_shutdown() {
    pthread_join(*c_exec, 0);
    free(c_exec);
    pthread_join(*i_exec, 0);
    free(i_exec);
}