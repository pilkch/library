#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cassert>

// writing on a text file
#include <iostream>
#include <fstream>
#include <sstream>

#include <list>
#include <vector>
#include <map>
#include <string>

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

#include <breathe/util/base.h>

#include <breathe/storage/filesystem.h>
#include <breathe/storage/xml.h>



#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

#include <breathe/game/cLevel.h>

namespace breathe
{
	namespace render
	{
		namespace material
		{
			cLayer::cLayer() :
				pTexture(NULL),
				uiTextureMode(TEXTURE_NONE)
			{
			}

			
			cShader::cShader() :
				bCameraPos(false),

				bTexUnit0(false),
				bTexUnit1(false),
				bTexUnit2(false),
				bTexUnit3(false),

				uiShaderVertex(0),
				uiShaderFragment(0),
				uiShaderProgram(0)
			{
			}

			
			void cShader::CheckStatusVertex()
			{
				int infologLength = 0;
				int charsWritten  = 0;

				glGetShaderiv(uiShaderVertex, GL_INFO_LOG_LENGTH, &infologLength);
				if (infologLength > 0)
				{
						char* infoLog = new char[infologLength];
						glGetShaderInfoLog(uiShaderVertex, infologLength, &charsWritten, infoLog);
						std::string sInfo(infoLog);
						if (	sInfo.find("not been successfully compiled") != std::string::npos ||
								sInfo.find("ERROR") != std::string::npos)
						{
							sInfo = string::Replace(sInfo, "\n", "<br>");
							LOG.Error("Material", std::string("Vertex Shader") + sShaderVertex + std::string(": ") + sInfo);
						}
						SAFE_DELETE_ARRAY(infoLog);
				}
			}

			void cShader::CheckStatusFragment()
			{
				int infologLength = 0;
				int charsWritten  = 0;

				glGetShaderiv(uiShaderFragment, GL_INFO_LOG_LENGTH, &infologLength);
				if (infologLength > 0)
				{
						char *infoLog = new char[infologLength];
						glGetShaderInfoLog(uiShaderFragment, infologLength, &charsWritten, infoLog);
						std::string sInfo(infoLog);
						if (	sInfo.find("not been successfully compiled") != std::string::npos ||
								sInfo.find("ERROR") != std::string::npos)
						{
							sInfo = string::Replace(sInfo, "\n", "<br>");
							LOG.Error("Material", std::string("Fragment Shader ") + sShaderFragment + ": " + sInfo);
						}
						SAFE_DELETE_ARRAY(infoLog);
				}
			}
			
			void cShader::CheckStatusProgram()
			{
				int infologLength = 0;
				int charsWritten  = 0;

				glGetProgramiv(uiShaderProgram, GL_INFO_LOG_LENGTH, &infologLength);
				if (infologLength > 0)
				{
						char *infoLog = new char[infologLength];
						glGetProgramInfoLog(uiShaderProgram, infologLength, &charsWritten, infoLog);
						std::string sInfo(infoLog);
						if (	sInfo.find("not been successfully compiled") != std::string::npos ||
								sInfo.find("Warning") != std::string::npos)
							LOG.Error("Material", std::string("Program ") + sShaderVertex + " " + sShaderFragment + ": " + infoLog);
						else
							LOG.Success("Material", std::string("Program ") + sShaderVertex + " " + sShaderFragment + ": " + infoLog);
						SAFE_DELETE_ARRAY(infoLog);
				}
			}

