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

#include "BarberRoom.hpp"
#include "CustomerGenerator.hpp"
#include "WaitingRoom.hpp"


using namespace std;

typedef struct incomingCustomers{
	int amount;
} incomingCustomers;

typedef struct nextToBeShaved{
	bool exists;
} nextToBeShaved;


void barberProcess()
{
	BarberRoom barber(1000u);
	
}

void waitingRoomProcess()
{
	WaitingRoom room(3);
}

void customerGeneratorProcess()
{
	CustomerGenerator customers(500u, 1500u);
	customers.generate(10);
}




int main(int argc, char **argv)
{
	// create IPCs
	//const key_t key_incomingCustomers = 1357;
	//const key_t key_nextToBeShaved = 2568;
	int incomingCustomers_id;
	int nextToBeShaved_id;
	incomingCustomers *shm_incomingCustomers;
	nextToBeShaved *shm_nextToBeShaved;
	
	// create the shared memories
    if ((incomingCustomers_id = shmget(IPC_PRIVATE, sizeof(incomingCustomers), IPC_CREAT | 0666)) < 0)
    {
        perror("Parent process : shmget() failed for incomingCustomers !");
        exit(EXIT_FAILURE);
    }
    else
    {
		cout << "Parent process : Shared memory incomingCustomers created"<< endl;
	}
    if ((nextToBeShaved_id = shmget(IPC_PRIVATE, sizeof(nextToBeShaved), IPC_CREAT | 0666)) < 0)
    {
        perror("Parent process : shmget() failed for nextToBeShaved !");
        exit(EXIT_FAILURE);
    }
    else
    {
		cout << "Parent process : Shared memory nextToBeShaved created"<< endl;
	}
	
	// attach to memory
	if ((shm_incomingCustomers = (incomingCustomers *)shmat(incomingCustomers_id, 0, 0)) == (incomingCustomers *) -1)
	{
        perror("Parent process : shmat() failed for incomingCustomers !");
        exit(EXIT_FAILURE);
    }
    else
    {
		cout << "Parent process : Shared memory incomingCustomers attached"<< endl;
	}
    if ((shm_nextToBeShaved = (nextToBeShaved *)shmat(nextToBeShaved_id, 0, 0)) == (nextToBeShaved *) -1)
    {
        perror("Parent process : shmat() failed for nextToBeShaved !");
        exit(EXIT_FAILURE);
    }
    else
    {
		cout << "Parent process : Shared memory nextToBeShaved attached"<< endl;
	}
	
	
	// start processes
    pid_t barber_pid, customers_pid, room_pid;
        
    // create the barber room
    if ((barber_pid = fork()) == 0)
    {
		// Barber process
		barberProcess();
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
			customerGeneratorProcess();			
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
				waitingRoomProcess();
				cout << "WaitingRoom : End of waiting room process" << endl;
			}
			else if (customers_pid > 0)
			{
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
				if (shmctl(incomingCustomers_id, IPC_RMID, (struct shmid_ds *) 0) < 0)
				{
					perror("Parent process : shmctl() failed for incomingCustomers !");
					exit(EXIT_FAILURE);
				}
				else
				{
					cout << "Parent process : Shared memory incomingCustomers removed"<< endl;
				}
				if (shmctl(nextToBeShaved_id, IPC_RMID, (struct shmid_ds *) 0) < 0)
				{
					perror("Parent process : shmctl() failed for nextToBeShaved !");
					exit(EXIT_FAILURE);
				}
				else
				{
					cout << "Parent process : Shared memory nextToBeShaved removed"<< endl;
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
