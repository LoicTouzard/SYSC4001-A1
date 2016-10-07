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

#include "ipcInfo.hpp"
#include "mutex.hpp"
#include "BarberRoom.hpp"
#include "CustomerGenerator.hpp"
#include "WaitingRoom.hpp"


using namespace std;

void barberProcess(BarberRoom* barber, WaitingRoom* room, int sem_barberState_id, int sem_waitingCustomers_id)
{
	int haircut = 0;
	while(true)
	{
		if(!room->isEmpty())// if there is a customer
		{
			mutexWait(sem_barberState_id, 0);
			barber->awaken();
			mutexSignal(sem_barberState_id);
			
			mutexWait(sem_waitingCustomers_id, 0);
			room->freeCustomer(); // he leaves the waiting room
			mutexSignal(sem_waitingCustomers_id);
			
			barber->shaveCustomer(); // takes some time
			cout << ++haircut << " customers shaved" << endl;
		}
		else
		{
			mutexWait(sem_barberState_id, 0);
			barber->fallAsleep();
			mutexSignal(sem_barberState_id);
		}
	}
}

void waitingRoomProcess()
{
	// How is this process supposed to do something ? the waiting Room is for me just a shared ressource used by the 2 others processes.
}

void customerGeneratorProcess(CustomerGenerator* customers, WaitingRoom* room, int sem_waitingCustomers_id)
{
	for(int i = 1; i <= 10; ++i) // generate 100000 customers
	{
		customers->nextCustomer(); // generates a new customer
		mutexWait(sem_waitingCustomers_id, 0);
		room->newCustomer(); // attemps to add him in the waiting room
		mutexSignal(sem_waitingCustomers_id);
	}
}


