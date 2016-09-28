/*
 * Main.c
 *
 *  Created on: Sep 27, 2016
 *      Author: gpollep
 */

#include <stdio.h>
#include <ucontext.h>
#include "mythread.h"

static ucontext_t ctx[3];

void f3()
{
	puts("f3");
        return;
}

void f4()
{
	puts("f4");
}


static void
f1 (void* args)
{
/*    puts("start f1");
    swapcontext(&ctx[1], &ctx[2]);
    puts("finish f1");*/

	f3();
    puts("From 1");
}


static void
f2 (void* args)
{
/*    puts("start f2");
    swapcontext(&ctx[2], &ctx[1]);
    puts("finish f2");*/

	f4();
    puts("From 2");
}


int
main2iii (void)
{
    char st1[8192];
    char st2[8192];


    getcontext(&ctx[1]);
    ctx[1].uc_stack.ss_sp = st1;
    ctx[1].uc_stack.ss_size = sizeof st1;
    ctx[1].uc_link = &ctx[0];
    makecontext(&ctx[1], f1, 0);


    getcontext(&ctx[2]);
    ctx[2].uc_stack.ss_sp = st2;
    ctx[2].uc_stack.ss_size = sizeof st2;
    ctx[2].uc_link = NULL;
    makecontext(&ctx[2], f2, 0);


    swapcontext(&ctx[0], &ctx[2]);
    swapcontext(&ctx[0], &ctx[1]);
    return 0;
}

void f0(void* args)
{
	printf("Entered foo\n");
	MyThreadCreate(f1,0);
	MyThreadCreate(f2,0);

	MyThreadJoinAll();

	printf("\nBoth Threads exited");

}

int main()
{
	MyThreadInitExtra(f0,0);
}
