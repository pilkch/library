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

// OpenGL headers
#include <GL/GLee.h>
#include <GL/glu.h>

// SDL headers
#include <SDL/SDL_image.h>

// libopenglmm headers
#include <libopenglmm/cContext.h>
#ifdef BUILD_OPENGLMM_FONT
#include <libopenglmm/cFont.h>
#endif
#include <libopenglmm/cShader.h>
#include <libopenglmm/cSystem.h>
#include <libopenglmm/cTexture.h>
#include <libopenglmm/cVertexArray.h>
#include <libopenglmm/cVertexBufferObject.h>
#include <libopenglmm/cWindow.h>

namespace opengl
{
  cContext::cContext(cSystem& _system, const cWindow& window) :
    system(_system),
    bIsRenderingToWindow(true),
    bIsValid(false),
    resolution(window.GetResolution()),
    pSurface(nullptr),
    clearColour(0.0f, 0.0f, 0.0f, 1.0f),
    ambientLightColour(1.0f, 1.0f, 1.0f, 1.0f),
    pCurrentShader(nullptr)
  {
    std::cout<<"cContext::cContext"<<std::endl;

    if (!GLeeInit()) {
      std::cout<<"cContext::cContext GLeeInit FAILED error="<<GLeeGetErrorString()<<std::endl;
    } else {
      std::cout<<"cContext::cContext GLeeInit succeeded"<<std::endl;
    }


    if (!_SetWindowVideoMode(window.IsFullScreen())) {
      std::cout<<"cContext::cContext Error setting video mode"<<std::endl;
      assert(false);
    }

    _SetDefaultFlags();
    _SetPerspective(resolution.width, resolution.height);

    system.UpdateCapabilities();


    /*
    const cCapabilities& capabilities = system.GetCapabilities();

    bIsFSAAEnabled = false;

    if (capabilities.bIsFSAASupported) {
      // Only sample at 2, 4, 8 or 16
      if (capabilities.uiFSAASampleLevel > 16) capabilities.uiFSAASampleLevel = 16;
      else if (capabilities.uiFSAASampleLevel > 8) capabilities.uiFSAASampleLevel = 8;
      else if (capabilities.uiFSAASampleLevel > 4) capabilities.uiFSAASampleLevel = 4;
      else capabilities.uiFSAASampleLevel = 2;

      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, uiFSAASampleLevel);


      if (!IsMultiSampling()) {
          bIsFSAAEnabled = false;
      } else {
          size_t uiActualSampleLevel = GetMultiSampleLevel();
          if (uiFSAASampleLevel != uiActualSampleLevel) {
            std::cout<<"cSystem::Create Requested sample level is "<<uiFSAASampleLevel<<", actual sample level is "<<uiActualSampleLevel<<std::endl;
          }

          if ((uiActualSampleLevel == 2) || (uiActualSampleLevel == 4) || (uiActualSampleLevel == 8) || (uiActualSampleLevel == 16)) bIsFSAAEnabled = true;
      }
    }*/

    bIsValid = true;
  }

  cContext::cContext(cSystem& _system, const cResolution& _resolution) :
    system(_system),
    bIsRenderingToWindow(false),
    bIsValid(false),
    resolution(_resolution),
    pSurface(nullptr),
    clearColour(0.0f, 0.0f, 0.0f, 1.0f),
    ambientLightColour(1.0f, 1.0f, 1.0f, 1.0f),
    pCurrentShader(nullptr)
  {
    std::cout<<"cContext::cContext"<<std::endl;

    _SetDefaultFlags();
    _SetPerspective(resolution.width, resolution.height);
  }

  cContext::~cContext()
  {
    std::cout<<"cContext::~cContext"<<std::endl;

    assert(textures.empty());
    assert(shaders.empty());
    assert(staticVertexBufferObjects.empty());
    assert(dynamicVertexArrays.empty());

    if (pSurface != nullptr) {
      SDL_FreeSurface(pSurface);
      pSurface = nullptr;
    }
  }

