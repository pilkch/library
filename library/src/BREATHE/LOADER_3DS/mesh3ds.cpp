#include "BREATHE/LOADER_3DS/mesh3ds.h"
#include "BREATHE/LOADER_3DS/build3ds.h"

#include <iostream>
#include <sstream>
using namespace std;

#include <BREATHE/cLog.h>

namespace BREATHE
{
	namespace LOADER_3DS
	{
		Mesh3DS::Mesh3DS()
		{
			bFoundMeshes=false;
		}

		Mesh3DS::~Mesh3DS()
		{

		}

		void Mesh3DS::Parse(const string &name , Model3DSChunk c)
		{
			bFoundMeshes=true;

#ifdef DEBUG3DS
			std::ostringstream t;
			t<<"Mesh3DS::Parse(" << name.c_str() << ")";
			pLog->Success("c3ds", t.str());
#endif //DEBUG3DS

			Mesh3DSObject mesh(name , c);
			meshes.push_back(mesh);
		}

		const std::vector<Mesh3DSObject> &Mesh3DS::Meshes()
		{
			return meshes;
		}
	}
}