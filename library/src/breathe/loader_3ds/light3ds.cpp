#include <iostream>
#include <fstream>

#include <list>
#include <vector>
#include <string>
#include <sstream>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

// Breathe headers
#include <breathe/breathe.h>

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
