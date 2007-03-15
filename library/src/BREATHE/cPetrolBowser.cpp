#include <BREATHE/cPetrolBowser.h>

namespace BREATHE
{
	cPetrolBowser::cPetrolBowser(unsigned int type, float price)
	{
		uiType=type;
		fPrice=price;

		fHealth=1.0f; //Just a tap and it will explode
	}

	cPetrolBowser::~cPetrolBowser()
	{
		
	}
}
