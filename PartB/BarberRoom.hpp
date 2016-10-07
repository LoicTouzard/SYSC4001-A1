#if ! defined ( BARBERROOM_H )
#define BARBERROOM_H

#include "ipcInfo.hpp"

class BarberRoom
{
	// methods
	public:

		BarberRoom(unsigned int timeForShavingMS, barberState *state);

		//cuts hair of a custommer
		void shaveCustomer();
		
		// tells if the barber is sleeping
		bool isSleeping();
	
		virtual ~BarberRoom();
		
	
	// attributes
	private:
	
		barberState* state;
		
		unsigned int timeForShavingMS;
		
};

#endif // BARBERROOM_H
