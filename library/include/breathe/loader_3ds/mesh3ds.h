#ifndef MESH3DS_H
#define MESH3DS_H

#include <string>
#include <vector>

#include "chunk.h"

#include "mesh3dsobject.h"

namespace breathe
{
  namespace loader_3ds
  {
    class Mesh3DS
    {
    public:
      Mesh3DS();
      ~Mesh3DS();

      void Parse(const std::string &name , Model3DSChunk c);

      const std::vector<Mesh3DSObject> &Meshes();

      bool bFoundMeshes;

    private:
      std::vector<Mesh3DSObject> meshes;
    };
  }
}

#endif
