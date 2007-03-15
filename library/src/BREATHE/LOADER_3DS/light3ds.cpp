#include "BREATHE/LOADER_3DS/light3ds.h"
#include "BREATHE/LOADER_3DS/build3ds.h"

#include <BREATHE/cLog.h>

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
			pLog->Error("c3ds", "object light");
		}
	}
}