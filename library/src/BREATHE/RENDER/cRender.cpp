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

const float fDetailScale = 0.5f;

BREATHE::RENDER::cRender* pRender = NULL;

namespace BREATHE
{
	namespace RENDER
	{
		cRender::cRender() : 
			bRenderWireframe(false),
			bLight(true),
			bCubemap(true),
			bShader(true),
			
			bCanCubemap(false),
			bCanShader(false),
			bCanFrameBufferObject(false),

			bActiveShader(false),
			bActiveColour(false),

			bFullscreen(true),
			uiWidth(1024),
			uiHeight(768),
			uiDepth(32)
		{
			pRender = this;

			uiFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE;

			iMaxTextureSize=0;

			uiActiveUnits=0;

			v4SunPosition.Set(10.0f, 10.0f, 5.0f, 0.0f);


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

			glLightfv(GL_LIGHT0, GL_POSITION, v4SunPosition );

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


			// Cube Map Support
			if(FindExtension("GL_ARB_texture_cube_map"))
			{
				LOG.Success("Render", "Found GL_ARB_texture_cube_map");
				bCanCubemap = true;
			}
			else
			{
				LOG.Error("Render", "Not Found GL_ARB_texture_cube_map");
				return false;
			}


			// GLSL Support
			float fShaderVersion=0.0f;
			char buffer[100]="";
			strcpy(buffer, (char*)glGetString(GL_VERSION));	//GL_SHADING_LANGUAGE_VERSION
				
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


			// Frame Buffer Object Support
			if(FindExtension("GL_EXT_framebuffer_object"))
			{
				LOG.Success("Render", "Found GL_EXT_framebuffer_object");
				bCanFrameBufferObject = true;
			}
			else
				LOG.Error("Render", "Not Found GL_EXT_framebuffer_object");


			return BREATHE::GOOD;
		}
		
		void cRender::BeginRenderToScreen()
		{
			// Set viewport
			glViewport(0, 0, uiWidth, uiHeight);

			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
			glLoadIdentity();

			glMultMatrixf(pFrustum->m);

			if(bRenderWireframe)
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			else
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

			
			if(bLight)
			{
				glLightfv (GL_LIGHT0, GL_POSITION, v4SunPosition);
				glEnable( GL_LIGHTING );
			}
			else
				glDisable(GL_LIGHTING);

			ClearColour();
			ClearMaterial();

			uiTextureModeChanges = uiTextureChanges = uiTriangles = 0;
		}

		void cRender::EndRenderToScreen()
		{
			SDL_GL_SwapBuffers();
		}

		

		void cRender::BeginRenderToTexture(cTextureFrameBufferObject* pTexture)
		{
			// First we bind the FBO so we can render to it
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, pTexture->uiFBO);
			
			// Save the view port and set it to the size of the texture
			glPushAttrib(GL_VIEWPORT_BIT);
			glViewport(0, 0, FBO_TEXTURE_WIDTH, FBO_TEXTURE_HEIGHT);

			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glMultMatrixf(pFrustum->m);
		}

		void cRender::EndRenderToTexture(cTextureFrameBufferObject* pTexture)
		{
			// Restore old view port and set rendering back to default frame buffer
			glPopAttrib();
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

			
			// Now bind the texture to use it
			glBindTexture(GL_TEXTURE_2D, pTexture->uiTexture);

#ifdef RENDER_GENERATEFBOMIPMAPS
			glGenerateMipmapEXT(GL_TEXTURE_2D);
#endif //RENDER_GENERATEFBOMIPMAPS

			glDisable(GL_TEXTURE_2D);
		}




		// Our screen coordinates look like this
		// 0.0f, 0.0f						1.0f, 0.0f
		//
		//
		// 0.0f, 1.0f						1.0f, 1.0f