			void cShader::Init()
			{
				if ("" != sShaderVertex)
				{
					uiShaderVertex = glCreateShader(GL_VERTEX_SHADER);

					std::string buffer="";
					std::string line="";
					std::ifstream f(sShaderVertex.c_str());
					if (f.is_open())
					{
						while(!f.eof())
						{
							std::getline(f, line);

							buffer+=line;
							buffer+="\n";

							line="";
						};

						const char* str=buffer.c_str();
						glShaderSource(uiShaderVertex, 1, &str, NULL);
						glCompileShader(uiShaderVertex);
						
						CheckStatusVertex();
					}
					else
					{
						LOG.Error("Material", std::string("Shader not found ") + sShaderVertex);
						uiShaderVertex=0;
					}
				}

				
				if ("" != sShaderFragment)
				{
					uiShaderFragment = glCreateShader(GL_FRAGMENT_SHADER);

					std::string buffer;
					std::string line;
					std::ifstream f(sShaderFragment.c_str());
					if (f.is_open())
					{
						while(!f.eof())
						{
							std::getline(f, line);

							buffer+=line;
							buffer+="\n";
						};

						const char* str=buffer.c_str();
						glShaderSource(uiShaderFragment, 1, &str, NULL);
						glCompileShader(uiShaderFragment);
						
						CheckStatusFragment();
					}
					else
					{
						LOG.Error("Material", std::string("Shader not found ") + sShaderFragment);
						uiShaderFragment=0;
					}
				}
			
				if (uiShaderVertex || uiShaderFragment)
				{
					uiShaderProgram = glCreateProgram();

					if (uiShaderVertex)
						glAttachShader(uiShaderProgram, uiShaderVertex);

					if (uiShaderFragment)
						glAttachShader(uiShaderProgram, uiShaderFragment);

					glLinkProgram(uiShaderProgram);
					
					CheckStatusProgram();

					glUseProgram(uiShaderProgram);
					glUseProgram(NULL);

					CheckStatusProgram();
				}
			}

			void cShader::Destroy()
			{
				if (uiShaderFragment)
					glDeleteShader(uiShaderFragment);

				if (uiShaderVertex)
					glDeleteShader(uiShaderVertex);

				glDeleteProgram(uiShaderProgram);
				
				uiShaderFragment = 0;
				uiShaderVertex = 0;
				uiShaderProgram = 0;
			}


			cMaterial::cMaterial(const std::string& name) :
				bShadow_cast(true),
				bShadow_receive(true),
				bLight_receive(true),
				bLight_transmit(true),

				bCollideTrimesh(false),

				chDustR(0),
				chDustG(0), 
				chDustB(0),

				uiAudioScrape(0),
				uiAudioBounce(0),
				
				fFriction(0.0f),
				fBounce(0.0f),

				fCorrugation(0.0f),

				pShader(NULL),
				
				sName(name)
			{				
				unsigned int i=0;
				for (i=0;i<nLayers;i++)
				{
					vLayer.push_back(new cLayer());
				}
			}

			cMaterial::~cMaterial()
			{
				unsigned int i=0;
				for (i=0;i<nLayers;i++)
					SAFE_DELETE(vLayer[i]);
			}

