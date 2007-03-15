#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <map>
#include <string>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>



#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/cBreathe.h>

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

#include <BREATHE/RENDER/cCamera.h>

#include <BREATHE/RENDER/cTexture.h>
#include <BREATHE/RENDER/cTextureAtlas.h>
#include <BREATHE/RENDER/cMaterial.h>
#include <BREATHE/RENDER/cRender.h>

#include <BREATHE/UTIL/cBase.h>
#include <BREATHE/MODEL/cMesh.h>
#include <BREATHE/MODEL/cModel.h>
#include <BREATHE/MODEL/cStatic.h>

#include <BREATHE/GAME/cLevel.h>

namespace BREATHE
{
	namespace RENDER
	{
		namespace MATERIAL
		{
			cLayer::cLayer()
			{
				uiTexture=0;
				uiTextureMode=TEXTURE_NONE;
			}

			
			cShader::cShader()
			{
				uiShaderVertex = 0;
				uiShaderFragment = 0;
				uiShaderProgram = 0;
			}

			
			void cShader::CheckStatusVertex()
			{
				int infologLength = 0;
				int charsWritten  = 0;
				char *infoLog;

				glGetShaderiv(uiShaderVertex, GL_INFO_LOG_LENGTH, &infologLength);
				if (infologLength > 0)
				{
						infoLog = new char[infologLength];
						glGetShaderInfoLog(uiShaderVertex, infologLength, &charsWritten, infoLog);
						pLog->Success("Render", "Shader %s: %s", sShaderVertex.c_str(), infoLog);
						delete infoLog;
				}
			}

			void cShader::CheckStatusFragment()
			{
				int infologLength = 0;
				int charsWritten  = 0;
				char *infoLog;

				glGetShaderiv(uiShaderFragment, GL_INFO_LOG_LENGTH, &infologLength);
				if (infologLength > 0)
				{
						infoLog = new char[infologLength];
						glGetShaderInfoLog(uiShaderFragment, infologLength, &charsWritten, infoLog);
						pLog->Success("Render", "Shader %s: %s", sShaderFragment.c_str(), infoLog);
						delete infoLog;
				}
			}
			
