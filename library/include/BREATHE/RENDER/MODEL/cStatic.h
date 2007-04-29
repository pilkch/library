#ifndef CMODEL_STATIC_H
#define CMODEL_STATIC_H

//TODO: USE OCTREE, NO MATTER WHAT, EVEN IF THERE ARE 2 TRIANGLES AND THEY BOTH GO IN THE ROOT NODE

namespace BREATHE
{
	namespace RENDER
	{
		class cCamera;
	}

	namespace LOADER_3DS
	{
		class Model3DSChunk;
		class Light3DSObject;
		class Mesh3DSObject;
	}

	namespace MODEL
	{
		class cStatic : public cModel, MATH::cOctree
		{
		private:
			void ParseEditor3D(LOADER_3DS::Model3DSChunk c, std::string sFilename);	
			void ParseEditObject(LOADER_3DS::Model3DSChunk c, std::string sFilename);
			int Load3DS(std::string sFilename);
			
			/*
			std::vector<LOADER_3DS::Light3DSObject> lights;
			std::vector<LOADER_3DS::Mesh3DSObject> meshes;*/
			std::vector<std::string> vMaterial;

			unsigned int uiCurrentMesh;

			float fScale;

			cMesh *pCurrentMesh;

			bool bFoundMeshes, bFoundVertices, bFoundTextureCoords, bFoundMaterials;

			void ParseMesh(const std::string &name, LOADER_3DS::Model3DSChunk c, std::string sFilename);
			void ParseMaterial(LOADER_3DS::Model3DSChunk c);
			void NewMaterial(LOADER_3DS::Model3DSChunk c);

			void ParseCamera(LOADER_3DS::Model3DSChunk c);
			void ParseLight(const std::string &name, LOADER_3DS::Model3DSChunk c);

		public:
			std::vector<cMesh*>vMesh; //A vector of all the meshes in this model

			std::vector<RENDER::cCamera *>vCamera;

			cStatic();
			~cStatic();

			int Load(std::string sFilename);
			void Update(float fTime);
			int Render();
			//void Split(unsigned int uiMesh, cModel_Static **pDest, unsigned int uiDest); //Which mesh to subtract, destination model
		};
	}
}

#endif //CMODEL_STATIC_H