// writing on a text file
#include <iostream>
#include <fstream>

#include <list>

// Breathe
#include <BREATHE/cBreathe.h>

#include <BREATHE/UTIL/cLog.h>

#include "BREATHE/LOADER_3DS/light3ds.h"
#include "BREATHE/LOADER_3DS/build3ds.h"



namespace BREATHE
{
	namespace LOADER_3DS
	{
		Light3DS::Light3DS()
		{

		}
		Light3DS::~Light3DS()
		{

		}

		void Light3DS::Parse(const std::string &name , Model3DSChunk c)
		{
			LOG.Error("c3ds", "object light");
		}
	}
}