// Standard headers
#include <cstdio>
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
#include <SDL2/SDL_image.h>

// Spitfire headers
#include <spitfire/util/log.h>
#include <spitfire/util/string.h>

// libopenglmm headers
#include <libopenglmm/libopenglmm.h>
#include <libopenglmm/cContext.h>
#include <libopenglmm/cSystem.h>
#include <libopenglmm/cWindow.h>
#include <libopenglmm/opengl.h>

namespace opengl
{
  PIXELFORMAT GetPixelFormatFromSDLPixelFormatEnum(uint32_t uiPixelFormat)
  {
    switch (uiPixelFormat) {
      case SDL_PIXELFORMAT_RGBA8888: return PIXELFORMAT::R8G8B8A8;
      case SDL_PIXELFORMAT_RGB888: return PIXELFORMAT::R8G8B8;
      case SDL_PIXELFORMAT_RGB565: return PIXELFORMAT::R5G6B5;
    };

    assert(false);
    return PIXELFORMAT::R5G6B5;
  }

  // *** cCapabilities

  void cCapabilities::Clear()
  {
    bIsOpenGLTwoPointZeroOrLaterSupported = false;
    bIsOpenGLThreePointZeroOrLaterSupported = false;
    bIsShadersTwoPointZeroOrLaterSupported = false;
    bIsVertexBufferObjectSupported = false;
    bIsFrameBufferObjectSupported = false;
    bIsShadowsSupported = false;
    bIsCubemappingSupported = false;
    nTextureUnits = 0;
    iMaxTextureSize = 0;

    bIsFSAASupported = false;
    nMaxFSAALevels = 0;
  }


  // *** cSystem

  cSystem::cSystem() :
    nContexts(0)
  {
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0) {
      LOGERROR("SDL_Init FAILED error=", SDL_GetError());
      return;
    }

