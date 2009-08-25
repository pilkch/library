#ifndef CSTATICMODELLOADER_H
#define CSTATICMODELLOADER_H

namespace breathe
{
  namespace render
  {
    namespace model
    {
      class cStaticModelMesh
      {
      public:
        void Clear();

        std::vector<float_t> vertices;
        std::vector<float_t> textureCoordinates;
        std::vector<float_t> normals;
        std::vector<size_t> indices;

        string_t sMaterial;
      };

      class cStaticModel
      {
      public:
        ~cStaticModel() { Clear(); }

        void Clear();

        std::vector<cStaticModelMesh*> mesh;
      };

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
