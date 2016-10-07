#if ! defined ( CUSTOMERGENERATOR_H )
#define CUSTOMERGENERATOR_H


#include "WaitingRoom.hpp"

class CustomerGenerator
{
	// methods
	public:

		CustomerGenerator(unsigned int minMS, unsigned int maxMS);

		
		// generate customers (infinite loop)
		void generate();
		
		// generate n customers
		void generate(unsigned int n);
		
		virtual ~CustomerGenerator();

		// create one customer in a random time
		void nextCustomer();

	// attributes
	private:
		// min and max in milliseconds for the generation of a customer 
		unsigned int minMS;
		unsigned int maxMS;
};

#endif // CUSTOMERGENERATOR_H
