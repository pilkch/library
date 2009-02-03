#ifndef MATERIAL3DS_H
#define MATERIAL3DS_H

#include <vector>

#include "chunk.h"

namespace breathe
{
  namespace loader_3ds
  {
    class Material3DS
    {
    public:
      Material3DS();
      ~Material3DS();

      void Parse(Model3DSChunk c);

      std::vector<string_t> materials;

    private:
      void NewMaterial(Model3DSChunk c);
    };
  }
}

#endif
