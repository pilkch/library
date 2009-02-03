// writing on a text file
#include <iostream>
#include <fstream>

#include <list>
#include <vector>
#include <string>
#include <sstream>

// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>

#include <breathe/loader_3ds/light3ds.h>
#include <breathe/loader_3ds/build3ds.h>

namespace breathe
{
  namespace loader_3ds
  {
    void Light3DS::Parse(const string_t& name, Model3DSChunk c)
    {
      LOG.Error("c3ds", "object light");
    }
  }
}
