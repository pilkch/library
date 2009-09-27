#include <cstdio>
#include <cmath>
#include <cassert>

#include <list>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

// Boost includes
#include <boost/shared_ptr.hpp>

#include <GL/GLee.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/storage/filesystem.h>

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

#include <breathe/util/base.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>
#include <breathe/render/cVertexBufferObject.h>

#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>
#include <breathe/render/model/cWater.h>

namespace breathe
{
  namespace render
  {
    namespace model
    {
      void cWater::Create()
      {
        const size_t n = 256;
        const size_t fScale = 2.0f;
        const float fDiffuseTextureScale = 10.0f * (1.0f / float(n));
        const float fDetailRepeat = 2.0f;
        const float fPositionZ = 80.0f;

        const spitfire::math::cVec3 normal(0.0f, 0.0f, 1.0f);

        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> textureCoordinates;
        std::vector<uint16_t> indices;

        for (size_t y = 0; y < n; y++) {
          for (size_t x = 0; x < n; x++) {
            const float fX = float(x) * fScale;
            const float fY = float(y) * fScale;

            textureCoordinates.push_back(float(x) * fDiffuseTextureScale);
            textureCoordinates.push_back(float(y) * fDiffuseTextureScale);
            textureCoordinates.push_back(0.0f);
            textureCoordinates.push_back(0.0f);
            vertices.push_back(fX);
            vertices.push_back(fY);
            vertices.push_back(fPositionZ);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            textureCoordinates.push_back((float(x) + 1.0f) * fDiffuseTextureScale);
            textureCoordinates.push_back(float(y) * fDiffuseTextureScale);
            textureCoordinates.push_back(fDetailRepeat);
            textureCoordinates.push_back(0.0f);
            vertices.push_back(fX + fScale);
            vertices.push_back(fY);
            vertices.push_back(fPositionZ);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            textureCoordinates.push_back((float(x) + 1.0f) * fDiffuseTextureScale);
            textureCoordinates.push_back((float(y) + 1.0f) * fDiffuseTextureScale);
            textureCoordinates.push_back(fDetailRepeat);
            textureCoordinates.push_back(fDetailRepeat);
            vertices.push_back(fX + fScale);
            vertices.push_back(fY + fScale);
            vertices.push_back(fPositionZ);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            textureCoordinates.push_back(float(x) * fDiffuseTextureScale);
            textureCoordinates.push_back((float(y) + 1.0f) * fDiffuseTextureScale);
            textureCoordinates.push_back(0.0f);
            textureCoordinates.push_back(fDetailRepeat);
            vertices.push_back(fX);
            vertices.push_back(fY + fScale);
            vertices.push_back(fPositionZ);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
          }
        }

        vbo.SetVertices(vertices);
        vbo.SetNormals(normals);
        vbo.SetTextureCoordinates(textureCoordinates);
        //vbo.SetIndices(indices);
        vbo.Compile();


        const spitfire::string_t sFilename(TEXT("materials/water.mat"));
        pRender->AddMaterial(sFilename);
        pMaterial = pRender->GetMaterial(sFilename);
      }

      void cWater::Render(spitfire::sampletime_t currentTime)
      {
        render::ApplyMaterial apply(pMaterial);

        float fCurrentTime = float(currentTime);
        pRender->SetShaderConstant("fCurrentTime", fCurrentTime);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

          glDisable(GL_CULL_FACE);

            vbo.Bind();

              vbo.RenderQuads();

            vbo.Unbind();

          glEnable(GL_CULL_FACE);

        glDisable(GL_BLEND);
      }
    }
  }
}
