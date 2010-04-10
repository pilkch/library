// Standard headers
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <cmath>

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

// OpenGL headers
#include <GL/GLee.h>
#include <GL/glu.h>

// SDL headers
#include <SDL/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

// libopenglmm headers
#include <libopenglmm/cTexture.h>

#ifdef PLATFORM_LINUX_OR_UNIX
#include <sys/stat.h>

bool FileExists(const std::string& strFilename)
{
  struct stat statInfo;

  int iResult = stat(strFilename.c_str(), &statInfo);

  return (iResult == 0);
}
#endif

namespace opengl
{
  // ** cTexture

  cTexture::cTexture() :
    uiTexture(0),

    uiWidth(0),
    uiHeight(0),
    uiType(TEXTURE_TYPE::RGBA),

    fScale(1.0f),
    fU(0.0f),
    fV(0.0f),

    pSurface(nullptr)
  {
  }

  cTexture::~cTexture()
  {
    if (pSurface != nullptr) {
      SDL_FreeSurface(pSurface);
      pSurface = nullptr;
    }

    data.clear();
  }

  bool cTexture::Load(const std::string& inFilename)
  {
    std::cout<<"cTexture::Load \""<<inFilename<<"\""<<std::endl;

    sFilename = inFilename;

    //unsigned int mode = 0;
    pSurface = IMG_Load(sFilename.c_str());

    // Could not load filename
    if (pSurface == nullptr) {
      if (FileExists(sFilename)) std::cout<<"cTexture::Load Texture "<<sFilename<<" exists"<<std::endl;
      else std::cout<<"cTexture::Load Texture "<<sFilename<<" doesn't exist"<<std::endl;

      std::cout<<"cTexture::Load Couldn't Load Texture "<<sFilename<<std::endl;
      return false;
    }



    // Check the format
    if (8 == pSurface->format->BitsPerPixel) {
      std::cout<<"cTexture::Load Texture Greyscale Heightmap Image "<<sFilename<<std::endl;
      uiType = TEXTURE_TYPE::HEIGHTMAP;
    } else if (16 == pSurface->format->BitsPerPixel) {
      std::cout<<"Texture"<<"Greyscale Heightmap Image "<<sFilename<<std::endl;
      uiType = TEXTURE_TYPE::HEIGHTMAP;
    } else if (24 == pSurface->format->BitsPerPixel) {
      std::cout<<"Texture"<<sFilename<<" is a 24 bit RGB image"<<std::endl;
      // Add alpha channel
      SDL_PixelFormat format = {
        NULL, 32, 4, 0, 0, 0, 0,
        0, 8, 16, 24,
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
        0, 255
      };
      SDL_Surface* pConvertedSurface = SDL_ConvertSurface(pSurface, &format, SDL_SWSURFACE);
      SDL_FreeSurface(pSurface);
      pSurface = pConvertedSurface;
    } else if (32 == pSurface->format->BitsPerPixel) {
      std::cout<<"Texture"<<sFilename<<" is a 32 bit RGBA image"<<std::endl;
      uiType = TEXTURE_TYPE::RGBA;

      // Convert if BGR
      if (pSurface->format->Rshift > pSurface->format->Bshift) {
        SDL_PixelFormat format = {
          NULL, 32, 4, 0, 0, 0, 0,
          0, 8, 16, 24,
          0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
          0, 255};
        SDL_Surface* pConvertedSurface = SDL_ConvertSurface(pSurface, &format, SDL_SWSURFACE);
        SDL_FreeSurface(pSurface);
        pSurface = pConvertedSurface;
      }

      /*int nHH = pSurface->h / 2;
      int nPitch = pSurface->pitch;

      unsigned char* pBuf = new unsigned char[nPitch];
      unsigned char* pSrc = (unsigned char*)pSurface->pixels;
      unsigned char* pDst = (unsigned char*)pSurface->pixels + nPitch * (pSurface->h - 1);

      while (nHH--)
      {
        std::memcpy(pBuf, pSrc, nPitch);
        std::memcpy(pSrc, pDst, nPitch);
        std::memcpy(pDst, pBuf, nPitch);

        pSrc += nPitch;
        pDst -= nPitch;
      };

      SAFE_DELETE_ARRAY(pBuf);*/
    } else {
      std::ostringstream t;
      t << pSurface->format->BitsPerPixel;
      std::cout<<"Texture"<<"Error Unknown Image Format ("<<t.str()<<"bit) "<<sFilename<<std::endl;
      return false;
    }

    uiWidth = pSurface->w;
    uiHeight = pSurface->h;

    std::cout<<"Texture "<<uiWidth<<"x"<<uiHeight<<std::endl;

    CopyFromSurfaceToData(pSurface->w, pSurface->h);

    return true;
  }