			void cShader::CheckStatusProgram()
			{
				int infologLength = 0;
				int charsWritten  = 0;
				char *infoLog;

				glGetProgramiv(uiShaderProgram, GL_INFO_LOG_LENGTH, &infologLength);
				if (infologLength > 0)
				{
						infoLog = new char[infologLength];
						glGetProgramInfoLog(uiShaderProgram, infologLength, &charsWritten, infoLog);
						pLog->Success("Render", "Program %s %s: %s", sShaderVertex.c_str(), sShaderFragment.c_str(), infoLog);
						delete infoLog;
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
						pLog->Error("Render", "Shader not found %s", sShaderVertex.c_str());
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
						pLog->Error("Render", "Shader not found %s", sShaderFragment.c_str());
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


			cMaterial::cMaterial(std::string name)
			{			
				pShader=NULL;

				chDustR=0;
				chDustG=0; 
				chDustB=0;

				uiAudioScrape=0;
				uiAudioBounce=0;

				bShadow_cast=true;
				bShadow_receive=true;
				bLight_receive=true;
				bLight_transmit=true;

				bCollideTrimesh=false;
				
				fFriction=0.0f; 
				fBounce=0.0f; 

				fCorrugation=0.0f;
				
				sName=name;
				
				unsigned int i=0;
				for(i=0;i<nLayers;i++)
					vLayer.push_back(new cLayer());
			}

			bool cMaterial::Load(std::string sFilename)
			{
				pLog->Success("Material", "Loading %s", sFilename.c_str());
				sName=sFilename;

				sFilename=pFileSystem->FindFile(sFilename);
				std::string sPath=pFileSystem->GetPath(sFilename);
				
				std::string line;
				std::ifstream f(sFilename.c_str());
				if(f.is_open())
				{
					while(!f.eof())
					{
						std::getline(f, line);

						int n=line.find("//");
						if(n != std::string::npos)
							line=line.substr(0, n);

						if(line.find("sShaderVertex=") != std::string::npos)
						{
							if(!pShader) pShader=new cShader();
							pShader->sShaderVertex=pFileSystem->FindFile(sPath+line.substr(14));
						}
						else if(line.find("sShaderFragment=") != std::string::npos)
						{
							if(!pShader) pShader=new cShader();
							pShader->sShaderFragment=pFileSystem->FindFile(sPath+line.substr(16));
						}

						else if(line.find("sTexture0=") != std::string::npos)
							sTexture0=pFileSystem->FindFile(sPath+line.substr(10));
						else if(line.find("sTexture1=") != std::string::npos)
							sTexture1=pFileSystem->FindFile(sPath+line.substr(10));
						else if(line.find("sTexture2=") != std::string::npos)
							sTexture2=pFileSystem->FindFile(sPath+line.substr(10));

						else if(line.find("uiTextureMode0=") != std::string::npos)
						{
							std::string t=line.substr(15);

							if(t.find("TEXTURE_NORMAL") != std::string::npos)
								vLayer[0]->uiTextureMode=TEXTURE_NORMAL;
							else if(t.find("TEXTURE_MASK") != std::string::npos)
								vLayer[0]->uiTextureMode=TEXTURE_MASK;
							else if(t.find("TEXTURE_BLEND") != std::string::npos)
								vLayer[0]->uiTextureMode=TEXTURE_BLEND;
							else if(t.find("TEXTURE_CUBEMAP") != std::string::npos)
								vLayer[0]->uiTextureMode=TEXTURE_CUBEMAP;
						}
						else if(line.find("uiTextureMode1=") != std::string::npos)
						{
							std::string t=line.substr(15);

							if(t.find("TEXTURE_NORMAL") != std::string::npos)
								vLayer[1]->uiTextureMode=TEXTURE_NORMAL;
							else if(t.find("TEXTURE_MASK") != std::string::npos)
								vLayer[1]->uiTextureMode=TEXTURE_MASK;
							else if(t.find("TEXTURE_BLEND") != std::string::npos)
								vLayer[1]->uiTextureMode=TEXTURE_BLEND;
							else if(t.find("TEXTURE_CUBEMAP") != std::string::npos)
								vLayer[1]->uiTextureMode=TEXTURE_CUBEMAP;
						}
						else if(line.find("uiTextureMode2=") != std::string::npos)
						{
							std::string t=line.substr(15);

							if(t.find("TEXTURE_NORMAL") != std::string::npos)
								vLayer[2]->uiTextureMode=TEXTURE_NORMAL;
							else if(t.find("TEXTURE_MASK") != std::string::npos)
								vLayer[2]->uiTextureMode=TEXTURE_MASK;
							else if(t.find("TEXTURE_BLEND") != std::string::npos)
								vLayer[2]->uiTextureMode=TEXTURE_BLEND;
							else if(t.find("TEXTURE_CUBEMAP") != std::string::npos)
								vLayer[2]->uiTextureMode=TEXTURE_CUBEMAP;
						}

						else if(line.find("uiTextureAtlas0=") != std::string::npos)
						{
							std::string t=line.substr(16);

							if(t.find("ATLAS_LANDSCAPE") != std::string::npos)
								vLayer[0]->uiTexture=ATLAS_LANDSCAPE;
							else if(t.find("ATLAS_BUILDING") != std::string::npos)
								vLayer[0]->uiTexture=ATLAS_BUILDING;
							else if(t.find("ATLAS_FOLIAGE") != std::string::npos)
								vLayer[0]->uiTexture=ATLAS_FOLIAGE;
							else if(t.find("ATLAS_VEHICLES") != std::string::npos)
								vLayer[0]->uiTexture=ATLAS_VEHICLES;
							else if(t.find("ATLAS_PROPS") != std::string::npos)
								vLayer[0]->uiTexture=ATLAS_PROPS;
							else if(t.find("ATLAS_WEAPONS") != std::string::npos)
								vLayer[0]->uiTexture=ATLAS_WEAPONS;
							else if(t.find("ATLAS_EFFECTS") != std::string::npos)
								vLayer[0]->uiTexture=ATLAS_EFFECTS;
							else if(t.find("ATLAS_NONE") != std::string::npos)
								vLayer[0]->uiTexture=ATLAS_NONE;
						}
						else if(line.find("uiTextureAtlas1=") != std::string::npos)
						{
							std::string t=line.substr(16);

							if(t.find("ATLAS_LANDSCAPE") != std::string::npos)
								vLayer[1]->uiTexture=ATLAS_LANDSCAPE;
							else if(t.find("ATLAS_BUILDING") != std::string::npos)
								vLayer[1]->uiTexture=ATLAS_BUILDING;
							else if(t.find("ATLAS_FOLIAGE") != std::string::npos)
								vLayer[1]->uiTexture=ATLAS_FOLIAGE;
							else if(t.find("ATLAS_VEHICLES") != std::string::npos)
								vLayer[1]->uiTexture=ATLAS_VEHICLES;
							else if(t.find("ATLAS_PROPS") != std::string::npos)
								vLayer[1]->uiTexture=ATLAS_PROPS;
							else if(t.find("ATLAS_WEAPONS") != std::string::npos)
								vLayer[1]->uiTexture=ATLAS_WEAPONS;
							else if(t.find("ATLAS_EFFECTS") != std::string::npos)
								vLayer[1]->uiTexture=ATLAS_EFFECTS;
							else if(t.find("ATLAS_NONE") != std::string::npos)
								vLayer[1]->uiTexture=ATLAS_NONE;
						}
						else if(line.find("uiTextureAtlas2=") != std::string::npos)
						{
							std::string t=line.substr(16);

							if(t.find("ATLAS_LANDSCAPE") != std::string::npos)
								vLayer[2]->uiTexture=ATLAS_LANDSCAPE;
							else if(t.find("ATLAS_BUILDING") != std::string::npos)
								vLayer[2]->uiTexture=ATLAS_BUILDING;
							else if(t.find("ATLAS_FOLIAGE") != std::string::npos)
								vLayer[2]->uiTexture=ATLAS_FOLIAGE;
							else if(t.find("ATLAS_VEHICLES") != std::string::npos)
								vLayer[2]->uiTexture=ATLAS_VEHICLES;
							else if(t.find("ATLAS_PROPS") != std::string::npos)
								vLayer[2]->uiTexture=ATLAS_PROPS;
							else if(t.find("ATLAS_WEAPONS") != std::string::npos)
								vLayer[2]->uiTexture=ATLAS_WEAPONS;
							else if(t.find("ATLAS_EFFECTS") != std::string::npos)
								vLayer[2]->uiTexture=ATLAS_EFFECTS;
							else if(t.find("ATLAS_NONE") != std::string::npos)
								vLayer[2]->uiTexture=ATLAS_NONE;
						}

						
						else if(line.find("bCollideTrimesh=") != std::string::npos)
						{
							if(line.find("true") != std::string::npos || line.find("0") != std::string::npos)
								bCollideTrimesh=true;
						}

						else if(""!=line)
							pLog->Error("Config", line);
					}
					f.close();

					pLog->Success("Material", "sTexture0 %s", sTexture0.c_str());
					pLog->Success("Material", "sTexture1 %s", sTexture1.c_str());

					if(pShader)
					{
						pLog->Success("Material", "sShaderVertex %s", pShader->sShaderVertex.c_str());
						pLog->Success("Material", "sShaderFragment %s", pShader->sShaderFragment.c_str());
					}

					pLog->Success("Material", "Loaded %s", sFilename.c_str());
				}
				else
				{
					pLog->Error("Material", "Not found %s", sFilename.c_str());
					return false;
				}

				return true;
			}

		}
	}
}
