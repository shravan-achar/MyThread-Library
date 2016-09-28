/******************************************************************************
 *
 *  File Name........: mythreadextra.h
 *
 *  Description......:
 *
 *  Revision History.:
 *
 *
 *****************************************************************************/

#ifndef MYTHREADEXTRA_H
#define MYTHREADEXTRA_H

// Leave these definitions alone.  They are opaque handles.  The
// public definition of these should not contain the internal
// structure.
// In the library routines you should cast these parameters to their
// internals definitions.  Eg,
//		_MyThread internal_name = (_MyThread)parameter_name;
// and when returning these handles to the user
//		MyThread parameter_name = (MyThread)internal_name;
typedef void *MyThread;
typedef void *MySemaphore;

// ****** THREAD OPERATIONS ****** 
// Create a new thread.
MyThread MyThreadCreate(void(*start_funct)(void *), void *args);

// Yield invoking thread
void MyThreadYield(void);

// Join with a child thread
int MyThreadJoin(MyThread thread);

// Join with all children
void MyThreadJoinAll(void);

// Terminate invoking thread
void MyThreadExit(void);

// ****** SEMAPHORE OPERATIONS ****** 
// Create a semaphore
MySemaphore MySemaphoreInit(int initialValue);

// Signal a semaphore
void MySemaphoreSignal(MySemaphore sem);

// Wait on a semaphore
void MySemaphoreWait(MySemaphore sem);

// Destroy on a semaphore
int MySemaphoreDestroy(MySemaphore sem);

// ****** CALLS ONLY FOR UNIX PROCESS ****** 
// Create and run the "main" thread
void MyThreadInit(void(*start_funct)(void *), void *args);

void MyThreadInitExtra(void);

#endif /* MYTHREADEXTRA_H */
/*........................ end of mythread.h ................................*/
