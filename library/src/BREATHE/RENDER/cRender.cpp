// Standard includes
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cmath>

// STL includes
#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <vector>
#include <map>
#include <list>

// Anything else
#include <GL/Glee.h>
#include <GL/glu.h>

//#include <SDL/SDL.h>
//#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>

// Breathe
#include <BREATHE/cBreathe.h>
#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/UTIL/cFileSystem.h>

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

#include <BREATHE/RENDER/cTexture.h>
#include <BREATHE/RENDER/cTextureAtlas.h>
#include <BREATHE/RENDER/cMaterial.h>
#include <BREATHE/RENDER/cRender.h>
#include <BREATHE/RENDER/cVertexBufferObject.h>

#include <BREATHE/RENDER/MODEL/cMesh.h>
#include <BREATHE/RENDER/MODEL/cModel.h>
#include <BREATHE/RENDER/MODEL/cStatic.h>

#include <BREATHE/GAME/cLevel.h>

#define MAX_TEXTURE_SIZE 1024

const float fDetailScale = 20.0f;

BREATHE::RENDER::cRender* pRender = NULL;

namespace BREATHE
{
	namespace RENDER
	{
		cRender::cRender()
		{
			bLight=true;
			bCubemap=true;
			bShader=true;
			bRenderWireframe=false;

			bCanShader=false;

			bActiveShader=false;
			bActiveColour=false;

			bFullscreen=true;
			uiWidth=1024;
			uiHeight=768;
			uiDepth = 32;

			uiFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE;

			iMaxTextureSize=0;

			uiActiveUnits=0;

			v3SunPosition.Set(10.0f, 10.0f, 5.0f, 0.0f);


			pCurrentMaterial=NULL;
			pLevel=NULL;

			pFrustum=new MATH::cFrustum();

			unsigned int i=0;
			for(i=0;i<nAtlas;i++)
    		vTextureAtlas.push_back(new cTextureAtlas(i));

			pMaterialNotFoundTexture=NULL;
			pTextureNotFoundTexture=NULL;

			for(i=0;i<MATERIAL::nLayers;i++)
				vLayer.push_back(MATERIAL::cLayer());
			
			pMaterialNotFoundMaterial=NULL;
		}

		cRender::~cRender()
		{
			//TODO: Delete materials and shader objects and atlases etc.

			LOG.Success("Delete", "Frustum");
			SAFE_DELETE(pFrustum);
		}

		bool cRender::FindExtension(std::string sExt)
		{
			std::ostringstream t;
			t<<static_cast<const unsigned char *>(glGetString( GL_EXTENSIONS ));

			return (t.str().find(sExt) != std::string::npos);
		}

		void cRender::ToggleFullscreen()
		{
			bFullscreen = !bFullscreen;

			if(bFullscreen)
			{
#ifdef BUILD_DEBUG
				uiWidth = 1024;
				uiHeight = 768;
				return;
#endif

				if(uiWidth<1280)
				{
					if(uiWidth<1024)
					{
						if(uiWidth<800)
						{
							uiWidth = 640;
							uiHeight = 480;
						}
						else
						{
							uiWidth = 800;
							uiHeight = 600;
						}
					}
					else
					{
						uiWidth = 1024;
						uiHeight = 768;
					}
				}
				else
				{
					uiWidth = 1600;
					uiHeight = 1280;
				}
			}
		}

