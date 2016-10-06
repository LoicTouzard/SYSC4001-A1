#include <iostream>
using namespace std;

#include "WaitingRoom.hpp"

WaitingRoom::WaitingRoom(unsigned int chairs)
{
	if(chairs < 1) chairs=1;
	this->chairs = chairs;
	this->customers = 0;
	cout << "WaitingRoom : Created (with " << this->chairs << " chairs)" << endl;
}

bool WaitingRoom::isFull()
{
	return this->chairs <= this->customers;
}

bool WaitingRoom::isEmpty()
{
	return this->customers == 0;
}

void WaitingRoom::acceptCustomer()
{
	this->customers++;
	cout << "WaitingRoom : Accepted a new customer (" << this->customers << "customers in the room)" << endl;
}

void WaitingRoom::freeCustomer()
{
	this->customers--;
	cout << "WaitingRoom : Freed a customer (" << this->customers << "customers in the room)" << endl;
}

WaitingRoom::~WaitingRoom()
{
	cout << "WaitingRoom : Destroyed" << endl;
}
