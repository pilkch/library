
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

#include <breathe/loader_3ds/material3ds.h>
#include <breathe/loader_3ds/build3ds.h>


namespace breathe
{
  namespace loader_3ds
  {
    Material3DS::Material3DS()
    {

    }
    Material3DS::~Material3DS()
    {

    }

    void Material3DS::Parse(Model3DSChunk c)
    {
#ifdef DEBUG3DS
      LOG.Success("3ds", "Edit material");
#endif

      for (Model3DSChunk cc = c.Child(); cc.IsValid(); cc = cc.Sibling()) {
        switch(cc.ID())
        {
          case(0xa000):
            NewMaterial(cc);
          break;

          default:
          break;
        }
      }
    }

    void Material3DS::NewMaterial(Model3DSChunk c)
    {
      string_t mat_name = c.Str();

#ifdef DEBUG3DS
      if (mat_name.find(".mat") != string_t::npos) LOG.Success("3ds", "Material: %s", mat_name.c_str());
      else LOG.Error("3ds", "Invalid material: %s", mat_name.c_str());
#endif

      materials.push_back(mat_name);
    }
  }
}
