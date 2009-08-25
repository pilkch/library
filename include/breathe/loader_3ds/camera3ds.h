#ifndef CAMERA3DS_H
#define CAMERA3DS_H

#include <vector>

#include "chunk.h"

namespace breathe
{
  namespace render
  {
    class cCamera;
  }

  namespace loader_3ds
  {
    class Camera3DS
    {
    public:
      Camera3DS();
      ~Camera3DS();

      void Parse(Model3DSChunk c);

      std::vector<render::cCamera*> vCamera;
    };
  }
}

#endif // CAMERA3DS_H
