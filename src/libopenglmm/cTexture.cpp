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
#include <SDL3_image/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/util/log.h>

// libopenglmm headers
#include <libopenglmm/cTexture.h>
#include <libopenglmm/cSystem.h>
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

  unsigned int cTexture::GetTextureType() const
  {
    // TODO: This doesn't take into account cubemaps
  
    const size_t width = GetWidth();
    const size_t height = GetHeight();

    if (width == height) {
      //std::cout<<"Returning GL_TEXTURE_2D"<<std::endl;
      return GL_TEXTURE_2D;
    } else if ((width == 1) || (height == 1)) {
      //std::cout<<"Returning GL_TEXTURE_1D"<<std::endl;
      return GL_TEXTURE_1D;
    }

    //std::cout<<"Returning GL_TEXTURE_RECTANGLE"<<std::endl;
    return GL_TEXTURE_RECTANGLE;
  }

  bool cTexture::CreateFromImage(const voodoo::cImage& _image)
  {
    assert(_image.IsValid());

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
    const GLenum textureType = GetTextureType();

    glBindTexture(textureType, uiTexture);
  }

  void cTexture::_Destroy()
  {
    // Destroy old texture
    glDeleteTextures(1, &uiTexture);
    uiTexture = 0;
  }

  void cTexture::CopyFromImageToTexture()
  {
    LOG("glGetError=", cSystem::GetErrorString());

    if (!IsValid()) {
      LOGERROR("Texture is invalid, returning");
      return;
    }

    const uint8_t* pBuffer = image.GetPointerToBuffer();
    if (pBuffer == nullptr) {
      LOGERROR("Image is invalid, returning");
      return;
    }

    const size_t uiWidth = GetWidth();
    const size_t uiHeight = GetHeight();

    const bool bIsRectangle = uiWidth != uiHeight;

    // Turn off mipmaps for rectangular textures
    if (bIsRectangle) bIsUsingMipMaps = false;

    const GLenum textureType = GetTextureType();

    // Bind so that the next operations happen on this texture
    glBindTexture(textureType, uiTexture);

    // Destination
    GLenum internal = GL_RGBA;

    // Source
    GLenum format = GL_RGBA;
    GLenum type = GL_UNSIGNED_BYTE;

    // 16 bit floats as a source is not supported because I don't know what type the source would be, something like float16? float16_t?
    assert(image.GetPixelFormat() != voodoo::PIXELFORMAT::RGB16F);
  
    if (image.GetPixelFormat() == voodoo::PIXELFORMAT::H8) {
      internal = GL_RED;
      format = GL_RED;
      type = GL_UNSIGNED_BYTE;
    } else if (image.GetPixelFormat() == voodoo::PIXELFORMAT::R8G8B8) {
      internal = GL_RGB;
      format = GL_RGB;
      type = GL_UNSIGNED_BYTE;
    } else if ((image.GetPixelFormat() == voodoo::PIXELFORMAT::RGB16F) || (image.GetPixelFormat() == voodoo::PIXELFORMAT::RGB32F)) {
      internal = GL_RGB16F;
      format = GL_RGB;
      type = GL_FLOAT;
    }


    // Settings to make the texture look a bit nicer when we do blit it to the screen
    glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (!bIsRectangle) {
      glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    if (bIsUsingMipMaps) {
      glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      #if BUILD_LIBOPENGLMM_OPENGL_VERSION < 300
      glTexParameteri(textureType, GL_GENERATE_MIPMAP, GL_TRUE);
      #endif
    } else {
      glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // Copy from image to texture
    if (textureType == GL_TEXTURE_1D) glTexImage1D(textureType, 0, internal, int(max(uiWidth, uiHeight)), 0, format, type, pBuffer);
    else if ((image.GetPixelFormat() == voodoo::PIXELFORMAT::RGB16F) || (image.GetPixelFormat() == voodoo::PIXELFORMAT::RGB32F)) {
      const float* pBufferAsFloat = reinterpret_cast<const float*>(image.GetPointerToBuffer());
      if (pBufferAsFloat == nullptr) {
        LOGERROR("Float image is invalid, returning");
        return;
      }

      std::cout<<"Creating float texture "<<uiWidth<<"x"<<uiHeight<<std::endl;
      glTexImage2D(textureType, 0, internal, int(uiWidth), int(uiHeight), 0, format, type, pBufferAsFloat);
    } else {
      glTexImage2D(textureType, 0, internal, int(uiWidth), int(uiHeight), 0, format, type, pBuffer);
    }

    if (bIsUsingMipMaps) glGenerateMipmap(textureType);
  }

  void cTexture::SetMinFilter(TEXTURE_FILTER filter)
  {
    const GLenum textureType = GetTextureType();
    GLenum mode = GL_LINEAR;
    if (filter == TEXTURE_FILTER::NEAREST) mode = GL_NEAREST;
    else if (filter == TEXTURE_FILTER::LINEAR_MIPMAP_NEAREST) mode = GL_LINEAR_MIPMAP_NEAREST;
    glBindTexture(textureType, uiTexture);
    glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, mode);
    glBindTexture(textureType, uiTexture);
  }

  void cTexture::SetMagFilter(TEXTURE_FILTER filter)
  {
    LOG("glGetError=", cSystem::GetErrorString());
    const GLenum textureType = GetTextureType();
    GLenum mode = GL_LINEAR;
    if (filter == TEXTURE_FILTER::NEAREST) mode = GL_NEAREST;
    else if (filter == TEXTURE_FILTER::LINEAR_MIPMAP_NEAREST) mode = GL_LINEAR_MIPMAP_NEAREST;
    glBindTexture(textureType, uiTexture);
    glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, mode);
    glBindTexture(textureType, uiTexture);
  }

  void cTexture::SetWrap(TEXTURE_WRAP wrap)
  {
    LOG("glGetError=", cSystem::GetErrorString());
    const GLenum textureType = GetTextureType();
    const GLenum mode = (wrap == TEXTURE_WRAP::CLAMP_TO_EDGE ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glBindTexture(textureType, uiTexture);
    glTexParameteri(textureType, GL_TEXTURE_WRAP_S, mode);
    glTexParameteri(textureType, GL_TEXTURE_WRAP_T, mode);
    glBindTexture(textureType, uiTexture);
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
    const opengl::string_t& filePathNegativeZ,
    bool bIsFloat
  )
  {
    struct AxisToFilePath {
      GLuint axis;
      const char_t* szFilePath;
    };

    // http://stackoverflow.com/questions/11685608/convention-of-faces-in-opengl-cubemapping
    const AxisToFilePath axisToFilePath[6] = {
      { GL_TEXTURE_CUBE_MAP_POSITIVE_X, filePathPositiveX.c_str() },
      { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, filePathNegativeX.c_str() },
      { GL_TEXTURE_CUBE_MAP_POSITIVE_Y, filePathPositiveY.c_str() },
      { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, filePathNegativeY.c_str() },
      { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, filePathPositiveZ.c_str() },
      { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, filePathNegativeZ.c_str() },
    };

    // Create new texture
    glGenTextures(1, &uiTexture);

    // Bind so that the next operations happen on this texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, uiTexture);

    {
      voodoo::cImage image;

      for (size_t i = 0; i < 6; i++) {
        image.LoadFromFile(axisToFilePath[i].szFilePath);

        assert(image.IsValid());
        assert(spitfire::math::IsPowerOfTwo(image.GetWidth()));
        assert(spitfire::math::IsPowerOfTwo(image.GetHeight()));
        assert(image.GetWidth() == image.GetHeight());
        if (bIsFloat) assert(image.GetPixelFormat() == voodoo::PIXELFORMAT::RGB32F);

        if (image.GetPointerToBuffer() != nullptr) {
          // Copy from image to texture
          if (bIsFloat) {
            const float* pBufferAsFloat = reinterpret_cast<const float*>(image.GetPointerToBuffer());
            if (pBufferAsFloat != nullptr) {
              glTexImage2D(axisToFilePath[i].axis, 0, GL_RGB16F, int(image.GetWidth()), int(image.GetHeight()), 0, GL_RGB, GL_FLOAT, pBufferAsFloat);
            }
          } else {
            GLenum type = GL_RGBA;
            if (image.GetPixelFormat() == voodoo::PIXELFORMAT::R8G8B8) {
              type = GL_RGB;
            }

            const uint8_t* pBuffer = image.GetPointerToBuffer();
            if (pBuffer != nullptr) {
              glTexImage2D(axisToFilePath[i].axis, 0, type, int(image.GetWidth()), int(image.GetHeight()), 0, type, GL_UNSIGNED_BYTE, pBuffer);
            }
          }

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
    uiDepthTexture(0),
    bIsCubeMap(false)
  {
    image.SetWidth(DEFAULT_FBO_TEXTURE_WIDTH);
    image.SetHeight(DEFAULT_FBO_TEXTURE_HEIGHT);
  }

  cTextureFrameBufferObject::~cTextureFrameBufferObject()
  {
    assert(uiTexture == 0);
    assert(uiDepthTexture == 0);
  }

  bool cTextureFrameBufferObject::IsDepthTexturePresent() const
  {
    return (uiDepthTexture != 0);
  }

  unsigned int cTextureFrameBufferObject::GetDepthTexture() const
  {
    //ASSERT(glIsTexture(uiFBODepthBuffer) == GL_TRUE);
    //return uiFBODepthBuffer;
    ASSERT(glIsTexture(uiDepthTexture) == GL_TRUE);
    return uiDepthTexture;
  }

  bool cTextureFrameBufferObject::CreateFrameBufferObject(size_t width, size_t height, PIXELFORMAT pixelFormat, const FLAGS& flags)
  {
    image.SetWidth(width);
    image.SetHeight(height);

    Create(pixelFormat, flags);

    return IsValid();
  }

  void cTextureFrameBufferObject::Create(PIXELFORMAT pixelFormat, const FLAGS& flags)
  {
    // http://www.opengl.org/wiki/GL_EXT_framebuffer_object#Quick_example.2C_render_to_texture_.282D.29.2C_mipmaps
    // http://www.opengl.org/wiki/GL_EXT_framebuffer_object#Quick_example.2C_render_to_texture_.28Cubemap.29
    // http://www.gamedev.net/topic/617910-rendering-to-and-using-a-depth-texture/

    const size_t uiWidth = image.GetWidth();
    const size_t uiHeight = image.GetHeight();
  
    bIsCubeMap = flags.bCubeMap;

    glGenFramebuffers(1, &uiFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, uiFBO);

    // Create FBO for colour buffer
    if (flags.bColourBuffer) {
      if (!bIsCubeMap) {
        const bool bIsRectangle = uiWidth != uiHeight;

        // Turn off mipmaps for rectangular textures
        if (bIsRectangle) bIsUsingMipMaps = false;

        const GLenum textureType = GetTextureType();

        // Now setup a texture to render to
        glGenTextures(1, &uiTexture);
        glBindTexture(textureType, uiTexture);

        GLenum internal = GL_RGBA8;
        if (pixelFormat == PIXELFORMAT::RGB16F) internal = GL_RGB16F;

        glTexImage2D(textureType, 0, internal, int(uiWidth), int(uiHeight), 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);

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
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum internal = GL_RGBA8;
        if (pixelFormat == PIXELFORMAT::RGB16F) internal = GL_RGB16F;

        for (size_t i = 0; i < 6; i++) {
          // NULL means reserve texture memory, but texels are undefined
          glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal, int(uiWidth), int(uiHeight), 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
        }

        // Attach one of the faces of the Cubemap texture to this FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, uiTexture, 0);
      }
    }

    // TODO: Do we ever not want a depth buffer? 2D GUI rendering perhaps?
    assert(flags.bDepthBuffer);

    if (flags.bDepthBuffer) {
      const GLenum textureType = GetTextureType();

      const GLenum internal = GL_DEPTH_COMPONENT32;
      const GLenum type = GL_FLOAT;

      // Create the texture for the depth
      glGenTextures(1, &uiDepthTexture);
      glBindTexture(textureType, uiDepthTexture);

      glTexParameterf(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameterf(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(textureType, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

      if (flags.bDepthShadow) {
        // Add a border colour so that everything outside the shadowmap is rendered in full light
        glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        const float borderColour[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(textureType, GL_TEXTURE_BORDER_COLOR, borderColour);
        glTexParameteri(textureType, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
      } else {
        // Just a normal depth map
        glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(textureType, GL_TEXTURE_COMPARE_MODE, GL_NONE);
      }

      glTexImage2D(textureType, 0, internal, int(uiWidth), int(uiHeight), 0, GL_DEPTH_COMPONENT, type, nullptr);

      glBindTexture(textureType, 0);

      // Create the Render Buffer for Depth
      glGenRenderbuffers(1, &uiFBODepthBuffer);
      glBindRenderbuffer(GL_RENDERBUFFER, uiFBODepthBuffer);
      glRenderbufferStorage(GL_RENDERBUFFER, internal, int(uiWidth), int(uiHeight));

      // Attach the depth render buffer to the FBO as it's depth attachment
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, uiFBODepthBuffer);

      // Attach it to the FBO so we can render to it
      //glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, uiDepthTexture, 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureType, uiDepthTexture, 0);
    }


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

      LOGERROR("FAILED status=", sError);
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

    glDeleteTextures(1, &uiDepthTexture);
    uiDepthTexture = 0;

    glDeleteTextures(1, &uiTexture);
    uiTexture = 0;
  }

  void cTextureFrameBufferObject::GenerateMipMaps()
  {
    ASSERT(GetWidth() == GetHeight());

    const GLenum textureType = (bIsCubeMap ? GL_TEXTURE_CUBE_MAP : GetTextureType());

    glBindTexture(textureType, uiTexture);

      glGenerateMipmap(textureType);

    glBindTexture(textureType, 0);
  }

  void cTextureFrameBufferObject::GenerateMipMapsIfRequired()
  {
    if (bIsUsingMipMaps) {
      GenerateMipMaps();
    }
  }

  void cTextureFrameBufferObject::SelectMipMapLevelOfDetail(float fLevelOfDetail)
  {
    (void)fLevelOfDetail;

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
