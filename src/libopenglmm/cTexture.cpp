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

// SDL headers
#include <SDL/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

// libopenglmm headers
#include <libopenglmm/cTexture.h>
#include <libopenglmm/opengl.h>

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
        #if BUILD_LIBOPENGLMM_OPENGL_VERSION < 300
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        #endif
      }

      // Copy from image to texture
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.GetWidth(), image.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pBuffer);

      if (bIsUsingMipMaps) glGenerateMipmap(GL_TEXTURE_2D);
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




  // ** Cube Map Texture

  cTextureCubeMap::cTextureCubeMap() :
    uiTexture(0)
  {
  }

  cTextureCubeMap::~cTextureCubeMap()
  {
    assert(uiTexture == 0);
  }

  bool cTextureCubeMap::CreateFromFilePaths(
    const opengl::string_t& filePathPositiveX,
    const opengl::string_t& filePathNegativeX,
    const opengl::string_t& filePathPositiveY,
    const opengl::string_t& filePathNegativeY,
    const opengl::string_t& filePathPositiveZ,
    const opengl::string_t& filePathNegativeZ
  )
  {
    const GLuint axis[6] = {
      GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
      GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
      GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    std::vector<opengl::string_t> filePaths;
    filePaths.push_back(filePathPositiveX);
    filePaths.push_back(filePathNegativeX);
    filePaths.push_back(filePathPositiveY);
    filePaths.push_back(filePathNegativeY);
    filePaths.push_back(filePathPositiveZ);
    filePaths.push_back(filePathNegativeZ);

    // Create new texture
    glGenTextures(1, &uiTexture);

    // Bind so that the next operations happen on this texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, uiTexture);

    {
      voodoo::cImage image;

      for (size_t i = 0; i < 6; i++) {
        image.LoadFromFile(filePaths[i]);

        assert(image.IsValid());
        assert(spitfire::math::IsPowerOfTwo(image.GetWidth()));
        assert(spitfire::math::IsPowerOfTwo(image.GetHeight()));
        assert(image.GetWidth() == image.GetHeight());

        const uint8_t* pBuffer = image.GetPointerToBuffer();
        if (pBuffer != nullptr) {
          // Copy from image to texture
          glTexImage2D(axis[i], 0, GL_RGBA, image.GetWidth(), image.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pBuffer);

          glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
          glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
      }
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return IsValid();
  }

  void cTextureCubeMap::Destroy()
  {
    // Destroy old texture
    glDeleteTextures(1, &uiTexture);
    uiTexture = 0;
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

    // Create FBO
    glGenFramebuffers(1, &uiFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, uiFBO);

    if (!bIsCubeMap) {
      const bool bIsRectangle = uiWidth != uiHeight;

      // Turn off mipmaps for rectangular textures
      if (bIsRectangle) bIsUsingMipMaps = false;

      const GLenum textureType = (bIsRectangle ? GL_TEXTURE_RECTANGLE : GL_TEXTURE_2D);

      // Now setup a texture to render to
      glGenTextures(1, &uiTexture);
      glBindTexture(textureType, uiTexture);

      GLenum internal = GL_RGBA8;
      GLenum type = GL_UNSIGNED_BYTE;

      // We want all FBO textures to be 16bit as we will get more precision hopefully
      internal = GL_RGBA16F; // This seems good enough and won't use twice as much(!) memory as 32bit
      //internal = GL_RGBA32F;
      type = GL_FLOAT;

      glTexImage2D(textureType, 0, internal, uiWidth, uiHeight, 0, GL_RGBA, type, NULL);

      glTexParameterf(textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameterf(textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameterf(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameterf(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      if (bIsUsingMipMaps) {
        glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(textureType);
      }

      // And attach it to the FBO so we can render to it
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureType, uiTexture, 0);
    } else {
      // Cube maps have to be square, power of two textures
      assert(spitfire::math::IsPowerOfTwo(uiWidth));
      assert(spitfire::math::IsPowerOfTwo(uiHeight));
      assert(uiWidth == uiHeight);

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
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, uiTexture, 0);
    }



    // Create the Render Buffer for Depth
    glGenRenderbuffers(1, &uiFBODepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, uiFBODepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, uiWidth, uiHeight);

    // Attach the depth render buffer to the FBO as it's depth attachment
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, uiFBODepthBuffer);



    // Check our status to make sure that we have a complete and ready to use FBO
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
      std::string sError = "UNKNOWN ERROR";

      switch (status) {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: {
          sError = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
          break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: {
          sError = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
          break;
        }
        #if BUILD_LIBOPENGLMM_OPENGL_VERSION < 300
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: {
          sError = "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT";
          break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: {
          sError = "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT";
          break;
        }
        #endif
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: {
          sError = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
          break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: {
          sError = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
          break;
        }
        case GL_FRAMEBUFFER_UNSUPPORTED: {
          sError = "GL_FRAMEBUFFER_UNSUPPORTED";
          break;
        }
        case GL_INVALID_FRAMEBUFFER_OPERATION: {
          sError = "GL_INVALID_FRAMEBUFFER_OPERATION";
          break;
        }
      }

      std::cout<<"cTextureFrameBufferObject::_Create FAILED status="<<sError<<std::endl;
      assert(status == GL_FRAMEBUFFER_COMPLETE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the FBO for now
  }

  void cTextureFrameBufferObject::_Destroy()
  {
    glDeleteFramebuffers(1, &uiFBO);
    uiFBO = 0;

    glDeleteRenderbuffers(1, &uiFBODepthBuffer);
    uiFBODepthBuffer = 0;

    // Unbind this texture if it is bound
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDeleteTextures(1, &uiTexture);
    uiTexture = 0;
  }

  void cTextureFrameBufferObject::GenerateMipMapsIfRequired()
  {
    if (bIsUsingMipMaps) {
      glBindTexture(GL_TEXTURE_2D, uiTexture);

        glGenerateMipmap(GL_TEXTURE_2D);

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

    #if BUILD_LIBOPENGLMM_OPENGL_VERSION < 300
    glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, fLevelOfDetail);
    #endif
  }
}
