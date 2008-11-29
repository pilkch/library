#ifndef CMESH_H
#define CMESH_H

#include <breathe/util/cSmartPtr.h>

namespace breathe
{
	namespace render
	{
		namespace material
		{
      class cMaterial;
      typedef cSmartPtr<cMaterial> cMaterialRef;
		}


		namespace model
    {
      class cMesh;
      typedef cSmartPtr<cMesh> cMeshRef;

			//uiTextures	|	uiTriangles	fVerticies	vNormals	fTexCoords
			//------------+---------------------------------------------
			//1						|	n						n*9					n*9				n*6
			//2						|	n						n*9					n*9				n*12
			//3						|	n						n*9					n*9				n*18

			class cMeshData
			{
			public:
				cMeshData();
				~cMeshData();

				void CloneTo(cMeshData* rhs);
				void CreateVBO();


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

			//Static Mesh
			class cMesh
			{
			public:
				cMesh();

				void CreateNewMesh();

				void CloneTo(cMeshRef rhs);

        void SetMaterial(material::cMaterialRef pMaterial);
				void SetMaterial(const std::string& sMaterial);

				cMeshData* pMeshData;

				std::string sMaterial;
        material::cMaterialRef pMaterial;
			};
		}
	}
}

#endif // CMESH_H

