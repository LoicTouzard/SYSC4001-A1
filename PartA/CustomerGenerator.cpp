#include <iostream>
#include <unistd.h>
#include <cstdlib>
using namespace std;

#include "CustomerGenerator.hpp"

CustomerGenerator::CustomerGenerator(unsigned int minMS, unsigned int maxMS)
{
	if(maxMS < minMS) maxMS=minMS;
	this->minMS = minMS;
	this->maxMS = maxMS;
	srand (time(NULL));
	cout << "CustomerGenerator : Created (Generating rate between " << this->minMS << "ms and " << this->maxMS << "ms)" << endl;
}


void CustomerGenerator::generate()
{
	cout << "CustomerGenerator : Starting infinite generation" << endl;
	while(true)
	{
		this->nextCustomer();
	}
}

void CustomerGenerator::generate(unsigned int n)
{
	cout << "CustomerGenerator : Starting generation of " << n << "customers" << endl;
	for(unsigned int i = 0; i < n; ++i)
	{
		this->nextCustomer();
	}
}

void CustomerGenerator::nextCustomer()
{
	unsigned int waitMS = rand() % (this->maxMS - this->minMS) + this->minMS;
	usleep(waitMS*1000);
	cout << "CustomerGenerator : New customer created in " << waitMS << "ms" << endl;
}

CustomerGenerator::~CustomerGenerator()
{
	cout << "CustomerGenerator : Destroyed" << endl;
}
