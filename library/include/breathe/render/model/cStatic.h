#ifndef CMODEL_STATIC_H
#define CMODEL_STATIC_H

#include <breathe/math/cOctree.h>

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
				int Load3DS(const std::string& sFilename);

				void ParseEditor3D(loader_3ds::Model3DSChunk c, std::string sFilename);
				void ParseEditObject(loader_3ds::Model3DSChunk c, std::string sFilename);

				void ParseMesh(const std::string &name, loader_3ds::Model3DSChunk c, std::string sFilename);
				void ParseMaterial(loader_3ds::Model3DSChunk c);

				void ParseCamera(loader_3ds::Model3DSChunk c);
				void ParseLight(const std::string &name, loader_3ds::Model3DSChunk c);

				void NewMaterial(loader_3ds::Model3DSChunk c);

				/*
				std::vector<loader_3ds::Light3DSObject> lights;
				std::vector<loader_3ds::Mesh3DSObject> meshes;*/
				std::vector<std::string> vMaterial;

				unsigned int uiCurrentMesh;

				float fScale;

				cMeshRef pCurrentMesh;

				bool bFoundMeshes, bFoundVertices, bFoundTextureCoords, bFoundMaterials;

			public:
				std::vector<cMeshRef>vMesh; //A vector of all the meshes in this model

				cStatic();
				~cStatic();

				int Load(const std::string& sFilename);
				void Update(sampletime_t currentTime);
				unsigned int Render();

				unsigned int Render(cOctree* pNode);

				//void Split(unsigned int uiMesh, cModel_Static** pDest, unsigned int uiDest); //Which mesh to subtract, destination model
				void CloneTo(cStaticRef rhs);

				cMeshRef GetMesh(unsigned int index);
      };

      typedef cSmartPtr<cStatic> cStaticRef;
		}
	}
}

#endif // CMODEL_STATIC_H
