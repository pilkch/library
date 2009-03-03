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
#include <SDL/SDL_opengl.h>
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
#include <breathe/render/model/cHeightmapPatch.h>
#include <breathe/render/model/cHeightmap.h>

namespace breathe
{
  namespace render
  {
    namespace model
    {
      // *** cTerrain

      void cTerrain::Create(const cTerrainHeightMapLoader& loader)
      {
        // NOTE: We don't want to access outside the buffer so go one less than the size of the buffer
        const size_t n = 511;
        const size_t fScale = 1.0f;
        const float fDiffuseTextureScale = 1.0f / float(n);
        const float fDetailRepeat = 2.0f;

        math::cVec3 normal;

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
            vertices.push_back(loader.GetHeight(fX, fY));
            normal = loader.GetNormal(fX, fY);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            textureCoordinates.push_back((float(x) + 1.0f) * fDiffuseTextureScale);
            textureCoordinates.push_back(float(y) * fDiffuseTextureScale);
            textureCoordinates.push_back(fDetailRepeat);
            textureCoordinates.push_back(0.0f);
            vertices.push_back(fX + fScale);
            vertices.push_back(fY);
            vertices.push_back(loader.GetHeight(fX + fScale, fY));
            normal = loader.GetNormal(fX + fScale, fY);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            textureCoordinates.push_back((float(x) + 1.0f) * fDiffuseTextureScale);
            textureCoordinates.push_back((float(y) + 1.0f) * fDiffuseTextureScale);
            textureCoordinates.push_back(fDetailRepeat);
            textureCoordinates.push_back(fDetailRepeat);
            vertices.push_back(fX + fScale);
            vertices.push_back(fY + fScale);
            vertices.push_back(loader.GetHeight(fX + fScale, fY + fScale));
            normal = loader.GetNormal(fX + fScale, fY + fScale);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            textureCoordinates.push_back(float(x) * fDiffuseTextureScale);
            textureCoordinates.push_back((float(y) + 1.0f) * fDiffuseTextureScale);
            textureCoordinates.push_back(0.0f);
            textureCoordinates.push_back(fDetailRepeat);
            vertices.push_back(fX);
            vertices.push_back(fY + fScale);
            vertices.push_back(loader.GetHeight(fX, fY + fScale));
            normal = loader.GetNormal(fX, fY + fScale);
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


        const spitfire::string_t sFilename(TEXT("materials/terrain.mat"));
        pRender->AddMaterial(sFilename);
        pMaterial = pRender->GetMaterial(sFilename);
      }

      void cTerrain::Update(spitfire::sampletime_t currentTime)
      {
      }

      void cTerrain::Render(spitfire::sampletime_t currentTime)
      {
        pRender->SetMaterial(pMaterial);

        vbo.Bind();

          vbo.RenderQuads();

        vbo.Unbind();
      }



      // *** cGrass

      void cGrass::Create(const cTerrainHeightMapLoader& loader, float fOffsetX, float fOffsetY, float fWidth, float fHeight)
      {
        const size_t fTerrainScale = 10.0f;
        const size_t nDensity = 10;
        const size_t nStartX = fOffsetX / fTerrainScale;
        const size_t nStartY = fOffsetY / fTerrainScale;
        const size_t nEndX = (fOffsetX + fWidth) / fTerrainScale;
        const size_t nEndY = (fOffsetY + fHeight) / fTerrainScale;

        const float fMinimumSize = 2.0f;
        const float fMaximumSize = 5.0f;

        const float fMinimumGrassAltitude = 100.0f;

        std::vector<float> vertices;
        std::vector<float> textureCoordinates;
        std::vector<uint16_t> indices;

        for (size_t y = nStartX; y < nEndX; y++) {
          for (size_t x = nStartY; x < nEndY; x++) {
            for (size_t i = 0; i < nDensity; i++) {
              const float fX = (float(x) * fTerrainScale) + (fTerrainScale * spitfire::math::randomMinusOneToPlusOnef());
              const float fY = (float(y) * fTerrainScale) + (fTerrainScale * spitfire::math::randomMinusOneToPlusOnef());
              const float fZ = loader.GetHeight(fX, fY);
              if (fZ < fMinimumGrassAltitude) continue;

              const float fHalfWidth = spitfire::math::randomf(fMinimumSize, fMaximumSize);
              const float fHeight = fHalfWidth + fHalfWidth;

              float fRotationZDegrees = (180.0f * spitfire::math::randomMinusOneToPlusOnef());

              const size_t nRotationsPerElement = 6;
              const float fRotationPerElement = 360.0f / float(nRotationsPerElement);

              for (size_t i = 0; i < nRotationsPerElement; i++, fRotationZDegrees += fRotationPerElement) {
                spitfire::math::cQuaternion rotation;
                rotation.SetFromAxisAngle(spitfire::math::v3Up, spitfire::math::DegreesToRadians(fRotationZDegrees));

                const spitfire::math::cMat4 mat = rotation.GetMatrix();

                spitfire::math::cVec3 left(-fHalfWidth, 0.0f, 0.0f);
                left = mat.GetRotatedVec3(left);

                spitfire::math::cVec3 right(fHalfWidth, 0.0f, 0.0f);
                right = mat.GetRotatedVec3(right);

                textureCoordinates.push_back(0.0f);
                textureCoordinates.push_back(0.0f);
                vertices.push_back(fX + left.x);
                vertices.push_back(fY + left.y);
                vertices.push_back(fZ + fHeight);

                textureCoordinates.push_back(1.0f);
                textureCoordinates.push_back(0.0f);
                vertices.push_back(fX + right.x);
                vertices.push_back(fY + right.y);
                vertices.push_back(fZ + fHeight);

                textureCoordinates.push_back(1.0f);
                textureCoordinates.push_back(1.0f);
                vertices.push_back(fX + right.x);
                vertices.push_back(fY + right.y);
                vertices.push_back(fZ);

                textureCoordinates.push_back(0.0f);
                textureCoordinates.push_back(1.0f);
                vertices.push_back(fX + left.x);
                vertices.push_back(fY + left.y);
                vertices.push_back(fZ);
              }
            }
          }
        }

        vbo.SetVertices(vertices);
        vbo.SetTextureCoordinates(textureCoordinates);
        //vbo.SetIndices(indices);
        vbo.Compile();


        const spitfire::string_t sFilename(TEXT("materials/vegetation_grass.mat"));
        pRender->AddMaterial(sFilename);
        pMaterial = pRender->GetMaterial(sFilename);
      }

      void cGrass::Update(spitfire::sampletime_t currentTime)
      {
      }

      void cGrass::Render(spitfire::sampletime_t currentTime)
      {
        pRender->SetMaterial(pMaterial);

        vbo.Bind();

          vbo.RenderQuads();

        vbo.Unbind();
      }
    }
  }
}
