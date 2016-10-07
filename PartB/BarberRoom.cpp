#include <iostream>
#include <unistd.h>
using namespace std;

#include "BarberRoom.hpp"

BarberRoom::BarberRoom(unsigned int timeForShavingMS, barberState *state)
{
	this->timeForShavingMS = timeForShavingMS;
	this->state = state;
	this->state->sleeping = true;
	cout << "BarberRoom : Created (Shaving time set at "<< this->timeForShavingMS << "ms)" << endl;
}

//cuts hair of a custommer
void BarberRoom::shaveCustomer()
{
	this->state->sleeping = false;
	//usleep(this->timeForShavingMS*1000);
	cout << "BarberRoom : Shave a customer" << endl;
	
}


bool BarberRoom::isSleeping()
{
	return this->state->sleeping;
}

BarberRoom::~BarberRoom()
{
	//cout << "BarberRoom : Destroyed" << endl;
}
