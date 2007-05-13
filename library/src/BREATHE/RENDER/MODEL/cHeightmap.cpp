#include <cstdio>
#include <cmath>

#include <list>
#include <vector>
#include <map>
#include <fstream>
#include <string>

#include <GL/Glee.h>

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

#include <BREATHE/UTIL/cBase.h>
#include <BREATHE/UTIL/cFileSystem.h>

#include <BREATHE/RENDER/cTexture.h>
#include <BREATHE/RENDER/cTextureAtlas.h>
#include <BREATHE/RENDER/cMaterial.h>
#include <BREATHE/RENDER/cRender.h>
#include <BREATHE/RENDER/cVertexBufferObject.h>

#include <BREATHE/RENDER/MODEL/cMesh.h>
#include <BREATHE/RENDER/MODEL/cModel.h>
#include <BREATHE/RENDER/MODEL/cStatic.h>
#include <BREATHE/RENDER/MODEL/cHeightmapPatch.h>
#include <BREATHE/RENDER/MODEL/cHeightmap.h>

namespace BREATHE
{
	namespace RENDER
	{
		namespace MODEL
		{
			cHeightmap::cHeightmap()
			{
				uiTriangles = 0;

				// How many tiles in each direction
				uiWidth = 0;
				uiHeight = 0;

				// Width of each tile
				fWidth = 1.0f;
				fHeight = 1.0f;

				// Vertical scale
				fScale = 1.0f;

				pHeight = NULL;
				pVBO = NULL;

				pMaterial = NULL;
			}

			cHeightmap::~cHeightmap()
			{
				SAFE_DELETE(pVBO);
				SAFE_DELETE(pHeight);

				pMaterial = NULL;
			}
			
