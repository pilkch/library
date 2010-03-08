// Standard headers
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <cmath>

#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <vector>
#include <map>
#include <list>
#include <algorithm>

// Boost headers
#include <boost/shared_ptr.hpp>

// Anything else
#include <GL/GLee.h>
#include <GL/glu.h>

#include <SDL/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>
#include <spitfire/util/cTimer.h>

#include <spitfire/algorithm/algorithm.h>

#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/settings.h>

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
#include <breathe/util/cVar.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cResourceManager.h>
#include <breathe/render/cResourceManager.h>
#include <breathe/render/cVertexBufferObject.h>

#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

breathe::render::cResourceManager* pResourceManager = nullptr;

namespace breathe
{
  namespace render
  {
    // *** cResourceManager

    cResourceManager::cResourceManager(cDevice& _device) :
      device(_device)
      //iMaxTextureSize(0)
    {
      for (size_t i = 0; i < nAtlas; i++) {
        cTextureAtlasRef pNewTextureAtlas(new cTextureAtlas(i));
        vTextureAtlas.push_back(pNewTextureAtlas);
      }

      for (size_t i = 0; i < material::nLayers; i++) {
        material::cLayer layer;
        vLayer.push_back(layer);
      }
    }

    cResourceManager::~cResourceManager()
    {
      //TODO: Delete materials and shader objects and atlases etc.

      for (size_t i = 0; i < nAtlas; i++) vTextureAtlas[i].reset();
      vTextureAtlas.clear();

      vLayer.clear();
    }

    string_t cResourceManager::GetErrorString(GLenum error) const
    {
      switch (error) {
        case GL_NO_ERROR: return TEXT("GL_NO_ERROR");
        case GL_INVALID_ENUM: return TEXT("GL_INVALID_ENUM");
        case GL_INVALID_VALUE: return TEXT("GL_INVALID_VALUE");
        case GL_INVALID_OPERATION: return TEXT("GL_INVALID_OPERATION");
        case GL_STACK_OVERFLOW: return TEXT("GL_STACK_OVERFLOW");
        case GL_STACK_UNDERFLOW: return TEXT("GL_STACK_UNDERFLOW");
        case GL_OUT_OF_MEMORY: return TEXT("GL_OUT_OF_MEMORY");
      };

      return TEXT("Unknown error");
    }

    string_t cResourceManager::GetErrorString() const
    {
      return GetErrorString(glGetError());
    }

    bool cResourceManager::Create()
    {
      LOG<<"cResourceManager::Create"<<std::endl;

      return true;
    }

    void cResourceManager::Destroy()
    {
      //LOG.Success("Delete", "Frame Buffer Objects");
      //pFrameBuffer0.reset();
      //pFrameBuffer1.reset();

      //pHDRBloomExposureFrameBuffer.reset();
    }

    void cResourceManager::SetAtlasWidth(unsigned int uiNewSegmentWidthPX, unsigned int uiNewSegmentSmallPX, unsigned int uiNewAtlasWidthPX)
    {
      uiSegmentWidthPX=uiNewSegmentWidthPX;
      uiAtlasWidthPX=uiNewAtlasWidthPX;
      uiSegmentSmallPX=uiNewSegmentSmallPX;
    }

    void cResourceManager::BeginLoadingTextures()
    {
      for (size_t i = 0; i < nAtlas; i++) vTextureAtlas[i]->Begin(uiSegmentWidthPX, uiSegmentSmallPX, uiAtlasWidthPX);

      if (pTextureNotFoundTexture == nullptr) AddTextureNotFoundTexture(TEXT("textures/texturenotfound.png"));
      if (pMaterialNotFoundMaterial == nullptr) AddMaterialNotFoundMaterial(TEXT("textures/materialnotfound.png"));
    }

    void cResourceManager::EndLoadingTextures()
    {
      for (size_t i = 0; i < nAtlas; i++) vTextureAtlas[i]->End();
    }



