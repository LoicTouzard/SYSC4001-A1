#if ! defined ( BARBERROOM_H )
#define BARBERROOM_H

class BarberRoom
{
	// methods
	public:

		BarberRoom(unsigned int timeForShavingMS);
		
		BarberRoom();
		
		//cuts hair of a custommer
		void ShaveCustomer();
		
		// call the next custommer
		void CallCustomer();
		
		// tells if the barber is sleeping
		bool isSleeping();
	
		virtual ~BarberRoom();
		
	
	// attributes
	private:
	
		bool sleeping;
		
		unsigned int timeForShavingMS;

};

#endif // BARBERROOM_H
