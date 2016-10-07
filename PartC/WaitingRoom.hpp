#if ! defined ( WAITINGROOM_H )
#define WAITINGROOM_H

#include "ipcInfo.hpp"

class WaitingRoom
{
	// methods
	public:
		
		WaitingRoom(unsigned int chairs, barberShop *shopState);
		
		// tells if the room is full of customer or not
		bool isFull();
		
		// tells if the room is empty or not
		bool isEmpty();
		
		// A new customer sits in the room
		void acceptCustomer();
		
		// A new customer can't sit in the room
		void rejectCustomer();
		
		// A customer leaves the room
		void freeCustomer();
		
		virtual ~WaitingRoom();
		
		int getRejectedCustomers();
	
	// attributes
	private:
		
		// the amout of chairs in the waiting room
		unsigned int chairs;
		
		// the amout of customers currently in the waiting room
		barberShop *shopState;
		
		int rejectedCustomers;
};

#endif // WAITINGROOM_H