    cTextureRef cResourceManager::AddTextureToAtlas(const string_t& sNewFilename, unsigned int uiAtlas)
    {
      ASSERT(sNewFilename != TEXT(""));
      ASSERT(ATLAS_NONE != uiAtlas);

      string_t sFilename;
      breathe::filesystem::FindFile(breathe::string::ToString_t(sNewFilename), sFilename);

      cTextureRef p = vTextureAtlas[uiAtlas]->AddTexture(sFilename);
      if ((p == nullptr) || (p == pTextureNotFoundTexture)) {
        LOG.Error("Texture", breathe::string::ToUTF8(sFilename) + " pTextureNotFound");
        return pTextureNotFoundTexture;
      }

      std::ostringstream t;
      t << p->uiTexture;
      LOG.Success("Texture", breathe::string::ToUTF8(sFilename) + " " + t.str());

      return p;
    }

    cTextureRef cResourceManager::AddTexture(const string_t& sNewFilename)
    {
      ASSERT(sNewFilename != TEXT(""));

      string_t sFilename;
      breathe::filesystem::FindFile(breathe::string::ToString_t(sNewFilename), sFilename);

      string_t s = breathe::filesystem::GetFile(sFilename);

      cTextureRef p = GetTexture(s);
      if (p == nullptr) {
        ASSERT(p != nullptr);
        return pTextureNotFoundTexture;
      }

      p = cTextureRef(new cTexture);
      if (p->Load(sFilename) != true) {
        LOG.Error("Render", "Failed to load " + breathe::string::ToUTF8(sFilename));
        p.reset();
        return pTextureNotFoundTexture;
      }

      p->Create();
      p->CopyFromSurfaceToTexture();

      mTexture[s]=p;

      std::ostringstream t;
      t << p->uiTexture;
      LOG.Success("Texture", "Texture " + breathe::string::ToUTF8(s) + " uiTexture=" + breathe::string::ToUTF8(t.str()));
      return p;
    }

#ifndef M_PI
#define M_PI 3.14159265
#endif

static GLint T0 = 0;
static GLint Frames = 0;


/**

  Draw a gear wheel.  You'll probably want to call this function when
  building a display list since we do a lot of trig here.

  Input:  inner_radius - radius of hole at center
          outer_radius - radius at center of teeth
          width - width of gear
          teeth - number of teeth
          tooth_depth - depth of tooth

 **/

void gear(GLfloat inner_radius, GLfloat outer_radius, GLfloat width,
  GLint teeth, GLfloat tooth_depth)
{
  GLint i;
  GLfloat r0, r1, r2;
  GLfloat angle, da;
  GLfloat u, v, len;

  r0 = inner_radius;
  r1 = outer_radius - tooth_depth / 2.0;
  r2 = outer_radius + tooth_depth / 2.0;

  da = 2.0 * M_PI / teeth / 4.0;

  glShadeModel(GL_FLAT);

  glNormal3f(0.0, 0.0, 1.0);

  /* draw front face */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
    glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
    if (i < teeth) {
      glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
      glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
    }
  }
  glEnd();

  /* draw front sides of teeth */
  glBegin(GL_QUADS);
  da = 2.0 * M_PI / teeth / 4.0;
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;

    glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
  }
  glEnd();

  glNormal3f(0.0, 0.0, -1.0);