  void cTexture::CopyFromSurfaceToData(unsigned int w, unsigned int h)
  {
    // Fill out the pData structure array, we use this for when we have to reload this data
    // on a task switch or fullscreen mode change

    uiWidth = w;
    uiHeight = h;

    CopyFromSurfaceToData();
  }

  void cTexture::CopyFromSurfaceToData()
  {
    assert(pSurface != nullptr);

    // Fill out the pData structure array, we use this for when we have to reload this data
    // on a task switch or fullscreen mode change
    if (data.empty()) data.resize(uiWidth * uiHeight * (uiType == TEXTURE_TYPE::HEIGHTMAP ? 1 : 4), 0);

    std::memcpy(&data[0], pSurface->pixels, uiWidth * uiHeight * (uiType == TEXTURE_TYPE::HEIGHTMAP ? 1 : 4));
  }

  void cTexture::CopyFromDataToSurface()
  {
    assert(pSurface != nullptr);

    if (data.empty()) return;

    std::memcpy(pSurface->pixels, &data[0], uiWidth * uiHeight * (uiType == TEXTURE_TYPE::HEIGHTMAP ? 1 : 4));
  }

  bool cTexture::SaveToBMP(const std::string& inFilename) const
  {
    assert(pSurface != nullptr);
    SDL_SaveBMP(pSurface, inFilename.c_str());
    return true;
  }

  void cTexture::_Create()
  {
    // Create new texture
    glGenTextures(1, &uiTexture);

    // Bind so that the next operations happen on this texture
    glBindTexture(GL_TEXTURE_2D, uiTexture);
  }

  void cTexture::Destroy()
  {
    // Destroy old texture
    glDeleteTextures(1, &uiTexture);
  }

  void cTexture::CopyFromSurfaceToTexture()
  {
    // Bind so that the next operations happen on this texture
    glBindTexture(GL_TEXTURE_2D, uiTexture);

    if (pSurface != nullptr) {
      // Copy from surface to texture
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pSurface->w, pSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pSurface->pixels);

      //Remove this line if there are artifacts
      gluBuild2DMipmaps(GL_TEXTURE_2D, 4, pSurface->w, pSurface->h, GL_RGBA, GL_UNSIGNED_BYTE, pSurface->pixels);

      // Settings to make the texture look a bit nicer when we do blit it to the screen
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
  }

  void cTexture::Reload()
  {
    // Delete the old texture
    Destroy();

    // Create a new one
    Create();

    // Copy from data buffer to surface
    CopyFromDataToSurface();

    // Copy from surface to texture
    CopyFromSurfaceToTexture();
  }



  // ** Frame Buffer Object

  cTextureFrameBufferObject::cTextureFrameBufferObject() :
    uiFBO(0),
    uiFBODepthBuffer(0),
    bIsUsingMipMaps(true),
    bIsCubeMap(false)
  {
    uiType = TEXTURE_TYPE::FRAMEBUFFEROBJECT;

    uiWidth = DEFAULT_FBO_TEXTURE_WIDTH;
    uiHeight = DEFAULT_FBO_TEXTURE_HEIGHT;
  }

  cTextureFrameBufferObject::~cTextureFrameBufferObject()
  {
    glDeleteFramebuffersEXT(1, &uiFBO);
    glDeleteRenderbuffersEXT(1, &uiFBODepthBuffer);

    // Unbind this texture if it is bound
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    glDeleteTextures(1, &uiTexture);
  }

  bool cTextureFrameBufferObject::CreateFrameBufferObject(size_t width, size_t height)
  {
    uiWidth = width;
    uiHeight = height;

    return true;
  }