		void cRender::RenderHUDElement(float fX, float fY, float fWidth, float fHeight)
		{
			fWidth *= uiWidth;
			fHeight *= uiHeight;
			fX *= uiWidth;
			fY = (1.0f - fY) * uiHeight;

			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(fX, fY - fHeight);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(fX + fWidth, fY - fHeight);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(fX + fWidth, fY);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(fX, fY);
			glEnd();
		}

		void cRender::BeginHUD()
		{
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			glDisable(GL_LIGHTING);
			
			ClearColour();
			ClearMaterial();


			// Our screen coordinates look like this
			// 0.0f, 1.0f						1.0f, 1.0f
			//
			//
			// 0.0f, 0.0f						1.0f, 0.0f

			glPushAttrib(GL_TRANSFORM_BIT);
				glMatrixMode(GL_PROJECTION);		// Select Projection
				glPushMatrix();									// Push The Matrix
				glLoadIdentity();								// Reset The Matrix
				glOrtho( 0, uiWidth, 0, uiHeight, -1, 1 );	// Select Ortho Mode
				glMatrixMode(GL_MODELVIEW);			// Select Modelview Matrix
				glPushMatrix();									// Push The Matrix
				glLoadIdentity();								// Reset The Matrix
		}
		
		void cRender::EndHUD()
		{
				glMatrixMode( GL_PROJECTION );	// Select Projection
				glPopMatrix();									// Pop The Matrix
				glMatrixMode( GL_MODELVIEW );		// Select Modelview
				glPopMatrix();									// Pop The Matrix
			glPopAttrib();			
		}

		void cRender::RenderBox(MATH::cVec3& vMin, MATH::cVec3& vMax)
		{
    	glBegin(GL_LINES);

				// Bottom Square
				glVertex3f(vMin.x, vMin.y, vMin.z);
				glVertex3f(vMax.x, vMin.y, vMin.z);

				glVertex3f(vMin.x, vMin.y, vMin.z);
				glVertex3f(vMin.x, vMax.y, vMin.z);

				glVertex3f(vMax.x, vMax.y, vMin.z);
				glVertex3f(vMax.x, vMin.y, vMin.z);

				glVertex3f(vMax.x, vMax.y, vMin.z);
				glVertex3f(vMin.x, vMax.y, vMin.z);

				// Side Edges
				glVertex3f(vMin.x, vMin.y, vMin.z);
				glVertex3f(vMin.x, vMin.y, vMax.z);

				glVertex3f(vMax.x, vMin.y, vMin.z);
				glVertex3f(vMax.x, vMin.y, vMax.z);

				glVertex3f(vMin.x, vMax.y, vMin.z);
				glVertex3f(vMin.x, vMax.y, vMax.z);

				glVertex3f(vMax.x, vMax.y, vMin.z);
				glVertex3f(vMax.x, vMax.y, vMax.z);

				// Upper Square
				glVertex3f(vMin.x, vMin.y, vMax.z);
				glVertex3f(vMax.x, vMin.y, vMax.z);

				glVertex3f(vMin.x, vMin.y, vMax.z);
				glVertex3f(vMin.x, vMax.y, vMax.z);

				glVertex3f(vMax.x, vMax.y, vMax.z);
				glVertex3f(vMax.x, vMin.y, vMax.z);

				glVertex3f(vMax.x, vMax.y, vMax.z);
				glVertex3f(vMin.x, vMax.y, vMax.z);

			glEnd();
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
			if((""==sNewFilename) || (ATLAS_NONE == uiAtlas))
				return pTextureNotFoundTexture;
			
			std::string sFilename=BREATHE::FILESYSTEM::FindFile(sNewFilename);
			std::string s = BREATHE::FILESYSTEM::GetFile(sFilename);

			cTexture* p = vTextureAtlas[uiAtlas]->AddTexture(sFilename);
			if(NULL==p)
			{
				LOG.Error("Texture", sFilename + " pTextureNotFound");
				return pTextureNotFoundTexture;
			}
			
			std::ostringstream t;
			t << p->uiTexture;
			LOG.Success("Texture", sFilename + " " + t.str());

			return p;
		}

