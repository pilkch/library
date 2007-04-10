#include "BREATHE/LOADER_3DS/mesh3ds.h"
#include "BREATHE/LOADER_3DS/build3ds.h"

// writing on a text file
#include <iostream>
#include <fstream>

#include <sstream>

#include <BREATHE/UTIL/cLog.h>

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

		void Mesh3DS::Parse(const std::string &name , Model3DSChunk c)
		{
			bFoundMeshes=true;

#ifdef DEBUG3DS
			pLog->Success("c3ds", std::string("Mesh3DS::Parse(") + name + ")");
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