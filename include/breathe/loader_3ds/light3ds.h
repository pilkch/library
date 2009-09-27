#ifndef LIGHT3DS_H
#define LIGHT3DS_H

#include <string>
#include <vector>

#include "chunk.h"

#include "light3dsobject.h"

namespace breathe
{
  namespace loader_3ds
  {
    class Light3DS
    {
    public:
      Light3DS();
      ~Light3DS();

      void Parse(const string_t& name, Model3DSChunk c);

    private:
      std::vector<Light3DSObject> lights;
    };
  }
}

#endif
