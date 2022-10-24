#include <pthread.h>
#include <ucontext.h>
#include <unistd.h>
#include "sut.h"
#include "queue.h"

pthread_t *c_exec, *i_exec;
struct queue q;
ucontext_t *c_exec_context;

#define STACK_SIZE (1024*1024)

_Noreturn void *c_exec_execute(__attribute__((unused)) void *arg) {
    while (true) {
        const struct queue_entry *const pop = queue_pop_head(&q);
        if (pop == NULL) {
            usleep(100);
        } else {
            const ucontext_t *const ucontext = (ucontext_t *) pop;
            swapcontext(c_exec_context, ucontext);
        }
    }
}

void sut_init() {
    q = queue_create();
    queue_init(&q);
    c_exec_context = (ucontext_t *) malloc(sizeof(ucontext_t));
    // unnecessary
    getcontext(c_exec_context);

    c_exec = (pthread_t *) malloc(sizeof(pthread_t));
    i_exec = (pthread_t *) malloc(sizeof(pthread_t));

    pthread_create(c_exec, NULL, c_exec_execute, NULL);
}

bool add_context_to_queue(ucontext_t *const ucontext) {
    struct queue_entry *const node = queue_new_node(ucontext);
    if (node == NULL) {
        return false;
    }

    queue_insert_tail(&q, node);

    return true;
}

bool sut_create(sut_task_f fn) {
    ucontext_t *const ucontext = (ucontext_t *) malloc(sizeof(ucontext_t));
    if (ucontext == NULL) {
        return false;
    }

    if (getcontext(ucontext) < 0) {
        return false;
    }

    char *const uc_s = (char *) malloc(sizeof(char) * (STACK_SIZE));
    if (uc_s == NULL) {
        return false;
    }

    ucontext->uc_stack.ss_sp = uc_s;
    ucontext->uc_stack.ss_size = sizeof(char) * (STACK_SIZE);
    ucontext->uc_stack.ss_flags = 0;
    ucontext->uc_link = c_exec_context;
    makecontext(ucontext, fn, 0);

    return add_context_to_queue(ucontext);
}

void sut_yield() {
    ucontext_t *const ucontext = (ucontext_t *) malloc(sizeof(ucontext_t));
    add_context_to_queue(ucontext);
    swapcontext(ucontext, c_exec_context);
}

void sut_exit() {
    setcontext(c_exec_context);
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
    pthread_join(*c_exec, NULL);
    free(c_exec);
    pthread_join(*i_exec, NULL);
    free(i_exec);
}