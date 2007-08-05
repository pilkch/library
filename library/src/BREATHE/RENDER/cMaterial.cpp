#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

// writing on a text file
#include <iostream>
#include <fstream>

#include <list>
#include <vector>
#include <map>
#include <string>

#include <GL/Glee.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>


// Breathe
#include <BREATHE/cBreathe.h>

#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/UTIL/cString.h>

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

#include <BREATHE/UTIL/cBase.h>
#include <BREATHE/RENDER/MODEL/cMesh.h>
#include <BREATHE/RENDER/MODEL/cModel.h>
#include <BREATHE/RENDER/MODEL/cStatic.h>

#include <BREATHE/GAME/cLevel.h>

#include <BREATHE/UTIL/cXML.h>

namespace BREATHE
{
	namespace RENDER
	{
		namespace MATERIAL
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
						char *infoLog = new char[infologLength];
						glGetShaderInfoLog(uiShaderVertex, infologLength, &charsWritten, infoLog);
						std::string sInfo(infoLog);
						if(	sInfo.find("not been successfully compiled") != std::string::npos ||
								sInfo.find("ERROR") != std::string::npos)
						{
							sInfo = STRING::Replace(sInfo, "\n", "<br>");
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
						if(	sInfo.find("not been successfully compiled") != std::string::npos ||
								sInfo.find("ERROR") != std::string::npos)
						{
							sInfo = STRING::Replace(sInfo, "\n", "<br>");
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
						if(	sInfo.find("not been successfully compiled") != std::string::npos ||
								sInfo.find("Warning") != std::string::npos)
							LOG.Error("Material", std::string("Program ") + sShaderVertex + " " + sShaderFragment + ": " + infoLog);
						else
							LOG.Success("Material", std::string("Program ") + sShaderVertex + " " + sShaderFragment + ": " + infoLog);
						SAFE_DELETE_ARRAY(infoLog);
				}
			}