    UpdateResolutions();
  }

  cSystem::~cSystem()
  {
    assert(nContexts == 0);

    SDL_Quit();
  }

  string_t cSystem::GetErrorString(GLenum error)
  {
    switch (error) {
      case GL_NO_ERROR: return TEXT("GL_NO_ERROR");
      case GL_INVALID_ENUM: return TEXT("GL_INVALID_ENUM");
      case GL_INVALID_VALUE: return TEXT("GL_INVALID_VALUE");
      case GL_INVALID_OPERATION: return TEXT("GL_INVALID_OPERATION");
      #if BUILD_LIBOPENGLMM_OPENGL_VERSION < 300
      // NOTE: These only makes sense for OpenGL 2 and earlier because the matrix stack has been removed in OpenGL 3
      case GL_STACK_OVERFLOW: return TEXT("GL_STACK_OVERFLOW");
      case GL_STACK_UNDERFLOW: return TEXT("GL_STACK_UNDERFLOW");
      #endif
      case GL_OUT_OF_MEMORY: return TEXT("GL_OUT_OF_MEMORY");
    };

    spitfire::ostringstream_t o;
    o<<TEXT("Unknown error ");
    o<<error;
    return o.str();
  }

  string_t cSystem::GetErrorString()
  {
    const GLenum error = glGetError();
    const string_t sError = GetErrorString(error);
    if (error == GL_NO_ERROR) return sError;

    LOG(TEXT("cSystem::GetErrorString Error "), sError);
    assert(error == GL_NO_ERROR);
    return sError;
  }

  bool cSystem::IsGPUATI() const
  {
    std::ostringstream tVendor;
    tVendor<<glGetString(GL_VENDOR);

    const std::string sVendor(tVendor.str());
    return (sVendor.find("ATI") != std::string::npos) || (sVendor.find("AMD") != std::string::npos);
  }

  bool cSystem::IsGPUNVIDIA() const
  {
    std::ostringstream tVendor;
    tVendor<<glGetString(GL_VENDOR);

    const std::string sVendor(tVendor.str());
    return (sVendor.find("NVIDIA") != std::string::npos);
  }

  #define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
  #define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

  size_t cSystem::GetGPUMemoryTotalMB() const
  {
    size_t nTotalMB = 0;

    if (IsGPUATI()) {

    } else if (IsGPUNVIDIA()) {
      GLint total_mem_kb = 0;
      glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &total_mem_kb);
      nTotalMB = total_mem_kb;
    }

    return nTotalMB;
  }

  size_t cSystem::GetGPUMemoryUsedMB() const
  {
    size_t nTotalMB = GetGPUMemoryTotalMB();
    size_t nUsedMB = 0;

    if (IsGPUATI()) {
      // TODO: Test this on an ATI card
      //int cur_avail_mem_kb = 0;
      //glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, &cur_avail_mem_kb);
      //size_t nAvailableMB = cur_avail_mem_kb / 1024;
      //assert(nAvailableMB <= nTotalMB);
      //nUsedMB = nTotalMB - nAvailableMB;
    } else if (IsGPUNVIDIA()) {
      GLint cur_avail_mem_kb = 0;
      glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &cur_avail_mem_kb);
      size_t nAvailableMB = cur_avail_mem_kb / 1024;
      assert(nAvailableMB <= nTotalMB);
      nUsedMB = nTotalMB - nAvailableMB;
    }

    return nUsedMB;
  }

  std::string cSystem::GetExtensions() const
  {
    #if BUILD_LIBOPENGLMM_OPENGL_VERSION >= 300
    LOG("");
    std::ostringstream o;
    int iNumberOfExtensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &iNumberOfExtensions);
    o<<"Extensions not added";
    //for (int i = 0; i < iNumberOfExtensions; i++) {
    //  o<<glGetStringi(GL_EXTENSIONS, i);
    //  LOG("\"", o.str(), "\"");
    //  o<<" ";
    //}
    return o.str();
    #else
    return (const char*)glGetString(GL_EXTENSIONS);
    #endif
  }

  bool cSystem::FindExtension(const std::string& sExt) const
  {
    #if BUILD_LIBOPENGLMM_OPENGL_VERSION >= 300
    LOG("");

    (void)sExt;

    // There is a bug in some drivers where glGetStringi returns NULL, so we just return true for all extensions (We aren't using anything special and if we are running OpenGL 3 we probably support all the basic extensions anyway)
    return true;

    /*int iNumberOfExtensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &iNumberOfExtensions);
    for (int i = 0; i < iNumberOfExtensions; i++) {
      std::ostringstream t;
      t<<glGetStringi(GL_EXTENSIONS, i);
      ASSERT(glGetStringi(GL_EXTENSIONS, i) != nullptr);
      ASSERT(glGetStringi(GL_EXTENSIONS, i)[0] != 0);
      LOG("\"", t.str(), "\"");
      if (sExt == t.str()) return true;
    }
    return false;*/
    #else
    std::ostringstream t;
    t<<const_cast<const unsigned char*>(glGetString(GL_EXTENSIONS));

    return (t.str().find(sExt) != std::string::npos);
    #endif
  }

  float cSystem::GetShaderVersion() const
  {
    float fGLVersion = 0.0f;
    {
      const std::string sVersion((const char*)glGetString(GL_VERSION));
      std::istringstream i(sVersion);
      i>>fGLVersion;
    }

    float fGLSLVersion = 0.0f;
    if (fGLVersion >= 2.0f) {
      const char* ptr = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

      if (ptr != nullptr) {
        std::string temp(ptr);

        std::istringstream stm(temp);
        stm>>fGLSLVersion;
      }
    }

    // Shader Model 4.0
    if (fGLSLVersion >= 4.0f) return fGLSLVersion;
    if (FindExtension("GL_NV_gpu_program4") || FindExtension("GL_NV_geometry_program4") ||
      FindExtension("GL_NV_vertex_program4") || FindExtension("GL_NV_fragment_program4") ||
      FindExtension("GL_EXT_gpu_shader4") || FindExtension("GL_EXT_geometry_shader4")) return 4.0f;

    // Shader Model 3.0
    if (fGLSLVersion >= 3.0f) return fGLSLVersion;
    if (FindExtension("GL_NV_vertex_program3") || FindExtension("GL_NV_fragment_program2") ||
      FindExtension("GL_ATI_shader_texture_lod")) return 3.0f;

    // Shader Model 2.0
    if (fGLSLVersion >= 2.0f) return fGLSLVersion;

    // Shader Model 1.0
    if (fGLSLVersion >= 1.0f) return fGLSLVersion;
    if (FindExtension("GL_ARB_shading_language_100")) return 1.0f;

    // Shader Model before 1.0
    return 0.0f;
  }

  void cSystem::UpdateResolutions()
  {
    SDL_DisplayMode currentDisplayMode;
    int iResult = SDL_GetDesktopDisplayMode(0, &currentDisplayMode);
    if (iResult != 0) {
      LOG("SDL_GetDesktopDisplayMode FAILED error=", SDL_GetError());
      return;
    }

    //for each resolution found {
    //  cResolution resolution;
    //  resolution.width = 1024;
    //  resolution.height = 768;
    //  resolution.pixelFormat = PIXELFORMAT::R8G8B8A8;
    //
    //  capabilities.AddResolution(resolution);
    //}

    cResolution resolution;
    resolution.width = currentDisplayMode.w;
    resolution.height = currentDisplayMode.h;
    resolution.pixelFormat = GetPixelFormatFromSDLPixelFormatEnum(currentDisplayMode.format);

    capabilities.SetCurrentResolution(resolution);
  }

  void cSystem::UpdateCapabilities()
  {
    LOG("glGetError=", cSystem::GetErrorString());

    const char* szValue = nullptr;

    szValue = (const char*)glGetString(GL_VENDOR);
    assert(szValue != nullptr);
    LOG("Vendor: ", (szValue != nullptr ? szValue : ""));
    szValue = (const char*)glGetString(GL_RENDERER);
    assert(szValue != nullptr);
    LOG("Renderer: ", (szValue != nullptr ? szValue : ""));
    szValue = (const char*)glGetString(GL_VERSION);
    assert(szValue != nullptr);
    LOG("Version: ", (szValue != nullptr ? szValue : ""));
    szValue = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    assert(szValue != nullptr);
    LOG("GLSL Version: ", (szValue != nullptr ? szValue : ""));
    const std::string sValue = GetExtensions();
    LOG("Extensions: ", spitfire::string::ToString_t(sValue));

    if (!IsGPUNVIDIA() && !IsGPUATI()) {
      std::ostringstream tVendor;
      tVendor<<glGetString(GL_VENDOR);

      const std::string sVendor(tVendor.str());
      LOG("Vendor is neither ATI nor NVIDIA, vendor=", spitfire::string::ToString_t(sVendor));
    }

    if (FindExtension("GL_ARB_multitexture")) LOG("Found GL_ARB_multitexture");
    else {
      LOG("GL_ARB_multitexture is not present");
      assert(false);
    }


    GLint iValue = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &iValue);
    assert(iValue > 0);
    capabilities.iMaxTextureSize = iValue;

     // Limit the max texture size to MAX_TEXTURE_SIZE
    if (capabilities.iMaxTextureSize > MAX_TEXTURE_SIZE) capabilities.iMaxTextureSize = MAX_TEXTURE_SIZE;

    LOG("Max texture size ", capabilities.iMaxTextureSize);

    // Cube Map Support
    if (FindExtension("GL_ARB_texture_cube_map")) {
      LOG("Found GL_ARB_texture_cube_map");
      capabilities.bIsCubemappingSupported = true;
    } else {
      LOG("Not Found GL_ARB_texture_cube_map");
    }


    // GLSL Version
    float fShaderVersion = GetShaderVersion();
    {
      const string_t sShaderVersion = spitfire::string::ToString(fShaderVersion);

      if (fShaderVersion >= 1.0f) {
        LOG("Found Shader", sShaderVersion);
        capabilities.bIsShadersTwoPointZeroOrLaterSupported = true;
      } else {
        LOG("Not Found Shader1.1, version found is Shader", sShaderVersion);
        capabilities.bIsShadersTwoPointZeroOrLaterSupported = false;
      }
    }

    if (capabilities.bIsShadersTwoPointZeroOrLaterSupported) LOG("Can use shaders, shaders turned on");
    else LOG("Cannot use shaders, shaders turned off");


    // How many textures can we access in a vertex shader (As opposed to in the fragment shader)
    GLint iTextureUnitsInVertexShader = 0;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &iTextureUnitsInVertexShader);
    LOG(iTextureUnitsInVertexShader, " texture units accessable in vertex shader");


    // Frame Buffer Object Support
    if (FindExtension("GL_EXT_framebuffer_object")) {
      LOG("Found GL_EXT_framebuffer_object");
      capabilities.bIsFrameBufferObjectSupported = true;
    } else LOG("Not Found GL_EXT_framebuffer_object");


    //capabilities.bIsFSAASupported = (GL_ARB_multisample != 0);
    /*if (capabilities.bIsFSAASupported) {
      // Only sample at 2, 4, 8 or 16
      if (capabilities.nMaxFSAALevels > 16) capabilities.nMaxFSAALevels = 16;
      else if (capabilities.nMaxFSAALevels > 8) capabilities.nMaxFSAALevels = 8;
      else if (capabilities.nMaxFSAALevels > 4) capabilities.nMaxFSAALevels = 4;
      else capabilities.nMaxFSAALevels = 2;
    }*/
  }