			int cHeightmap::Load(std::string sFilename)
			{
				pMaterial = pRender->AddMaterial("grass.mat");

				sFilename=pFileSystem->FindFile("data/level/node00/heightmap.png");

				{
					cTexture* pTexture = new cTexture();
					if(pTexture->Load(sFilename) == BREATHE::BAD)
					{
						LOG.Error("Heightmap", "Failed to load " + sFilename);
						return 0;
					}

					uiWidth = pTexture->uiWidth;
					uiHeight = pTexture->uiHeight;
					uiTriangles = uiWidth * uiHeight * 2;

					pHeight = new float[(uiWidth + 1) * (uiHeight + 1)];
					
					float fRolling = 0.3f;

					unsigned int uiCount = 0;
					for(unsigned int h = 0; h < uiHeight; h++)
					{
						for(unsigned int w = 0; w < uiWidth; w++)
						{
							pHeight[w + (h * (uiWidth + 1))] = fScale * pTexture->pData[uiCount++];
						}
					}

					// Set the last extra row on the end
					for(unsigned int h = 0; h < uiHeight; h++)
					{
						pHeight[uiWidth + (h * uiWidth) + h] = fScale*(sinf(fRolling*static_cast<float>(uiWidth)) + 
							cosf(fRolling*static_cast<float>(h)));
					}
					
					// Set the last extra row on the bottom
					for(unsigned int w = 0; w < uiWidth; w++)
					{
						pHeight[(uiWidth * (uiHeight + 1)) + w] = fScale*(sinf(fRolling*static_cast<float>(w)) + 
							cosf(fRolling*static_cast<float>(uiHeight)));
					}
					
					// Set the last extra element
					pHeight[(uiWidth + 1) * (uiHeight + 1)] = fScale*(sinf(fRolling*static_cast<float>(uiWidth+1)) + 
							cosf(fRolling*static_cast<float>(uiHeight+1)));
				}

				float* p = &pHeight[0];

				pVBO = pRender->AddVertexBufferObject();

				unsigned int uiVBOStrideWidth = 8;
				unsigned int uiVBOStrideHeight = 8;
				unsigned int uiVBOWidth = uiWidth/uiVBOStrideWidth;
				unsigned int uiVBOHeight = uiHeight/uiVBOStrideHeight;
				
				float fHalfWidth = static_cast<float>(uiWidth) * 0.5f;
				float fHalfHeight = static_cast<float>(uiHeight) * 0.5f;
				float fHTW = fWidth * 0.5f * uiVBOStrideWidth;
				float fHTH = fHeight * 0.5f * uiVBOStrideHeight;
				float fHTW2 = fWidth * uiVBOStrideWidth;
				float fHTH2 = fHeight * uiVBOStrideHeight;

				for(unsigned int w = 0; w < uiVBOWidth; w++)
				{
					for(unsigned int h = 0; h < uiVBOHeight; h++)
					{
						float fX = (static_cast<float>(w * uiVBOStrideWidth) - fHalfWidth) * fWidth;
						float fY = (static_cast<float>(h * uiVBOStrideHeight) - fHalfHeight) * fHeight;

						vVertex.push_back(MATH::cVec3(fX - fHTW, fY - fHTH, Height(fX, fY)));
						vVertex.push_back(MATH::cVec3(fX + fHTW, fY - fHTH, Height(fX + fHTW2, fY)));
						vVertex.push_back(MATH::cVec3(fX + fHTW, fY + fHTH, Height(fX + fHTW2, fY + fHTH2)));
						vVertex.push_back(MATH::cVec3(fX + fHTW, fY + fHTH, Height(fX + fHTW2, fY + fHTH2)));
						vVertex.push_back(MATH::cVec3(fX - fHTW, fY + fHTH, Height(fX, fY + fHTH2)));
						vVertex.push_back(MATH::cVec3(fX - fHTW, fY - fHTH, Height(fX, fY)));

						vNormal.push_back(MATH::cVec3(0.0f, 0.0f, 1.0f));
						vNormal.push_back(MATH::cVec3(0.0f, 0.0f, 1.0f));
						vNormal.push_back(MATH::cVec3(0.0f, 0.0f, 1.0f));
						vNormal.push_back(MATH::cVec3(0.0f, 0.0f, 1.0f));
						vNormal.push_back(MATH::cVec3(0.0f, 0.0f, 1.0f));
						vNormal.push_back(MATH::cVec3(0.0f, 0.0f, 1.0f));


						// Base texture
						vTextureCoord.push_back(MATH::cVec2(0.0f, 0.0f));
						vTextureCoord.push_back(MATH::cVec2(1.0f, 0.0f));
						vTextureCoord.push_back(MATH::cVec2(1.0f, 1.0f));
						vTextureCoord.push_back(MATH::cVec2(1.0f, 1.0f));
						vTextureCoord.push_back(MATH::cVec2(0.0f, 1.0f));
						vTextureCoord.push_back(MATH::cVec2(0.0f, 0.0f));

						// Detail texture
						vTextureCoord.push_back(MATH::cVec2(0.0f, 0.0f));
						vTextureCoord.push_back(MATH::cVec2(1.0f, 0.0f));
						vTextureCoord.push_back(MATH::cVec2(1.0f, 1.0f));
						vTextureCoord.push_back(MATH::cVec2(1.0f, 1.0f));
						vTextureCoord.push_back(MATH::cVec2(0.0f, 1.0f));
						vTextureCoord.push_back(MATH::cVec2(0.0f, 0.0f));
					}
				}

				pVBO->pVertex.SetData(vVertex);
				pVBO->pNormal.SetData(vNormal);
				pVBO->pTextureCoord.SetData(vTextureCoord);

				pVBO->Init();

				return 0;
			}

			unsigned int cHeightmap::Render()
			{
				//pRender->ClearMaterial();
				pRender->SetMaterial(pMaterial);
          
				pVBO->Render();

				return uiTriangles;
			}


			void cHeightmap::Update(float time)
			{	
				
			}
		}
	}
}