		cTexture *cRender::AddTexture(std::string sNewFilename)
		{
			if(""==sNewFilename)
				return pTextureNotFoundTexture;
			
			std::string sFilename = BREATHE::FILESYSTEM::FindFile(sNewFilename);
			std::string s = BREATHE::FILESYSTEM::GetFile(sFilename);

			
			cTexture* p = GetTexture(s);
			
			if(p != pTextureNotFoundTexture) return p;
			
			p=new cTexture();
			
			if(p->Load(sFilename) != BREATHE::GOOD)
			{
				SAFE_DELETE(p);
				return pTextureNotFoundTexture;
			}
			
			p->Create();
			p->CopyFromSurfaceToTexture();
			
			mTexture[s]=p;		

			std::ostringstream t;
			t << p->uiTexture;
			LOG.Success("Texture", "Texture " + s + " uiTexture=" + t.str());
			return p;
		}

		bool cRender::AddTextureNotFoundTexture(std::string sNewFilename)
		{
			cTexture* p=new cTexture();

			std::string sFilename=BREATHE::FILESYSTEM::FindFile(sNewFilename);
			
			if(p->Load(sFilename) != BREATHE::GOOD)
			{
				SAFE_DELETE(p);
				return BREATHE::BAD;
			}
			
			p->Create();
			p->CopyFromSurfaceToTexture();

			mTexture[sNewFilename]=p;	
			pTextureNotFoundTexture = p;

			std::ostringstream t;
			t << pTextureNotFoundTexture->uiTexture;
			LOG.Success("Texture", "TextureNotFoundTexture " + t.str());

			return BREATHE::GOOD;
		}

		bool cRender::AddMaterialNotFoundTexture(std::string sNewFilename)
		{
			cTexture* p=new cTexture();

			std::string sFilename=BREATHE::FILESYSTEM::FindFile(sNewFilename);

			LOG.Success("Texture", "Loading " + sFilename);
			
			if(p->Load(sFilename) != BREATHE::GOOD)
			{
				SAFE_DELETE(p);
				return BREATHE::BAD;
			}
			
			p->Create();
			p->CopyFromSurfaceToTexture();

			mTexture[sNewFilename]=p;
			pMaterialNotFoundTexture = p;

			std::ostringstream t;
			t << pMaterialNotFoundTexture->uiTexture;
			LOG.Success("Texture", "MaterialNotFoundTexture " + t.str());

			return BREATHE::GOOD;
		}


