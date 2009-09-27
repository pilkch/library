#ifndef LIGHT3DSOBJECT_H
#define LIGHT3DSOBJECT_H

#include <spitfire/util/cString.h>

#include <breathe/breathe.h>

namespace breathe
{
  namespace loader_3ds
  {
    class Light3DSObject
    {
    public:
      explicit Light3DSObject(const string_t& nname);
      ~Light3DSObject();

    private:
      string_t name;
    };
  }
}

#endif
