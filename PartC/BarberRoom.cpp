#include <iostream>
#include <unistd.h>
using namespace std;

#include "BarberRoom.hpp"

BarberRoom::BarberRoom(unsigned int timeForShavingMS, barberShop *shopState)
{
	this->timeForShavingMS = timeForShavingMS;
	this->shopState = shopState;
	cout << "BarberRoom : Created (Shaving time set at "<< this->timeForShavingMS << "ms)" << endl;
}

//cuts hair of a custommer
void BarberRoom::shaveCustomer()
{
	this->shopState->sleeping = false;
	//usleep(this->timeForShavingMS*1000);
	cout << "BarberRoom : Shave a customer" << endl;
	
}


bool BarberRoom::isSleeping()
{
	return this->shopState->sleeping;
}

BarberRoom::~BarberRoom()
{
	//cout << "BarberRoom : Destroyed" << endl;
}
