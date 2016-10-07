#include <iostream>
using namespace std;

#include "WaitingRoom.hpp"
#include "ipcInfo.hpp"


WaitingRoom::WaitingRoom(unsigned int chairs, waitingCustomers *customers)
{
	if(chairs < 1) chairs=1;
	this->chairs = chairs;
	this->customers = customers;
	this->customers->amount = 0;
	this->rejectedCustomers = 0;
	cout << "WaitingRoom : Created (with " << this->chairs << " chairs)" << endl;
}

bool WaitingRoom::isFull()
{
	return this->chairs <= this->customers->amount;
}

bool WaitingRoom::isEmpty()
{
	return this->customers->amount == 0;
}

void WaitingRoom::newCustomer()
{
	if(!this->isFull())
	{
		this->customers->amount++;
		cout << "WaitingRoom : Accepted a new customer (" << this->customers->amount << " customers in the room)" << endl;	
	}
	else
	{
		rejectedCustomers++;
		cout << "WaitingRoom : Rejected a new customer (" << this->customers->amount << " customers already in the room) " << rejectedCustomers << " so far." << endl;	
	}
}

void WaitingRoom::freeCustomer()
{
	this->customers->amount--;
	cout << "WaitingRoom : Freed a customer (" << this->customers->amount << " customers in the room)" << endl;
}

WaitingRoom::~WaitingRoom()
{
	//cout << "WaitingRoom : Destroyed" << endl;
}