  /* draw back face */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
    glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
    if (i < teeth) {
      glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
      glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
    }
  }
  glEnd();

  /* draw back sides of teeth */
  glBegin(GL_QUADS);
  da = 2.0 * M_PI / teeth / 4.0;
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;

    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
    glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
  }
  glEnd();

  /* draw outward faces of teeth */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;

    glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
    glVertex3f(r1 * cos(angle), r1 * sin(angle), -width * 0.5);
    u = r2 * cos(angle + da) - r1 * cos(angle);
    v = r2 * sin(angle + da) - r1 * sin(angle);
    len = sqrt(u * u + v * v);
    u /= len;
    v /= len;
    glNormal3f(v, -u, 0.0);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), -width * 0.5);
    glNormal3f(cos(angle), sin(angle), 0.0);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), -width * 0.5);
    u = r1 * cos(angle + 3 * da) - r2 * cos(angle + 2 * da);
    v = r1 * sin(angle + 3 * da) - r2 * sin(angle + 2 * da);
    glNormal3f(v, -u, 0.0);
    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), -width * 0.5);
    glNormal3f(cos(angle), sin(angle), 0.0);
  }

  glVertex3f(r1 * cos(0), r1 * sin(0), width * 0.5);
  glVertex3f(r1 * cos(0), r1 * sin(0), -width * 0.5);

  glEnd();

  glShadeModel(GL_SMOOTH);

  /* draw inside radius cylinder */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glNormal3f(-cos(angle), -sin(angle), 0.0);
    glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
    glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
  }
  glEnd();

}

static GLfloat view_rotx = 20.0, view_roty = 30.0, view_rotz = 0.0;
static GLint gear1, gear2, gear3;
static GLfloat angle = 0.0;

void draw(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();
  glRotatef(view_rotx, 1.0, 0.0, 0.0);
  glRotatef(view_roty, 0.0, 1.0, 0.0);
  glRotatef(view_rotz, 0.0, 0.0, 1.0);

  glPushMatrix();
  glTranslatef(-3.0, -2.0, 0.0);
  glRotatef(angle, 0.0, 0.0, 1.0);
  glCallList(gear1);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(3.1, -2.0, 0.0);
  glRotatef(-2.0 * angle - 9.0, 0.0, 0.0, 1.0);
  glCallList(gear2);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-3.1, 4.2, 0.0);
  glRotatef(-2.0 * angle - 25.0, 0.0, 0.0, 1.0);
  glCallList(gear3);
  glPopMatrix();

  glPopMatrix();

  SDL_GL_SwapBuffers();

  Frames++;
  {
     GLint t = SDL_GetTicks();
     if (t - T0 >= 5000) {
        GLfloat seconds = (t - T0) / 1000.0;
        GLfloat fps = Frames / seconds;
        printf("%d frames in %g seconds = %g FPS\n", Frames, seconds, fps);
        T0 = t;
        Frames = 0;
     }
  }
}


void idle(void)
{
  angle += 2.0;
}

/* new window size or exposure */
void reshape(int width, int height)
{
  GLfloat h = (GLfloat) height / (GLfloat) width;

  glViewport(0, 0, (GLint) width, (GLint) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -40.0);
}

