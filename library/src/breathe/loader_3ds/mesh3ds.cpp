#include <iostream>
#include <fstream>

#include <list>
#include <vector>
#include <sstream>


// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

// Breathe headers
#include <breathe/breathe.h>

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

    void Mesh3DS::Parse(const string_t& name , Model3DSChunk c)
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
