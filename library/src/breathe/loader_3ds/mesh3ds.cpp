
// writing on a text file
#include <iostream>
#include <fstream>

#include <list>
#include <vector>
#include <sstream>


// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>

#include <breathe/loader_3ds/mesh3ds.h>
#include <breathe/loader_3ds/build3ds.h>


namespace breathe
{
	namespace loader_3ds
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
			LOG.Success("c3ds", std::string("Mesh3DS::Parse(") + name + ")");
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
