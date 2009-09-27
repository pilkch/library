#ifndef CMODELWATER_H
#define CMODELWATER_H

#include <breathe/render/cVertexBufferObject.h>

namespace breathe
{
  namespace render
  {
    namespace model
    {
      class cWater
      {
      public:
        void Create();

        void Render(spitfire::sampletime_t currentTime);

      private:
        breathe::render::cStaticVertexBuffer vbo;

        breathe::render::material::cMaterialRef pMaterial;
      };
    }
  }
}

#endif // CMODELWATER_H
