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
#include <list>
#include <vector>

// OpenGL headers
#include <GL/GLee.h>

// SDL headers
#include <SDL/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

// libopenglmm headers
#include <libopenglmm/cTexture.h>

#ifdef PLATFORM_LINUX_OR_UNIX
#include <sys/stat.h>
#endif

namespace opengl
{
#ifdef PLATFORM_LINUX_OR_UNIX
  bool FileExists(const opengl::string_t& strFilename)
  {
    struct stat statInfo;

    int iResult = stat(opengl::string::ToUTF8(strFilename).c_str(), &statInfo);

    return (iResult == 0);
  }
#endif


  // ** cTexture

  cTexture::cTexture() :
    uiTexture(0),
    bIsUsingMipMaps(true)
  {
  }

  cTexture::~cTexture()
  {
    assert(uiTexture == 0);
  }

  bool cTexture::CreateFromImage(const voodoo::cImage& _image)
  {
    assert(_image.IsValid());
    assert(spitfire::math::IsPowerOfTwo(_image.GetWidth()));
    assert(spitfire::math::IsPowerOfTwo(_image.GetHeight()));
    assert(_image.GetWidth() == _image.GetHeight());

    image = _image;

    Create();
    CopyFromImageToTexture();

    return IsValid();
  }

  void cTexture::_Create()
  {
    // Create new texture
    glGenTextures(1, &uiTexture);

    // Bind so that the next operations happen on this texture
    glBindTexture(GL_TEXTURE_2D, uiTexture);
  }

  void cTexture::_Destroy()
  {
    // Destroy old texture
    glDeleteTextures(1, &uiTexture);
    uiTexture = 0;
  }

  void cTexture::CopyFromImageToTexture()
  {
    // Bind so that the next operations happen on this texture
    glBindTexture(GL_TEXTURE_2D, uiTexture);

    const uint8_t* pBuffer = image.GetPointerToBuffer();
    if (pBuffer != nullptr) {
      // Copy from image to texture
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.GetWidth(), image.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pBuffer);

      if (bIsUsingMipMaps) {
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, image.GetWidth(), image.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, pBuffer);
      }

      // Settings to make the texture look a bit nicer when we do blit it to the screen
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      if (bIsUsingMipMaps) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
      }
    }
  }

  void cTexture::Reload()
  {
    // Delete the old texture
    Destroy();

    // Create a new one
    Create();

    // Copy from image to texture
    CopyFromImageToTexture();
  }



  // ** Frame Buffer Object

  cTextureFrameBufferObject::cTextureFrameBufferObject() :
    uiFBO(0),
    uiFBODepthBuffer(0),
    bIsCubeMap(false)
  {
    image.SetWidth(DEFAULT_FBO_TEXTURE_WIDTH);
    image.SetHeight(DEFAULT_FBO_TEXTURE_HEIGHT);
  }

  cTextureFrameBufferObject::~cTextureFrameBufferObject()
  {
    assert(uiTexture == 0);
  }

  bool cTextureFrameBufferObject::CreateFrameBufferObject(size_t width, size_t height)
  {
    // Only allow square, power of two FBO textures at the moment
    assert(spitfire::math::IsPowerOfTwo(width));
    assert(spitfire::math::IsPowerOfTwo(height));
    assert(width == height);

    image.SetWidth(width);
    image.SetHeight(height);

    Create();

    return IsValid();
  }

  void cTextureFrameBufferObject::_Create()
  {
    // http://www.opengl.org/wiki/GL_EXT_framebuffer_object#Quick_example.2C_render_to_texture_.282D.29.2C_mipmaps
    // http://www.opengl.org/wiki/GL_EXT_framebuffer_object#Quick_example.2C_render_to_texture_.28Cubemap.29

    const unsigned int uiWidth = image.GetWidth();
    const unsigned int uiHeight = image.GetHeight();

    // Only allow square, power of two FBO textures at the moment
    assert(spitfire::math::IsPowerOfTwo(uiWidth));
    assert(spitfire::math::IsPowerOfTwo(uiHeight));
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
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      if (bIsUsingMipMaps) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

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

  void cTextureFrameBufferObject::_Destroy()
  {
    glDeleteFramebuffersEXT(1, &uiFBO);
    uiFBO = 0;

    glDeleteRenderbuffersEXT(1, &uiFBODepthBuffer);
    uiFBODepthBuffer = 0;

    // Unbind this texture if it is bound
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    glDeleteTextures(1, &uiTexture);
    uiTexture = 0;
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
