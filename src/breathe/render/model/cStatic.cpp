#include <cmath>

#include <vector>
#include <string>

#include <breathe/render/model/cStatic.h>

namespace breathe
{
  namespace render
  {
    namespace model
    {
      // *** cStaticModelMesh

      void cStaticModelMesh::Clear()
      {
        vertices.clear();
        normals.clear();
        indices.clear();

        sMaterial.clear();
      }

      void cStaticModel::Clear()
      {
        const size_t n = mesh.size();
        for (size_t i = 0; i < n; i++) spitfire::SAFE_DELETE(mesh[i]);

        mesh.clear();
      }
    }
  }
}