			bool cMaterial::Load(const std::string& inFilename)
			{
				LOG.Success("Material", std::string("Looking for ") + inFilename);

				string_t sFilename = breathe::filesystem::FindFile(breathe::string::ToString_t(inFilename));

				LOG.Success("Material", std::string("Actually loading ") + breathe::string::ToUTF8(sFilename));

				string_t sPath = breathe::filesystem::GetPath(sFilename);
				
				xml::cNode root(breathe::string::ToUTF8(sFilename));
				xml::cNode::iterator iter(root);
				
				if (!iter) return breathe::BAD;

				//<material collide="true" sShaderVertex="normalmap.vert" sShaderFragment="normalmap.frag">
				//	<layer sTexture="concrete.png" uiTextureMode="TEXTURE_NORMAL" uiTextureAtlas="ATLAS_NONE"/>
				//	<layer sTexture="concrete_normalmap.png" uiTextureMode="TEXTURE_NORMAL" uiTextureAtlas="ATLAS_NONE"/>
				//</material>

				iter.FindChild("material");
				if (!iter)
				{
					LOG.Error("Material", std::string("Not Found ") + breathe::string::ToUTF8(sFilename));
					for (unsigned int i=0;i<nLayers;i++)
						vLayer[i]->pTexture = pRender->pMaterialNotFoundMaterial->vLayer[0]->pTexture;

					return breathe::BAD;
				}
				
				iter.GetAttribute("collide", bCollideTrimesh);
					
				std::string sValue;
				if (iter.GetAttribute("sShaderVertex", sValue))
				{
					if (pShader == nullptr) pShader = new cShader();
					pShader->sShaderVertex = breathe::string::ToUTF8(breathe::filesystem::FindFile(breathe::string::ToString_t(sPath), breathe::string::ToString_t(sValue)));
				}
				if (iter.GetAttribute("sShaderFragment", sValue))
				{
					if (pShader == nullptr) pShader = new cShader();
					pShader->sShaderFragment = breathe::string::ToUTF8(breathe::filesystem::FindFile(breathe::string::ToString_t(sPath), breathe::string::ToString_t(sValue)));
				}

				if (pShader)
				{
					iter.GetAttribute("cameraPos", pShader->bCameraPos);
					
					iter.GetAttribute("texUnit0", pShader->bTexUnit0);
					iter.GetAttribute("texUnit1", pShader->bTexUnit1);
					iter.GetAttribute("texUnit2", pShader->bTexUnit2);
					iter.GetAttribute("texUnit3", pShader->bTexUnit3);

					pShader->Init();
				}


				iter.FirstChild();
				
				cLayer* pLayer = NULL;
				size_t n = vLayer.size();
				size_t i = 0;
				while(iter && i < nLayers)
				{
					pLayer = vLayer[i];
					if ("layer" == iter.GetName())
					{
						std::string sTexture;
						if (iter.GetAttribute("sTexture", sTexture))
						{
							// Try in the same directory as the material
							pLayer->sTexture = breathe::string::ToUTF8(breathe::filesystem::FindFile(breathe::string::ToString_t(sPath), breathe::string::ToString_t(sTexture)));
						}

						std::string sValue;
						if (iter.GetAttribute("uiTextureMode", sValue))
						{
							if (sValue == "TEXTURE_NORMAL")						pLayer->uiTextureMode=TEXTURE_NORMAL;
							else if (sValue == "TEXTURE_MASK")					pLayer->uiTextureMode=TEXTURE_MASK;
							else if (sValue == "TEXTURE_BLEND")				pLayer->uiTextureMode=TEXTURE_BLEND;
							else if (sValue == "TEXTURE_DETAIL")				pLayer->uiTextureMode=TEXTURE_DETAIL;
							else if (sValue == "TEXTURE_CUBEMAP")			pLayer->uiTextureMode=TEXTURE_CUBEMAP;
							else if (sValue == "TEXTURE_POST_RENDER")	pLayer->uiTextureMode=TEXTURE_POST_RENDER;
						}

						unsigned int uiTextureAtlas = ATLAS_NONE;
						if (iter.GetAttribute("uiTextureAtlas", sValue))
						{
							if (sValue == "ATLAS_LANDSCAPE")			uiTextureAtlas = ATLAS_LANDSCAPE;
							else if (sValue == "ATLAS_BUILDING")	uiTextureAtlas = ATLAS_BUILDING;
							else if (sValue == "ATLAS_FOLIAGE")	uiTextureAtlas = ATLAS_FOLIAGE;
							else if (sValue == "ATLAS_VEHICLES")	uiTextureAtlas = ATLAS_VEHICLES;
							else if (sValue == "ATLAS_PROPS")		uiTextureAtlas = ATLAS_PROPS;
							else if (sValue == "ATLAS_WEAPONS")	uiTextureAtlas = ATLAS_WEAPONS;
							else if (sValue == "ATLAS_EFFECTS")	uiTextureAtlas = ATLAS_EFFECTS;
						}

						if (TEXTURE_CUBEMAP == pLayer->uiTextureMode)
						{
							uiTextureAtlas = ATLAS_NONE;
							LOG.Error("CUBEMAP", "CUBEMAP");
						}
						
						if ((TEXTURE_CUBEMAP != pLayer->uiTextureMode) && (TEXTURE_POST_RENDER != pLayer->uiTextureMode))
						{
							if (ATLAS_NONE != uiTextureAtlas) pLayer->pTexture = pRender->AddTextureToAtlas(pLayer->sTexture, uiTextureAtlas);
							
							if (NULL == pLayer->pTexture)
							{
								uiTextureAtlas = ATLAS_NONE;
								pLayer->pTexture = pRender->AddTexture(pLayer->sTexture);
							}
						}
					}

					i++;
					iter++;
				}

				LOG.Success("Material", breathe::string::ToUTF8(TEXT("Loaded ") + sFilename));
				return breathe::GOOD;
			}

		}
	}
}
