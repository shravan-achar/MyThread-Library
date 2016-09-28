#include <stdlib.h>
#include <stdio.h>
#include "thread.h"

/* Source: https://gist.github.com/ArnonEilat/4471278 */

queue *build_queue() {
    queue *q =  malloc(sizeof (queue));
    if (q == NULL) handle_error ("queue malloc");
    
    q->size = 0;
    q->head = NULL;
    q->tail = NULL;

    return q;
}

void delete_queue(queue *queue) {
    node *p;
    while (!isEmpty(queue)) {
        p = dequeue(queue);
        free(p);
    }
    free(queue);
}

int enqueue(queue *q, node *item) {
    /* Bad parameter */
    if ((q == NULL) || (item == NULL)) {
        return -1;
    }
    /*the queue is empty*/
    item->prev = NULL;
    if (q->size == 0) {
        q->head = item;
        q->tail = item;

    } else {
        /*adding item to the end of the queue*/
        q->tail->prev = item;
        q->tail = item;
    }
    q->size++;
    return TRUE;
}

node * dequeue(queue *q) {
    /*the queue is empty or bad param*/
    node *item;
    if (isEmpty(q))
        return NULL;
    item = q->head;
    q->head = (q->head)->prev;
    q->size--;
    return item;
}

int isEmpty(queue* q) {
    if (q == NULL) {
        return -1;
    }
    if (q->size == 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}
/*
int main() {
    int i;
    queue *q = build_queue();
    node *p;

    for (i = 0; i < 9; i++) {
        p = malloc(sizeof (node));
        enqueue(q, p);
    }

    while (!isEmpty(q)) {
        p = dequeue(q);
        free(p);
    }
    delete_queue(q);
    return (EXIT_SUCCESS);
}*/
