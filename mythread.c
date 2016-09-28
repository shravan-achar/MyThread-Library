#include "thread.h"
#include "mythread.h"

void MyThreadInitExtra(void(*start_func)(void *), void *args)
{
	ucontext_t * gd, * gc = 0;
        MyThread main = 0;

	
	blockedq = build_queue();
	readyq = build_queue();
	running = 0;

	running_node = malloc(sizeof(node));
	memset(running_node, 0, sizeof(node));

	grand_daddy = malloc(sizeof(ctx_t));
	if (grand_daddy == 0) handle_error("malloc");
	memset(grand_daddy, 0, sizeof(ctx_t));

	getcontext(&(grand_daddy->uc));
	
	garbage_collector = malloc(sizeof(ctx_t));
	if (garbage_collector == 0) handle_error("malloc");
	memset(garbage_collector, 0, sizeof(ctx_t));

	getcontext(&(garbage_collector->uc));
        gc = &(garbage_collector->uc);
	gc->uc_link = 0; /* No successor context. Believe this is necessary for extra credit*/
	gc->uc_stack.ss_sp = malloc (STACK_SIZE);
	if (gc->uc_stack.ss_sp == 0) handle_error("malloc"); 
	gc->uc_stack.ss_size = STACK_SIZE;
	gc->uc_stack.ss_flags = 0;
	makecontext(gc, MyThreadExit, 1, args);

	gd = &(grand_daddy->uc);
        running = grand_daddy;
	running_node->uc = running;
	main = MyThreadCreate(start_func, args);
        MyThreadJoin(main);

}


void MyThreadInit (void(*start_funct)(void *), void *args)
{
	/*Initialize all queues*/
	blockedq = build_queue();
	readyq = build_queue();
	running = 0;

	running_node = malloc(sizeof(node));
	memset(running_node, 0, sizeof(node));

	MyThreadCreate(start_funct, args);
	running_node->uc = running;
	if (!isEmpty(readyq)) {
		running_node = dequeue(readyq);
		running = running_node->uc;
		setcontext(&(running->uc));
	}
}

MyThread MyThreadCreate(void(*start_func)(void *), void *args)
{
	/* Create a new thread and gets queued in the ready queue */
	/*Get current context*/
	ctx_t * current;
	node * n = malloc(sizeof(node));
	if (n == 0) handle_error("queue malloc"); 
	ucontext_t  * cur;
	current = malloc(sizeof(ctx_t));
	if (current == 0) handle_error("ctx malloc");
	memset(current, 0, sizeof(ctx_t));
	getcontext(&(current->uc));
	cur = &(current->uc);

	cur->uc_link = &(garbage_collector->uc);
	cur->uc_stack.ss_sp = malloc(STACK_SIZE);
	if (cur->uc_stack.ss_sp == 0) handle_error("malloc"); 
	cur->uc_stack.ss_size = STACK_SIZE;
	cur->uc_stack.ss_flags = 0;
	makecontext(cur, start_func, 1, args);
	n->uc = current;

	if (running == 0) {
		running = current; 
	} else {
		current->puc = running_node;
		running->num_child++;
	} 
	enqueue(readyq, n);
	return (MyThread) current;
}

void MyThreadYield(void) 
{

	/*Put the invoking thread into the readyq and dequeue readyq */
	assert (running != 0);
	ctx_t * temp = running;
	enqueue(readyq, running_node);
	running_node = dequeue(readyq);
	running = running_node->uc;
	switch_context(temp, running_node->uc);
}

int MyThreadJoin(MyThread thread)
{
/*Joins the invoking function with the specified child thread. 
If the child has already terminated, do not block. 
Note: A child may have terminated without the parent having joined with it. 
Returns 0 on success (after any necessary blocking). 
It returns -1 on failure. Failure occurs if specified thread is not an immediate child of invoking thread.*/

assert(running != 0);
ctx_t * child = (ctx_t *) thread;
if (child->puc != running_node) {
	/*Thread is not the child of given thread*/
	return -1;
}
if (child->st == TERMINATED) {
	/* child has terminated */
	return 0;
}
child->is_par_waiting = TRUE;
ctx_t * temp = running;

/*Enlist in the blocked queue*/
enqueue(blockedq, running_node);
running_node->uc->st = BLOCKED;

/*Dequeue from ready queue*/
running_node = dequeue(readyq);

/*Check if ready queue is NULL*/
/* Ideally should cleanup blocked threads*/
if (running_node == 0) exit(EXIT_SUCCESS);

/*If it is not NULL, assign running to the newly dequeued*/ 
running = running_node->uc;
/*Save current context in temp and switch to newly dequeued one*/
running_node->uc->st = RUN;
switch_context(temp, running_node->uc);

}

void MyThreadJoinAll(void)
{

ctx_t * temp = running;

if (running->num_child == 0) { 
/* No children are alive so return*/
	return;
}

/* TODO: Perform Blocked queue Memory cleanup before exiting */
if (isEmpty(readyq) == TRUE) exit(EXIT_SUCCESS);

/* readyq is not empty and atleast one child is alive but could 
be in blocked or ready state */
enqueue(blockedq, running_node);
running_node->uc->st = BLOCKED;

running_node = dequeue(readyq);

running = running_node->uc;

/*Save current context in temp and switch to newly dequeued one*/
running_node->uc->st = RUN;
switch_context(temp, running_node->uc);
}

void MyThreadExit(void)
{

assert(running != 0); 

node * par = running->puc;

/* Clean up thread memory */
if (running->uc.uc_stack.ss_sp) free(running->uc.uc_stack.ss_sp);
memset(&(running->uc), 0, sizeof(ucontext_t));
running->st = TERMINATED;

if (par == 0 || par->uc == 0) {
	/* Parent has terminated already*/
	goto enq;
}
/* Check if a parent is waiting on a Join call but not JoinAll call*/
if (running->is_par_waiting) {

	/* For some reason parent has a different status than BLOCKED */
	if (par->uc->st != BLOCKED) {
/* Will enqueue the readyq*/
	} else {
		par->uc->st = READY;
		enqueue(readyq, par);
	}
} else {
	/* is_par_waiting is not set. But parent could be waiting on a joinall */
	/* Decrement number of children*/	
	par->uc->num_child--;

	if (par->uc->st == BLOCKED) {
		/* Parent was blocked on joinall */
		if (par->uc->num_child == 0) {
			par->uc->st = READY;
			enqueue(readyq, par);
		}
	}
	/* Parent wasnt blocked. This thread has run to completion*/
}

enq: 

/* Cleanup queue node*/
/*Pointer to parent node was saved earlier*/
free(running_node);
/*TODO: Cleanup thread memory in blocked queue*/
if (isEmpty(readyq)) exit(EXIT_SUCCESS);
running_node = dequeue(readyq);
running = running_node->uc;
restore(running);

}
