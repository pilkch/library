#ifndef CMESH_H
#define CMESH_H

#include <breathe/breathe.h>

#include <spitfire/util/string.h>

namespace breathe
{
  namespace render
  {
    namespace material
    {
      class cMaterial;
      typedef std::shared_ptr<cMaterial> cMaterialRef;
    }


    namespace model
    {
      class cMesh;
      typedef std::shared_ptr<cMesh> cMeshRef;

      //uiTextures  |  uiTriangles  fVerticies  vNormals  fTexCoords
      //------------+---------------------------------------------
      //1            |  n            n*9          n*9        n*6
      //2            |  n            n*9          n*9        n*12
      //3            |  n            n*9          n*9        n*18

      class cMeshData
      {
      public:
        cMeshData();
        ~cMeshData();

        void CopyFrom(const cMeshData& rhs);
        void CreateVBO();

        void WeldVerticesWithinThreshold(float fThreshold);

        unsigned int uiVertexBuffer;
        unsigned int uiIndexBuffer;
        unsigned int uiNormalBuffer;
        unsigned int uiTextureCoordBuffer;

        unsigned int uiTriangles;
        unsigned int uiTextures;

        std::vector<unsigned int> vIndex;

        std::vector<float> vVertex;
        std::vector<float> vNormal;
        std::vector<float> vTextureCoord;
      };

      // Static Mesh
      class cMesh
      {
      public:
        cMesh();

        void CreateNewMesh();

        void CopyFrom(const cMeshRef rhs);

        void SetMaterial(material::cMaterialRef pMaterial);
        void SetMaterial(const string_t& sMaterial);

        cMeshData* pMeshData;

        string_t sMaterial;
        material::cMaterialRef pMaterial;
      };
    }
  }
}

#endif // CMESH_H