  void cTextureFrameBufferObject::_Create()
  {
    // http://www.opengl.org/wiki/GL_EXT_framebuffer_object#Quick_example.2C_render_to_texture_.282D.29.2C_mipmaps
    // http://www.opengl.org/wiki/GL_EXT_framebuffer_object#Quick_example.2C_render_to_texture_.28Cubemap.29

    // Only allow square FBO textures at the moment
    assert(uiWidth == uiHeight);

    // Create FBO
    glGenFramebuffersEXT(1, &uiFBO);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, uiFBO);


    if (!bIsCubeMap) {
      // Now setup a texture to render to
      glGenTextures(1, &uiTexture);
      glBindTexture(GL_TEXTURE_2D, uiTexture);

      GLenum internal = GL_RGBA8;
      GLenum type = GL_UNSIGNED_BYTE;

      // We want all FBO textures to be 16bit as we will get more precision hopefully
      internal = GL_RGBA16F_ARB; // This seems good enough and won't use twice as much(!) memory as 32bit
      //internal = GL_RGBA32F_ARB;
      type = GL_FLOAT;

      glTexImage2D(GL_TEXTURE_2D, 0, internal, uiWidth, uiHeight, 0, GL_RGBA, type, NULL);

      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

      if (bIsUsingMipMaps) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glGenerateMipmapEXT(GL_TEXTURE_2D);
      }

      // And attach it to the FBO so we can render to it
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, uiTexture, 0);
    } else {
      // Now setup a texture to render to
      glGenTextures(1, &uiTexture);
      glBindTexture(GL_TEXTURE_CUBE_MAP, uiTexture);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      // NULL means reserve texture memory, but texels are undefined
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 0, 0, GL_RGBA8, uiWidth, uiHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 1, 0, GL_RGBA8, uiWidth, uiHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 2, 0, GL_RGBA8, uiWidth, uiHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 3, 0, GL_RGBA8, uiWidth, uiHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 4, 0, GL_RGBA8, uiWidth, uiHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + 5, 0, GL_RGBA8, uiWidth, uiHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

      // Attach one of the faces of the Cubemap texture to this FBO
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, uiTexture, 0);
    }



    // Create the Render Buffer for Depth
    glGenRenderbuffersEXT(1, &uiFBODepthBuffer);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, uiFBODepthBuffer);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, uiWidth, uiHeight);

    // Attach the depth render buffer to the FBO as it's depth attachment
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, uiFBODepthBuffer);



    // Check our status to make sure that we have a complete and ready to use FBO
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
      std::string sError = "UNKNOWN ERROR";

      switch (status) {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT: {
          sError = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT";
          break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: {
          sError = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT";
          break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: {
          sError = "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT";
          break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: {
          sError = "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT";
          break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: {
          sError = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT";
          break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: {
          sError = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT";
          break;
        }
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT: {
          sError = "GL_FRAMEBUFFER_UNSUPPORTED_EXT";
          break;
        }
        case GL_INVALID_FRAMEBUFFER_OPERATION_EXT: {
          sError = "GL_INVALID_FRAMEBUFFER_OPERATION_EXT";
          break;
        }
      }

      std::cout<<"cTextureFrameBufferObject::_Create FAILED status="<<sError<<std::endl;
      assert(status == GL_FRAMEBUFFER_COMPLETE_EXT);
    }

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); // Unbind the FBO for now
  }

  void cTextureFrameBufferObject::GenerateMipMapsIfRequired()
  {
    if (bIsUsingMipMaps) {
      glBindTexture(GL_TEXTURE_2D, uiTexture);

        glGenerateMipmapEXT(GL_TEXTURE_2D);

      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }

  void cTextureFrameBufferObject::SelectMipMapLevelOfDetail(float fLevelOfDetail)
  {
    assert(bIsUsingMipMaps);

    #ifdef MAX_TEXTURE_LOD_BIAS
    // fLevelOfDetail must be in the range of -MAX_TEXTURE_LOD_BIAS..MAX_TEXTURE_LOD_BIAS
    assert(fLevelOfDetail < MAX_TEXTURE_LOD_BIAS);
    assert(fLevelOfDetail > -MAX_TEXTURE_LOD_BIAS);
    #endif

    glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, fLevelOfDetail);
  }
}
