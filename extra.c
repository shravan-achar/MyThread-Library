#include <stdio.h>
#include <stdlib.h>
#include "mythreadextra.h"

void f0(void* args)
{
        printf("\nIn f0");
//      MyThreadExit();
}

int main()
{
        MyThreadInitExtra();
        printf("\nAfter MyThreadInitExtra");
        MyThreadCreate(f0,0);
        MyThreadYield();
        printf("\nExiting Main");
        MyThreadExit();
}
