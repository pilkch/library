#include <string>

// writing on a text file
#include <iostream>
#include <fstream>

#include <sstream>

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

// Breathe
#include <BREATHE/cBreathe.h>

#include <BREATHE/UTIL/cLog.h>

#include <BREATHE/UTIL/cNetwork.h>

namespace BREATHE
{
	cNetwork *pNetwork=NULL;

	cNetwork::cNetwork()
	{

	}

	bool cNetwork::Init()
	{
		if(SDLNet_Init()==-1) 
		{
			std::ostringstream t;
			t<<"SDLNet_Init: "<<SDLNet_GetError();
			pLog->Error("cNetwork::cNetwork", t.str());

			return false;
		}

		pLog->Success("cNetwork", "Init");

		return true;
	}

	/*
		Uint32 number;

		// put my number into a data buffer to prepare for sending to a remote host
		char out[1024];
		number=0xDEADBEEF;
		SDLNet_Write32(number, out);




		// get a number from a data buffer to use on this host
		char in[1024];
		char *ptr=&in[40]; //this points into a previously received data buffer
		number=0;
		number=SDLNet_Read32(ptr);
		// number is now in your hosts byte order, ready to use. 
	*/

	cNetwork::~cNetwork()
	{
		if(strlen(SDLNet_GetError()))
		{
			std::ostringstream t;
			t<<"SDLNet_GetError: "<<SDLNet_GetError();
			pLog->Error("cNetwork", t.str());
		}

		SDLNet_Quit();

		pLog->Success("cNetwork", "Shutdown");
	}
}