  bool cContext::IsValid() const
  {
    return bIsValid;
  }


  cTexture* cContext::CreateTexture(const opengl::string_t& sFileName)
  {
    voodoo::cImage image;
    if (!image.LoadFromFile(sFileName)) return nullptr;

    cTexture* pTexture = new cTexture;
    if (!pTexture->CreateFromImage(image)) {
      delete pTexture;
      return nullptr;
    }

    //textures.push_back(pTexture);

    return pTexture;
  }

  cTexture* cContext::CreateTextureFromImage(const voodoo::cImage& image)
  {
    assert(image.IsValid());

    cTexture* pTexture = new cTexture;
    if (!pTexture->CreateFromImage(image)) {
      delete pTexture;
      return nullptr;
    }

    //textures.push_back(pTexture);

    return pTexture;
  }

  cTexture* cContext::CreateTextureFromBuffer(const uint8_t* pBuffer, size_t width, size_t height, PIXELFORMAT pixelFormat)
  {
    assert(pBuffer != nullptr);

    voodoo::cImage image;
    if (!image.CreateFromBuffer(pBuffer, width, height, pixelFormat)) return nullptr;

    cTexture* pTexture = new cTexture;
    if (!pTexture->CreateFromImage(image)) {
      delete pTexture;
      return nullptr;
    }

    //textures.push_back(pTexture);

    return pTexture;
  }

  void cContext::DestroyTexture(cTexture* pTexture)
  {
    assert(pTexture != nullptr);

    //textures.remove(pTexture);

    pTexture->Destroy();
    delete pTexture;
  }


  cTextureFrameBufferObject* cContext::CreateTextureFrameBufferObject(size_t width, size_t height, PIXELFORMAT pixelFormat)
  {
    cTextureFrameBufferObject* pTexture = new cTextureFrameBufferObject;
    if (!pTexture->CreateFrameBufferObject(width, height)) {
      delete pTexture;
      return nullptr;
    }

    //textures.push_back(pTexture);

    return pTexture;
  }

  void cContext::DestroyTextureFrameBufferObject(cTextureFrameBufferObject* pTexture)
  {
    assert(pTexture != nullptr);

    //textures.remove(pTexture);

    pTexture->Destroy();
    delete pTexture;
  }


  cShader* cContext::CreateShader(const opengl::string_t& sVertexShaderFileName, const opengl::string_t& sFragmentShaderFileName)
  {
    cShader* pShader = new cShader;
    if (!pShader->LoadVertexShaderAndFragmentShader(sVertexShaderFileName, sFragmentShaderFileName)) {
      delete pShader;
      return nullptr;
    }

    shaders.push_back(pShader);

    return pShader;
  }

  void cContext::DestroyShader(cShader* pShader)
  {
    assert(pShader != nullptr);

    shaders.remove(pShader);

    pShader->Destroy();
    delete pShader;
  }

  cStaticVertexBufferObject* cContext::CreateStaticVertexBufferObject()
  {
    cStaticVertexBufferObject* pStaticVertexBufferObject = new cStaticVertexBufferObject;
    staticVertexBufferObjects.push_back(pStaticVertexBufferObject);
    return pStaticVertexBufferObject;
  }

  void cContext::DestroyStaticVertexBufferObject(cStaticVertexBufferObject* pStaticVertexBufferObject)
  {
    assert(pStaticVertexBufferObject != nullptr);

    staticVertexBufferObjects.remove(pStaticVertexBufferObject);

    pStaticVertexBufferObject->Destroy();
    delete pStaticVertexBufferObject;
  }

  cDynamicVertexArray* cContext::CreateDynamicVertexArray()
  {
    cDynamicVertexArray* pDynamicVertexArray = new cDynamicVertexArray;
    dynamicVertexArrays.push_back(pDynamicVertexArray);
    return pDynamicVertexArray;
  }