		cTexture *cRender::GetTexture(std::string sNewFilename)
		{			
			std::map<std::string, cTexture *>::iterator iter = mTexture.find(sNewFilename);
			if(iter != mTexture.end())
				return iter->second;

			return pTextureNotFoundTexture;
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
			/*TODO: Surface of 1x6 that holds the cubemap faces,
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

			std::string sFile=BREATHE::FILESYSTEM::GetFileNoExtension(sFilename);
			std::string sExt=BREATHE::FILESYSTEM::GetExtension(sFilename);
			
			for(i=0;i<6;i++)
			{
				s.str( "" );

				s<<sFile<<"/"<<sFile<<i<<"."<<sExt;
				sFilename=BREATHE::FILESYSTEM::FindFile(s.str());

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

			sNewFilename=BREATHE::FILESYSTEM::FindFile(sNewFilename);

			pMaterialNotFoundMaterial=new MATERIAL::cMaterial("MaterialNotFound");

			pMaterialNotFoundMaterial->vLayer[0]->sTexture=sNewFilename;
			pMaterialNotFoundMaterial->vLayer[0]->pTexture=pMaterialNotFoundTexture;
			pMaterialNotFoundMaterial->vLayer[0]->uiTextureMode=TEXTURE_NORMAL;

			return pMaterialNotFoundMaterial;
		}

		MATERIAL::cMaterial *cRender::AddMaterial(std::string sNewfilename)
		{
			if(""==sNewfilename)
				return NULL;

			MATERIAL::cMaterial *pMaterial = GetMaterial(sNewfilename);
			
			if(pMaterial != pMaterialNotFoundMaterial)
				return pMaterial;
			
			std::string sFilename = FILESYSTEM::FindFile(sNewfilename);
			pMaterial=new MATERIAL::cMaterial(sFilename);

			if(BREATHE::BAD == pMaterial->Load(sFilename))
			{
				SAFE_DELETE(pMaterial);
				pMaterial=pMaterialNotFoundMaterial;
			}
			
			mMaterial[sFilename]=pMaterial;

			return pMaterial;
		}

		float Angle(const MATH::cVec2 & a, const MATH::cVec2 & b)
		{
			if(a.x>b.x)
			{
				if(a.y>b.y)
					return (atan((a.y-b.y)/(a.x-b.x)) + MATH::cPI_DIV_180 * 90.0f) * MATH::c180_DIV_PI;
				
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

					//glEnable(GL_LIGHTING);
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
					layerNew->pTexture=pLevel->FindClosestCubeMap(pos);

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
					layerOld->pTexture=layerNew->pTexture;

					if(TEXTURE_NONE==layerOld->uiTextureMode)
						glDisable(GL_TEXTURE_2D);
					else if(TEXTURE_NORMAL==layerOld->uiTextureMode)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);
					}
					else if(TEXTURE_MASK==layerOld->uiTextureMode)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);

						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
						glEnable(GL_BLEND);
					}
					else if(TEXTURE_BLEND==layerOld->uiTextureMode)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);

						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
						glEnable(GL_BLEND);
					}
					else if(TEXTURE_DETAIL==layerOld->uiTextureMode)
					{
						glActiveTexture(GL_TEXTURE0_ARB);
						//glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
						//glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);

						glActiveTexture(GL_TEXTURE1_ARB);
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);
						//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
						//glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);
										
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
						glBindTexture(GL_TEXTURE_CUBE_MAP, layerOld->pTexture->uiTexture);

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
						glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);
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
				else if(layerOld->pTexture->uiTexture!=layerNew->pTexture->uiTexture)
				{
					uiTextureChanges++;
					if(	TEXTURE_MASK==layerOld->uiTextureMode || 
							TEXTURE_DETAIL==layerOld->uiTextureMode)
					{
						layerOld->pTexture->uiTexture=layerNew->pTexture->uiTexture;
						glBindTexture(GL_TEXTURE_2D, layerNew->pTexture->uiTexture);
					}
					else if(TEXTURE_BLEND==layerOld->uiTextureMode)
					{
						layerOld->pTexture->uiTexture=layerNew->pTexture->uiTexture;
						glBindTexture(GL_TEXTURE_2D, layerNew->pTexture->uiTexture);

						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
						glEnable(GL_BLEND);
					}
					else if(TEXTURE_CUBEMAP==layerOld->uiTextureMode)
					{
						layerOld->pTexture->uiTexture=layerNew->pTexture->uiTexture;

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
					layerOld->pTexture->uiTexture=layerNew->pTexture->uiTexture;




					
					if(TEXTURE_NONE==layerOld->uiTextureMode)
						glDisable(GL_TEXTURE_2D);
					else if(TEXTURE_NORMAL==layerOld->uiTextureMode)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);
					}
					else if(TEXTURE_MASK==layerOld->uiTextureMode)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);

						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
						glEnable(GL_BLEND);
					}
					else if(TEXTURE_BLEND==layerOld->uiTextureMode || 
									TEXTURE_DETAIL==layerOld->uiTextureMode)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);

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
						glBindTexture(GL_TEXTURE_CUBE_MAP, layerOld->pTexture->uiTexture);

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
						glBindTexture(GL_TEXTURE_2D, layerOld->pTexture->uiTexture);
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
					//glEnable(GL_LIGHTING);
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
					glBindTexture(GL_TEXTURE_2D, pMaterial->vLayer[0]->pTexture->uiTexture);
					//glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
					//glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);

					// TEXTURE-UNIT #1
					glActiveTexture(GL_TEXTURE1_ARB);
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, pMaterial->vLayer[1]->pTexture->uiTexture);
					//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
					//glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);
									
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
						//glDisable(GL_LIGHTING);
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
				  glUniform3f(loc, pFrustum->eye.x, pFrustum->eye.y, pFrustum->eye.z);
				else
					LOG.Error("Shader", pMaterial->sName + " Couldn't set cameraPos");*/


				if(uiActiveUnits>0)
				{
					loc=glGetUniformLocation(pMaterial->pShader->uiShaderProgram, "texUnit0");
					if(loc != -1)
						glUniform1i(loc, 0);
					else
						LOG.Error("Shader", pMaterial->sName + ": Couldn't set texUnit0");
				}
	
				if(uiActiveUnits>1)
				{
					loc=glGetUniformLocation(pMaterial->pShader->uiShaderProgram, "texUnit1");
					if(loc != -1)
						glUniform1i(loc, 1);
					else
						LOG.Error("Shader", pMaterial->sName + ": Couldn't set texUnit1");
				}

				glEnable(GL_LIGHTING);
			}
			else if(bActiveShader)
			{
				bActiveShader=false;
				
				if(bCanShader)
					glUseProgram(NULL);
			}
			
