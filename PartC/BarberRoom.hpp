#if ! defined ( BARBERROOM_H )
#define BARBERROOM_H

#include "ipcInfo.hpp"

class BarberRoom
{
	// methods
	public:

		BarberRoom(unsigned int timeForShavingMS, barberShop *shopState);

		//cuts hair of a custommer
		void shaveCustomer();
		
		void awaken();
		
		void fallAsleep();
	
		virtual ~BarberRoom();
		
	
	// attributes
	private:
	
		barberShop* shopState;
		
		unsigned int timeForShavingMS;
		
};

#endif // BARBERROOM_H
