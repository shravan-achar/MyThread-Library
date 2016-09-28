#ifndef THREAD_H
#define THREAD_H

#include <ucontext.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#include "mythread.h"


#define save(ctx) \
    (void) getcontext (&((ctx)->uc))
#define restore(ctx) \
    (void) setcontext (&((ctx)->uc))
#define switch_context(ctx_old, ctx_new) \
    (void) swapcontext (&((ctx_old)->uc), &((ctx_new)->uc))

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define STACK_SIZE 1024*8

#define TRUE 1
#define FALSE 0


enum status {TERMINATED = -1, RUN = 0, READY = 1, BLOCKED = 2};
struct _node;

typedef struct _ctx_t {
	ucontext_t uc;
      
        struct _node * puc;  /*Queue node containing Parent context*/
        enum status st;
        int num_child;  /* For JoinAll op */
        int is_par_waiting; /* For Join op */

} ctx_t;

typedef struct _node {
    ctx_t * uc;
    struct _node *prev;
} node;

typedef struct _queue {
    node *head;
    node *tail;
    int size;
} queue;

queue *build_queue();
void delete_queue(queue *q);
int enqueue(queue *q, node *item);
node *dequeue(queue *q);
int isEmpty(queue* q);

queue * blockedq;
queue * readyq;
ctx_t * running;
node * running_node;
ctx_t * grand_daddy;
ctx_t * garbage_collector;

#endif