int main(int argc, char **argv)
{
	/* create IPCs */
	int barberShop_id;
	barberShop *shm_barberShop;
	int sem_barberState_id;
	int sem_waitingCustomers_id;
	
	
	
	
	// create the shared memory
    if ((barberShop_id = shmget(IPC_PRIVATE, sizeof(barberShop), IPC_CREAT | 0666)) < 0)
    {
        perror("Parent process : shmget() failed for barberShop !");
        exit(EXIT_FAILURE);
    }
    else
    {
		cout << "Parent process : Shared memory barberShop created"<< endl;
	}
	
	// attach to memory
	if ((shm_barberShop = (barberShop *)shmat(barberShop_id, 0, 0)) == (barberShop *) -1)
	{
        perror("Parent process : shmat() failed for barberShop !");
        // Remove the shared memory
		if (shmctl(barberShop_id, IPC_RMID, (struct shmid_ds *) 0) < 0)
		{
			perror("Parent process : shmctl() failed for barberShop !");
		}
		else
		{
			cout << "Parent process : Shared memory barberShop removed"<< endl;
		}
        exit(EXIT_FAILURE);
    }
    else
    {
		shm_barberShop->amount = 0;
		shm_barberShop->sleeping = true;
		cout << "Parent process : Shared memory barberShop attached"<< endl;
	}
	
	
	
	
	
	// creates the semaphores
	if ((sem_barberState_id = mutexCreate()) < 0)
    {
        perror("Parent process : mutexCreate() failed for barberState !");
        // Remove the shared memory
		if (shmctl(barberShop_id, IPC_RMID, (struct shmid_ds *) 0) < 0)
		{
			perror("Parent process : shmctl() failed for barberShop !");
		}
		else
		{
			cout << "Parent process : Shared memory barberShop removed"<< endl;
		}
        exit(EXIT_FAILURE);
    }
    else
    {
		cout << "Parent process : Mutex barberState created"<< endl;
	}
	if ((sem_waitingCustomers_id = mutexCreate()) < 0)
    {
        perror("Parent process : mutexCreate() failed for waitingCustomers !");
        //remove the other semaphore
		mutexRemove(sem_barberState_id);
		cout << "Parent process : Mutex barberState removed"<< endl;
        // Remove the shared memory
		if (shmctl(barberShop_id, IPC_RMID, (struct shmid_ds *) 0) < 0)
		{
			perror("Parent process : shmctl() failed for barberShop !");
		}
		else
		{
			cout << "Parent process : Shared memory barberShop removed"<< endl;
		}
        exit(EXIT_FAILURE);
    }
    else
    {
		cout << "Parent process : Mutex waitingCustomers created"<< endl;
	}
	
	
	
	
	
	/* creates objects*/
	
	WaitingRoom* room = new WaitingRoom(4, shm_barberShop);
	BarberRoom* barber = new BarberRoom(1u, shm_barberShop);
	CustomerGenerator* customers = new CustomerGenerator(500u,600u);
	

	/* start processes */
    pid_t barber_pid, customers_pid, room_pid;
        
    // create the barber room
    if ((barber_pid = fork()) == 0)
    {
		
		delete customers;
		// Barber process
		barberProcess(barber, room, sem_barberState_id, sem_waitingCustomers_id);
		delete room;
		delete barber;
		cout << "BarberRoom : End of barber process" << endl;
		
    }
    else if (barber_pid > 0)
    {
        // parent process
        cout << "Parent process : Barber PID = " << barber_pid << endl;
        
		// create the customers generator
		if ((customers_pid = fork()) == 0)
		{
			
			// customers generator process
			delete barber;
			customerGeneratorProcess(customers, room, sem_waitingCustomers_id);
			delete customers;
			delete room;
			cout << "CustomerGenerator : End of customer generator process" << endl;
			cout << "Total of " << room->getRejectedCustomers() << " rejected customers" << endl;
			
		}
		else if (customers_pid > 0)
		{
			// parent process
			cout << "Parent process : Customer Generator PID = " << customers_pid << endl;
			
			// create the waiting room
			if ((room_pid = fork()) == 0)
			{
				
				// waiting room process
				delete customers;
				delete room;
				delete barber;	
				waitingRoomProcess();
				cout << "WaitingRoom : End of waiting room process" << endl;
				
			}
			else if (customers_pid > 0)
			{
				
				delete customers;
				delete room;
				delete barber;
				// parent process
				cout << "Parent process : Waiting Room PID = " << room_pid << endl;
				
				// All processes have been created, waiting for their task to end.
				pid_t dead;
				dead = wait(NULL);
				cout << "Parent process : Got dead child : " << dead << endl;
				dead = wait(NULL);
				cout << "Parent process : Got dead child : " << dead << endl;
				dead = wait(NULL);
				cout << "Parent process : Got dead child : " << dead << endl;
				
				
				//remove barberState Mutex
				mutexRemove(sem_barberState_id);
				cout << "Parent process : Mutex barberState removed"<< endl;
				//remove waitingCustomers Mutex
				mutexRemove(sem_waitingCustomers_id);
				cout << "Parent process : Mutex waitingCustomers removed"<< endl;
				// Remove the shared memory
				if (shmctl(barberShop_id, IPC_RMID, (struct shmid_ds *) 0) < 0)
				{
					perror("Parent process : shmctl() failed for barberShop !");
					exit(EXIT_FAILURE);
				}
				else
				{
					cout << "Parent process : Shared memory barberShop removed"<< endl;
				}
								
				cout << "Parent process : End of parent process" << endl;
			}
			else
			{
				// fork failed
				cout << "Parent process : fork() failed for creation of Waiting Room!" << endl;
				//remove barberState Mutex
				mutexRemove(sem_barberState_id);
				cout << "Parent process : Mutex barberState removed"<< endl;
				//remove waitingCustomers Mutex
				mutexRemove(sem_waitingCustomers_id);
				cout << "Parent process : Mutex waitingCustomers removed"<< endl;
				// Remove the shared memory
				if (shmctl(barberShop_id, IPC_RMID, (struct shmid_ds *) 0) < 0)
				{
					perror("Parent process : shmctl() failed for barberShop !");
					exit(EXIT_FAILURE);
				}
				else
				{
					cout << "Parent process : Shared memory barberShop removed"<< endl;
				}
				return EXIT_FAILURE;
			}
		}
		else
		{
			// fork failed
			cout << "Parent process : fork() failed for creation of Customer Generator!" << endl;
			//remove barberState Mutex
			mutexRemove(sem_barberState_id);
			cout << "Parent process : Mutex barberState removed"<< endl;
			//remove waitingCustomers Mutex
			mutexRemove(sem_waitingCustomers_id);
			cout << "Parent process : Mutex waitingCustomers removed"<< endl;
			// Remove the shared memory
			if (shmctl(barberShop_id, IPC_RMID, (struct shmid_ds *) 0) < 0)
			{
				perror("Parent process : shmctl() failed for barberShop !");
				exit(EXIT_FAILURE);
			}
			else
			{
				cout << "Parent process : Shared memory barberShop removed"<< endl;
			}
			return EXIT_FAILURE;
		}
    }
    else
    {
        // fork failed
        cout << "Parent process : fork() failed for creation of Barber Roon!" << endl;
		//remove barberState Mutex
		mutexRemove(sem_barberState_id);
		cout << "Parent process : Mutex barberState removed"<< endl;
		//remove waitingCustomers Mutex
		mutexRemove(sem_waitingCustomers_id);
		cout << "Parent process : Mutex waitingCustomers removed"<< endl;
        // Remove the shared memory
		if (shmctl(barberShop_id, IPC_RMID, (struct shmid_ds *) 0) < 0)
		{
			perror("Parent process : shmctl() failed for barberShop !");
			exit(EXIT_FAILURE);
		}
		else
		{
			cout << "Parent process : Shared memory barberShop removed"<< endl;
		}
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
