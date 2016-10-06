#if ! defined ( IPCINFO_H )
#define IPCINFO_H

#include <sys/ipc.h>
#include <sys/shm.h>



typedef struct incomingCustomers{
	int amount;
} incomingCustomers;

typedef struct nextToBeShaved{
	bool exists;
} nextToBeShaved;


#endif // IPCINFO_H