			void cShader::Init()
			{
				if("" != sShaderVertex)
				{
					uiShaderVertex = glCreateShader(GL_VERTEX_SHADER);

					std::string buffer="";
					std::string line="";
					std::ifstream f(sShaderVertex.c_str());
					if(f.is_open())
					{
						while(!f.eof())
						{
							std::getline(f, line);

							buffer+=line;
							buffer+="\n";

							line="";
						};

						const char *str=buffer.c_str();
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

				
				if("" != sShaderFragment)
				{
					uiShaderFragment = glCreateShader(GL_FRAGMENT_SHADER);

					std::string buffer;
					std::string line;
					std::ifstream f(sShaderFragment.c_str());
					if(f.is_open())
					{
						while(!f.eof())
						{
							std::getline(f, line);

							buffer+=line;
							buffer+="\n";
						};

						const char *str=buffer.c_str();
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
			
				if(uiShaderVertex || uiShaderFragment)
				{
					uiShaderProgram = glCreateProgram();

					if(uiShaderVertex)
						glAttachShader(uiShaderProgram, uiShaderVertex);

					if(uiShaderFragment)
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
				if(uiShaderFragment)
					glDeleteShader(uiShaderFragment);

				if(uiShaderVertex)
					glDeleteShader(uiShaderVertex);

				glDeleteProgram(uiShaderProgram);
				
				uiShaderFragment = 0;
				uiShaderVertex = 0;
				uiShaderProgram = 0;
			}


			cMaterial::cMaterial(std::string name) :
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
				for(i=0;i<nLayers;i++)
				{
					vLayer.push_back(new cLayer());
				}
			}

			cMaterial::~cMaterial()
			{
				unsigned int i=0;
				for(i=0;i<nLayers;i++)
					SAFE_DELETE(vLayer[i]);
			}

			bool cMaterial::Load(std::string sFilename)
			{
				LOG.Success("Material", std::string("Loading ") + sFilename);

				BREATHE::FILESYSTEM::FindFile(sFilename);

				std::string sPath=BREATHE::FILESYSTEM::GetPath(sFilename);
				
				XML::cNode root(sFilename);
				XML::cNode* p=root.FindChild("material");

				//<material collide="true" sShaderVertex="normalmap.vert" sShaderFragment="normalmap.frag">
				//	<layer sTexture="concrete.png" uiTextureMode="TEXTURE_NORMAL" uiTextureAtlas="ATLAS_NONE"/>
				//	<layer sTexture="concrete_normalmap.png" uiTextureMode="TEXTURE_NORMAL" uiTextureAtlas="ATLAS_NONE"/>
				//</material>

				if(NULL == p)
				{
					LOG.Error("Material", std::string("Not Found ") + sFilename);
					for(unsigned int i=0;i<nLayers;i++)
						vLayer[i]->pTexture = pRender->pMaterialNotFoundMaterial->vLayer[0]->pTexture;

					return BREATHE::BAD;
				}
				
				
				p->GetAttribute("collide", &bCollideTrimesh);
					
				std::string sValue;
				if(p->GetAttribute("sShaderVertex", &sValue))
				{
					if(!pShader) pShader=new cShader();
					pShader->sShaderVertex=BREATHE::FILESYSTEM::FindFile(sPath + sValue);
				}
				if(p->GetAttribute("sShaderFragment", &sValue))
				{
					if(!pShader) pShader=new cShader();
					pShader->sShaderFragment=BREATHE::FILESYSTEM::FindFile(sPath + sValue);
				}

				if(pShader)
				{
					p->GetAttribute("cameraPos", &pShader->bCameraPos);
					
					p->GetAttribute("texUnit0", &pShader->bTexUnit0);
					p->GetAttribute("texUnit1", &pShader->bTexUnit1);
					p->GetAttribute("texUnit2", &pShader->bTexUnit2);
					p->GetAttribute("texUnit3", &pShader->bTexUnit3);

					pShader->Init();
				}


				p=p->FirstChild();
				
				cLayer* l = NULL;
				unsigned int n = vLayer.size();
				unsigned int i = 0;
				while(p && i < nLayers)
				{
					l = vLayer[i];
					if("layer" == p->sName)
					{
						std::string sTexture;
						if(p->GetAttribute("sTexture", &sTexture))
							l->sTexture = BREATHE::FILESYSTEM::FindFile(sPath + sTexture);

						std::string sValue;
						if(p->GetAttribute("uiTextureMode", &sValue))
						{
							if(sValue == "TEXTURE_NORMAL")						l->uiTextureMode=TEXTURE_NORMAL;
							else if(sValue == "TEXTURE_MASK")					l->uiTextureMode=TEXTURE_MASK;
							else if(sValue == "TEXTURE_BLEND")				l->uiTextureMode=TEXTURE_BLEND;
							else if(sValue == "TEXTURE_DETAIL")				l->uiTextureMode=TEXTURE_DETAIL;
							else if(sValue == "TEXTURE_CUBEMAP")			l->uiTextureMode=TEXTURE_CUBEMAP;
							else if(sValue == "TEXTURE_POST_RENDER")	l->uiTextureMode=TEXTURE_POST_RENDER;
						}

						unsigned int uiTextureAtlas = ATLAS_NONE;
						if(p->GetAttribute("uiTextureAtlas", &sValue))
						{
							if(sValue == "ATLAS_LANDSCAPE")			uiTextureAtlas = ATLAS_LANDSCAPE;
							else if(sValue == "ATLAS_BUILDING")	uiTextureAtlas = ATLAS_LANDSCAPE;
							else if(sValue == "ATLAS_FOLIAGE")	uiTextureAtlas = ATLAS_FOLIAGE;
							else if(sValue == "ATLAS_VEHICLES")	uiTextureAtlas = ATLAS_VEHICLES;
							else if(sValue == "ATLAS_PROPS")		uiTextureAtlas = ATLAS_PROPS;
							else if(sValue == "ATLAS_WEAPONS")	uiTextureAtlas = ATLAS_WEAPONS;
							else if(sValue == "ATLAS_EFFECTS")	uiTextureAtlas = ATLAS_EFFECTS;
						}

						if(TEXTURE_CUBEMAP == l->uiTextureMode)
						{
							LOG.Error("CUBEMAP", "CUBEMAP");
						}
						
						if((TEXTURE_CUBEMAP != l->uiTextureMode) && (TEXTURE_POST_RENDER != l->uiTextureMode))
						{
							if(ATLAS_NONE != uiTextureAtlas) l->pTexture = pRender->AddTextureToAtlas(l->sTexture, uiTextureAtlas);
							
							if(NULL == l->pTexture)
							{
								uiTextureAtlas = ATLAS_NONE;
								l->pTexture = pRender->AddTexture(l->sTexture);
							}
						}
					}

					i++;
					p = p->Next();
				}

				LOG.Success("Material", std::string("Loaded ") + sFilename);
				return BREATHE::GOOD;
			}

		}
	}
}
