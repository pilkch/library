#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <list>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>

#include <BREATHE/cMem.h>

#include <BREATHE/cLog.h>
#include <BREATHE/cBreathe.h>
#include <BREATHE/cFileSystem.h>

#include <math.h>
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

#include <BREATHE/RENDER/cCamera.h>


/*#include <BREATHE/cBase.h>

#include <BREATHE/cModel_Animation.h>
#include <BREATHE/cModel_Static.h>
#include <BREATHE/cModel_Roam_Patch.h>
#include <BREATHE/cModel_Roam.h>
#include <BREATHE/cModel.h>*/

#include <BREATHE/RENDER/cTexture.h>
#include <BREATHE/RENDER/cTextureAtlas.h>
#include <BREATHE/RENDER/cMaterial.h>
#include <BREATHE/RENDER/cRender.h>

#include <BREATHE/cBase.h>
#include <BREATHE/MODEL/cMesh.h>
#include <BREATHE/MODEL/cModel.h>
#include <BREATHE/MODEL/cStatic.h>

#include <BREATHE/cLevel.h>

PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLMULTITEXCOORD2FPROC glMultiTexCoord2f;
PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;

PFNGLGENBUFFERSARBPROC glGenBuffersARB;
PFNGLBINDBUFFERARBPROC glBindBufferARB;
PFNGLBUFFERDATAARBPROC glBufferDataARB;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;