  void cContext::DestroyDynamicVertexArray(cDynamicVertexArray* pDynamicVertexArray)
  {
    assert(pDynamicVertexArray != nullptr);

    dynamicVertexArrays.remove(pDynamicVertexArray);

    pDynamicVertexArray->Destroy();
    delete pDynamicVertexArray;
  }

#ifdef BUILD_OPENGLMM_FONT
  cFont* cContext::CreateFont(const opengl::string_t& sFileName, size_t fontSize, const opengl::string_t& sVertexShader, const opengl::string_t& sFragmentShader)
  {
    cFont* pFont = new cFont;
    if (!pFont->Load(*this, sFileName, fontSize, sVertexShader, sFragmentShader)) {
      delete pFont;
      return nullptr;
    }

    fonts.push_back(pFont);

    return pFont;
  }

  void cContext::DestroyFont(cFont* pFont)
  {
    assert(pFont != nullptr);

    fonts.remove(pFont);

    pFont->Destroy(*this);
    delete pFont;
  }
#endif


  bool cContext::_SetWindowVideoMode(bool bIsFullScreen)
  {
    assert(bIsRenderingToWindow);

    // Avoid a divide by zero
    if (resolution.height == 0) resolution.height = 1;


    // Destroy the old surface
    if (pSurface != nullptr) {
      SDL_FreeSurface(pSurface);
      pSurface = nullptr;
    }


    unsigned int uiFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE;


    if (bIsFullScreen) {
      std::cout<<"cContext::_SetWindowVideoMode fullscreen"<<std::endl;
      uiFlags |= SDL_FULLSCREEN;
    } else {
      std::cout<<"cContext::_SetWindowVideoMode window"<<std::endl;
      uiFlags &= ~SDL_FULLSCREEN;
    }



    const SDL_VideoInfo* pVideoInfo = SDL_GetVideoInfo();
    if (pVideoInfo == nullptr) {
      std::cout<<"cContext::_SetWindowVideoMode SDL_GetVideoInfo FAILED error="<<SDL_GetError()<<std::endl;
      return false;
    }


    // This checks to see if surfaces can be stored in memory
    if (pVideoInfo->hw_available) {
      std::cout<<"cContext::_SetWindowVideoMode Hardware surface"<<std::endl;
      uiFlags |= SDL_HWSURFACE;
      uiFlags &= ~SDL_SWSURFACE;
    } else {
      std::cout<<"cContext::_SetWindowVideoMode Software surface"<<std::endl;
      uiFlags |= SDL_SWSURFACE;
      uiFlags &= ~SDL_HWSURFACE;
    }

    // This checks if hardware blits can be done
    if (pVideoInfo->blit_hw) {
      std::cout<<"cContext::_SetWindowVideoMode Hardware blit"<<std::endl;
      uiFlags |= SDL_HWACCEL;
    } else {
      std::cout<<"cContext::_SetWindowVideoMode Software blit"<<std::endl;
      uiFlags &= ~SDL_HWACCEL;
    }

    // Sets up OpenGL double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // We definitely want the OpenGL flag for SDL_SetVideoMode
    assert((uiFlags & SDL_OPENGL) != 0);


    // Create an SDL surface
    std::cout<<"cContext::_SetWindowVideoMode Calling SDL_SetVideoMode"<<std::endl;
    pSurface = SDL_SetVideoMode(resolution.width, resolution.height, GetBitsForPixelFormat(resolution.pixelFormat), uiFlags);
    if (pSurface == nullptr) {
      std::cout<<"cContext::_SetWindowVideoMode SDL_SetVideoMode FAILED error="<<SDL_GetError()<<std::endl;
      return false;
    }

    return true;
  }

  void cContext::_SetDefaultFlags()
  {
    // Setup rendering options
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  }

