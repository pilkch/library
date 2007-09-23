#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include <list>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <memory>

// writing on a text file
#include <iostream>
#include <fstream>

#include <GL/Glee.h>

// Breathe
#include <BREATHE/cBreathe.h>
#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/UTIL/cFileSystem.h>

#include <BREATHE/MATH/cMath.h>
#include <BREATHE/MATH/cVec2.h>
#include <BREATHE/MATH/cVec3.h>
#include <BREATHE/MATH/cVec4.h>
#include <BREATHE/MATH/cMat4.h>
#include <BREATHE/MATH/cPlane.h>
#include <BREATHE/MATH/cQuaternion.h>
#include <BREATHE/MATH/cFrustum.h>
#include <BREATHE/MATH/cColour.h>

#include <BREATHE/UTIL/cBase.h>

#include <BREATHE/RENDER/cVertexBufferObject.h>

namespace BREATHE
{
	namespace RENDER
	{
		#define BUFFER_OFFSET(i) ((char *)NULL + (i))

		cVertexBufferObject::cVertexBufferObject()
		{
			uiVertices = 0;

			uiOffsetTextureUnit0 = 0;
			uiOffsetTextureUnit1 = 0;
			uiOffsetTextureUnit2 = 0;

			bufferID = NULL;
		}

		cVertexBufferObject::~cVertexBufferObject()
		{
			uiVertices = 0;

			Destroy();
		}

		void cVertexBufferObject::Destroy()
		{
			if(bufferID)
			{
				glBindBufferARB(GL_ARRAY_BUFFER, 0);
				glDeleteBuffersARB(1, &bufferID);
			}
		}

		void cVertexBufferObject::Init()
		{
			// Create a big array that holds all of the data
			// Set the buffer data to this big array
			// Assign offsets for our arrays
			uiVertices = pVertex.vData.size();

			unsigned int uiVertexSize = pVertex.vData.size() * sizeof(MATH::cVec3);
			unsigned int uiTextureCoordSize = pTextureCoord.vData.size() * sizeof(MATH::cVec2);
			unsigned int uiNormalSize = pNormal.vData.size() * sizeof(MATH::cVec3);
			
			pVertex.uiOffset = 0 + 0;
			pTextureCoord.uiOffset = pVertex.uiOffset + uiVertexSize;
			pNormal.uiOffset = pTextureCoord.uiOffset + uiTextureCoordSize;
			
			uiOffsetTextureUnit0 = pTextureCoord.uiOffset;
			if(pTextureCoord.vData.size() > uiVertices)
			{
				uiOffsetTextureUnit1 = uiOffsetTextureUnit0 + (uiVertices * sizeof(MATH::cVec2));

				if(pTextureCoord.vData.size()/2 > uiVertices)
					uiOffsetTextureUnit2 = uiOffsetTextureUnit1 + (uiVertices * sizeof(MATH::cVec2));
			}

			std::vector<float>vData;
			unsigned int i = 0;
			/*vData.resize(	uiVertexSize + uiTextureCoordSize + uiNormalSize);
			memcpy(&vData[pVertex.uiOffset],				&pVertex.vData[0], uiVertexSize);
			memcpy(&vData[pTextureCoord.uiOffset],	&pTextureCoord.vData[0], uiTextureCoordSize);
			memcpy(&vData[pNormal.uiOffset],				&pNormal.vData[0], uiNormalSize);*/

			for (i =0;i<pVertex.vData.size(); i++)
			{
				vData.push_back(pVertex.vData[i].x);
				vData.push_back(pVertex.vData[i].y);
				vData.push_back(pVertex.vData[i].z);
			}
			for (i =0;i<pTextureCoord.vData.size(); i++)
			{
				vData.push_back(pTextureCoord.vData[i].u);
				vData.push_back(pTextureCoord.vData[i].v);
			}
			for (i =0;i<pNormal.vData.size(); i++)
			{
				vData.push_back(pNormal.vData[i].x);
				vData.push_back(pNormal.vData[i].y);
				vData.push_back(pNormal.vData[i].z);
			}

			size_t x = vData.size();
			//glGenBuffersARB(1, &index_buf);
			//glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, index_buf);
			//glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, I_SIZ*sizeof(GLushort), tet_index, GL_STATIC_DRAW_ARB);

			glGenBuffersARB(1, &bufferID);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, bufferID);
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, vData.size() * sizeof(float), &vData[0], GL_STATIC_DRAW_ARB);
		}
		
		unsigned int cVertexBufferObject::Render()
		{
			// TODO: Call this only once at start of rendering?  Not per vbo?
			
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, bufferID);

			// Index Array
			//glEnableClientState( GL_NORMAL_ARRAY );
			//glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, index_buf);
			// Don't need this apparently? glIndexPointer(GL_UNSIGNED_SHORT, 0, BUFFER_OFFSET(pIndex.uiOffset));
		
			glEnableClientState( GL_NORMAL_ARRAY );
			glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(pNormal.uiOffset));

			glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(uiOffsetTextureUnit0));

			if(uiOffsetTextureUnit1)
			{
				glClientActiveTextureARB(GL_TEXTURE1_ARB);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(uiOffsetTextureUnit1));
				
				if(uiOffsetTextureUnit2)
				{
					glClientActiveTextureARB(GL_TEXTURE2_ARB);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(uiOffsetTextureUnit2));
				}
			}
			

			glEnableClientState( GL_VERTEX_ARRAY );
			glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(pVertex.uiOffset));

				glDrawArrays(GL_TRIANGLES, 0, uiVertices);
				//glDrawRangeElements( GL_TRIANGLES, 0, uiIndicies, uiIndicies, GL_UNSIGNED_INT, BUFFER_OFFSET(pIndex.uiOffset));
				//glDrawElements(GL_TRIANGLES, uiIndicies, GL_UNSIGNED_SHORT, BUFFER_OFFSET(pIndex.uiOffset));
		
			glDisableClientState( GL_VERTEX_ARRAY );
			
			if(uiOffsetTextureUnit1)
			{
				if(uiOffsetTextureUnit2)
				{
					glClientActiveTexture( GL_TEXTURE2 );
					glDisableClientState( GL_TEXTURE_COORD_ARRAY );
				}

				glClientActiveTexture( GL_TEXTURE1 );
				glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			}

			glClientActiveTexture( GL_TEXTURE0 );
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );

			glDisableClientState( GL_NORMAL_ARRAY );

			return 0;
		}
	}
}
