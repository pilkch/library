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

// Boost headers
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

// OpenGL headers
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
#include <breathe/render/cResourceManager.h>
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

      void cTerrain::Create(const game::cTerrainHeightMap& loader, const size_t nWidthOrHeight)
      {
        ASSERT(nWidthOrHeight != 0);

        // NOTE: We don't want to access outside the buffer so go one less than the dimensions of the buffer
        const size_t n = nWidthOrHeight - 1;
        const float fScale = 512.0f / n;
        const float fDiffuseTextureScale = 1.0f / float(n);
        const float fDetailRepeat = 0.25f;
        const float fHalfTerrainWidthOrHeight = 256.0f;

        math::cVec3 normal;

        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> textureCoordinates;
        std::vector<uint16_t> indices;

        for (size_t y = 0; y < n; y++) {
          for (size_t x = 0; x < n; x++) {
            const float fX = float(x) * fScale;
            const float fY = float(y) * fScale;

            // Triangle 0

            textureCoordinates.push_back(float(x) * fDiffuseTextureScale);
            textureCoordinates.push_back(float(y) * fDiffuseTextureScale);
            textureCoordinates.push_back(0.0f);
            textureCoordinates.push_back(0.0f);
            vertices.push_back(-fHalfTerrainWidthOrHeight + fX);
            vertices.push_back(-fHalfTerrainWidthOrHeight + fY);
            vertices.push_back(loader.GetHeight(fX, fY));
            normal = loader.GetNormal(fX, fY);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            textureCoordinates.push_back((float(x) + 1.0f) * fDiffuseTextureScale);
            textureCoordinates.push_back(float(y) * fDiffuseTextureScale);
            textureCoordinates.push_back(fDetailRepeat);
            textureCoordinates.push_back(0.0f);
            vertices.push_back(-fHalfTerrainWidthOrHeight + fX + fScale);
            vertices.push_back(-fHalfTerrainWidthOrHeight + fY);
            vertices.push_back(loader.GetHeight(fX + fScale, fY));
            normal = loader.GetNormal(fX + fScale, fY);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            textureCoordinates.push_back(float(x) * fDiffuseTextureScale);
            textureCoordinates.push_back((float(y) + 1.0f) * fDiffuseTextureScale);
            textureCoordinates.push_back(0.0f);
            textureCoordinates.push_back(fDetailRepeat);
            vertices.push_back(-fHalfTerrainWidthOrHeight + fX);
            vertices.push_back(-fHalfTerrainWidthOrHeight + fY + fScale);
            vertices.push_back(loader.GetHeight(fX, fY + fScale));
            normal = loader.GetNormal(fX, fY + fScale);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);


            // Triangle 1

            textureCoordinates.push_back((float(x) + 1.0f) * fDiffuseTextureScale);
            textureCoordinates.push_back(float(y) * fDiffuseTextureScale);
            textureCoordinates.push_back(fDetailRepeat);
            textureCoordinates.push_back(0.0f);
            vertices.push_back(-fHalfTerrainWidthOrHeight + fX + fScale);
            vertices.push_back(-fHalfTerrainWidthOrHeight + fY);
            vertices.push_back(loader.GetHeight(fX + fScale, fY));
            normal = loader.GetNormal(fX + fScale, fY);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            textureCoordinates.push_back((float(x) + 1.0f) * fDiffuseTextureScale);
            textureCoordinates.push_back((float(y) + 1.0f) * fDiffuseTextureScale);
            textureCoordinates.push_back(fDetailRepeat);
            textureCoordinates.push_back(fDetailRepeat);
            vertices.push_back(-fHalfTerrainWidthOrHeight + fX + fScale);
            vertices.push_back(-fHalfTerrainWidthOrHeight + fY + fScale);
            vertices.push_back(loader.GetHeight(fX + fScale, fY + fScale));
            normal = loader.GetNormal(fX + fScale, fY + fScale);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);

            textureCoordinates.push_back(float(x) * fDiffuseTextureScale);
            textureCoordinates.push_back((float(y) + 1.0f) * fDiffuseTextureScale);
            textureCoordinates.push_back(0.0f);
            textureCoordinates.push_back(fDetailRepeat);
            vertices.push_back(-fHalfTerrainWidthOrHeight + fX);
            vertices.push_back(-fHalfTerrainWidthOrHeight + fY + fScale);
            vertices.push_back(loader.GetHeight(fX, fY + fScale));
            normal = loader.GetNormal(fX, fY + fScale);
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
          }
        }


        pVBO.reset(new render::cVertexBufferObject);

        pVBO->SetVertices(vertices);
        pVBO->SetNormals(normals);
        pVBO->SetTextureCoordinates(textureCoordinates);
        //pVBO->SetIndices(indices);
        pVBO->Compile();


        const spitfire::string_t sFilename(TEXT("materials/terrain.mat"));
        pResourceManager->AddMaterial(sFilename);
        pMaterial = pResourceManager->GetMaterial(sFilename);
      }

      void cTerrain::Update(spitfire::durationms_t currentTime)
      {
      }

      void cTerrain::Render(spitfire::durationms_t currentTime)
      {
        render::ApplyMaterial apply(pMaterial);

        pVBO->Bind();

          pVBO->RenderQuads();

        pVBO->Unbind();
      }



      // *** cGrassStatic

      void cGrassStatic::Create(const game::cTerrainHeightMap& loader, float fOffsetX, float fOffsetY, float fWidth, float fHeight)
      {
        const size_t fTerrainScale = 10.0f;
        const size_t nDensity = 4;
        const size_t nStartX = fOffsetX / fTerrainScale;
        const size_t nStartY = fOffsetY / fTerrainScale;
        const size_t nEndX = (fOffsetX + fWidth) / fTerrainScale;
        const size_t nEndY = (fOffsetY + fHeight) / fTerrainScale;

        const float fMinimumSize = 1.0f;
        const float fMaximumSize = 2.0f;

        //const float fMinimumGrassAltitude = 100.0f;

        std::vector<float> vertices;
        std::vector<float> textureCoordinates;
        std::vector<uint16_t> indices;

        for (size_t y = nStartX; y < nEndX; y++) {
          for (size_t x = nStartY; x < nEndY; x++) {
            for (size_t i = 0; i < nDensity; i++) {
              const float fX = (float(x) * fTerrainScale) + (fTerrainScale * spitfire::math::randomMinusOneToPlusOnef());
              const float fY = (float(y) * fTerrainScale) + (fTerrainScale * spitfire::math::randomMinusOneToPlusOnef());
              const float fZ = loader.GetHeight(fX, fY);
              //if (fZ < fMinimumGrassAltitude) continue;

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
        pResourceManager->AddMaterial(sFilename);
        pMaterial = pResourceManager->GetMaterial(sFilename);
      }

      void cGrassStatic::Render(spitfire::durationms_t currentTime)
      {
        render::ApplyMaterial apply(pMaterial);

        vbo.Bind();

          vbo.RenderQuads();

        vbo.Unbind();
      }


      // *** cGrassAnimated

      void cGrassAnimated::Create(const game::cTerrainHeightMap& loader, float fOffsetX, float fOffsetY, float fWidth, float fHeight)
      {
        const size_t fTerrainScale = 10.0f;
        const size_t nDensity = 4;
        const size_t nStartX = fOffsetX / fTerrainScale;
        const size_t nStartY = fOffsetY / fTerrainScale;
        const size_t nEndX = (fOffsetX + fWidth) / fTerrainScale;
        const size_t nEndY = (fOffsetY + fHeight) / fTerrainScale;

        const float fMinimumSize = 1.0f;
        const float fMaximumSize = 2.0f;

        //const float fMinimumGrassAltitude = 100.0f;

#if 1
        const size_t nSegments = 8;
        const float segmentBendMultipliersXY[nSegments] = { 0.0f, 0.1f, 0.3f, 0.5f, 0.7f, 0.85f, 0.95f, 1.0f };
        const float segmentHeights[nSegments] = { 0.3f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f, 0.1f };
#else
        const size_t nSegments = 4;
        const float segmentBendMultipliersXY[nSegments] = { 0.1f, 0.4f, 0.7f, 1.0f };
        const float segmentHeights[nSegments] = { 0.2f, 0.3f, 0.2f, 0.1f };
#endif

        std::vector<float> vertices;
        std::vector<float> colours;
        std::vector<float> textureCoordinates;
        std::vector<uint16_t> indices;

        for (size_t y = nStartX; y < nEndX; y++) {
          for (size_t x = nStartY; x < nEndY; x++) {
            for (size_t i = 0; i < nDensity; i++) {
              const float fX = (float(x) * fTerrainScale) + (fTerrainScale * spitfire::math::randomMinusOneToPlusOnef());
              const float fY = (float(y) * fTerrainScale) + (fTerrainScale * spitfire::math::randomMinusOneToPlusOnef());
              const float fZ = loader.GetHeight(fX, fY);
              //if (fZ < fMinimumGrassAltitude) continue;

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


                float fPreviousBendMultiplierXY = 0.0f;
                float fPreviousZ = fZ;
                float fPreviousV = 1.0f;

                for (size_t iSegment = 0; iSegment < nSegments; iSegment++) {
                  const float fCurrentBendMultiplierXY = segmentBendMultipliersXY[iSegment];
                  const float fCurrentHeight = (segmentHeights[iSegment] * fHeight);
                  const float fCurrentV = (fPreviousV - segmentHeights[iSegment]);
                  textureCoordinates.push_back(0.0f);
                  textureCoordinates.push_back(fCurrentV);
                  colours.push_back(fCurrentBendMultiplierXY);
                  colours.push_back(fCurrentBendMultiplierXY);
                  colours.push_back(0.0f);
                  vertices.push_back(fX + left.x);
                  vertices.push_back(fY + left.y);
                  vertices.push_back(fPreviousZ + fCurrentHeight);

                  textureCoordinates.push_back(1.0f);
                  textureCoordinates.push_back(fCurrentV);
                  colours.push_back(fCurrentBendMultiplierXY);
                  colours.push_back(fCurrentBendMultiplierXY);
                  colours.push_back(0.0f);
                  vertices.push_back(fX + right.x);
                  vertices.push_back(fY + right.y);
                  vertices.push_back(fPreviousZ + fCurrentHeight);

                  textureCoordinates.push_back(1.0f);
                  textureCoordinates.push_back(fPreviousV);
                  colours.push_back(fPreviousBendMultiplierXY);
                  colours.push_back(fPreviousBendMultiplierXY);
                  colours.push_back(0.0f);
                  vertices.push_back(fX + right.x);
                  vertices.push_back(fY + right.y);
                  vertices.push_back(fPreviousZ);

                  textureCoordinates.push_back(0.0f);
                  textureCoordinates.push_back(fPreviousV);
                  colours.push_back(fPreviousBendMultiplierXY);
                  colours.push_back(fPreviousBendMultiplierXY);
                  colours.push_back(0.0f);
                  vertices.push_back(fX + left.x);
                  vertices.push_back(fY + left.y);
                  vertices.push_back(fPreviousZ);

                  fPreviousBendMultiplierXY = fCurrentBendMultiplierXY;
                  fPreviousZ += fCurrentHeight;
                  fPreviousV = fCurrentV;
                }
              }
            }
          }
        }

        vbo.SetVertices(vertices);
        vbo.SetColours(colours);
        vbo.SetTextureCoordinates(textureCoordinates);
        //vbo.SetIndices(indices);
        vbo.Compile();


        const spitfire::string_t sFilename(TEXT("materials/vegetation_grass.mat"));
        pResourceManager->AddMaterial(sFilename);
        pMaterial = pResourceManager->GetMaterial(sFilename);
      }

      void cGrassAnimated::Render(spitfire::durationms_t currentTime)
      {
        render::ApplyMaterial apply(pMaterial);

        float fCurrentTime = 0.0005f * float(currentTime);
        pContext->SetShaderConstant("time", fCurrentTime);

        vbo.Bind();

          vbo.RenderQuads();

        vbo.Unbind();
      }
    }
  }
}