void init()
{
  static GLfloat pos[4] =
  {5.0, 5.0, 10.0, 0.0};
  static GLfloat red[4] =
  {0.8, 0.1, 0.0, 1.0};
  static GLfloat green[4] =
  {0.0, 0.8, 0.2, 1.0};
  static GLfloat blue[4] =
  {0.2, 0.2, 1.0, 1.0};

  glLightfv(GL_LIGHT0, GL_POSITION, pos);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);

  /* make the gears */
  gear1 = glGenLists(1);
  glNewList(gear1, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
  gear(1.0, 4.0, 1.0, 20, 0.7);
  glEndList();

  gear2 = glGenLists(1);
  glNewList(gear2, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
  gear(0.5, 2.0, 2.0, 10, 0.7);
  glEndList();

  gear3 = glGenLists(1);
  glNewList(gear3, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
  gear(1.3, 2.0, 0.5, 10, 0.7);
  glEndList();

  glEnable(GL_NORMALIZE);

  printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
  printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
  printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
  printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
}






    bool cResourceManager::AddTextureNotFoundTexture(const string_t& sNewFilename)
    {

  /*SDL_Surface *screen;
  int done;
  Uint8 *keys;

  SDL_Init(SDL_INIT_VIDEO);

  screen = SDL_SetVideoMode(300, 300, 16, SDL_OPENGL|SDL_RESIZABLE);
  if ( ! screen ) {
    fprintf(stderr, "Couldn't set 300x300 GL video mode: %s\n", SDL_GetError());
    SDL_Quit();
    exit(2);
  }
  SDL_WM_SetCaption("Gears", "gears");

  init();
  reshape(screen->w, screen->h);*/


      string_t sFilename;
      breathe::filesystem::FindResourceFile(breathe::string::ToString_t(sNewFilename), sFilename);

      cTextureRef p(new cTexture);
      if (p->Load(sFilename) != true) {
        // Just assert, don't even try to come back from this situation
        LOG.Error("Render", "Failed to load texture not found texture");
        CONSOLE<<"cResourceManager::AddTextureNotFoundTexture failed to load "<<sNewFilename<<" "<<sFilename<<std::endl;
        ASSERT(false);
        p.reset();
        return false;
      }

      p->Create();
      p->CopyFromSurfaceToTexture();

      mTexture[sNewFilename]=p;
      pTextureNotFoundTexture = p;

      std::ostringstream t;
      t << pTextureNotFoundTexture->uiTexture;
      LOG.Success("Texture", "TextureNotFoundTexture " + t.str());




/*
  done = 0;
  while ( ! done ) {
    SDL_Event event;

    idle();
    while ( SDL_PollEvent(&event) ) {
      switch(event.type) {
        case SDL_VIDEORESIZE:
          screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 16,
                                    SDL_OPENGL|SDL_RESIZABLE);
          if ( screen ) {
            reshape(screen->w, screen->h);
          } else {
            // Uh oh, we couldn't set the new video mode??
          }
          break;

        case SDL_QUIT:
          done = 1;
          break;
      }
    }
    keys = SDL_GetKeyState(NULL);

    if ( keys[SDLK_ESCAPE] ) {
      done = 1;
    }
    if ( keys[SDLK_UP] ) {
      view_rotx += 5.0;
    }
    if ( keys[SDLK_DOWN] ) {
      view_rotx -= 5.0;
    }
    if ( keys[SDLK_LEFT] ) {
      view_roty += 5.0;
    }
    if ( keys[SDLK_RIGHT] ) {
      view_roty -= 5.0;
    }
    if ( keys[SDLK_z] ) {
      if ( SDL_GetModState() & KMOD_SHIFT ) {
        view_rotz -= 5.0;
      } else {
        view_rotz += 5.0;
      }
    }

    draw();
  }
  SDL_Quit();*/


      return true;
    }

    bool cResourceManager::AddMaterialNotFoundTexture(const string_t& sNewFilename)
    {
      string_t sFilename;
      breathe::filesystem::FindResourceFile(breathe::string::ToString_t(sNewFilename), sFilename);

      LOG.Success("Texture", "Loading " + breathe::string::ToUTF8(sFilename));

      cTextureRef p(new cTexture);
      if (!p->Load(sFilename)) {
        // Just assert, don't even try to come back from this situation
        LOG.Error("Render", "Failed to load material not found texture");
        CONSOLE<<"cResourceManager::AddMaterialNotFoundTexture failed to load "<<sNewFilename<<" "<<sFilename<<std::endl;
        ASSERT(false);
        p.reset();
        return false;
      }

      p->Create();
      p->CopyFromSurfaceToTexture();

      mTexture[sNewFilename]=p;
      pMaterialNotFoundTexture = p;

      std::ostringstream t;
      t << pMaterialNotFoundTexture->uiTexture;
      LOG.Success("Texture", "MaterialNotFoundTexture " + t.str());

      return true;
    }


    cTextureRef cResourceManager::GetTextureAtlas(ATLAS atlas)
    {
      ASSERT(atlas < nAtlas);

      return vTextureAtlas[atlas];
    }

    cTextureRef cResourceManager::GetTexture(const string_t& sNewFilename)
    {
      std::map<string_t, cTextureRef>::iterator iter = mTexture.find(sNewFilename);
      if (iter != mTexture.end()) return iter->second;

      return pTextureNotFoundTexture;
    }

    cVertexBufferObjectRef cResourceManager::AddVertexBufferObject()
    {
      cVertexBufferObjectRef pVertexBufferObject(new cVertexBufferObject);
      vVertexBufferObject.push_back(pVertexBufferObject);

      return pVertexBufferObject;
    }

    cTextureRef cResourceManager::GetCubeMap(const string_t& sNewFilename)
    {
      if (TEXT("") == sNewFilename) return cTextureRef();

      std::map<string_t, cTextureRef>::iterator iter=mCubeMap.find(sNewFilename);

      if (mCubeMap.end() != iter) return iter->second;

      return cTextureRef();
    }

    cTextureRef cResourceManager::AddCubeMap(const string_t& sFilename)
    {
      /*TODO: Surface of 1x6 that holds the cubemap faces,
      not actually used for rendering, just collecting each surface

      class cCubeMap : protected cTexture
      {
      public:
        CopyFromSurface(uiWidth, 6 * uiHeight);
      };
      */

      if (TEXT("")==sFilename)
        return pTextureNotFoundTexture;

      cTextureRef p = mCubeMap[sFilename];
      if (p) return p;

      p.reset(new cTexture);

      mCubeMap[sFilename] = p;

      p->sFilename = sFilename;

      GLuint cube_map_directions[6] =
      {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
      };

      LOG.Success("Texture", "Loading CubeMap " + breathe::string::ToUTF8(sFilename));

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

      string_t sFile = breathe::filesystem::GetFileNoExtension(breathe::string::ToString_t(sFilename));
      string_t sExt = breathe::filesystem::GetExtension(breathe::string::ToString_t(sFilename));

      for (i=0;i<6;i++)
      {
        s.str("");

        s<<breathe::string::ToUTF8(sFile)<<"/"<<breathe::string::ToUTF8(sFile)<<i<<"."<<breathe::string::ToUTF8(sExt);
        string_t sFilename;
        breathe::filesystem::FindResourceFile(breathe::string::ToString_t(s.str()), sFilename);

        unsigned int mode=0;

        SDL_Surface* surface = IMG_Load(breathe::string::ToUTF8(sFilename).c_str());

        // could not load filename
        if (!surface)
        {
          LOG.Error("Texture", "Couldn't Load Texture " + breathe::string::ToUTF8(sFilename));
          return cTextureRef();
        }

        if (surface->format->BytesPerPixel == 3) // RGB 24bit
        {
          mode = GL_RGB;
          LOG.Success("Texture", "RGB Image");
        }
        else if (surface->format->BytesPerPixel == 4)// RGBA 32bit
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

          return cTextureRef();
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

    material::cMaterialRef cResourceManager::AddMaterialNotFoundMaterial(const string_t& sNewFilename)
    {
      AddMaterialNotFoundTexture(sNewFilename);

      pMaterialNotFoundMaterial.reset(new material::cMaterial(TEXT("MaterialNotFound")));

      string_t sFilename;
      breathe::filesystem::FindResourceFile(breathe::string::ToString_t(sNewFilename), sFilename);
      pMaterialNotFoundMaterial->vLayer[0]->sTexture = sFilename;
      pMaterialNotFoundMaterial->vLayer[0]->pTexture = pMaterialNotFoundTexture;
      pMaterialNotFoundMaterial->vLayer[0]->uiTextureMode = TEXTURE_MODE::TEXTURE_NORMAL;

      return pMaterialNotFoundMaterial;
    }

    material::cMaterialRef cResourceManager::AddMaterial(const string_t& sNewfilename)
    {
      if (sNewfilename.empty()) return material::cMaterialRef();

      material::cMaterialRef pMaterial = _GetMaterial(sNewfilename);

      if (pMaterial != pMaterialNotFoundMaterial) return pMaterial;

      string_t sFilename;
      filesystem::FindResourceFile(breathe::string::ToString_t(sNewfilename), sFilename);
      pMaterial.reset(new material::cMaterial(sFilename));

      if (!pMaterial->Load(sFilename)) {
        pMaterial = pMaterialNotFoundMaterial;
      }

      mMaterial[filesystem::GetFile(sFilename)] = pMaterial;

      return pMaterial;
    }

    material::cMaterialRef cResourceManager::AddMaterialAsAlias(const string_t& sNewfilename, const string_t& sAlias)
    {
      LOG<<"cResourceManager::AddMaterialAsAlias sNewfilename=\""<<sNewfilename<<"\""<<std::endl;
      if (sNewfilename.empty()) {
        LOG<<"cResourceManager::AddMaterialAsAlias sNewfilename is empty, returning"<<std::endl;
        return material::cMaterialRef();
      }

      string_t sFilename;
      filesystem::FindResourceFile(breathe::string::ToString_t(sNewfilename), sFilename);

      LOG<<"cResourceManager::AddMaterialAsAlias sFilename=\""<<sFilename<<"\""<<std::endl;

      material::cMaterialRef pMaterial(new material::cMaterial(sFilename));

      if (!pMaterial->Load(sFilename)) {
        LOG<<"cResourceManager::AddMaterialAsAlias FAILED to load material"<<std::endl;
        pMaterial = pMaterialNotFoundMaterial;
      }

      mMaterial[sAlias] = pMaterial;

      LOG<<"cResourceManager::AddMaterialAsAlias returning"<<std::endl;

      return pMaterial;
    }

    material::cMaterialRef cResourceManager::GetMaterial(const string_t& sFilename)
    {
      AddMaterial(sFilename);
      return _GetMaterial(sFilename);
    }

    material::cMaterialRef cResourceManager::_GetMaterial(const string_t& sFilename)
    {
      std::map<string_t, material::cMaterialRef>::iterator iter = mMaterial.begin();
      std::map<string_t, material::cMaterialRef>::iterator iterEnd = mMaterial.end();

      string_t temp = filesystem::GetFile(sFilename);
      while(iter != iterEnd) {
        if (temp == iter->first) return iter->second;
        iter++;
      }

      return pMaterialNotFoundMaterial;
    }

    void cResourceManager::TransformModels()
    {
      /*cTextureRef t;
      material::cMaterialRef mat;

      model::cStaticRef s;
      model::cMeshRef pMesh;
      float* fTextureCoords=NULL;
      size_t nMeshes=0;
      size_t nTexcoords=0;
      unsigned int mesh=0;
      unsigned int texcoord=0;

      //Transform uv texture coordinates
      std::map<string_t, model::cStaticRef>::iterator iter = mStatic.begin();
      std::map<string_t, model::cStaticRef>::iterator iterEnd = mStatic.end();
      while (iter != iterEnd) {
        string_t sFilename = iter->first;
        s = iter->second;

        LOG.Success("cResourceManager::TransformModels", "UV model=" + breathe::string::ToUTF8(sFilename));
        ASSERT(s != nullptr);

        nMeshes = s->vMesh.size();

        std::ostringstream sOut;
        sOut<<static_cast<unsigned int>(nMeshes);
        LOG.Success("cResourceManager::TransformModels", "UV model=" + breathe::string::ToUTF8(sFilename) + " meshes=" + sOut.str());

        for (mesh = 0; mesh < nMeshes; mesh++) {
          pMesh = s->vMesh[mesh];
          fTextureCoords = &pMesh->pMeshData->vTextureCoord[0];
          nTexcoords = pMesh->pMeshData->vTextureCoord.size();

          mat = GetMaterial(pMesh->sMaterial);

          if (mat != nullptr) {
            if (!mat->vLayer.empty()) {
              t = mat->vLayer[0]->pTexture;

              if (t == nullptr) t = GetTexture(mat->vLayer[0]->sTexture);

              if (t != nullptr) {
                for (texcoord=0;texcoord<nTexcoords;texcoord+=2) t->Transform(fTextureCoords[texcoord], fTextureCoords[texcoord+1]);
              } else LOG.Error("Transform", "Texture not found " + breathe::string::ToUTF8(mat->vLayer[0]->sTexture));
            } else LOG.Error("Transform", "Material doesn't have any layers");
          } else LOG.Error("Transform", "Material not found " + breathe::string::ToUTF8(pMesh->sMaterial));
        }

        iter++;
      }


      float* fNormals=NULL;

      //Calculate normals
      for (iter=mStatic.begin();iter!=mStatic.end();iter++)
      {
        LOG.Success("Transform", "Normals " + breathe::string::ToUTF8(iter->first));

        s=iter->second;

        if (s)
        {
          nMeshes=s->vMesh.size();

          for (mesh=0;mesh<nMeshes;mesh++)
          {
            pMesh = s->vMesh[mesh];
            fNormals = &pMesh->pMeshData->vNormal[0];

            //Init all vertex normals to zero

            //for all faces:
            //  compute face normal

            //for every vertex in every face:
            //  add face normal to vertex normal
            //  for all adjacent faces:
            //      if the dotproduct of the face normal and the adjacentface normal is > 0.71:
            //          add adjacentface normal to vertex normal

            //for all vertex normals:
            //                                                                    normalise vertex normal
          }
        }
      }


      // TODO: Optimise order for rendering
      unsigned int uiPass=0;
      unsigned int i=0;
      unsigned int uiMode0=0;
      //unsigned int uiMode1=0;

      for (iter = mStatic.begin(); iter != mStatic.end(); iter++) {
        LOG.Success("Transform", "Optimising " + breathe::string::ToUTF8(iter->first));

        s = iter->second;

        if (s != nullptr) {
          nMeshes = s->vMesh.size();

          for (uiPass = 1; uiPass < nMeshes; uiPass++) {
            for (i = 0; i < nMeshes - uiPass; i++) {
              uiMode0=GetMaterial(s->vMesh[i]->sMaterial)->vLayer[0]->uiTextureMode;

              //x[i] > x[i+1]
              if ((TEXTURE_MASK == uiMode0) || (TEXTURE_BLEND == uiMode0))
                std::swap(s->vMesh[i], s->vMesh[i+1]);
            }
          }
        }
      }

      LOG.Success("Render", "TransformModels returning");*/
    }

    void cResourceManager::ReloadTextures()
    {
      LOG.Success("Render", "ReloadTextures");

      {
        LOG.Success("Render", "ReloadTextures Atlases");
        cTextureAtlasRef pAtlas;
        size_t n = vTextureAtlas.size();
        for (size_t i = 0;i<n;i++)
          vTextureAtlas[i]->Reload();
      }

      {
        LOG.Success("Render", "ReloadTextures Misc Textures");
        cTextureRef pTexture;
        std::map<string_t, cTextureRef>::iterator iter=mTexture.begin();
        std::map<string_t, cTextureRef>::iterator iterEnd=mTexture.end();
        while(iter != iterEnd)
        {
          pTexture = iter->second;

          pTexture->Reload();

          iter++;
        }
      }

      {
        LOG.Success("Render", "ReloadTextures Materials");
        material::cMaterialRef pMaterial;
        std::map<string_t, material::cMaterialRef>::iterator iter=mMaterial.begin();
        const std::map<string_t, material::cMaterialRef>::iterator iterEnd=mMaterial.end();
        while (iter != iterEnd) {
          pMaterial = iter->second;

          if (pMaterial->pShader != nullptr) {
            pMaterial->pShader->Destroy();
            pMaterial->pShader->Init();
          }

          iter++;
        }
      }

      {
        LOG.Success("Render", "ReloadTextures Vertex Buffer Objects");
        const size_t n = vVertexBufferObject.size();
        for (size_t i = 0; i < n; i++) {
          vVertexBufferObject[i]->Destroy();
          vVertexBufferObject[i]->Compile();
        }
      }
    }
  }
}
