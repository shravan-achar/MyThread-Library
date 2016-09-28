#include <stdio.h>
#include "mythreadextra.h"

MySemaphore sem;

void f0(void *args)
{
        int n = *(int*)args;
        printf("Thread%d\n",n);

        MySemaphoreWait(sem);
        printf("\nIn critical section of %d",n);
        MyThreadYield();
        printf("\nAfter Yieldi of %d\n",n);
        MySemaphoreSignal(sem);
}

int main()
{
        MyThreadInitExtra();
        sem = MySemaphoreInit(1);
        int n1=1,n2=2,n3=3,n4=4;
        
        //MySemaphoreWait(sem);

        MyThreadCreate(f0,(void*)&n1);
        MyThreadCreate(f0,(void*)&n2);
        MyThreadCreate(f0,(void*)&n3);
        MyThreadCreate(f0,(void*)&n4);

        MyThreadJoinAll();
        MySemaphoreDestroy(sem);
}
