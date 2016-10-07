#include <iostream>
using namespace std;

#include "WaitingRoom.hpp"
#include "ipcInfo.hpp"


WaitingRoom::WaitingRoom(unsigned int chairs, barberShop *shopState)
{
	if(chairs < 1) chairs=1;
	this->chairs = chairs;
	this->shopState = shopState;
	this->rejectedCustomers = 0;
	cout << "WaitingRoom : Created (with " << this->chairs << " chairs)" << endl;
}

bool WaitingRoom::isFull()
{
	return this->chairs <= this->shopState->amount;
}

bool WaitingRoom::isEmpty()
{
	return this->shopState->amount == 0;
}

void WaitingRoom::acceptCustomer()
{
	this->shopState->amount++;
	cout << "WaitingRoom : Accepted a new customer (" << this->shopState->amount << " customers in the room)" << endl;	
}

void WaitingRoom::rejectCustomer()
{
	rejectedCustomers++;
	cout << "WaitingRoom : Rejected a new customer (" << this->shopState->amount << " customers already in the room) " << rejectedCustomers << " so far." << endl;
}

void WaitingRoom::freeCustomer()
{
	this->shopState->amount--;
	cout << "WaitingRoom : Freed a customer (" << this->shopState->amount << " customers in the room)" << endl;
}

int WaitingRoom::getRejectedCustomers()
{
	return this->rejectedCustomers;
}

WaitingRoom::~WaitingRoom()
{
	//cout << "WaitingRoom : Destroyed" << endl;
}
