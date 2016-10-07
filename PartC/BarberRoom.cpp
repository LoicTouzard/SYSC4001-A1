#include <iostream>
#include <unistd.h>
using namespace std;

#include "BarberRoom.hpp"
#include "mutex.hpp"

BarberRoom::BarberRoom(unsigned int timeForShavingMS, barberShop *shopState)
{
	this->timeForShavingMS = timeForShavingMS;
	this->shopState = shopState;
	cout << "BarberRoom : Created (Shaving time set at "<< this->timeForShavingMS << "ms)" << endl;
}

//cuts hair of a custommer
void BarberRoom::shaveCustomer()
{
	//usleep(this->timeForShavingMS*1000);
	cout << "BarberRoom : Shave a customer" << endl;
	
}

void BarberRoom::awaken()
{
	this->shopState->sleeping = false;
}

void BarberRoom::fallAsleep()
{
	this->shopState->sleeping = true;
}

BarberRoom::~BarberRoom()
{
	//cout << "BarberRoom : Destroyed" << endl;
}
