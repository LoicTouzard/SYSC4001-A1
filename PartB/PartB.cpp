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

#include "ipcInfo.hpp"
#include "BarberRoom.hpp"
#include "CustomerGenerator.hpp"
#include "WaitingRoom.hpp"


using namespace std;

void barberProcess(BarberRoom* barber, WaitingRoom* room)
{
	int haircut = 0;
	while(true)
	{
		if(!room->isEmpty())// if there is a custommer
		{
			room->freeCustomer(); // he leaves the waiting room
			barber->shaveCustomer();// takes some time
			cout << ++haircut << " customers shaved" << endl;
		}
	}
}

void waitingRoomProcess(WaitingRoom* room, BarberRoom* barber)
{
	// How is this process supposed to do something ? the waiting Room is for me just a shared ressource used by the 2 others processes.
}

void customerGeneratorProcess(CustomerGenerator* customers, WaitingRoom* room)
{
	for(int i = 1; i <= 100000; ++i) // generate 100000 customers
	{
		customers->nextCustomer(); // generates a new customer
		room->newCustomer(); // attemps to add him in the waiting room
	}
}




int main(int argc, char **argv)
{
	// create IPCs
	//const key_t key_waitingCustomers = 1357;
	//const key_t key_barberState = 2568;
	int waitingCustomers_id;
	int barberState_id;
	waitingCustomers *shm_waitingCustomers;
	barberState *shm_barberState;
	
	
	// create the shared memories
    if ((waitingCustomers_id = shmget(IPC_PRIVATE, sizeof(waitingCustomers), IPC_CREAT | 0666)) < 0)
    {
        perror("Parent process : shmget() failed for waitingCustomers !");
        exit(EXIT_FAILURE);
    }
    else
    {
		cout << "Parent process : Shared memory waitingCustomers created"<< endl;
	}
    if ((barberState_id = shmget(IPC_PRIVATE, sizeof(barberState), IPC_CREAT | 0666)) < 0)
    {
        perror("Parent process : shmget() failed for barberState !");
        exit(EXIT_FAILURE);
    }
    else
    {
		cout << "Parent process : Shared memory barberState created"<< endl;
	}
	
	// attach to memory
	if ((shm_waitingCustomers = (waitingCustomers *)shmat(waitingCustomers_id, 0, 0)) == (waitingCustomers *) -1)
	{
        perror("Parent process : shmat() failed for waitingCustomers !");
        exit(EXIT_FAILURE);
    }
    else
    {
		cout << "Parent process : Shared memory waitingCustomers attached"<< endl;
	}
    if ((shm_barberState = (barberState *)shmat(barberState_id, 0, 0)) == (barberState *) -1)
    {
        perror("Parent process : shmat() failed for barberState !");
        exit(EXIT_FAILURE);
    }
    else
    {
		cout << "Parent process : Shared memory barberState attached"<< endl;
	}
	
	
	// creates objects
	
	WaitingRoom* room = new WaitingRoom(4, shm_waitingCustomers);
	BarberRoom* barber = new BarberRoom(1u, shm_barberState);
	CustomerGenerator* customers = new CustomerGenerator(500u,600u);
	
	
	// start processes
    pid_t barber_pid, customers_pid, room_pid;
        
    // create the barber room
    if ((barber_pid = fork()) == 0)
    {
		delete customers;
		// Barber process
		barberProcess(barber, room);
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
			customerGeneratorProcess(customers, room);
			delete customers;
			delete room;
			cout << "CustomerGenerator : End of customer generator process" << endl;
		}
		else if (customers_pid > 0)
		{
			// parent process
			cout << "Parent process : Customer Generator PID = " << customers_pid << endl;
			
			// create the waiting room
			if ((room_pid = fork()) == 0)
			{
				// waiting room process
				waitingRoomProcess(room, barber);
				delete customers;
				delete room;
				delete barber;	
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
				
				// Remove the shared memory
				if (shmctl(waitingCustomers_id, IPC_RMID, (struct shmid_ds *) 0) < 0)
				{
					perror("Parent process : shmctl() failed for waitingCustomers !");
					exit(EXIT_FAILURE);
				}
				else
				{
					cout << "Parent process : Shared memory waitingCustomers removed"<< endl;
				}
				if (shmctl(barberState_id, IPC_RMID, (struct shmid_ds *) 0) < 0)
				{
					perror("Parent process : shmctl() failed for barberState !");
					exit(EXIT_FAILURE);
				}
				else
				{
					cout << "Parent process : Shared memory barberState removed"<< endl;
				}
				
				
				
				cout << "Parent process : End of parent process" << endl;
			}
			else
			{
				// fork failed
				cout << "Parent process : fork() failed for creation of Waiting Room!" << endl;
				return EXIT_FAILURE;
			}
		}
		else
		{
			// fork failed
			cout << "Parent process : fork() failed for creation of Customer Generator!" << endl;
			return EXIT_FAILURE;
		}
    }
    else
    {
        // fork failed
        cout << "Parent process : fork() failed for creation of Barber Roon!" << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
