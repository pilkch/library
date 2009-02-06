#ifndef CMODEL_STATIC_H
#define CMODEL_STATIC_H

#include <spitfire/math/cOctree.h>

#include <breathe/breathe.h>

#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>

//TODO: USE OCTREE, NO MATTER WHAT, EVEN IF THERE ARE 2 TRIANGLES AND THEY BOTH GO IN THE ROOT NODE

namespace breathe
{
  namespace loader_3ds
  {
    class Model3DSChunk;
    class Light3DSObject;
    class Mesh3DSObject;
  }

  namespace render
  {
    namespace model
    {
      class cStatic;
      typedef cSmartPtr<cStatic> cStaticRef;

      class cStatic : public cModel, protected math::cOctree
      {
      private:
        int Load3DS(const string_t& sFilename);

        void ParseEditor3D(loader_3ds::Model3DSChunk c, const string_t& sFilename);
        void ParseEditObject(loader_3ds::Model3DSChunk c, const string_t& sFilename);

        void ParseMesh(const string_t& name, loader_3ds::Model3DSChunk c, const string_t& sFilename);
        void ParseMaterial(loader_3ds::Model3DSChunk c);

        void ParseCamera(loader_3ds::Model3DSChunk c);
        void ParseLight(const string_t& name, loader_3ds::Model3DSChunk c);

        void NewMaterial(loader_3ds::Model3DSChunk c);

        /*
        std::vector<loader_3ds::Light3DSObject> lights;
        std::vector<loader_3ds::Mesh3DSObject> meshes;*/
        std::vector<string_t> vMaterial;

        unsigned int uiCurrentMesh;

        float fScale;

        cMeshRef pCurrentMesh;

        bool bFoundMeshes, bFoundVertices, bFoundTextureCoords, bFoundMaterials;

      public:
        std::vector<cMeshRef>vMesh; //A vector of all the meshes in this model

        cStatic();
        ~cStatic();

        int Load(const string_t& sFilename);
        void Update(sampletime_t currentTime);
        size_t Render();

        size_t Render(cOctree* pNode);

        //void Split(unsigned int uiMesh, cModel_Static** pDest, unsigned int uiDest); //Which mesh to subtract, destination model
        void CopyFrom(const cStaticRef rhs);

        cMeshRef GetMesh(unsigned int index);
      };

      typedef cSmartPtr<cStatic> cStaticRef;
    }
  }
}

#endif // CMODEL_STATIC_H
