#ifndef CSTATICMODELLOADER_H
#define CSTATICMODELLOADER_H

#include <breathe/render/model/cStatic.h>

namespace breathe
{
  namespace render
  {
    namespace model
    {
      class cStaticModelLoader
      {
      public:
        bool Load(const string_t& sFilename, cStaticModel& model) const;
      };

      class cStaticModelWriter
      {
      public:
        bool Save(const string_t& sFilename, const cStaticModel& model) const;
      };
    }
  }
}

#endif // CSTATICMODELLOADER_H
