#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <sys/sem.h>
#include <semaphore.h>

#include "mutex.hpp"

#define SEM_MODE (0400 | 0200 | 044)

int mutexWait(int semid, int iMayBlock)
{
	struct sembuf sbOperation;
	sbOperation.sem_num = 0;
	sbOperation.sem_op = -1;
	sbOperation.sem_flg = iMayBlock;
	return semop( semid, &sbOperation, 1 );
}

int mutexSignal(int semid)
{
	struct sembuf sbOperation;
	sbOperation.sem_num = 0;
	sbOperation.sem_op = +1;
	sbOperation.sem_flg = 0;
	return semop( semid, &sbOperation, 1 );
}

void semaphoreRemove( int semid )
{
	if(semid >= 0 )
	{
		semctl( semid, 0, IPC_RMID , 0);
	}
}

void mutexRemove( int semid )
{
	semaphoreRemove(semid);
}

union semun {
    int val;
    struct semid_ds *buf;
    ushort * arraygcc;
} arg ;

int semaphoreCreate(int iInitialValue)
{
	int semid;
	union semun suInitData;
	int iError;
	/* get a semaphore */
	semid = semget(IPC_PRIVATE, 1, SEM_MODE );
	/* check for errors */
	if( semid < 0 ) return semid;
	/* now initialize the semaphore */
	suInitData.val = iInitialValue;
	if((iError = semctl( semid, 0, SETVAL, suInitData)) < 0 )
	{ /* error occurred, so remove semaphore */
		semaphoreRemove(semid);
		return iError;
	}
	return semid;
}

int mutexCreate()
{
	return semaphoreCreate(1);
}
