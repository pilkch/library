#ifndef CMODEL_STATIC_H
#define CMODEL_STATIC_H

#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>

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

        spitfire::string_t sMaterial;
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
