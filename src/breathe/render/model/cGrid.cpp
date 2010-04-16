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

#include <breathe/render/cContext.h>
#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cVertexBufferObject.h>

#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>
#include <breathe/render/model/cGrid.h>

namespace breathe
{
  namespace render
  {
    namespace model
    {
      // Grid with fixed resolution: meters
      // In the future:
      // if (distance from grid to camera < 20m) resolution = 10cm, thick lines at 1m
      // else resolution = 1m, thick lines at 10m
      //
      // Each line is at 100m, thick lines at 1000m
      // Each line is at 10m, thick lines at 100m
      // Each line is at 1m, thick lines at 10m

      cGridRenderer::cGridRenderer() :
        distanceBetweenEachLineInMeters(10)
      {
      }

      void cGridRenderer::Create()
      {
        std::vector<float> vertices;

        // Lighter lines
        // There is one extra for the border
        for (size_t i = 0; i < 100; i++) {
          // x axis line
          vertices.push_back(0.0f);
          vertices.push_back(float_t(i));
          vertices.push_back(0.0f);

          vertices.push_back(100.0f);
          vertices.push_back(float_t(i));
          vertices.push_back(0.0f);

          // y axis line
          vertices.push_back(float_t(i));
          vertices.push_back(0.0f);
          vertices.push_back(0.0f);

          vertices.push_back(float_t(i));
          vertices.push_back(100.0f);
          vertices.push_back(0.0f);
        }

        vboLight.SetVertices(vertices);
        vboLight.Compile();

        vertices.clear();

        // Solid lines
        // 11 is one extra for the border
        for (size_t i = 0; i < 11; i++) {
          // x axis line
          vertices.push_back(0.0f);
          vertices.push_back(10.0f * float_t(i));
          vertices.push_back(0.0f);

          vertices.push_back(100.0f);
          vertices.push_back(10.0f * float_t(i));
          vertices.push_back(0.0f);

          // y axis line
          vertices.push_back(10.0f * float_t(i));
          vertices.push_back(0.0f);
          vertices.push_back(0.0f);

          vertices.push_back(10.0f * float_t(i));
          vertices.push_back(100.0f);
          vertices.push_back(0.0f);
        }

        vboSolid.SetVertices(vertices);
        vboSolid.Compile();
      }

      void cGridRenderer::SetResolutionInMeters(size_t resolutionInMeters)
      {
        switch (resolutionInMeters) {
          case 1:
          case 10:
          case 100: {
            distanceBetweenEachLineInMeters = resolutionInMeters;
            break;
          }
          default:
            distanceBetweenEachLineInMeters = 10;
        }

        SetPosition(spitfire::math::v3Zero);
      }

      void cGridRenderer::SetPosition(const spitfire::math::cVec3& _position)
      {
        ASSERT(distanceBetweenEachLineInMeters != 0);

        const float fRemainderX = float(int(_position.x) % int(distanceBetweenEachLineInMeters));
        const float fRemainderY = float(int(_position.y) % int(distanceBetweenEachLineInMeters));

        // z position is always purposely ignored
        position.Set(
          fRemainderX - (50.0f * float_t(distanceBetweenEachLineInMeters)),
          fRemainderY - (50.0f * float_t(distanceBetweenEachLineInMeters)),
          0.0f
        );
      }

      void cGridRenderer::Render(spitfire::sampletime_t currentTime)
      {
        pContext->RenderAxisReference();

        glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);

          pContext->SetColour(0.0f, 1.0f, 0.0f);

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
              glTranslatef(position.x, position.y, position.z);
              glScalef(float(distanceBetweenEachLineInMeters), float(distanceBetweenEachLineInMeters), float(distanceBetweenEachLineInMeters));

              glLineWidth(1);
              vboLight.Bind();

                vboLight.RenderLines();

              vboLight.Unbind();

              glLineWidth(3);
              vboSolid.Bind();

                vboSolid.RenderLines();

              vboSolid.Unbind();

              glLineWidth(1);
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();

          pContext->ClearColour();

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);
      }
    }
  }
}
