#ifndef CMESH_H
#define CMESH_H

namespace BREATHE
{
	namespace RENDER
	{
		namespace MATERIAL
		{
			class cMaterial;	
		}

		
		namespace MODEL
		{
			//Static Mesh
			class cMesh
			{
			public:
				unsigned int uiVertexBuffer;
				unsigned int uiIndexBuffer;
				unsigned int uiNormalBuffer;
				unsigned int uiTextureCoordBuffer;

				unsigned int uiTriangles;
				unsigned int uiTextures;

				MATH::cMat4 m4Mat;

				std::vector<unsigned int> vIndex;

				std::vector<float> vVertex;
				std::vector<float> vNormal;
				std::vector<float> vTextureCoord;
				
				std::string sMaterial;
				
				RENDER::MATERIAL::cMaterial* pMaterial;

				cMesh();
				~cMesh();

				void CreateVBO();

				//uiTextures	|	uiTriangles	fVerticies	vNormals	fTexCoords
				//------------+---------------------------------------------
				//1						|	n						n*9					n*9				n*6
				//2						|	n						n*9					n*9				n*12				
				//3						|	n						n*9					n*9				n*18
			};
		}
	}
}

#endif //CMESH_H