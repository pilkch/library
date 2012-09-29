#ifndef CMODEL_STATIC_H
#define CMODEL_STATIC_H

// Spitfire headers
#include <spitfire/util/string.h>

// Breathe headers
#include <breathe/breathe.h>

namespace breathe
{
  namespace render
  {
    namespace model
    {
      class cStaticModelMesh
      {
      public:
        void Clear();

        std::vector<float_t> vertices;
        std::vector<float_t> textureCoordinates;
        std::vector<float_t> normals;
        std::vector<size_t> indices;

        string_t sMaterial;
      };


      class cStaticModel
      {
      public:
        ~cStaticModel() { Clear(); }

        void Clear();

        std::vector<cStaticModelMesh*> mesh;
      };
    }
  }
}

#endif // CMODEL_STATIC_H
