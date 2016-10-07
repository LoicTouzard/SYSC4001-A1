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
#include "sem-mut-utils.hpp"
#include "BarberRoom.hpp"
#include "CustomerGenerator.hpp"
#include "WaitingRoom.hpp"

#define WAITING_ROOM_SEATS 4
#define CUSTOMER_TO_GENERATE 10
#define HAIRCUT_TIME 1000u // ms
#define GENERATION_MIN 500u // ms
#define GENERATION_MAX 1000u // ms


using namespace std;

void barberProcess(BarberRoom* barber, WaitingRoom* room, int sem_barberReady_id, int sem_waitingRoomSeats_id, int sem_customerReady_id)
{
	int haircutDone = 0;
	while(haircutDone < CUSTOMER_TO_GENERATE)
	{
		semaphoreWaitOne(sem_customerReady_id, 0);// wait for a customer
		semaphoreWaitOne(sem_waitingRoomSeats_id, 0); // need access to modify the number of seats renaining
		room->freeCustomer(); // the customer leaves the waiting room
		mutexSignal(sem_barberReady_id);
		barber->awaken();
		semaphoreSignalOne(sem_waitingRoomSeats_id); // free the seats shared memory
		barber->shaveCustomer(); // takes some time
		cout << "BarberRoom : " << ++haircutDone << " customers shaved" << endl;
	}
}

void waitingRoomProcess()
{
	// How is this process supposed to do something ? the waiting Room is for me just a shared ressource used by the 2 others processes.
}

void customerGeneratorProcess(CustomerGenerator* customers, WaitingRoom* room, BarberRoom* barber, int sem_barberReady_id, int sem_waitingRoomSeats_id, int sem_customerReady_id)
{
	for(int i = 1; i <= CUSTOMER_TO_GENERATE; ++i) // generate 100000 customers
	{
		customers->nextCustomer(); // generates a new customer
		semaphoreWaitOne(sem_waitingRoomSeats_id, 0); // need access to modify the number of seats renaining
		if(!room->isFull())// If the room is not full
		{
			room->acceptCustomer();
			semaphoreSignalOne(sem_customerReady_id); // another customer is ready
			semaphoreSignalOne(sem_waitingRoomSeats_id);
			mutexWait(sem_barberReady_id,0);// Job's done for the barber, get back to sleep
			barber->fallAsleep();
		}
		else
		{
			semaphoreSignalOne(sem_waitingRoomSeats_id);
		}
	}
}


int main(int argc, char **argv)
{
	/* create IPCs */
	int barberShop_id;
	barberShop *shm_barberShop;
	int sem_barberReady_id;
	int sem_waitingRoomSeats_id;
	int sem_customerReady_id;
	
	
	
	
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
	if ((sem_barberReady_id = mutexCreate()) < 0)
    {
        perror("Parent process : mutexCreate() failed for barberReady !");
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
		cout << "Parent process : Mutex barberReady created"<< endl;
	}
	if ((sem_waitingRoomSeats_id = semaphoreCreate(1)) < 0)
    {
        perror("Parent process : semaphoreCreate() failed for waitingRoomSeats !");
        //remove the other semaphore
		mutexRemove(sem_barberReady_id);
		cout << "Parent process : Mutex barberReady removed"<< endl;
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
		cout << "Parent process : Semaphore waitingRoomSeats created"<< endl;
	}
	if ((sem_customerReady_id = semaphoreCreate(0)) < 0)
    {
        perror("Parent process : semaphoreCreate() failed for sem_customerReady_id !");
        //remove the other semaphore
		semaphoreRemove(sem_waitingRoomSeats_id);
		mutexRemove(sem_barberReady_id);
		cout << "Parent process : Mutex barberReady removed"<< endl;
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
		cout << "Parent process : Semaphore sem_customerReady_id created"<< endl;
	}
	
	
	
	
	
	/* creating objects*/
	WaitingRoom* room = new WaitingRoom(WAITING_ROOM_SEATS, shm_barberShop);
	BarberRoom* barber = new BarberRoom(HAIRCUT_TIME, shm_barberShop);
	CustomerGenerator* customers = new CustomerGenerator(GENERATION_MIN,GENERATION_MAX);
	

	/* start processes */
    pid_t barber_pid, customers_pid, room_pid;
        
    // create the barber room
    if ((barber_pid = fork()) == 0)
    {
		
		delete customers;
		// Barber process
		barberProcess(barber, room, sem_barberReady_id, sem_waitingRoomSeats_id, sem_customerReady_id);
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
			customerGeneratorProcess(customers, room, barber, sem_barberReady_id, sem_waitingRoomSeats_id, sem_customerReady_id);
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
				
				
				//remove barberReady Mutex
				mutexRemove(sem_barberReady_id);
				cout << "Parent process : Mutex barberReady removed"<< endl;
				//remove waitingRoomSeats Semaphore
				semaphoreRemove(sem_waitingRoomSeats_id);
				cout << "Parent process : Semaphore waitingRoomSeats removed"<< endl;
				//remove waitingRoomSeats Semaphore
				semaphoreRemove(sem_customerReady_id);
				cout << "Parent process : Semaphore sem_customerReady_id removed"<< endl;
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
				//remove barberReady Mutex
				mutexRemove(sem_barberReady_id);
				cout << "Parent process : Mutex barberReady removed"<< endl;
				//remove waitingRoomSeats Semaphore
				semaphoreRemove(sem_waitingRoomSeats_id);
				cout << "Parent process : Semaphore waitingRoomSeats removed"<< endl;
				//remove waitingRoomSeats Semaphore
				semaphoreRemove(sem_customerReady_id);
				cout << "Parent process : Semaphore sem_customerReady_id removed"<< endl;
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
			//remove barberReady Mutex
			mutexRemove(sem_barberReady_id);
			cout << "Parent process : Mutex barberReady removed"<< endl;
			//remove waitingRoomSeats Semaphore
			semaphoreRemove(sem_waitingRoomSeats_id);
			cout << "Parent process : Semaphore waitingRoomSeats removed"<< endl;
			//remove waitingRoomSeats Semaphore
			semaphoreRemove(sem_customerReady_id);
			cout << "Parent process : Semaphore sem_customerReady_id removed"<< endl;
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
		//remove barberReady Mutex
		mutexRemove(sem_barberReady_id);
		cout << "Parent process : Mutex barberReady removed"<< endl;
		//remove waitingRoomSeats Semaphore
		semaphoreRemove(sem_waitingRoomSeats_id);
		cout << "Parent process : Semaphore waitingRoomSeats removed"<< endl;
		//remove waitingRoomSeats Semaphore
		semaphoreRemove(sem_customerReady_id);
		cout << "Parent process : Semaphore sem_customerReady_id removed"<< endl;
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