  void cContext::_SetPerspective(size_t width, size_t height)
  {
    // Protect against a divide by zero
    assert(height != 0);

    // Height / width ratio
    const GLfloat fRatio = (GLfloat)width / (GLfloat)height;

    // Setup our viewport
    glViewport(0, 0, (GLint)width, (GLint)height);

    // Set our perspective
    const float fMaximumViewDistance = 1000.0f;
    spitfire::math::cMat4 projection;
    projection.SetPerspective(45.0f, fRatio, 0.1f, fMaximumViewDistance);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projection.GetOpenGLMatrixPointer());

    // Set our modelview matrix
    glMatrixMode(GL_MODELVIEW);
  }

  void cContext::ResizeWindow(const cResolution& _resolution)
  {
    assert(bIsRenderingToWindow);

    resolution = _resolution;

    if (!_SetWindowVideoMode(false)) {
      std::cout<<"cContext::ResizeWindow Error setting video mode"<<std::endl;
      assert(false);
    }

    _SetPerspective(resolution.width, resolution.height);
  }

  void cContext::SetClearColour(const spitfire::math::cColour& _clearColour)
  {
    clearColour = _clearColour;
  }

  void cContext::SetAmbientLightColour(const spitfire::math::cColour& _ambientLightColour)
  {
    ambientLightColour = _ambientLightColour;
  }

  void cContext::_BeginRenderShared(size_t width, size_t height)
  {
    matProjection.LoadIdentity();
    matModelView.LoadIdentity();
    matTexture.LoadIdentity();

    _SetPerspective(width, height);


    //glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LEQUAL);

    glClearColor(clearColour.r, clearColour.g, clearColour.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set our default colour
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    //const GLfloat global_ambient[] = { ambientLightColour.r, ambientLightColour.g, ambientLightColour.b, 1.0f };
    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    //if (bIsFSAAEnabled) glEnable(GL_MULTISAMPLE_ARB);

    //if (bIsRenderWireframe) EnableWireframe();
    //else DisableWireframe();

    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_FILL);
  }

  void cContext::_EndRenderShared()
  {
    //if (bIsFSAAEnabled) glDisable(GL_MULTISAMPLE_ARB);
  }


  void cContext::BeginRendering()
  {
    _BeginRenderShared(resolution.width, resolution.height);
  }

  void cContext::EndRendering()
  {
    _EndRenderShared();

    if (bIsRenderingToWindow) {
      SDL_GL_SwapBuffers();
    }
  }

  void cContext::BeginRenderToTexture(cTextureFrameBufferObject& texture)
  {
    assert(texture.IsValid());
    assert(!texture.IsModeCubeMap()); // Cubemaps have to be rendered into each face separately

    glEnable(GL_TEXTURE_2D);

    // First we bind the FBO so we can render to it
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, texture.uiFBO);

    // Save the view port settings and set it to the size of the texture
    glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(0, 0, texture.GetWidth(), texture.GetHeight());

    _BeginRenderShared(texture.GetWidth(), texture.GetHeight());
  }

  void cContext::EndRenderToTexture(cTextureFrameBufferObject& texture)
  {
    _EndRenderShared();

    // Restore old view port settings and set rendering back to default frame buffer
    glPopAttrib();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    texture.GenerateMipMapsIfRequired();

    glDisable(GL_TEXTURE_2D);
  }

  void cContext::BeginRenderMode2D(MODE2D_TYPE type)
  {
    // Setup new matrices without touching the cached versions so that we can revert later

    spitfire::math::cMat4 matNewProjection;

    // Our screen coordinates look like this
    // 0.0f, 0.0f            1.0f, 0.0f
    //
    //
    // 0.0f, 1.0f            1.0f, 1.0f

    if (type == MODE2D_TYPE::Y_INCREASES_DOWN_SCREEN) {
      matNewProjection.SetOrtho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f); // Y axis increases down the screen
    } else {
      matNewProjection.SetOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f); // Y axis increases up the screen
    }

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(matNewProjection.GetOpenGLMatrixPointer());

    spitfire::math::cMat4 matNewModelView;
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(matNewModelView.GetOpenGLMatrixPointer());

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
  }

  void cContext::EndRenderMode2D()
  {
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    // Revert the previous ModelView matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(matProjection.GetOpenGLMatrixPointer());

    // Revert the previous projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(matProjection.GetOpenGLMatrixPointer());
  }


  // Under OpenGL 3.x we should use this method (We can probably do this under OpenGL 2.x too if we change the shaders)
  //glUniformMatrix4fv("projMat", 1, GL_FALSE, matProjection.GetOpenGLMatrixPointer());
  //glUniformMatrix4fv("???", 1, GL_FALSE, matModelView.GetOpenGLMatrixPointer());
  //glUniformMatrix4fv("???", 1, GL_FALSE, matTexture.GetOpenGLMatrixPointer());

  void cContext::SetProjectionMatrix(const spitfire::math::cMat4& matrix)
  {
    matProjection = matrix;

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(matrix.GetOpenGLMatrixPointer());
  }

  void cContext::SetModelViewMatrix(const spitfire::math::cMat4& matrix)
  {
    matModelView = matrix;

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(matrix.GetOpenGLMatrixPointer());
  }

  void cContext::SetTextureMatrix(const spitfire::math::cMat4& matrix)
  {
    matTexture = matrix;

    glMatrixMode(GL_TEXTURE);
    glLoadMatrixf(matrix.GetOpenGLMatrixPointer());
  }


  void cContext::EnableLighting()
  {
    glEnable(GL_LIGHTING);
  }

  void cContext::DisableLighting()
  {
    glDisable(GL_LIGHTING);
  }

  void cContext::EnableLight(size_t light)
  {
    glEnable(GL_LIGHT0 + light);
  }

  void cContext::DisableLighting(size_t light)
  {
    glDisable(GL_LIGHT0 + light);
  }

  void cContext::SetLightType(size_t light, LIGHT_TYPE type)
  {
  }

  void cContext::SetLightPosition(size_t light, const spitfire::math::cVec3& _position)
  {
    const GLfloat position[] = { _position.x, _position.y, _position.z, 0.0f };
    glLightfv(GL_LIGHT0 + light, GL_POSITION, position);
  }

  void cContext::SetLightRotation(size_t light, const spitfire::math::cQuaternion& rotation)
  {
  }

  void cContext::SetLightAmbientColour(size_t light, const spitfire::math::cColour& colour)
  {
    const GLfloat ambient[] = { colour.r, colour.g, colour.b, 1.0f };
    glLightfv(GL_LIGHT0 + light, GL_AMBIENT, ambient);
  }

  void cContext::SetLightDiffuseColour(size_t light, const spitfire::math::cColour& colour)
  {
    const GLfloat diffuse[] = { colour.r, colour.g, colour.b, 1.0f };
    glLightfv(GL_LIGHT0 + light, GL_DIFFUSE, diffuse);
  }

  void cContext::SetLightSpecularColour(size_t light, const spitfire::math::cColour& colour)
  {
    const GLfloat specular[] = { colour.r, colour.g, colour.b, 1.0f };
    glLightfv(GL_LIGHT0 + light, GL_SPECULAR, specular);
  }

  // TODO: Remove this, it is primarily for openglmm_gears and can probably be replaced with a shader
  void cContext::SetMaterialAmbientColour(const spitfire::math::cColour& _colour)
  {
    const GLfloat colour[] = { _colour.r, _colour.g, _colour.b, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, colour);
  }
  void cContext::SetMaterialDiffuseColour(const spitfire::math::cColour& _colour)
  {
    const GLfloat colour[] = { _colour.r, _colour.g, _colour.b, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, colour);
  }
  void cContext::SetMaterialSpecularColour(const spitfire::math::cColour& _colour)
  {
    const GLfloat colour[] = { _colour.r, _colour.g, _colour.b, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, colour);
  }
  void cContext::SetMaterialShininess(float fShininess)
  {
    const GLfloat shininess[] = { fShininess };
    glMaterialfv(GL_FRONT, GL_SPECULAR, shininess);
  }


  void cContext::EnableWireframe()
  {
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  void cContext::DisableWireframe()
  {
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }


#ifdef BUILD_OPENGLMM_FONT
  void cContext::BindFont(const cFont& font)
  {
    assert(font.IsValid());

    BindTexture(0, *(font.pTexture));
    BindShader(*(font.pShader));
  }

  void cContext::UnBindFont(const cFont& font)
  {
    assert(font.IsValid());

    UnBindShader(*(font.pShader));
    UnBindTexture(0, *(font.pTexture));
  }
#endif

  void cContext::BindTexture(size_t uTextureUnit, const cTexture& texture)
  {
    // Activate the current texture unit
    glActiveTexture(GL_TEXTURE0 + uTextureUnit);
    glClientActiveTexture(GL_TEXTURE0 + uTextureUnit);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture.GetTexture());
  }

  void cContext::UnBindTexture(size_t uTextureUnit, const cTexture& texture)
  {
    // Activate the current texture unit
    glActiveTexture(GL_TEXTURE0 + uTextureUnit);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
  }


  void cContext::BindShader(cShader& shader)
  {
    assert(shader.IsCompiledProgram());

    assert(pCurrentShader == nullptr);

    glUseProgram(shader.uiShaderProgram);

    pCurrentShader = &shader;

    // TODO: We also need some more variables within our post render shaders such as
    // brightness: HDR, Top Gear Shader, Night Vision
    // exposure: HDR, Top Gear Shader
    // sunPosition: Car Shader, shadows, this could be light[0] though

    //if (shader.bCameraPos) SetShaderConstant("cameraPos", frustum.eye);
    //if (shader.bAmbientColour) {
    //  SetShaderConstant("ambientColour", shaderConstants.GetValueVec4(TEXT("ambientColour")));
    //}
    //if (shader.bLightPosition) {
    //  SetShaderConstant("lightPosition", shaderConstants.GetValueVec3(TEXT("lightPosition")));
    //}
    if (shader.bTexUnit0) SetShaderConstant("texUnit0", 0);
    if (shader.bTexUnit1) SetShaderConstant("texUnit1", 1);
    if (shader.bTexUnit2) SetShaderConstant("texUnit2", 2);
    if (shader.bTexUnit3) SetShaderConstant("texUnit3", 3);
  }

  void cContext::UnBindShader(cShader& shader)
  {
    assert(pCurrentShader == &shader);

    glUseProgram(0);

    pCurrentShader = nullptr;
  }

  bool cContext::SetShaderConstant(const std::string& sConstant, int value)
  {
    GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
    if (loc == -1) {
      std::cout<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
      assert(loc > 0);
      return false;
    }

    glUniform1i(loc, value);
    return true;
  }

  bool cContext::SetShaderConstant(const std::string& sConstant, float value)
  {
    GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
    if (loc == -1) {
      std::cout<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
      assert(loc > 0);
      return false;
    }

    glUniform1f(loc, value);
    return true;
  }

  bool cContext::SetShaderConstant(const std::string& sConstant, const spitfire::math::cVec2& value)
  {
    GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
    if (loc == -1) {
      std::cout<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
      assert(loc > 0);
      return false;
    }

    glUniform2f(loc, value.x, value.y);
    return true;
  }

  bool cContext::SetShaderConstant(const std::string& sConstant, const spitfire::math::cVec3& value)
  {
    GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
    if (loc == -1) {
      std::cout<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
      assert(loc > 0);
      return false;
    }

    glUniform3f(loc, value.x, value.y, value.z);
    return true;
  }

  bool cContext::SetShaderConstant(const std::string& sConstant, const spitfire::math::cVec4& value)
  {
    GLint loc = glGetUniformLocation(pCurrentShader->uiShaderProgram, sConstant.c_str());
    if (loc == -1) {
      std::cout<<"cContext::SetShaderConstant \""<<opengl::string::ToUTF8(pCurrentShader->sShaderVertex)<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
      assert(loc > 0);
      return false;
    }

    glUniform4f(loc, value.x, value.y, value.z, value.w);
    return true;
  }

  bool cContext::SetShaderConstant(const std::string& sConstant, const spitfire::math::cColour& value)
  {
    return SetShaderConstant(sConstant, spitfire::math::cVec4(value.r, value.g, value.b, value.a));
  }


  // ** cStaticVertexBufferObject

  void cContext::BindStaticVertexBufferObject(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.Bind();
  }

  void cContext::UnBindStaticVertexBufferObject(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.Unbind();
  }

  void cContext::BindStaticVertexBufferObject2D(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.Bind2D();
  }

  void cContext::UnBindStaticVertexBufferObject2D(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.Unbind2D();
  }


  void cContext::DrawStaticVertexBufferObjectLines(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.RenderLines();
  }

  void cContext::DrawStaticVertexBufferObjectTriangles(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.RenderTriangles();
  }

  void cContext::DrawStaticVertexBufferObjectTriangleStrip(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.RenderTriangleStrip();
  }

  void cContext::DrawStaticVertexBufferObjectQuads(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.RenderQuads();
  }

  void cContext::DrawStaticVertexBufferObjectQuadStrip(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.RenderQuadStrip();
  }


  void cContext::DrawStaticVertexBufferObjectLines2D(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.RenderLines2D();
  }

  void cContext::DrawStaticVertexBufferObjectQuads2D(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.RenderQuads2D();
  }


  // ** cDynamicVertexArray

  void cContext::BindDynamicVertexArray(cDynamicVertexArray& dynamicVertexArray)
  {
    dynamicVertexArray.Bind();
  }

  void cContext::UnBindDynamicVertexArray(cDynamicVertexArray& dynamicVertexArray)
  {
    dynamicVertexArray.Unbind();
  }

  void cContext::BindDynamicVertexArray2D(cDynamicVertexArray& dynamicVertexArray)
  {
    dynamicVertexArray.Bind2D();
  }

  void cContext::UnBindDynamicVertexArray2D(cDynamicVertexArray& dynamicVertexArray)
  {
    dynamicVertexArray.Unbind2D();
  }


  void cContext::DrawDynamicVertexArrayLines(cDynamicVertexArray& dynamicVertexArray)
  {
    dynamicVertexArray.RenderLines();
  }

  void cContext::DrawDynamicVertexArrayTriangles(cDynamicVertexArray& dynamicVertexArray)
  {
    dynamicVertexArray.RenderTriangles();
  }

  void cContext::DrawDynamicVertexArrayTriangleStrip(cDynamicVertexArray& dynamicVertexArray)
  {
    dynamicVertexArray.RenderTriangleStrip();
  }

  void cContext::DrawDynamicVertexArrayQuads(cDynamicVertexArray& dynamicVertexArray)
  {
    dynamicVertexArray.RenderQuads();
  }

  void cContext::DrawDynamicVertexArrayQuadStrip(cDynamicVertexArray& dynamicVertexArray)
  {
    dynamicVertexArray.RenderQuadStrip();
  }


  void cContext::DrawDynamicVertexArrayLines2D(cDynamicVertexArray& dynamicVertexArray)
  {
    dynamicVertexArray.RenderLines2D();
  }

  void cContext::DrawDynamicVertexArrayQuads2D(cDynamicVertexArray& dynamicVertexArray)
  {
    dynamicVertexArray.RenderQuads2D();
  }
}
