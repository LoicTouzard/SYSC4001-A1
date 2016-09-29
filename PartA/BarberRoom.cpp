#include <iostream>
#include <unistd.h>
using namespace std;

#include "BarberRoom.hpp"

BarberRoom::BarberRoom(unsigned int timeForShavingMS)
{
	this->timeForShavingMS = timeForShavingMS;
	this->sleeping = true;
	cout << "BarberRoom : Created (Shaving time set at "<< this->timeForShavingMS << ")" << endl;
}

BarberRoom::BarberRoom ():BarberRoom(1000u)
{
}

//cuts hair of a custommer
void BarberRoom::ShaveCustomer()
{
	this->sleeping = false;
	usleep(this->timeForShavingMS*1000);
	cout << "BarberRoom : Shave a customer" << endl;
	
}

// call the next custommer
void BarberRoom::CallCustomer()
{
	cout << "BarberRoom : Call a customer" << endl;
}

bool BarberRoom::isSleeping()
{
	return this->sleeping;
}

BarberRoom::~BarberRoom()
{
	cout << "BarberRoom : Destroyed" << endl;
}
