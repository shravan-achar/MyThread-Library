all: 
	gcc -c -g -w mythread.c myqueue.c semaphores.c
	ar rcs mythread.a mythread.o myqueue.o semaphores.o

clean:
	rm *.o mythread.a
