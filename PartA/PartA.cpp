#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>

#include "BarberRoom.hpp"
#include "CustomerGenerator.hpp"
#include "WaitingRoom.hpp"


using namespace std;

int main(int argc, char **argv)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        // child process
        cout << "Child process" << endl;
    }
    else if (pid > 0)
    {
        // parent process
        cout << "Parent process : child PID = " << pid << endl;
		pid_t dead;
		dead = wait(NULL);
        cout << "Got dead child : " << dead << endl;
    }
    else
    {
        // fork failed
        cout << "fork() failed!" << endl;
        return 1;
    }
    cout << "End of process" << endl;

    return 0;
}
