#include "mythread.h"
#include "thread.h"


MySemaphore MySemaphoreInit(int initialValue)
{
	mysem * sem = calloc(1, sizeof(mysem));
	if (initialValue > -1) {
		sem->sem_id = id_counter++; 
		sem->bsemq = build_queue();
		if (sem->bsemq == 0) handle_error("malloc");
		sem->max_value = initialValue;
		sem->value = initialValue;
		return sem;
	} else return NULL;
}

void MySemaphoreSignal(MySemaphore sem)
{

mysem * ms = (mysem *) sem;
if(isEmpty(ms->bsemq)) { /* Can just be equal to */
	/*Blocked queue is empty if value > 0*/
	ms->value++;
} else {
	/* Unblock thread and put in ready queue*/
	assert(running != 0);
	/*if (isEmpty(ms->bsemq)) {
		ms->value++; 
		return;
	}*/
	node * p = dequeue(ms->bsemq);
	p->uc->st = READY;
	enqueue(readyq, p);
}        
}

void MySemaphoreWait(MySemaphore sem)
{

	mysem * ms = (mysem *) sem;
	if (ms->value > 0) { /* Can just be equal to */
		/* Semaphore available if value > 0 */
		ms->value--;
	} else {
		/* Put the invoking thread to semaphore block queue*/
		assert (running != 0);
		ctx_t * current = running;
		enqueue(ms->bsemq, running_node);
		running_node->uc->st = BLOCKED;
		if (isEmpty(readyq)) exit(EXIT_SUCCESS);
		running_node = dequeue(readyq);
                running = running_node->uc;
		swapcontext(&(current->uc), &(running->uc));

	}
}
int MySemaphoreDestroy(MySemaphore sem)
{
	mysem * ms = (mysem *) sem;
	if (!isEmpty(ms->bsemq)) return -1;
	
        /* Just free semaphore object*/
	if (ms->bsemq) free(ms->bsemq);
	free(ms);
        return 0;
}