#ifdef BUILD_LIBOPENGLMM_WINDOW_SDL
  cWindow* cSystem::CreateWindow(const opengl::string_t& sCaption, const cResolution& resolution, bool bIsFullScreen)
  {
    cWindow* pWindow = new cWindow(*this, sCaption, resolution, bIsFullScreen);

    return pWindow;
  }

  void cSystem::DestroyWindow(cWindow* pWindow)
  {
    assert(pWindow != nullptr);
    delete pWindow;
  }

  cContext* cSystem::CreateSharedContextFromWindow(cWindow& window)
  {
    nContexts++;

    return new cContext(*this, window);
  }
#endif
  
  #if defined(BUILD_LIBOPENGLMM_WINDOW_WIN32) || defined(BUILD_LIBOPENGLMM_WINDOW_GTKGLEXT)
  #ifdef BUILD_LIBOPENGLMM_WINDOW_GTKGLEXT
  cContext* cSystem::CreateSharedContextForWidget(const cResolution& resolution)
  #elif defined(BUILD_LIBOPENGLMM_WINDOW_WIN32)
  cContext* cSystem::CreateSharedContextForControl(HWND control, const cResolution& resolution)
  #endif
  {
    #ifdef BUILD_LIBOPENGLMM_WINDOW_WIN32
    // Ignored, we are really just documenting that this requires an already active control
    (void)control;
    #endif

    nContexts++;

    return new cContext(*this, resolution, true);
  }
  #endif

  cContext* cSystem::CreateSharedContextFromContext(const cContext& rhs)
  {
    nContexts++;

    return new cContext(*this, rhs.GetResolution(), false);
  }

  void cSystem::DestroyContext(cContext* pContext)
  {
    delete pContext;
    pContext = nullptr;

    nContexts--;
  }
}
