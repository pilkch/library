#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cmath>

#include <list>
#include <vector>
#include <map>

// writing on a text file
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/GLee.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>


// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec2.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cPlane.h>
#include <breathe/math/cQuaternion.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cOctree.h>
#include <breathe/math/cColour.h>




#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>

namespace breathe
{
	namespace render
	{
		namespace model
		{
      // *** cMeshData

			cMeshData::cMeshData() :
				uiVertexBuffer(0),
				uiIndexBuffer(0),
				uiNormalBuffer(0),
				uiTextureCoordBuffer(0),

				uiTriangles(0),
				uiTextures(0)
			{				
			}

			cMeshData::~cMeshData()
			{
				glDeleteBuffersARB(1, &uiVertexBuffer);
				glDeleteBuffersARB(1, &uiIndexBuffer);
				glDeleteBuffersARB(1, &uiTextureCoordBuffer);
				glDeleteBuffersARB(1, &uiNormalBuffer);
			}
			
			void cMeshData::CloneTo(cMeshData* rhs)
			{
				rhs->uiVertexBuffer = 0;
				rhs->uiIndexBuffer = 0;
				rhs->uiNormalBuffer = 0;
				rhs->uiTextureCoordBuffer = 0;

				rhs->uiTriangles = uiTriangles;
				rhs->uiTextures = uiTextures;

				rhs->vIndex = vIndex;

				rhs->vVertex = vVertex;
				rhs->vNormal = vNormal;
				rhs->vTextureCoord = vTextureCoord;
			}

			void cMeshData::CreateVBO()
			{
				glEnable(GL_TEXTURE_2D);


				glGenBuffersARB(1, &uiVertexBuffer);
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, uiVertexBuffer);
				glBufferDataARB(GL_ARRAY_BUFFER_ARB, vVertex.size()*sizeof(float), &vVertex[0], GL_STATIC_DRAW_ARB );
				
				glGenBuffersARB(1, &uiIndexBuffer);
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, uiIndexBuffer);
				glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vIndex.size()*sizeof(unsigned int), &vIndex[0], GL_STATIC_DRAW_ARB );

				glGenBuffersARB(1, &uiTextureCoordBuffer);
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, uiTextureCoordBuffer);
				glBufferDataARB(GL_ARRAY_BUFFER_ARB, vTextureCoord.size()*sizeof(float), &vTextureCoord[0], GL_STATIC_DRAW_ARB );

				glGenBuffersARB(1, &uiNormalBuffer);
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, uiNormalBuffer);
				glBufferDataARB(GL_ARRAY_BUFFER_ARB, vNormal.size()*sizeof(float), &vNormal[0], GL_STATIC_DRAW_ARB );
			}


      // *** cMesh

			cMesh::cMesh() :
				pMeshData(NULL),
				pMaterial(NULL)
			{
			}

			void cMesh::CreateNewMesh()
			{
				pMeshData = new cMeshData();
			}

			void cMesh::CloneTo(cMesh* rhs)
			{
				rhs->pMeshData = new cMeshData();
				pMeshData->CloneTo(rhs->pMeshData);
				rhs->pMaterial = pMaterial;
				rhs->sMaterial = sMaterial;
			}
			
			void cMesh::SetMaterial(material::cMaterial* pInMaterial)
			{
				assert(pMaterial);

				sMaterial = pInMaterial->sName;
				pMaterial = pInMaterial;
			}
			
			void cMesh::SetMaterial(const std::string& sInMaterial)
			{
				sMaterial = sInMaterial;
				pMaterial = pRender->GetMaterial(sMaterial);
			}
		}
	}
}
