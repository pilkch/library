#ifndef CMODEL_STATIC_H
#define CMODEL_STATIC_H

//TODO: USE OCTREE, NO MATTER WHAT, EVEN IF THERE ARE 2 TRIANGLES AND THEY BOTH GO IN THE ROOT NODE

namespace breathe
{
	namespace LOADER_3DS
	{
		class Model3DSChunk;
		class Light3DSObject;
		class Mesh3DSObject;
	}

	namespace RENDER
	{
		namespace MODEL
		{
			class cStatic : public cModel, protected math::cOctree
			{
			private:
				int Load3DS(std::string sFilename);

				void ParseEditor3D(LOADER_3DS::Model3DSChunk c, std::string sFilename);	
				void ParseEditObject(LOADER_3DS::Model3DSChunk c, std::string sFilename);
				
				void ParseMesh(const std::string &name, LOADER_3DS::Model3DSChunk c, std::string sFilename);
				void ParseMaterial(LOADER_3DS::Model3DSChunk c);

				void ParseCamera(LOADER_3DS::Model3DSChunk c);
				void ParseLight(const std::string &name, LOADER_3DS::Model3DSChunk c);

				void NewMaterial(LOADER_3DS::Model3DSChunk c);

				/*
				std::vector<LOADER_3DS::Light3DSObject> lights;
				std::vector<LOADER_3DS::Mesh3DSObject> meshes;*/
				std::vector<std::string> vMaterial;

				unsigned int uiCurrentMesh;

				float fScale;

				cMesh *pCurrentMesh;

				bool bFoundMeshes, bFoundVertices, bFoundTextureCoords, bFoundMaterials;

			public:
				std::vector<cMesh*>vMesh; //A vector of all the meshes in this model

				cStatic();
				~cStatic();

				int Load(std::string sFilename);
				void Update(float fCurrentTime);
				unsigned int Render();
				
				unsigned int Render(cOctree *pNode);

				//void Split(unsigned int uiMesh, cModel_Static **pDest, unsigned int uiDest); //Which mesh to subtract, destination model
				void CloneTo(cStatic* rhs);

				cMesh* GetMesh(unsigned int index);
			};
		}
	}
}

#endif //CMODEL_STATIC_H