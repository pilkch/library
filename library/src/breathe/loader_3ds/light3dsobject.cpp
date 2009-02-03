#include <string>
#include <vector>

// Breathe
#include <breathe/breathe.h>

#include <breathe/loader_3ds/light3dsobject.h>
#include <breathe/loader_3ds/build3ds.h>

namespace breathe
{
  namespace loader_3ds
  {
    Light3DSObject::Light3DSObject(const string_t& nname)
    : name(nname)
    {
    }
  }
}