PFNGLSHADERSOURCEARBPROC glShaderSource;
PFNGLCOMPILESHADERARBPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDELETEPROGRAMPROC glDeleteProgram;

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

			uiWidth=1024;
			uiHeight=768;
			uiBPP = 32;
			uiFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE;

			iMaxTextureSize=0;

			uiActiveUnits=0;

			v3SunPosition.Set(10.0f, 10.0f, 5.0f, 0.0f);


			pCurrentMaterial=NULL;
			pLevel=NULL;

			pCamera=new cCamera();

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

			pLog->Success("Delete", "Camera");
			if(pCamera)
			{
				delete pCamera;
				pCamera=NULL;
			}
		}

		bool cRender::FindExtension(std::string sExt)
		{
			std::ostringstream stm;
			stm<<static_cast<const unsigned char *>(glGetString( GL_EXTENSIONS ));

			return (stm.str().find(sExt) != std::string::npos);
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

	#ifdef _DEBUG
			pLog->Success("Render", "Screen BPP: %d", SDL_GetVideoSurface()->format->BitsPerPixel);
			pLog->Success("Render", "Vendor     : %s", glGetString( GL_VENDOR ));
			pLog->Success("Render", "Renderer   : %s", glGetString( GL_RENDERER ));
			pLog->Success("Render", "Version    : %s", glGetString( GL_VERSION ));
			pLog->Success("Render", "Extensions : %s", glGetString( GL_EXTENSIONS ));
			if(iMaxTextureSize>=1024)
				pLog->Success("Render", "Max Texture Size : %d", iMaxTextureSize);
			else
				pLog->Error("Render", "Max Texture Size : %d", iMaxTextureSize);
	#endif //_DEBUG

			if(iMaxTextureSize>=1024)
				iMaxTextureSize=1024;


			if(FindExtension("GL_ARB_multitexture"))
        pLog->Success("Render", "Found GL_ARB_multitexture");
			else
			{
				pLog->Error("Render", "Not Found GL_ARB_multitexture");
				return false;
			}



			if(FindExtension("GL_ARB_texture_cube_map"))
				pLog->Success("Render", "Found GL_ARB_texture_cube_map");
			else
			{
				pLog->Error("Render", "Not Found GL_ARB_texture_cube_map");
				return false;
			}

			
			*(void**)&glClientActiveTexture = SDL_GL_GetProcAddress("glClientActiveTexture");
			*(void**)&glActiveTexture = SDL_GL_GetProcAddress("glActiveTexture");
			*(void**)&glMultiTexCoord2f = SDL_GL_GetProcAddress("glMultiTexCoord2f");



			//GL_SHADING_LANGUAGE_VERSION

			float fShaderVersion=0.0f;
			char buffer[100]="";
			sprintf(buffer, "%s", glGetString(GL_VERSION));
				
			std::stringstream stm(buffer);
			
			stm >> fShaderVersion;
			
			if(fShaderVersion<2.0f)
			{
				pLog->Error("Render", "Not Found Shader2.0");
				bCanShader=false;
				bShader=false;
			}
			else
			{
				pLog->Success("Render", "Found Shader2.0");

				*(void**)&glShaderSource = SDL_GL_GetProcAddress("glShaderSource");
				*(void**)&glCompileShader = SDL_GL_GetProcAddress("glCompileShader");
				*(void**)&glCreateProgram = SDL_GL_GetProcAddress("glCreateProgram");
				*(void**)&glCreateShader = SDL_GL_GetProcAddress("glCreateShader");
				*(void**)&glAttachShader = SDL_GL_GetProcAddress("glAttachShader");
				*(void**)&glLinkProgram = SDL_GL_GetProcAddress("glLinkProgram");
				*(void**)&glUseProgram = SDL_GL_GetProcAddress("glUseProgram");
				*(void**)&glGetUniformLocation = SDL_GL_GetProcAddress("glGetUniformLocation");
				*(void**)&glUniform3f  = SDL_GL_GetProcAddress("glUniform3f");
				*(void**)&glUniformMatrix4fv = SDL_GL_GetProcAddress("glUniformMatrix4fv");
				*(void**)&glUniform1i = SDL_GL_GetProcAddress("glUniform1i");
				*(void**)&glGetShaderiv = SDL_GL_GetProcAddress("glGetShaderiv");
				*(void**)&glGetShaderInfoLog = SDL_GL_GetProcAddress("glGetShaderInfoLog");
				*(void**)&glGetProgramiv = SDL_GL_GetProcAddress("glGetProgramiv");
				*(void**)&glGetProgramInfoLog = SDL_GL_GetProcAddress("glGetProgramInfoLog");
				*(void**)&glDeleteShader = SDL_GL_GetProcAddress("glDeleteShader");
				*(void**)&glDeleteProgram = SDL_GL_GetProcAddress("glDeleteProgram");
				
				bCanShader=
					glShaderSource &&
					glCompileShader &&
					glCreateProgram &&
					glCreateShader &&
					glAttachShader &&
					glLinkProgram &&
					glUseProgram &&
					glGetUniformLocation &&
					glUniform3f &&
					glUniformMatrix4fv &&
					glUniform1i &&
					glGetShaderiv &&
					glGetShaderInfoLog &&
					glGetProgramiv &&
					glGetProgramInfoLog &&
					glDeleteShader &&
					glDeleteProgram;
        
				bShader=bCanShader;
			}
			
			if(bCanShader)
				pLog->Success("Render", "Can use shaders, shaders turned on");
			else
				pLog->Success("Render", "Cannot use shaders, shaders turned off");


			if(	NULL!=glActiveTexture && 
					NULL!=glMultiTexCoord2f &&
					NULL!=glClientActiveTexture)
				return BREATHE::GOOD;

			return BREATHE::BAD;
		}
		
		void cRender::SetCamera(cCamera *c)
		{
			
		}

		void cRender::BeginFrame(float fCurrentTime)
		{
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			glLoadIdentity();

			glMultMatrixf(pCamera->m);

			SetColour();

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
		}

		void cRender::BeginHUD(float fCurrentTime)
		{
			SetColour();

			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			glDisable(GL_LIGHTING);
		}

		void cRender::EndFrame()
		{
			SDL_GL_SwapBuffers( );
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
				pLog->Error("Texture", "%s pTextureNotFound", sNewFilename.c_str());
			}
			else
				pLog->Success("Texture", "%s %d", sNewFilename.c_str(), p->uiTexture);
			
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

			pLog->Success("Texture", "Loading %s", sNewFilename.c_str());
		
			unsigned int mode=0;
			SDL_Surface *surface = IMG_Load(sNewFilename.c_str());

			// could not load filename
			if (!surface)
			{
				pLog->Error("Texture", "Couldn't Load Texture %s", sNewFilename.c_str());
				return false;
			}

			if(surface->format->BytesPerPixel == 3) // RGB 24bit
			{
				mode = GL_RGB;
				pLog->Success("Texture", "RGB Image");
			}
			else if(surface->format->BytesPerPixel == 4)// RGBA 32bit
			{
				mode = GL_RGBA;
				pLog->Success("Texture", "RGBA Image");
			}
			else
			{
				SDL_FreeSurface(surface);
				pLog->Error("Texture", "Error Unknown Image Format (%d)", surface->format->BytesPerPixel);
				
				return false;
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
			
				delete[] pBuf;
			}

			// create one texture name
			glGenTextures(1, &p->uiTexture);

			// tell opengl to use the generated texture name
			glBindTexture(GL_TEXTURE_2D, p->uiTexture);

			// this reads from the sdl surface and puts it into an opengl texture
			glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

			// these affect how this texture is drawn later on...
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);

			// clean up
			SDL_FreeSurface(surface);

			std::ostringstream s;
			s<<sNewFilename<<" "<<p->uiTexture<<"\0";
			pLog->Success("Texture", s.str().c_str());
			
			mTexture[sNewFilename]=p;		

			return p;
		}

		bool cRender::AddTextureNotFoundTexture(std::string sFilename)
		{
			pTextureNotFoundTexture=new cTexture();

			sFilename=pFileSystem->FindFile(sFilename);

			pLog->Success("Texture", "Loading %s", sFilename.c_str());
		
			unsigned int mode=0;
			SDL_Surface *surface = IMG_Load(sFilename.c_str());

			// could not load filename
			if (!surface)
			{
				pLog->Error("Texture", "Couldn't Load Texture %s", sFilename.c_str());
				return false;
			}

			if(surface->format->BytesPerPixel == 3) // RGB 24bit
			{
				mode = GL_RGB;
				pLog->Success("Texture", "RGB Image");
			}
			else if(surface->format->BytesPerPixel == 4)// RGBA 32bit
			{
				mode = GL_RGBA;
				pLog->Success("Texture", "RGBA Image");
			}
			else
			{
				SDL_FreeSurface(surface);
				pLog->Error("Texture", "Error Unknown Image Format (%d)", surface->format->BytesPerPixel);
				
				return false;
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
			
				delete[] pBuf;
			}

			//TODO: Put into a texture atlas

			// create one texture name
			glGenTextures(1, &pTextureNotFoundTexture->uiTexture);

			// tell opengl to use the generated texture name
			glBindTexture(GL_TEXTURE_2D, pTextureNotFoundTexture->uiTexture);

			// this reads from the sdl surface and puts it into an opengl texture
			glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

			// these affect how this texture is drawn later on...
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// clean up
			SDL_FreeSurface(surface);

			pLog->Success("Texture", "TextureNotFoundTexture %d", pTextureNotFoundTexture->uiTexture);

			return true;
		}

		bool cRender::AddMaterialNotFoundTexture(std::string sFilename)
		{
			pMaterialNotFoundTexture=new cTexture();

			sFilename=pFileSystem->FindFile(sFilename);

			pLog->Success("Texture", "Loading %s", sFilename.c_str());
		
			unsigned int mode=0;
			SDL_Surface *surface = IMG_Load(sFilename.c_str());

			// could not load filename
			if (!surface)
			{
				pLog->Error("Texture", "Couldn't Load Texture %s", sFilename.c_str());
				return false;
			}

			if(surface->format->BytesPerPixel == 3) // RGB 24bit
			{
				mode = GL_RGB;
				pLog->Success("Texture", "RGB Image");
			}
			else if(surface->format->BytesPerPixel == 4)// RGBA 32bit
			{
				mode = GL_RGBA;
				pLog->Success("Texture", "RGBA Image");
			}
			else
			{
				SDL_FreeSurface(surface);
				pLog->Error("Texture", "Error Unknown Image Format (%d)", surface->format->BytesPerPixel);
				
				return false;
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
			
				delete[] pBuf;
			}

			//TODO: Put into a texture atlas

			// create one texture name
			glGenTextures(1, &pMaterialNotFoundTexture->uiTexture);

			// tell opengl to use the generated texture name
			glBindTexture(GL_TEXTURE_2D, pMaterialNotFoundTexture->uiTexture);

			// this reads from the sdl surface and puts it into an opengl texture
			glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

			// these affect how this texture is drawn later on...
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// clean up
			SDL_FreeSurface(surface);

			pLog->Success("Texture", "MaterialNotFoundTexture %d", pMaterialNotFoundTexture->uiTexture);

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

			pLog->Success("Texture", "Loading CubeMap %s", sFilename.c_str());
		
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
					pLog->Error("Texture", "Couldn't Load Texture %s", sFilename.c_str());
					return false;
				}

				if(surface->format->BytesPerPixel == 3) // RGB 24bit
				{
					mode = GL_RGB;
					pLog->Success("Texture", "RGB Image");
				}
				else if(surface->format->BytesPerPixel == 4)// RGBA 32bit
				{
					mode = GL_RGBA;
					pLog->Success("Texture", "RGBA Image");
				}
				else
				{
					SDL_FreeSurface(surface);
					pLog->Error("Texture", "Error Unknown Image Format (%d)", surface->format->BytesPerPixel);
					
					return false;
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
				
					delete[] pBuf;
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


			pLog->Success("Texture", "%d", p->uiTexture);

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
				delete pMaterial;
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

		bool cRender::SetMaterial()
		{
			unsigned int i=0;
			unsigned int n=0;
			unsigned int unit=GL_TEXTURE0;

			for(i=n;i<MATERIAL::nLayers;i++)
			{
				if(1==i)
					unit=GL_TEXTURE1;
				else if(2==i)
					unit=GL_TEXTURE2;
				else if(3==i)
					unit=GL_TEXTURE3;

				//Activate the current texture unit
				glActiveTexture(unit);

				//Undo last mode
				if(TEXTURE_MASK==vLayer[i].uiTextureMode || TEXTURE_BLEND==vLayer[i].uiTextureMode)
				{
					glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
					glBlendFunc(GL_ONE, GL_ZERO);
					glDisable(GL_BLEND);
				}
				else if(TEXTURE_CUBEMAP==vLayer[i].uiTextureMode)
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
				vLayer[i].uiTextureMode=TEXTURE_NONE;
			}

			glActiveTexture(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);

			bActiveShader=false;
			
			if(bCanShader)
        glUseProgram(NULL);

			SetColour();

			return true;
		}

		bool cRender::SetMaterial(MATERIAL::cMaterial* pMaterial, MATH::cVec3 pos, cCamera &camera)
		{
			if(pMaterial == NULL)
			{
				pLog->Error("Render", "No texture specified");
				return false;
			}

			if(pCurrentMaterial == pMaterial)
			{
				// Update camera possibly?
				return true;
			}

			unsigned int i=0;
			unsigned int n=MATERIAL::nLayers;
			unsigned int unit=0;

			std::vector<MATERIAL::cLayer>vNewLayer;

			for(i=0;i<n;i++)
				if((TEXTURE_NONE!=pMaterial->vLayer[i]->uiTextureMode && TEXTURE_CUBEMAP!=pMaterial->vLayer[i]->uiTextureMode) ||
					(TEXTURE_CUBEMAP==pMaterial->vLayer[i]->uiTextureMode && bCubemap))
						vNewLayer.push_back(*pMaterial->vLayer[i]);

			if(0==n)
				pLog->Error("Render", "here");

			n=vNewLayer.size();
			uiActiveUnits=n;

			unit=GL_TEXTURE0;
			
			for(i=0;i<n;i++)
			{
				if(1==i)
					unit=GL_TEXTURE1;
				else if(2==i)
					unit=GL_TEXTURE2;
				else if(3==i)
					unit=GL_TEXTURE3;

				//If this is a cubemap, set the material texture to the cubemap before we get there
				if(TEXTURE_CUBEMAP==vNewLayer[i].uiTextureMode)
					vNewLayer[i].uiTexture=pLevel->FindClosestCubeMap(pos)->uiTexture;

				//Activate the current texture unit
				glActiveTexture(unit);

				//Different mode, probably means different texture, change mode and bind it anyway
				if(vLayer[i].uiTextureMode!=vNewLayer[i].uiTextureMode)
				{
					//Undo last mode
					if(TEXTURE_MASK==vLayer[i].uiTextureMode || TEXTURE_BLEND==vLayer[i].uiTextureMode)
					{
						glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
						glBlendFunc(GL_ONE, GL_ZERO);
						glDisable(GL_BLEND);
					}
					else if(TEXTURE_CUBEMAP==vLayer[i].uiTextureMode)
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
					vLayer[i].uiTextureMode=vNewLayer[i].uiTextureMode;
					vLayer[i].uiTexture=vNewLayer[i].uiTexture;

					if(TEXTURE_NONE==vLayer[i].uiTextureMode)
						glDisable(GL_TEXTURE_2D);
					else if(TEXTURE_NORMAL==vLayer[i].uiTextureMode)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, vLayer[i].uiTexture);
					}
					else if(TEXTURE_MASK==vLayer[i].uiTextureMode)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, vLayer[i].uiTexture);

						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
						glEnable(GL_BLEND);
					}
					else if(TEXTURE_BLEND==vLayer[i].uiTextureMode)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, vLayer[i].uiTexture);

						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
						glEnable(GL_BLEND);
					}
					else if(TEXTURE_CUBEMAP==vLayer[i].uiTextureMode)
					{
						//Assume we got one we shouldn't be here if we didn't
						//It is possible if there are NO cubemaps in the whole level,
						//so make sure we load one already
						glDisable(GL_TEXTURE_2D);
						glEnable(GL_TEXTURE_CUBE_MAP);
						glBindTexture(GL_TEXTURE_CUBE_MAP, vLayer[i].uiTexture);

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
						
						float y=-Angle(MATH::cVec2(camera.eye.x, camera.eye.y), MATH::cVec2(camera.target.x, camera.target.y));
						float x=-Angle(MATH::cVec2(camera.eye.y, camera.eye.z), MATH::cVec2(camera.target.y, camera.target.z));
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
						glBindTexture(GL_TEXTURE_2D, vLayer[i].uiTexture);
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

				/*//Same Mode, just change texture
				else if(vLayer[i].uiTexture!=vNewLayer[i].uiTexture)
				{
					if(TEXTURE_MASK==vLayer[i].uiTextureMode)
					{
						vLayer[i].uiTexture=vNewLayer[i].uiTexture;
						glBindTexture(GL_TEXTURE_2D, vNewLayer[i].uiTexture);
					}
					else if(TEXTURE_BLEND==vLayer[i].uiTextureMode)
					{
						vLayer[i].uiTexture=vNewLayer[i].uiTexture;
						glBindTexture(GL_TEXTURE_2D, vNewLayer[i].uiTexture);

						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
						glEnable(GL_BLEND);
					}
					else if(TEXTURE_CUBEMAP==vLayer[i].uiTextureMode)
					{
						vLayer[i].uiTexture=vNewLayer[i].uiTexture;

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
				}*/
			}

			for(i=n;i<MATERIAL::nLayers;i++)
			{
				if(1==i)
					unit=GL_TEXTURE1;
				else if(2==i)
					unit=GL_TEXTURE2;
				else if(3==i)
					unit=GL_TEXTURE3;

				//Activate the current texture unit
				glActiveTexture(unit);

				//Undo last mode
				if(TEXTURE_MASK==vLayer[i].uiTextureMode || TEXTURE_BLEND==vLayer[i].uiTextureMode)
				{
					glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
					glBlendFunc(GL_ONE, GL_ZERO);
					glDisable(GL_BLEND);
				}
				else if(TEXTURE_CUBEMAP==vLayer[i].uiTextureMode)
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
				vLayer[i].uiTextureMode=TEXTURE_NONE;
			}



			if(1==uiActiveUnits)
			{			
				glActiveTexture(GL_TEXTURE0);
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
				glActiveTexture(GL_TEXTURE1);
				float a1[4] = {0.6f, 0.6f, 0.6f, 1.0f};
				glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, a1);
				
				glActiveTexture(GL_TEXTURE0);
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
			else if(3==uiActiveUnits)
			{
				glActiveTexture(GL_TEXTURE2);
				float a2[4] = {0.5f, 0.5f, 0.5f, 1.0f};
				glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, a2);
				
				glActiveTexture(GL_TEXTURE1);
				float a1[4] = {0.5f, 0.5f, 0.5f, 1.0f};
				glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, a1);
				
				glActiveTexture(GL_TEXTURE0);
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
				int n=1024;

				loc=glGetUniformLocation(pMaterial->pShader->uiShaderProgram, "cameraPos");
				if(loc != -1)
				  glUniform3f(loc, pCamera->eye.x, pCamera->eye.y, pCamera->eye.z);
				else
					pLog->Error("Shader", "%s: Couldn't set cameraPos", pMaterial->sName.c_str());


				if(uiActiveUnits>0)
				{
					loc=glGetUniformLocation(pMaterial->pShader->uiShaderProgram, "texUnit0");
					if(loc != -1)
						glUniform1i(loc, 0);//pMaterial->vLayer[0]->uiTexture);
					else
						pLog->Error("Shader", "%s: Couldn't set texUnit0", pMaterial->sName.c_str());
				}
	
				if(uiActiveUnits>1)
				{
					loc=glGetUniformLocation(pMaterial->pShader->uiShaderProgram, "texUnit1");
					if(loc != -1)
						glUniform1i(loc, 1);//pMaterial->vLayer[1]->uiTexture);
					else
						pLog->Error("Shader", "%s: Couldn't set texUnit1", pMaterial->sName.c_str());
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

		void cRender::SetColour()
		{
			bActiveColour=false;

			colour.SetBlack();
			colour.a=1.0f;
		}
		
		void cRender::SetColour(MATH::cColour inColour)
		{
			bActiveColour=true;

			colour=inColour;
			colour.a=1.0f;
		}

		void cRender::ReloadMaterials()
		{
			std::map<std::string, MATERIAL::cMaterial *>::iterator iter=mMaterial.begin();
			MATERIAL::cMaterial *pMaterial;

			while(iter != mMaterial.end())
			{
				pMaterial=iter->second;

				if(pMaterial->pShader)
				{
					pMaterial->pShader->Destroy();
					pMaterial->pShader->Init();
				}

				iter++;
			};
		}
	}
}
