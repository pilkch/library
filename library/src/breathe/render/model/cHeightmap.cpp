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

#include <breathe/game/cLevel.h>

namespace breathe
{
  namespace render
  {
    namespace model
    {
      cHeightmap::cHeightmap()
      {
        uiTriangles = 0;

        // How many tiles in each direction
        uiWidth = 0;
        uiHeight = 0;

        // Width of each tile
        fWidthOfTile = 1.0f;
        fHeightOfTile = 1.0f;

        // Vertical scale
        fScale = 0.5f;

        pNormal = nullptr;
        pHeight = nullptr;
      }

      cHeightmap::~cHeightmap()
      {
        SAFE_DELETE(pHeight);
        SAFE_DELETE_ARRAY(pNormal);
      }

      int cHeightmap::Load(const string_t& /*sFilename*/)
      {
        SetDimensions(pLevel->fNodeWidth, pLevel->fNodeWidth, 1.0f);

        sMaterial = TEXT("grass.mat");
        pMaterial = pRender->AddMaterial(sMaterial);

        string_t sFilename;
        breathe::filesystem::FindResourceFile(TEXT("level/node00/heightmap.png"), sFilename);

        float fHighest = -math::cINFINITY;
        float fLowest = math::cINFINITY;

        // Load heightmap
        {
          cTextureRef pTexture(new cTexture);
          if (pTexture->Load(sFilename) == breathe::BAD) {
            LOG.Error("Heightmap", "Failed to load " + breathe::string::ToUTF8(sFilename));
            return 0;
          }

          uiWidth = pTexture->uiWidth;
          uiHeight = pTexture->uiHeight;

          pHeight = new float[(uiWidth + 1) * (uiHeight + 1)];

          float fRolling = 10.3f;

          unsigned int uiCount = 0;
          for (unsigned int h = 0; h < uiHeight; h++) {
            for (unsigned int w = 0; w < uiWidth; w++) {
              pHeight[w + (h * (uiWidth + 1))] = fScale * pTexture->data[uiCount++];
              //pHeight[w + (h * (uiWidth + 1))] = fScale*(sinf(fRolling*static_cast<float>(w)) +
              //  cosf(fRolling*static_cast<float>(h)));

              if (pHeight[w + (h * (uiWidth + 1))] > fHighest) fHighest = pHeight[w + (h * (uiWidth + 1))];
              if (pHeight[w + (h * (uiWidth + 1))] < fLowest) fLowest = pHeight[w + (h * (uiWidth + 1))];
            }
          }

          // Set the last extra row on the end
          for (unsigned int h = 0; h < uiHeight; h++)
          {
            pHeight[uiWidth + (h * uiWidth) + h] = fScale * (sinf(fRolling * static_cast<float>(uiWidth)) +
              cosf(fRolling * static_cast<float>(h)));
          }

          // Set the last extra row on the bottom
          for (unsigned int w = 0; w < uiWidth; w++)
          {
            pHeight[(uiWidth * (uiHeight + 1)) + w] = fScale * (sinf(fRolling * static_cast<float>(w)) +
              cosf(fRolling * static_cast<float>(uiHeight)));
          }

          // Set the last extra element
          pHeight[(uiWidth + 1) * (uiHeight + 1)] = fScale * (sinf(fRolling * static_cast<float>(uiWidth+1)) +
              cosf(fRolling * static_cast<float>(uiHeight+1)));
        }




        unsigned int uiVBOStrideWidth = 8;
        unsigned int uiVBOStrideHeight = 8;

        fWidthOfTile = pLevel->fNodeWidth / uiWidth / uiVBOStrideWidth;
        fHeightOfTile = pLevel->fNodeWidth / uiHeight / uiVBOStrideHeight;

        unsigned int uiVBOWidth = uiWidth/uiVBOStrideWidth;
        unsigned int uiVBOHeight = uiHeight/uiVBOStrideHeight;

        float fHalfWidth = static_cast<float>(uiWidth) * 0.5f;
        float fHalfHeight = static_cast<float>(uiHeight) * 0.5f;
        float fHTW = fWidthOfTile * 0.5f * uiVBOWidth;
        float fHTH = fHeightOfTile * 0.5f * uiVBOHeight;
        float fHTW2 = fWidthOfTile * uiVBOWidth;
        float fHTH2 = fHeightOfTile * uiVBOHeight;


        // Fill out normals
        {
          pNormal = new math::cVec3[(uiWidth + 1) * (uiHeight + 1)];

          std::vector<math::cVec3>* normal_buffer = new std::vector<math::cVec3>[(uiWidth + 1) * (uiHeight + 1)];

          for (unsigned int h = 0; h < uiHeight; h++)
          {
            for (unsigned int w = 0; w < uiWidth; w++)
            {
              // get the three vertices that make the faces
              math::cVec3 p1(fWidthOfTile * w, fHeightOfTile * h, pHeight[w + (h * (uiWidth + 1))]);
              math::cVec3 p2(fWidthOfTile * (w + 1), fHeightOfTile * h, pHeight[w + 1 + (h * (uiWidth + 1))]);
              math::cVec3 p3(fWidthOfTile * w, fHeightOfTile * (h + 1), pHeight[w + ((h + 1) * (uiWidth + 1))]);

              math::cVec3 v1 = p2 - p1;
              math::cVec3 v2 = p3 - p1;
              math::cVec3 normal = v1.CrossProduct( v2 );

              // Store the face's normal for each of the vertices that make up the face.
              normal_buffer[w + (h * (uiWidth + 1))].push_back( normal );
              normal_buffer[w + 1 + (h * (uiWidth + 1))].push_back( normal );
              normal_buffer[w + ((h + 1) * (uiWidth + 1))].push_back( normal );
            }
          }

          // Now loop through each vertex vector, and average out all the normals stored.
          unsigned int i = 0;

          for (unsigned int h = 0; h < uiHeight; h++)
          {
            for (unsigned int w = 0; w < uiWidth; w++)
            {
              i = w + (h * (uiWidth + 1));
              for (unsigned int j = 0; j < normal_buffer[i].size(); ++j )
                pNormal[i] += normal_buffer[i][j];

                                                                                pNormal[i].Normalise();
            }
          }

          SAFE_DELETE_ARRAY(normal_buffer);
        }

        // Create VBO
        //float* p = &pHeight[0];

        pVBO = pRender->AddVertexBufferObject();

        for (unsigned int w = 0; w < uiWidth; w+=uiVBOStrideWidth)
        {
          for (unsigned int h = 0; h < uiHeight; h+=uiVBOStrideHeight)
          {
            float fX = (static_cast<float>(w * uiVBOStrideWidth) - fHalfWidth) * fWidthOfTile;
            float fY = (static_cast<float>(h * uiVBOStrideHeight) - fHalfHeight) * fHeightOfTile;

            vVertex.push_back(math::cVec3(fX - fHTW, fY - fHTH, Height(fX, fY)));
            vVertex.push_back(math::cVec3(fX + fHTW, fY - fHTH, Height(fX + fHTW2, fY)));
            vVertex.push_back(math::cVec3(fX + fHTW, fY + fHTH, Height(fX + fHTW2, fY + fHTH2)));
            vVertex.push_back(math::cVec3(fX + fHTW, fY + fHTH, Height(fX + fHTW2, fY + fHTH2)));
            vVertex.push_back(math::cVec3(fX - fHTW, fY + fHTH, Height(fX, fY + fHTH2)));
            vVertex.push_back(math::cVec3(fX - fHTW, fY - fHTH, Height(fX, fY)));

            vNormal.push_back(Normal(fX, fY));
            vNormal.push_back(Normal(fX + fHTW2, fY));
            vNormal.push_back(Normal(fX + fHTW2, fY + fHTH2));
            vNormal.push_back(Normal(fX + fHTW2, fY + fHTH2));
            vNormal.push_back(Normal(fX, fY + fHTH2));
            vNormal.push_back(Normal(fX, fY));
          }
        }


        // Stretched Base Texture
        if (!pMaterial->vLayer.empty())
        {
          cTextureRef pTexture(pMaterial->vLayer[0]->pTexture);
          if (pTexture != nullptr) {
            LOG.Success("Heightmap", "Adding base texture coordinates");

            // Now at the detail texture coordinates so that they are after the base texture coordinates
            for (unsigned int w = 0; w < uiWidth; w+=uiVBOStrideWidth)
            {
              for (unsigned int h = 0; h < uiHeight; h+=uiVBOStrideHeight)
              {
                float u1 = (float(w)) / float(uiWidth);
                float v1 = (float(h)) / float(uiHeight);
                float u2 = (float(w + uiVBOStrideWidth)) / float(uiWidth);
                float v2 = (float(h + uiVBOStrideHeight)) / float(uiHeight);

                pTexture->Transform(u1, v1);
                pTexture->Transform(u2, v2);

                vTextureCoord.push_back(math::cVec2(u1, v1));
                vTextureCoord.push_back(math::cVec2(u2, v1));
                vTextureCoord.push_back(math::cVec2(u2, v2));
                vTextureCoord.push_back(math::cVec2(u2, v2));
                vTextureCoord.push_back(math::cVec2(u1, v2));
                vTextureCoord.push_back(math::cVec2(u1, v1));
              }
            }
          }
        }

        // Detail Texture
        if (pMaterial->vLayer.size() > 1) {
          cTextureRef pDetailTexture(pMaterial->vLayer[1]->pTexture);
          if (pDetailTexture) {
            float u1 = 0.0f;
            float v1 = 0.0f;
            float u2 = 1.0f;
            float v2 = 1.0f;

            pDetailTexture->Transform(u1, v1);
            pDetailTexture->Transform(u2, v2);

            LOG.Success("Heightmap", "Adding detail texture coordinates");

            // Now at the detail texture coordinates so that they are after the base texture coordinates
            for (unsigned int w = 0; w < uiWidth; w+=uiVBOStrideWidth)
            {
              for (unsigned int h = 0; h < uiHeight; h+=uiVBOStrideHeight)
              {
                vTextureCoord.push_back(math::cVec2(u1, v1));
                vTextureCoord.push_back(math::cVec2(u2, v1));
                vTextureCoord.push_back(math::cVec2(u2, v2));
                vTextureCoord.push_back(math::cVec2(u2, v2));
                vTextureCoord.push_back(math::cVec2(u1, v2));
                vTextureCoord.push_back(math::cVec2(u1, v1));
              }
            }
          }
        }

        uiTriangles = uiVBOWidth * uiVBOHeight * 2;

        pVBO->pVertex.SetData(vVertex);
        pVBO->pNormal.SetData(vNormal);
        pVBO->pTextureCoord.SetData(vTextureCoord);

        pVBO->Init();

        SetDimensions(pLevel->fNodeWidth, pLevel->fNodeWidth, fHighest);

        return 0;
      }

      unsigned int cHeightmap::Render()
      {
        if (pMaterial && pVBO)
        {
          pRender->SetMaterial(pMaterial);
          pVBO->Render();
        }

        pRender->RenderWireframeBox(math::cVec3(-fWidth, -fLength, -fHeight), math::cVec3(fWidth, fLength, fHeight));

        return uiTriangles;
      }


      void cHeightmap::Update(sampletime_t currentTime)
      {

      }
    }
  }
}
