#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <cmath>

#include <list>
#include <vector>
#include <map>

// writing on a text file
#include <iostream>
#include <fstream>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>


// Breathe
#include <BREATHE/cBreathe.h>
#include <BREATHE/UTIL/cLog.h>

#include <BREATHE/MATH/cMath.h>
#include <BREATHE/MATH/cVec2.h>
#include <BREATHE/MATH/cVec3.h>
#include <BREATHE/MATH/cVec4.h>
#include <BREATHE/MATH/cMat4.h>
#include <BREATHE/MATH/cPlane.h>
#include <BREATHE/MATH/cQuaternion.h>
#include <BREATHE/MATH/cFrustum.h>
#include <BREATHE/MATH/cOctree.h>
#include <BREATHE/MATH/cColour.h>




#include <BREATHE/RENDER/cTexture.h>
#include <BREATHE/RENDER/cTextureAtlas.h>
#include <BREATHE/RENDER/cMaterial.h>
#include <BREATHE/RENDER/cRender.h>

#include <BREATHE/UTIL/cBase.h>
#include <BREATHE/RENDER/MODEL/cMesh.h>
#include <BREATHE/RENDER/MODEL/cModel.h>

namespace BREATHE
{
	namespace RENDER
	{
		namespace MODEL
		{
			cMesh::cMesh()
			{
				uiVertexBuffer=0;
				uiIndexBuffer=0;
				uiNormalBuffer=0;
				uiTextureCoordBuffer=0;

				uiTriangles=0;
				uiTextures=0;

				pMaterial=NULL;
			}

			cMesh::~cMesh()
			{
				glDeleteBuffersARB(1, &uiVertexBuffer);
				glDeleteBuffersARB(1, &uiIndexBuffer);
				glDeleteBuffersARB(1, &uiTextureCoordBuffer);
				glDeleteBuffersARB(1, &uiNormalBuffer);
			}	

			void cMesh::CreateVBO()
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
		}
	}
}
