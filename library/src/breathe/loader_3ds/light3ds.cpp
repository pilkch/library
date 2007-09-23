// writing on a text file
#include <iostream>
#include <fstream>

#include <list>

// Breathe
#include <breathe/breathe.h>

#include <breathe/util/log.h>

#include "breathe/loader_3ds/light3ds.h"
#include "breathe/loader_3ds/build3ds.h"



namespace breathe
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