#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "BarberRoom.hpp"
#include "CustomerGenerator.hpp"
#include "WaitingRoom.hpp"


using namespace std;


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
