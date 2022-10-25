#include <pthread.h>
#include <ucontext.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include "sut.h"
#include "queue.h"

pthread_t *c_exec, *i_exec;
pthread_mutex_t exec_lock, io_lock, sem_lock;
struct queue exec_queue, io_queue;
ucontext_t *c_exec_context, *i_exec_context;
int sem;
bool is_doing_work;

#define STACK_SIZE (1024*1024)

void *c_exec_execute(__attribute__((unused)) void *arg) {
    bool start = true;
    while (true) {
        pthread_mutex_lock(&exec_lock);
        const struct queue_entry *const pop = queue_pop_head(&exec_queue);
        pthread_mutex_unlock(&exec_lock);
        if (pop == NULL) {
            // start, is_doing_work and sem are all used to see if there is no more work left.
            if (!is_doing_work && !start && sem == 0) {
                return NULL;
            }
            nanosleep((const struct timespec[]) {{0, 100000L}}, NULL);
        } else {
            start = false;
            const ucontext_t *const ucontext = (ucontext_t *) (pop->data);
            swapcontext(c_exec_context, ucontext);
        }
    }
}

void *i_exec_execute(__attribute__((unused)) void *arg) {
    while (c_exec) {
        pthread_mutex_lock(&io_lock);
        const struct queue_entry *const pop = queue_pop_head(&io_queue);
        pthread_mutex_unlock(&io_lock);
        if (pop == NULL) {
            is_doing_work = false;
            nanosleep((const struct timespec[]) {{0, 100000L}}, NULL);
        } else {
            is_doing_work = true;
            const ucontext_t *const ucontext = (ucontext_t *) (pop->data);
            swapcontext(i_exec_context, ucontext);
        }
    }
    return NULL;
}

void sut_init() {
    sem = 0;
    is_doing_work = true;
    pthread_mutex_init(&sem_lock, PTHREAD_MUTEX_DEFAULT);
    pthread_mutex_init(&exec_lock, PTHREAD_MUTEX_DEFAULT);
    pthread_mutex_init(&io_lock, PTHREAD_MUTEX_DEFAULT);

    exec_queue = queue_create();
    queue_init(&exec_queue);
    io_queue = queue_create();
    queue_init(&io_queue);

    c_exec_context = (ucontext_t *) malloc(sizeof(ucontext_t));
    i_exec_context = (ucontext_t *) malloc(sizeof(ucontext_t));

    c_exec = (pthread_t *) malloc(sizeof(pthread_t));
    i_exec = (pthread_t *) malloc(sizeof(pthread_t));

    pthread_create(c_exec, NULL, c_exec_execute, NULL);
    pthread_create(i_exec, NULL, i_exec_execute, NULL);
}

void insert_node_in_exec_queue(struct queue_entry *const node) {
    pthread_mutex_lock(&exec_lock);
    queue_insert_tail(&exec_queue, node);
    pthread_mutex_unlock(&exec_lock);
}

bool add_context_to_queue(ucontext_t *const ucontext) {
    struct queue_entry *const node = queue_new_node(ucontext);
    if (node == NULL) {
        return false;
    }

    insert_node_in_exec_queue(node);

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

struct queue_entry *make_empty_context_and_add_to_io() {
    pthread_mutex_lock(&sem_lock);
    sem++;
    pthread_mutex_unlock(&sem_lock);

    ucontext_t *const ucontext = (ucontext_t *) malloc(sizeof(ucontext_t));

    struct queue_entry *const node = queue_new_node(ucontext);

    pthread_mutex_lock(&io_lock);
    queue_insert_tail(&io_queue, node);
    pthread_mutex_unlock(&io_lock);

    return node;
}

void decrement_sem() {
    pthread_mutex_lock(&sem_lock);
    sem--;
    pthread_mutex_unlock(&sem_lock);
}

int sut_open(char *file_name) {
    struct queue_entry *const node = make_empty_context_and_add_to_io();
    const ucontext_t *const ucontext = (ucontext_t *) node->data;

    swapcontext(ucontext, c_exec_context);

    const int result = open(file_name, O_RDWR | O_CREAT | O_APPEND, 0600);

    insert_node_in_exec_queue(node);

    swapcontext(ucontext, i_exec_context);

    decrement_sem();

    return result;
}

void sut_write(int fd, char *buf, int size) {
    struct queue_entry *const node = make_empty_context_and_add_to_io();
    const ucontext_t *const ucontext = (ucontext_t *) node->data;

    swapcontext(ucontext, c_exec_context);

    write(fd, buf, size);

    insert_node_in_exec_queue(node);

    swapcontext(ucontext, i_exec_context);

    decrement_sem();
}

void sut_close(int fd) {
    struct queue_entry *const node = make_empty_context_and_add_to_io();
    const ucontext_t *const ucontext = (ucontext_t *) node->data;

    swapcontext(ucontext, c_exec_context);

    close(fd);

    insert_node_in_exec_queue(node);

    swapcontext(ucontext, i_exec_context);

    decrement_sem();
}

char *sut_read(int fd, char *buf, int size) {
    struct queue_entry *const node = make_empty_context_and_add_to_io();
    const ucontext_t *const ucontext = (ucontext_t *) node->data;

    swapcontext(ucontext, c_exec_context);

    char *const result = read(fd, buf, size) < 0 ? NULL : buf;

    insert_node_in_exec_queue(node);

    swapcontext(ucontext, i_exec_context);

    decrement_sem();

    return result;
}

void sut_shutdown() {
    pthread_join(*c_exec, NULL);
    free(c_exec);
    c_exec = NULL;
    pthread_join(*i_exec, NULL);
    free(i_exec);
    free(c_exec_context);
    free(i_exec_context);
}