#if ! defined ( MUTEX_H )
#define MUTEX_H

// Most of the function here are from the '4 generic semaphore functions' from the unix_Tutorial.pdf file on CULearn for SYSC4001

int mutexWait(int semid, int iMayBlock);

int mutexSignal(int semid);

void mutexRemove( int semid );

int mutexCreate();

#endif // MUTEX_H
