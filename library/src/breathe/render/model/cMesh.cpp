#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cmath>

#include <list>
#include <vector>
#include <map>

#include <iostream>
#include <fstream>
#include <sstream>

// Boost includes
#include <boost/shared_ptr.hpp>

#include <GL/GLee.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/geometry.h>

// Breathe headers
#include <breathe/breathe.h>

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

      void cMeshData::CopyFrom(const cMeshData& rhs)
      {
        uiVertexBuffer = 0;
        uiIndexBuffer = 0;
        uiNormalBuffer = 0;
        uiTextureCoordBuffer = 0;

        uiTriangles = rhs.uiTriangles;
        uiTextures = rhs.uiTextures;

        vIndex = rhs.vIndex;

        vVertex = rhs.vVertex;
        vNormal = rhs.vNormal;
        vTextureCoord = rhs.vTextureCoord;
      }

      void cMeshData::CreateVBO()
      {
        glEnable(GL_TEXTURE_2D);


        glGenBuffersARB(1, &uiVertexBuffer);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, uiVertexBuffer);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, vVertex.size() * sizeof(float), &vVertex[0], GL_STATIC_DRAW_ARB );

        glGenBuffersARB(1, &uiIndexBuffer);
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, uiIndexBuffer);
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vIndex.size() * sizeof(unsigned int), &vIndex[0], GL_STATIC_DRAW_ARB );

        glGenBuffersARB(1, &uiTextureCoordBuffer);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, uiTextureCoordBuffer);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, vTextureCoord.size() * sizeof(float), &vTextureCoord[0], GL_STATIC_DRAW_ARB );

        glGenBuffersARB(1, &uiNormalBuffer);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, uiNormalBuffer);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, vNormal.size() * sizeof(float), &vNormal[0], GL_STATIC_DRAW_ARB );
      }


      // *** cMesh

      cMesh::cMesh() :
        pMeshData(nullptr)
      {
      }

      void cMesh::CreateNewMesh()
      {
        pMeshData = new cMeshData;
      }

      void cMesh::CopyFrom(const cMeshRef rhs)
      {
        ASSERT(rhs != nullptr);
        pMeshData = new cMeshData;
        pMeshData->CopyFrom(*rhs->pMeshData);
        sMaterial = rhs->sMaterial;
        pMaterial = rhs->pMaterial;
      }

      void cMesh::SetMaterial(material::cMaterialRef pInMaterial)
      {
        ASSERT(pMaterial);

        sMaterial = pInMaterial->sName;
        pMaterial = pInMaterial;
      }

      void cMesh::SetMaterial(const string_t& sInMaterial)
      {
        sMaterial = sInMaterial;
        pMaterial = pRender->GetMaterial(sMaterial);
      }
    }
  }
}