		bool cRender::Init()
		{
			glClearColor(1.0f, 0.0f, 1.0f, 0.0f);				// Clear The Background Color To Black
			glClearDepth(1.0);									// Enables Clearing Of The Depth Buffer
			glEnable(GL_DEPTH_TEST);							// Enable Depth Testing
			glDepthFunc( GL_LEQUAL );

			glCullFace( GL_BACK );
			glFrontFace( GL_CCW );
			glEnable( GL_CULL_FACE );

			glEnable( GL_TEXTURE_2D );
			glShadeModel( GL_SMOOTH );
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
			glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
			

			MATH::cColour LightAmbient(0.0f, 0.0f, 0.0f, 1.0f);
			MATH::cColour LightDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
			MATH::cColour LightSpecular(1.0f, 1.0f, 1.0f, 1.0f);

			MATH::cColour LightModelAmbient(0.2f, 0.2f, 0.2f, 1.0f);
			
			MATH::cColour MaterialSpecular(1.0f, 1.0f, 1.0f, 1.0f);
			MATH::cColour MaterialEmission(0.5f, 0.5f, 0.5f, 1.0f);

			glLightf (GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.000008f); //2.5f);
			glLightf (GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.00002f); //0.25f);
			glLightf (GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f); //0.1f);

			glLightfv(GL_LIGHT0, GL_POSITION, v3SunPosition );

			glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient );
			glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse );
			glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular );

			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

			glEnable( GL_LIGHTING );
			glEnable( GL_LIGHT0 );
			glEnable( GL_COLOR_MATERIAL );

			glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	 
			glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
			glMaterialfv(GL_FRONT, GL_EMISSION, MaterialEmission);
			



			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &iMaxTextureSize);

			std::ostringstream t;
			t << "Screen BPP: ";
			t << (unsigned int)(pSurface->format->BitsPerPixel);
			LOG.Success("Render", t.str());
			LOG.Success("Render", std::string("Vendor     : ") + (char *)glGetString( GL_VENDOR ));
			LOG.Success("Render", std::string("Renderer   : ") + (char *)glGetString( GL_RENDERER ));
			LOG.Success("Render", std::string("Version    : ") + (char *)glGetString( GL_VERSION ));
			LOG.Success("Render", std::string("Extensions : ") + (char *)glGetString( GL_EXTENSIONS ));
	
			t.str("");
			t << iMaxTextureSize;
			if(iMaxTextureSize>=MAX_TEXTURE_SIZE)
			{
				LOG.Success("Render", std::string("Max Texture Size : ") + t.str());
				iMaxTextureSize=MAX_TEXTURE_SIZE;
			}
			else
				LOG.Error("Render", std::string("Max Texture Size : ") + t.str());
	

			if(FindExtension("GL_ARB_multitexture"))
        LOG.Success("Render", "Found GL_ARB_multitexture");
			else
			{
				LOG.Error("Render", "Not Found GL_ARB_multitexture");
				return false;
			}



			if(FindExtension("GL_ARB_texture_cube_map"))
				LOG.Success("Render", "Found GL_ARB_texture_cube_map");
			else
			{
				LOG.Error("Render", "Not Found GL_ARB_texture_cube_map");
				return false;
			}

			//GL_SHADING_LANGUAGE_VERSION

			float fShaderVersion=0.0f;
			char buffer[100]="";
			strcpy(buffer, (char*)glGetString(GL_VERSION));
				
			std::stringstream stm(buffer);
			
			stm >> fShaderVersion;
			
			if(fShaderVersion<2.0f)
			{
				LOG.Error("Render", "Not Found Shader2.0");
				bCanShader=false;
				bShader=false;
			}
			else
			{
				LOG.Success("Render", "Found Shader2.0");

				bCanShader = true;
        
				bShader=bCanShader;
			}
			
			if(bCanShader)
				LOG.Success("Render", "Can use shaders, shaders turned on");
			else
				LOG.Success("Render", "Cannot use shaders, shaders turned off");


			return BREATHE::GOOD;
		}
		
		void cRender::BeginFrame(float fCurrentTime)
		{
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
			glLoadIdentity();

			glMultMatrixf(pFrustum->m);

			if(bRenderWireframe)
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			else
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

			
			if(bLight)
			{
				glLightfv (GL_LIGHT0, GL_POSITION, v3SunPosition);
				glEnable( GL_LIGHTING );
			}
			else
				glDisable(GL_LIGHTING);

			ClearColour();
			ClearMaterial();

			uiTextureModeChanges = uiTextureChanges = uiTriangles = 0;
		}

		void cRender::BeginHUD(float fCurrentTime)
		{
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			glDisable(GL_LIGHTING);
			
			ClearColour();
			ClearMaterial();
		}

		void cRender::EndFrame()
		{
			SDL_GL_SwapBuffers();
		}

		void cRender::SetAtlasWidth(unsigned int uiNewSegmentWidthPX, unsigned int uiNewSegmentSmallPX, unsigned int uiNewAtlasWidthPX)
		{
			uiSegmentWidthPX=uiNewSegmentWidthPX;
			uiAtlasWidthPX=uiNewAtlasWidthPX;
			uiSegmentSmallPX=uiNewSegmentSmallPX;
		}

		void cRender::BeginLoadingTextures()
		{
			unsigned int i=0;
			for(i=0;i<nAtlas;i++)
    		vTextureAtlas[i]->Begin(uiSegmentWidthPX, uiSegmentSmallPX, uiAtlasWidthPX);
		}
		
		void cRender::EndLoadingTextures()
		{
			unsigned int i=0;
			for(i=0;i<nAtlas;i++)
    		vTextureAtlas[i]->End();
		}



		cTexture *cRender::AddTextureToAtlas(std::string sNewFilename, unsigned int uiAtlas)
		{
			if(""==sNewFilename)
				return pTextureNotFoundTexture;
			
			cTexture *p=mTexture[sNewFilename];
			if(p)
				return p;

			p=vTextureAtlas[uiAtlas]->AddTexture(sNewFilename);
			if(NULL==p)
			{
				p=pTextureNotFoundTexture;
				LOG.Error("Texture", sNewFilename + " pTextureNotFound");
			}
			else
			{
				std::ostringstream t;
				t << p->uiTexture;
				LOG.Success("Texture", sNewFilename + " " + t.str());
			}
			
			mTexture[sNewFilename]=p;		

			return p;
		}

		cTexture *cRender::AddTexture(std::string sNewFilename)
		{
			if(""==sNewFilename)
				return pTextureNotFoundTexture;
			
			cTexture *p=mTexture[sNewFilename];
			if(p)
				return p;

			
			
			p=new cTexture();

			LOG.Success("Texture", "Loading " + sNewFilename);
			
			if(p->Load(sNewFilename) != BREATHE::GOOD)
			{
				SAFE_DELETE(p);
				return p;
			}
			
			p->GenerateOpenGLTexture();
			
			mTexture[sNewFilename]=p;		

			return p;
		}

		bool cRender::AddTextureNotFoundTexture(std::string sFilename)
		{
			pTextureNotFoundTexture=new cTexture();

			sFilename=pFileSystem->FindFile(sFilename);

			LOG.Success("Texture", "Loading " + sFilename);
		
			unsigned int mode=0;
			pTextureNotFoundTexture->surface = IMG_Load(sFilename.c_str());

			// could not load filename
			if (!pTextureNotFoundTexture->surface)
			{
				LOG.Error("Texture", "Couldn't Load Texture " + sFilename);
				return NULL;
			}

			if(pTextureNotFoundTexture->surface->format->BytesPerPixel == 3) // RGB 24bit
			{
				mode = GL_RGB;
				LOG.Success("Texture", "RGB Image");
			}
			else if(pTextureNotFoundTexture->surface->format->BytesPerPixel == 4)// RGBA 32bit
			{
				mode = GL_RGBA;
				LOG.Success("Texture", "RGBA Image");
			}
			else
			{
				std::ostringstream t;
				t << pTextureNotFoundTexture->surface->format->BytesPerPixel;
				LOG.Error("Texture", "Error Unknown Image Format (" + t.str() + ")");
				
				return NULL;
			}

			{
				int nHH = pTextureNotFoundTexture->surface->h / 2;
				int nPitch = pTextureNotFoundTexture->surface->pitch;
			
				unsigned char* pBuf = new unsigned char[nPitch];
				unsigned char* pSrc = (unsigned char*) pTextureNotFoundTexture->surface->pixels;
				unsigned char* pDst = (unsigned char*) pTextureNotFoundTexture->surface->pixels + 
					nPitch*(pTextureNotFoundTexture->surface->h - 1);
			
				while (nHH--)
				{
					std::memcpy(pBuf, pSrc, nPitch);
					std::memcpy(pSrc, pDst, nPitch);
					std::memcpy(pDst, pBuf, nPitch);
			
					pSrc += nPitch;
					pDst -= nPitch;
				};
			
				SAFE_DELETE_ARRAY(pBuf);
			}

			pTextureNotFoundTexture->CopyFromSurface(
				pTextureNotFoundTexture->surface->w,
				pTextureNotFoundTexture->surface->h);
			//TODO: Put into a texture atlas

			// create one texture name
			glGenTextures(1, &pTextureNotFoundTexture->uiTexture);

			// tell opengl to use the generated texture name
			glBindTexture(GL_TEXTURE_2D, pTextureNotFoundTexture->uiTexture);

			// this reads from the sdl surface and puts it into an opengl texture
			glTexImage2D(GL_TEXTURE_2D, 0, mode, 
				pTextureNotFoundTexture->surface->w, pTextureNotFoundTexture->surface->h, 
				0, mode, GL_UNSIGNED_BYTE, pTextureNotFoundTexture->surface->pixels);

			// these affect how this texture is drawn later on...
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			std::ostringstream t;
			t << pTextureNotFoundTexture->uiTexture;
			LOG.Success("Texture", "TextureNotFoundTexture " + t.str());

			return true;
		}

		bool cRender::AddMaterialNotFoundTexture(std::string sFilename)
		{
			pMaterialNotFoundTexture=new cTexture();

			sFilename=pFileSystem->FindFile(sFilename);

			LOG.Success("Texture", "Loading " + sFilename);
		
			unsigned int mode=0;
			pMaterialNotFoundTexture->surface = IMG_Load(sFilename.c_str());

			// could not load filename
			if (!pMaterialNotFoundTexture->surface)
			{
				LOG.Error("Texture", "Couldn't Load Texture " + sFilename);
				return NULL;
			}

			if(pMaterialNotFoundTexture->surface->format->BytesPerPixel == 3) // RGB 24bit
			{
				mode = GL_RGB;
				LOG.Success("Texture", "RGB Image");
			}
			else if(pMaterialNotFoundTexture->surface->format->BytesPerPixel == 4)// RGBA 32bit
			{
				mode = GL_RGBA;
				LOG.Success("Texture", "RGBA Image");
			}
			else
			{
				std::ostringstream t;
				t << pMaterialNotFoundTexture->surface->format->BytesPerPixel;
				LOG.Error("Texture", "Error Unknown Image Format (" + t.str() + ")");
				
				return NULL;
			}

			{
				int nHH = pMaterialNotFoundTexture->surface->h / 2;
				int nPitch = pMaterialNotFoundTexture->surface->pitch;
			
				unsigned char* pBuf = new unsigned char[nPitch];
				unsigned char* pSrc = (unsigned char*) pMaterialNotFoundTexture->surface->pixels;
				unsigned char* pDst = (unsigned char*) pMaterialNotFoundTexture->surface->pixels + 
					nPitch*(pMaterialNotFoundTexture->surface->h - 1);
			
				while (nHH--)
				{
					std::memcpy(pBuf, pSrc, nPitch);
					std::memcpy(pSrc, pDst, nPitch);
					std::memcpy(pDst, pBuf, nPitch);
			
					pSrc += nPitch;
					pDst -= nPitch;
				};
			
				SAFE_DELETE_ARRAY(pBuf);
			}

			pMaterialNotFoundTexture->CopyFromSurface(
				pMaterialNotFoundTexture->surface->w,
				pMaterialNotFoundTexture->surface->h);

			//TODO: Put into a texture atlas

			// create one texture name
			glGenTextures(1, &pMaterialNotFoundTexture->uiTexture);

			// tell opengl to use the generated texture name
			glBindTexture(GL_TEXTURE_2D, pMaterialNotFoundTexture->uiTexture);

			// this reads from the sdl surface and puts it into an opengl texture
			glTexImage2D(GL_TEXTURE_2D, 0, mode, 
				pMaterialNotFoundTexture->surface->w, pMaterialNotFoundTexture->surface->h, 
				0, mode, GL_UNSIGNED_BYTE, pMaterialNotFoundTexture->surface->pixels);

			// these affect how this texture is drawn later on...
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			std::ostringstream t;
			t << pMaterialNotFoundTexture->uiTexture;
			LOG.Success("Texture", "MaterialNotFoundTexture " + t.str());

			return true;
		}


		cTexture *cRender::GetTexture(std::string sNewFilename)
		{		
			if(""==sNewFilename)
				return pTextureNotFoundTexture;
			
			cTexture *p=mTexture[sNewFilename];
			if(p)
				return p;

			p=pTextureNotFoundTexture;
			
			mTexture[sNewFilename]=p;

			return p;
		}

		cVertexBufferObject* cRender::AddVertexBufferObject()
		{
			cVertexBufferObject* pVertexBufferObject = new cVertexBufferObject();
			vVertexBufferObject.push_back(pVertexBufferObject);

			return pVertexBufferObject;
		}

		cTexture *cRender::GetCubeMap(std::string sNewFilename)
		{		
			if(""==sNewFilename)
				return NULL;
			
			std::map<std::string, cTexture *>::iterator iter=mCubeMap.find(sNewFilename);

			if(mCubeMap.end()!=iter)
				return iter->second;

			return NULL;
		}

		cTexture *cRender::AddCubeMap(std::string sFilename)
		{
			/*TODO: 
			Surface of 1x6 that holds the cubemap faces,
			not actually used for rendering, just collecting each surface

			class cCubeMap : protected cTexture
			{
			public:
				CopyFromSurface(uiWidth, 6 * uiHeight);
			};
			*/

			if(""==sFilename)
				return pTextureNotFoundTexture;

			cTexture *p=mCubeMap[sFilename];
			if(p)
				return p;

			p=new cTexture();

			mCubeMap[sFilename]=p;

			p->sFilename=sFilename;

			GLuint cube_map_directions[6] = 
			{
				GL_TEXTURE_CUBE_MAP_POSITIVE_X,
				GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
				GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
				GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
			};

			LOG.Success("Texture", "Loading CubeMap " + sFilename);
		
			unsigned int i=0;
			unsigned int uiTempTexture=0;

			glEnable(GL_TEXTURE_CUBE_MAP);

			glGenTextures(1, &p->uiTexture);
			
			glBindTexture(GL_TEXTURE_CUBE_MAP, p->uiTexture);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


			std::stringstream s;

			std::string sFile=pFileSystem->GetFileNoExtension(sFilename);
			std::string sExt=pFileSystem->GetExtension(sFilename);
			
			for(i=0;i<6;i++)
			{
				s.str( "" );

				s<<sFile<<"/"<<sFile<<i<<"."<<sExt;
				sFilename=pFileSystem->FindFile(s.str());

				unsigned int mode=0;

				SDL_Surface *surface = IMG_Load(sFilename.c_str());

				// could not load filename
				if (!surface)
				{
					LOG.Error("Texture", "Couldn't Load Texture " + sFilename);
					return NULL;
				}

				if(surface->format->BytesPerPixel == 3) // RGB 24bit
				{
					mode = GL_RGB;
					LOG.Success("Texture", "RGB Image");
				}
				else if(surface->format->BytesPerPixel == 4)// RGBA 32bit
				{
					mode = GL_RGBA;
					LOG.Success("Texture", "RGBA Image");
				}
				else
				{
					SDL_FreeSurface(surface);
					std::ostringstream t;
					t << surface->format->BytesPerPixel;
					LOG.Error("Texture", "Error Unknown Image Format (" + t.str() + ")");
					
					return NULL;
				}

				{
					int nHH = surface->h / 2;
					int nPitch = surface->pitch;
				
					unsigned char* pBuf = new unsigned char[nPitch];
					unsigned char* pSrc = (unsigned char*) surface->pixels;
					unsigned char* pDst = (unsigned char*) surface->pixels + nPitch*(surface->h - 1);
				
					while (nHH--)
					{
						std::memcpy(pBuf, pSrc, nPitch);
						std::memcpy(pSrc, pDst, nPitch);
						std::memcpy(pDst, pBuf, nPitch);
				
						pSrc += nPitch;
						pDst -= nPitch;
					};
				
					SAFE_DELETE_ARRAY(pBuf);
				}


				

				// create one texture name
				glGenTextures(1, &uiTempTexture);

				// This sets the alignment requirements for the start of each pixel row in memory.
				glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

				// Bind the texture to the texture arrays index and init the texture
				glBindTexture(GL_TEXTURE_2D, uiTempTexture);

				// this reads from the sdl surface and puts it into an opengl texture
				glTexImage2D(cube_map_directions[i], 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

				// these affect how this texture is drawn later on...
				/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				//Trilinear mipmapping.
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				
				//build mipmaps
				//gluBuild2DMipmaps(cube_map_directions[i], mode, surface->w, surface->h, mode, GL_UNSIGNED_BYTE, surface->pixels);

				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);*/

				// clean up
				SDL_FreeSurface(surface);
			}

			
			glDisable(GL_TEXTURE_CUBE_MAP);
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);
			glEnable(GL_TEXTURE_2D);


			std::ostringstream t;
			t << p->uiTexture;
			LOG.Success("Texture", t.str());

			return p;
		}

		MATERIAL::cMaterial *cRender::AddMaterialNotFoundMaterial(std::string sNewFilename)
		{
			AddMaterialNotFoundTexture(sNewFilename);

			sNewFilename=pFileSystem->FindFile(sNewFilename);

			pMaterialNotFoundMaterial=new MATERIAL::cMaterial("MaterialNotFound");

			pMaterialNotFoundMaterial->sTexture0=sNewFilename;
			pMaterialNotFoundMaterial->vLayer[0]->uiTexture=pMaterialNotFoundTexture->uiTexture;
			pMaterialNotFoundMaterial->vLayer[0]->uiTextureMode=TEXTURE_NORMAL;

			return pMaterialNotFoundMaterial;
		}

		MATERIAL::cMaterial *cRender::AddMaterial(std::string sNewfilename)
		{
			if(""==sNewfilename)
				return NULL;

			std::map<std::string, MATERIAL::cMaterial * >::iterator iter=mMaterial.find(sNewfilename);

			MATERIAL::cMaterial *pMaterial=NULL;
			
			if(iter!=mMaterial.end())
				pMaterial=iter->second;

			if(pMaterial)
				return pMaterial;
				
			pMaterial=new MATERIAL::cMaterial(sNewfilename);

			if(pMaterial->Load(sNewfilename))
			{
				if("" != pMaterial->sTexture0)
				{
					pMaterial->sTexture0=pFileSystem->FindFile(pMaterial->sTexture0);

					if(TEXTURE_CUBEMAP==pMaterial->vLayer[0]->uiTextureMode)
						;
					else if(ATLAS_NONE==pMaterial->vLayer[0]->uiTexture)
						pMaterial->vLayer[0]->uiTexture=(AddTexture(pMaterial->sTexture0))->uiTexture;
					else
						pMaterial->vLayer[0]->uiTexture=(AddTextureToAtlas(pMaterial->sTexture0, pMaterial->vLayer[0]->uiTexture))->uiTexture;
				}

				if("" != pMaterial->sTexture1)
				{
					pMaterial->sTexture1=pFileSystem->FindFile(pMaterial->sTexture1);

					if(TEXTURE_CUBEMAP==pMaterial->vLayer[1]->uiTextureMode)
						;
					else if(ATLAS_NONE==pMaterial->vLayer[1]->uiTexture)
						pMaterial->vLayer[1]->uiTexture=(AddTexture(pMaterial->sTexture1))->uiTexture;
					else
						pMaterial->vLayer[1]->uiTexture=(AddTextureToAtlas(pMaterial->sTexture1, pMaterial->vLayer[1]->uiTexture))->uiTexture;
				}


				
				if(bCanShader)
				{
					if(pMaterial->pShader)
						pMaterial->pShader->Init();
				}
			}
			else
			{
				SAFE_DELETE(pMaterial);
				pMaterial=pMaterialNotFoundMaterial;
			}
			
			mMaterial[sNewfilename]=pMaterial;

			return pMaterial;
		}

		float Angle(const MATH::cVec2 & a, const MATH::cVec2 & b)
		{
			if(a.x>b.x)
			{
				if(a.y>b.y)
					return (atan((a.y-b.y)/(a.x-b.x)) + MATH::cPI_DIV_180 * 90.0f) * MATH::c180_DIV_PI;
				else
					return (-atan((a.y-b.y)/(b.x-a.x)) + MATH::cPI_DIV_180 * 90.0f) * MATH::c180_DIV_PI;
			}
			

			if(b.y>a.y)
				return (atan((b.y-a.y)/(b.x-a.x)) + MATH::cPI_DIV_180 * 270.0f) * MATH::c180_DIV_PI;

			return (atan((b.y-a.y)/(b.x-a.x)) + MATH::cPI_DIV_180 * 270.0f) * MATH::c180_DIV_PI;
		}

		bool cRender::ClearMaterial()
		{
			unsigned int i=0;
			unsigned int n=0;
			unsigned int unit=GL_TEXTURE0;

			MATERIAL::cLayer* layerOld;

			for(i=n;i<MATERIAL::nLayers;i++, unit++)
			{
				layerOld = &vLayer[i];

				//Activate the current texture unit
				glActiveTexture(unit);

				//Undo last mode
				if(	TEXTURE_MASK==layerOld->uiTextureMode || 
						TEXTURE_BLEND==layerOld->uiTextureMode)
				{
					glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
					glBlendFunc(GL_ONE, GL_ZERO);
					glDisable(GL_BLEND);
				}
				else if(	TEXTURE_DETAIL==layerOld->uiTextureMode)
				{
					// Reset the texture matrix
					glMatrixMode(GL_TEXTURE);
					glLoadIdentity();
					glMatrixMode(GL_MODELVIEW);

					glEnable(GL_LIGHTING);
				}
				else if(TEXTURE_CUBEMAP==layerOld->uiTextureMode)
				{
					glMatrixMode(GL_TEXTURE);
					glPopMatrix();

					glMatrixMode(GL_MODELVIEW);

					glDisable(GL_TEXTURE_CUBE_MAP);

					glDisable(GL_TEXTURE_GEN_R);
					glDisable(GL_TEXTURE_GEN_T);
					glDisable(GL_TEXTURE_GEN_S);

					glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 1);
				}
				
				glDisable(GL_TEXTURE_2D);
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

				//Set the current mode and texture
				layerOld->uiTextureMode=TEXTURE_NONE;
			}

			glActiveTexture(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);

			bActiveShader=false;
			
			if(bCanShader)
        glUseProgram(NULL);

			ClearColour();

			return true;
		}

		bool cRender::SetMaterial(MATERIAL::cMaterial* pMaterial, MATH::cVec3& pos)
		{
			if(pMaterial == NULL)
			{
				LOG.Error("Render", "No texture specified");
				return false;
			}

			if(pCurrentMaterial == pMaterial)
			{
				// Update camera possibly?
				return true;
			}

			uiTextureModeChanges++;
			//uiTextureChanges

			unsigned int i=0;
			unsigned int n=MATERIAL::nLayers;
			unsigned int unit=0;

			MATERIAL::cLayer* layerOld;
			MATERIAL::cLayer* layerNew;

			for(i=0;i<n;i++)
			{
				layerNew = pMaterial->vLayer[i];

				if(	TEXTURE_NONE==layerNew->uiTextureMode || 
						(TEXTURE_CUBEMAP==layerNew->uiTextureMode && !bCubemap))
						n = i;

				//if((TEXTURE_NONE!=layerNew->uiTextureMode && TEXTURE_CUBEMAP!=layerNew->uiTextureMode) ||
				//	(TEXTURE_CUBEMAP==layerNew->uiTextureMode && bCubemap))
				//		n = i;
			}

			if(0 == n)
				LOG.Error("Render", "No layers to render");

			uiActiveUnits=n;

			unit=GL_TEXTURE0_ARB;
			
			for(i=0;i<n;i++, unit++)
			{
				layerNew = pMaterial->vLayer[i];
				layerOld = &vLayer[i];

				//If this is a cubemap, set the material texture to the cubemap before we get there
				if(TEXTURE_CUBEMAP==layerNew->uiTextureMode)
					layerNew->uiTexture=pLevel->FindClosestCubeMap(pos)->uiTexture;

				//Activate the current texture unit
				glActiveTexture(unit);

				//Different mode, probably means different texture, change mode and bind it anyway
				if(layerOld->uiTextureMode!=layerNew->uiTextureMode)
				{
					//Undo last mode
					if(	TEXTURE_MASK==layerOld->uiTextureMode ||
							TEXTURE_BLEND==layerOld->uiTextureMode ||
							TEXTURE_DETAIL==layerOld->uiTextureMode)
					{
						glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
						glBlendFunc(GL_ONE, GL_ZERO);
						glDisable(GL_BLEND);
					}
					else if(TEXTURE_CUBEMAP==layerOld->uiTextureMode)
					{
						glMatrixMode(GL_TEXTURE);
						glPopMatrix();

						glMatrixMode(GL_MODELVIEW);

						glDisable(GL_TEXTURE_CUBE_MAP);

						glDisable(GL_TEXTURE_GEN_R);
						glDisable(GL_TEXTURE_GEN_T);
						glDisable(GL_TEXTURE_GEN_S);

						glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
						glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 1);

						glEnable(GL_TEXTURE_2D);
					}

					//Set the current mode and texture
					layerOld->uiTextureMode=layerNew->uiTextureMode;
					layerOld->uiTexture=layerNew->uiTexture;

					if(TEXTURE_NONE==layerOld->uiTextureMode)
						glDisable(GL_TEXTURE_2D);
					else if(TEXTURE_NORMAL==layerOld->uiTextureMode)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, layerOld->uiTexture);
					}
					else if(TEXTURE_MASK==layerOld->uiTextureMode)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, layerOld->uiTexture);

						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
						glEnable(GL_BLEND);
					}
					else if(TEXTURE_BLEND==layerOld->uiTextureMode)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, layerOld->uiTexture);

						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
						glEnable(GL_BLEND);
					}
					else if(TEXTURE_DETAIL==layerOld->uiTextureMode)
					{
						glActiveTexture(GL_TEXTURE0_ARB);
						glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
						glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);

						glActiveTexture(GL_TEXTURE1_ARB);
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, layerOld->uiTexture);
						glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
						glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);
										
						// Change the texture matrix so that we have more detail than normal texture
						glMatrixMode(GL_TEXTURE);
							glLoadIdentity();
							glScalef(fDetailScale, fDetailScale, 1);
							glMatrixMode(GL_MODELVIEW);

							// General Switches
							glDisable(GL_BLEND);
							glEnable(GL_LIGHTING);
					}
					else if(TEXTURE_CUBEMAP==layerOld->uiTextureMode)
					{
						//Assume we got one we shouldn't be here if we didn't
						//It is possible if there are NO cubemaps in the whole level,
						//so make sure we load one already
						glDisable(GL_TEXTURE_2D);
						glEnable(GL_TEXTURE_CUBE_MAP);
						glBindTexture(GL_TEXTURE_CUBE_MAP, layerOld->uiTexture);

						glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
						glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
						glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
						glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
						glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
						glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 2);


						
						glMatrixMode(GL_TEXTURE);
						glPushMatrix();
						glLoadIdentity();
						
						float y=-Angle(MATH::cVec2(pFrustum->eye.x, pFrustum->eye.y), MATH::cVec2(pFrustum->target.x, pFrustum->target.y));
						float x=-Angle(MATH::cVec2(pFrustum->eye.y, pFrustum->eye.z), MATH::cVec2(pFrustum->target.y, pFrustum->target.z));
						//std::cout<<y<<"\t"<<x<<"\n";

						glRotatef(y, 0.0f, 1.0f, 0.0f);
						glRotatef(x, 1.0f, 0.0f, 0.0f);

						
						//float mat[16];
						//glGetFloatv(GL_MODELVIEW_MATRIX, mat);

						//MATH::cQuaternion q(mat[8], mat[9], -mat[10]);

						//glLoadMatrixf(static_cast<float *>(q.GetMatrix()));


						glMatrixMode(GL_MODELVIEW);


						glEnable(GL_TEXTURE_GEN_S);
						glEnable(GL_TEXTURE_GEN_T);
						glEnable(GL_TEXTURE_GEN_R); 

						glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
						glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
						glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
					}
					else
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, layerOld->uiTexture);
					}

					if(0==unit)
					{
						if(n>1)
						{
							glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
						}
						else
						{
							glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
						}
					}
				}


				// **************************************************************************************
				// **************************************************************************************
				// **************************************************************************************
				// **************************************************************************************
				// **************************************************************************************
				// **************************************************************************************
				// **************************************************************************************
				// **************************************************************************************
				//Same Mode, just change texture
				else if(layerOld->uiTexture!=layerNew->uiTexture)
				{
					uiTextureChanges++;
					if(	TEXTURE_MASK==layerOld->uiTextureMode || 
							TEXTURE_DETAIL==layerOld->uiTextureMode)
					{
						layerOld->uiTexture=layerNew->uiTexture;
						glBindTexture(GL_TEXTURE_2D, layerNew->uiTexture);
					}
					else if(TEXTURE_BLEND==layerOld->uiTextureMode)
					{
						layerOld->uiTexture=layerNew->uiTexture;
						glBindTexture(GL_TEXTURE_2D, layerNew->uiTexture);

						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
						glEnable(GL_BLEND);
					}
					else if(TEXTURE_CUBEMAP==layerOld->uiTextureMode)
					{
						layerOld->uiTexture=layerNew->uiTexture;

						cTexture *t=pLevel->FindClosestCubeMap(pos);

						if(t)
						{
							glDisable(GL_TEXTURE_2D);
							glEnable(GL_TEXTURE_CUBE_MAP);
							glBindTexture(GL_TEXTURE_CUBE_MAP, t->uiTexture);
							
							//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
							glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MULT);

							glEnable(GL_TEXTURE_GEN_S);
							glEnable(GL_TEXTURE_GEN_T);
							glEnable(GL_TEXTURE_GEN_R); 
							
							glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_REFLECTION_MAP);
							glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_REFLECTION_MAP);
							glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_REFLECTION_MAP);
						}						
					}
					else
					{
						glBlendFunc(GL_ONE, GL_ZERO);
						glDisable(GL_BLEND);
					}

					
					//Set the current mode and texture
					layerOld->uiTextureMode=layerNew->uiTextureMode;
					layerOld->uiTexture=layerNew->uiTexture;




					
					if(TEXTURE_NONE==layerOld->uiTextureMode)
						glDisable(GL_TEXTURE_2D);
					else if(TEXTURE_NORMAL==layerOld->uiTextureMode)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, layerOld->uiTexture);
					}
					else if(TEXTURE_MASK==layerOld->uiTextureMode)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, layerOld->uiTexture);

						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
						glEnable(GL_BLEND);
					}
					else if(TEXTURE_BLEND==layerOld->uiTextureMode || 
									TEXTURE_DETAIL==layerOld->uiTextureMode)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, layerOld->uiTexture);

						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
						glEnable(GL_BLEND);
					}
					else if(TEXTURE_CUBEMAP==layerOld->uiTextureMode)
					{
						//Assume we got one we shouldn't be here if we didn't
						//It is possible if there are NO cubemaps in the whole level,
						//so make sure we load one already
						glDisable(GL_TEXTURE_2D);
						glEnable(GL_TEXTURE_CUBE_MAP);
						glBindTexture(GL_TEXTURE_CUBE_MAP, layerOld->uiTexture);

						glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
						glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
						glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
						glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
						glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
						glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 2);


						
						glMatrixMode(GL_TEXTURE);
						glPushMatrix();
						glLoadIdentity();
						
						float y=-Angle(MATH::cVec2(pFrustum->eye.x, pFrustum->eye.y), 
							MATH::cVec2(pFrustum->target.x, pFrustum->target.y));
						float x=-Angle(MATH::cVec2(pFrustum->eye.y, pFrustum->eye.z), 
							MATH::cVec2(pFrustum->target.y, pFrustum->target.z));
						//std::cout<<y<<"\t"<<x<<"\n";

						glRotatef(y, 0.0f, 1.0f, 0.0f);
						glRotatef(x, 1.0f, 0.0f, 0.0f);

						
						//float mat[16];
						//glGetFloatv(GL_MODELVIEW_MATRIX, mat);

						//MATH::cQuaternion q(mat[8], mat[9], -mat[10]);

						//glLoadMatrixf(static_cast<float *>(q.GetMatrix()));


						glMatrixMode(GL_MODELVIEW);


						glEnable(GL_TEXTURE_GEN_S);
						glEnable(GL_TEXTURE_GEN_T);
						glEnable(GL_TEXTURE_GEN_R); 

						glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
						glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
						glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
					}
					else
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, layerOld->uiTexture);
					}

					if(0==unit)
					{
						if(n>1)
						{
							glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
						}
						else
						{
							glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
						}
					}
				}



				

				// **************************************************************************************
				// **************************************************************************************
				// **************************************************************************************
				// **************************************************************************************
				// **************************************************************************************
				// **************************************************************************************
				// **************************************************************************************
				// **************************************************************************************
			}

			unit = GL_TEXTURE0_ARB + n;

			for(i=n;i<MATERIAL::nLayers;i++, unit++)
			{
				layerNew = pMaterial->vLayer[i];
				layerOld = &vLayer[i];

				//Activate the current texture unit
				glActiveTexture(unit);

				//Undo last mode
				if(	TEXTURE_MASK==layerOld->uiTextureMode || 
						TEXTURE_BLEND==layerOld->uiTextureMode)
				{
					glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
					glBlendFunc(GL_ONE, GL_ZERO);
					glDisable(GL_BLEND);
				}
				else if(TEXTURE_DETAIL==layerOld->uiTextureMode)
				{
					// Change the texture matrix so that we have more detail than normal texture
					glMatrixMode(GL_TEXTURE);
						glLoadIdentity();
					glMatrixMode(GL_MODELVIEW);

					// General Switches
					glDisable(GL_BLEND);
					glEnable(GL_LIGHTING);
				}
				else if(TEXTURE_CUBEMAP==layerOld->uiTextureMode)
				{
					glMatrixMode(GL_TEXTURE);
					glPopMatrix();

					glMatrixMode(GL_MODELVIEW);

					glDisable(GL_TEXTURE_CUBE_MAP);

					glDisable(GL_TEXTURE_GEN_R);
					glDisable(GL_TEXTURE_GEN_T);
					glDisable(GL_TEXTURE_GEN_S);

					glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 1);
				}
				
				glDisable(GL_TEXTURE_2D);

				//Set the current mode and texture
				layerOld->uiTextureMode=TEXTURE_NONE;
			}



			if(1==uiActiveUnits)
			{			
				glActiveTexture(GL_TEXTURE0_ARB);
				if(TEXTURE_NORMAL==vLayer[0].uiTextureMode && bActiveColour)
				{
					glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, colour);

					glColor4f(colour.r, colour.g, colour.b, colour.a);
				}
				else
				{
					float a0[4] = {1.0f, 1.0f, 1.0f, 1.0f};
					glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, a0);
				}
			}
			else if(2==uiActiveUnits)
			{
				if(TEXTURE_DETAIL==pMaterial->vLayer[1]->uiTextureMode)
				{
					// TEXTURE-UNIT #0
					glActiveTexture(GL_TEXTURE0_ARB);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, pMaterial->vLayer[0]->uiTexture);
					glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
					glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);

					// TEXTURE-UNIT #1
					glActiveTexture(GL_TEXTURE1_ARB);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, pMaterial->vLayer[1]->uiTexture);
					glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
					glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);
									
					// Now we want to enter the texture matrix.  This will allow us
					// to change the tiling of the detail texture.
					glMatrixMode(GL_TEXTURE);

						// Reset the current matrix and apply our chosen scale value
						glLoadIdentity();
						glScalef(fDetailScale, fDetailScale, 1);

						// Leave the texture matrix and set us back in the model view matrix
						glMatrixMode(GL_MODELVIEW);

						// General Switches
						glDisable(GL_BLEND);
						glDisable(GL_LIGHTING);
				}
				else
				{
					{
						glActiveTexture(GL_TEXTURE1_ARB);

						float a1[4] = {0.6f, 0.6f, 0.6f, 1.0f};
						glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, a1);
					}
					
					glActiveTexture(GL_TEXTURE0_ARB);
					if(	TEXTURE_NORMAL==vLayer[0].uiTextureMode && bActiveColour &&
							!TEXTURE_DETAIL==vLayer[1].uiTextureMode)
					{
						glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, colour);
						glColor4f(colour.r, colour.g, colour.b, colour.a);
					}
					else
					{
						float a0[4] = {1.0f, 1.0f, 1.0f, 1.0f};
						glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, a0);
					}
				}
			}
			else if(3==uiActiveUnits)
			{
				glActiveTexture(GL_TEXTURE2_ARB);
				float a2[4] = {0.5f, 0.5f, 0.5f, 1.0f};
				glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, a2);
				
				glActiveTexture(GL_TEXTURE1_ARB);
				float a1[4] = {0.5f, 0.5f, 0.5f, 1.0f};
				glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, a1);
				
				glActiveTexture(GL_TEXTURE0_ARB);
				if(TEXTURE_NORMAL==vLayer[0].uiTextureMode && bActiveColour)
				{
					glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, colour);

					glColor4f(colour.r, colour.g, colour.b, colour.a);
				}
				else
				{
					float a0[4] = {1.0f, 1.0f, 1.0f, 1.0f};
					glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, a0);
				}
			}
			//else disable none of them

			if(pMaterial->pShader && bShader)
			{
				bActiveShader=true;

				glUseProgram(pMaterial->pShader->uiShaderProgram);

				GLint loc=-1;

				/*loc=glGetUniformLocation(pMaterial->pShader->uiShaderProgram, "cameraPos");
				if(loc != -1)
				  glUniform3f(loc, pCamera->eye.x, pCamera->eye.y, pCamera->eye.z);
				else
					LOG.Error("Shader", ": pMaterial->sName + "Couldn't set cameraPos");*/


				if(uiActiveUnits>0)
				{
					loc=glGetUniformLocation(pMaterial->pShader->uiShaderProgram, "texUnit0");
					if(loc != -1)
						glUniform1i(loc, 0);//pMaterial->vLayer[0]->uiTexture);
					else
						LOG.Error("Shader", pMaterial->sName + ": Couldn't set texUnit0");
				}
	
				if(uiActiveUnits>1)
				{
					loc=glGetUniformLocation(pMaterial->pShader->uiShaderProgram, "texUnit1");
					if(loc != -1)
						glUniform1i(loc, 1);//pMaterial->vLayer[1]->uiTexture);
					else
						LOG.Error("Shader", pMaterial->sName + ": Couldn't set texUnit1");
				}

				//glDisable(GL_LIGHTING);
			}
			else if(bActiveShader)
			{
				bActiveShader=false;
				
				if(bCanShader)
					glUseProgram(NULL);
				
				//glEnable(GL_LIGHTING);
			}
			
			pCurrentMaterial=pMaterial;
			return true;
		}

		MATERIAL::cMaterial* cRender::GetMaterial(std::string sFilename)
		{
			std::map<std::string, MATERIAL::cMaterial * >::iterator iter=mMaterial.begin();

			std::string s;
			MATERIAL::cMaterial *pMaterial=pMaterialNotFoundMaterial;
			for(;iter!=mMaterial.end();iter++)
			{
				s=iter->first;
				if(s==sFilename)
					pMaterial=iter->second;
			};

			return pMaterial;
		}

		
		void cRender::ReloadTextures()
		{
			LOG.Success("Render", "ReloadTextures");
			
			LOG.Success("Render", "ReloadTextures Atlases");
			cTextureAtlas *pAtlas = NULL;
			unsigned int n = vTextureAtlas.size();
			std::vector<unsigned int>vOldTextureAtlas;
			for(unsigned int i = 0;i<n;i++)
			{
				pAtlas = vTextureAtlas[i];
				vOldTextureAtlas.push_back(pAtlas->uiTexture);
				
				// Destroy old texture
				glDeleteTextures(1, &pAtlas->uiTexture);
				
				// Create new texture				
				glGenTextures(1, &pAtlas->uiTexture);
				
				glBindTexture(GL_TEXTURE_2D, pAtlas->uiTexture);

				
				pAtlas->CopyToSurface();


				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pAtlas->surface->w, pAtlas->surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pAtlas->surface->pixels);

				//Remove this line if there are artifacts
				gluBuild2DMipmaps(GL_TEXTURE_2D, 4, pAtlas->surface->w, pAtlas->surface->h, GL_RGBA, GL_UNSIGNED_BYTE, pAtlas->surface->pixels);
			}

			{
				LOG.Success("Render", "ReloadTextures Misc Textures");
				cTexture *pTexture = NULL;
				std::map<std::string, cTexture* >::iterator iter=mTexture.begin();
				std::map<std::string, cTexture* >::iterator iterEnd=mTexture.end();
				while(iter != iterEnd)
				{
					pTexture = iter->second;

					// Destroy old texture
					glDeleteTextures(1, &pTexture->uiTexture);
					
					// Create new texture				
					glGenTextures(1, &pTexture->uiTexture);
					
					glBindTexture(GL_TEXTURE_2D, pTexture->uiTexture);

					if(pTexture->surface)
					{
						pTexture->CopyToSurface();

						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pTexture->surface->w, pTexture->surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pTexture->surface->pixels);

						//Remove this line if there are artifacts
						gluBuild2DMipmaps(GL_TEXTURE_2D, 4, pTexture->surface->w, pTexture->surface->h, GL_RGBA, GL_UNSIGNED_BYTE, pTexture->surface->pixels);
					}

					iter++;
				}
			}

			{
				LOG.Success("Render", "ReloadTextures Materials");
				MATERIAL::cMaterial * pMaterial = NULL;
				std::map<std::string, MATERIAL::cMaterial *>::iterator iter=mMaterial.begin();
				for(;iter!=mMaterial.end();iter++)
				{
					pMaterial=iter->second;

					unsigned int nLayer = pMaterial->vLayer.size();
					for(unsigned int iLayer = 0;iLayer<nLayer;iLayer++)
					{
						bool bFound = false;
						unsigned int iAtlas = 0;
						while(iAtlas<n && !bFound)
						{
							if(pMaterial->vLayer[iLayer]->uiTexture == vOldTextureAtlas[iAtlas])
							{
								pMaterial->vLayer[iLayer]->uiTexture = vTextureAtlas[iAtlas]->uiTexture;
								bFound = true;
							}

							iAtlas++;
						}

						if(!bFound)
						{
							pMaterial->vLayer[iLayer]->uiTexture = GetTexture(iLayer == 0 ? pMaterial->sTexture0 :
																																iLayer == 1 ? pMaterial->sTexture1 :
																																pMaterial->sTexture2)->uiTexture;
						}
					}

					if(pMaterial->pShader)
					{
						pMaterial->pShader->Destroy();
						pMaterial->pShader->Init();
					}
				}
			}

			{
				LOG.Success("Render", "ReloadTextures Vertex Buffer Objects");
				n = vVertexBufferObject.size();
				for(i = 0;i<n;i++)
				{
					vVertexBufferObject[i]->Destroy();
					vVertexBufferObject[i]->Init();
				}
			}
		}
	}
}
