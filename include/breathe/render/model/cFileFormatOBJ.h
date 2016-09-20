#ifndef CFILEFORMATOBJ_H
#define CFILEFORMATOBJ_H

// Standard headers
#include <map>
#include <vector>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/render/model/cStatic.h>

namespace breathe
{
  namespace render
  {
    namespace model
    {
      class cFileFormatOBJ
      {
      public:
        bool Load(const string_t& sFilePath, cStaticModel& model) const;
        bool Save(const string_t& sFilePath, const cStaticModel& model) const;

      private:
        bool SkipJunk(const std::string& sFirstToken) const;

        bool LoadMaterialList(const string_t& sFilePath, std::map<string_t, string_t>& materials) const;
        size_t LoadMesh(const std::vector<std::string> lines, size_t i, const size_t n, std::vector<float>& vertices, std::vector<float>& textureCoordinates, std::vector<float>& normals, cStaticModelMesh& mesh) const;
      };
    }
  }
}

#endif // CFILEFORMATOBJ_H
