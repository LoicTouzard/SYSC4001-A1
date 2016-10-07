#if ! defined ( IPCINFO_H )
#define IPCINFO_H


typedef struct waitingCustomers{
	unsigned int amount;
} waitingCustomers;

typedef struct barberState{
	bool sleeping;
} barberState;


#endif // IPCINFO_H
