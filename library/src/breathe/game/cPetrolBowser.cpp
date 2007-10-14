#include <string>

#include <breathe/breathe.h>

#include <breathe/game/cPetrolBowser.h>

namespace breathe
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
