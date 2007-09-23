#include <string>

// Breathe
#include <breathe/breathe.h>

#include "breathe/loader_3ds/light3dsobject.h"
#include "breathe/loader_3ds/build3ds.h"

namespace breathe
{
	namespace LOADER_3DS
	{
		Light3DSObject::Light3DSObject(const std::string &nname)
		: name(nname)
		{

		}
		Light3DSObject::~Light3DSObject()
		{

		}
	}
}