#ifndef CMODEL_STATIC_H
#define CMODEL_STATIC_H

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
			class cStatic : public cModel, protected math::cOctree
			{
			private:
				int Load3DS(std::string sFilename);

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

				cMesh *pCurrentMesh;

				bool bFoundMeshes, bFoundVertices, bFoundTextureCoords, bFoundMaterials;

			public:
				std::vector<cMesh*>vMesh; //A vector of all the meshes in this model

				cStatic();
				~cStatic();

				int Load(std::string sFilename);
				void Update(sampletime_t currentTime);
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