			pCurrentMaterial=pMaterial;
			return true;
		}

		MATERIAL::cMaterial* cRender::GetMaterial(std::string sFilename)
		{
			std::map<std::string, MATERIAL::cMaterial * >::iterator iter=mMaterial.begin();

			while(iter!=mMaterial.end())
			{
				if (sFilename == iter->first) 
					return iter->second;
				iter++;
			}

			return pMaterialNotFoundMaterial;
		}

		void cRender::ClearColour()
		{
			bActiveColour=false;

			colour.SetBlack();
			colour.a=1.0f;

			glColor4f(colour.r, colour.g, colour.b, colour.a);
		}
		
		void cRender::SetColour(MATH::cColour inColour)
		{
			bActiveColour=true;

			colour=inColour;
			colour.a=1.0f;

			glColor4f(colour.r, colour.g, colour.b, colour.a);
		}

		
		void cRender::ReloadTextures()
		{
			LOG.Success("Render", "ReloadTextures");
			
			{
				LOG.Success("Render", "ReloadTextures Atlases");
				cTextureAtlas *pAtlas = NULL;
				unsigned int n = vTextureAtlas.size();
				for(unsigned int i = 0;i<n;i++)
					vTextureAtlas[i]->Reload();
			}

			{
				LOG.Success("Render", "ReloadTextures Misc Textures");
				cTexture *pTexture = NULL;
				std::map<std::string, cTexture* >::iterator iter=mTexture.begin();
				std::map<std::string, cTexture* >::iterator iterEnd=mTexture.end();
				while(iter != iterEnd)
				{
					pTexture = iter->second;

					pTexture->Reload();

					iter++;
				}
			}

			{
				LOG.Success("Render", "ReloadTextures Materials");
				MATERIAL::cMaterial * pMaterial = NULL;
				std::map<std::string, MATERIAL::cMaterial *>::iterator iter=mMaterial.begin();
				std::map<std::string, MATERIAL::cMaterial *>::iterator iterEnd=mMaterial.end();
				while(iter != iterEnd)
				{
					pMaterial = iter->second;

					if(pMaterial->pShader)
					{
						pMaterial->pShader->Destroy();
						pMaterial->pShader->Init();
					}
          
					iter++;
				}
			}

			{
				LOG.Success("Render", "ReloadTextures Vertex Buffer Objects");
				unsigned int n = vVertexBufferObject.size();
				for(unsigned int i = 0; i<n; i++)
				{
					vVertexBufferObject[i]->Destroy();
					vVertexBufferObject[i]->Init();
				}
			}
		}
	}
}
