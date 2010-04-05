// Standard headers
#include <cstdio>
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

// libopenglmm headers
#include <libopenglmm/cContext.h>
#include <libopenglmm/cShader.h>
#include <libopenglmm/cSystem.h>
#include <libopenglmm/cTexture.h>
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
    clearColour(1.0f, 0.0f, 0.0f, 1.0f),
    pCurrentShader(nullptr)
  {
    std::cout<<"cContext::cContext"<<std::endl;

    if (!GLeeInit()) {
      std::cout<<"cContext::cContext GLeeInit FAILED error="<<GLeeGetErrorString()<<std::endl;
    } else {
      std::cout<<"cContext::cContext GLeeInit succeeded"<<std::endl;
    }


    if (!SetWindowVideoMode(window.IsFullScreen())) {
      std::cout<<"cContext::cContext Error setting video mode"<<std::endl;
      assert(false);
    }

    SetPerspective();

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
    pCurrentShader(nullptr)
  {
    std::cout<<"cContext::cContext"<<std::endl;
  }

  cContext::~cContext()
  {
    assert(pSurface != nullptr);

    SDL_FreeSurface(pSurface);
    pSurface = nullptr;
  }

  bool cContext::IsValid() const
  {
    return (pSurface != nullptr);
  }

  cTexture* cContext::CreateTexture(const std::string& sFileName)
  {
    cTexture* pTexture = new cTexture;
    if (!pTexture->Load(sFileName)) {
      delete pTexture;
      return nullptr;
    }

    pTexture->Create();
    pTexture->CopyFromSurfaceToTexture();

    return pTexture;
  }

  cTexture* cContext::CreateTexture(size_t width, size_t height, PIXELFORMAT pixelFormat)
  {
    return nullptr;
  }

  void cContext::DestroyTexture(cTexture* pTexture)
  {
    assert(pTexture != nullptr);
    delete pTexture;
  }

  cShader* cContext::CreateShader(const std::string& sVertexShaderFileName, const std::string& sFragmentShaderFileName)
  {
    cShader* pShader = new cShader;
    if (!pShader->LoadVertexShaderAndFragmentShader(sVertexShaderFileName, sFragmentShaderFileName)) {
      delete pShader;
      return nullptr;
    }

    return pShader;
  }

  void cContext::DestroyShader(cShader* pShader)
  {
    assert(pShader != nullptr);
    delete pShader;
  }

  cStaticVertexBufferObject* cContext::CreateStaticVertexBufferObject()
  {
    return new cStaticVertexBufferObject;
  }

  void cContext::DestroyStaticVertexBufferObject(cStaticVertexBufferObject* pStaticVertexBufferObject)
  {
    assert(pStaticVertexBufferObject != nullptr);
    delete pStaticVertexBufferObject;
  }


  bool cContext::SetWindowVideoMode(bool bIsFullScreen)
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
      std::cout<<"cContext::SetWindowVideoMode fullscreen"<<std::endl;
      uiFlags |= SDL_FULLSCREEN;
    } else {
      std::cout<<"cContext::SetWindowVideoMode window"<<std::endl;
      uiFlags &= ~SDL_FULLSCREEN;
    }



    const SDL_VideoInfo* pVideoInfo = SDL_GetVideoInfo();
    if (pVideoInfo == nullptr) {
      std::cout<<"cContext::SetWindowVideoMode SDL_GetVideoInfo FAILED error="<<SDL_GetError()<<std::endl;
      return false;
    }


    // This checks to see if surfaces can be stored in memory
    if (pVideoInfo->hw_available) {
      std::cout<<"cContext::SetWindowVideoMode Hardware surface"<<std::endl;
      uiFlags |= SDL_HWSURFACE;
      uiFlags &= ~SDL_SWSURFACE;
    } else {
      std::cout<<"cContext::SetWindowVideoMode Software surface"<<std::endl;
      uiFlags |= SDL_SWSURFACE;
      uiFlags &= ~SDL_HWSURFACE;
    }

    // This checks if hardware blits can be done
    if (pVideoInfo->blit_hw) {
      std::cout<<"cContext::SetWindowVideoMode Hardware blit"<<std::endl;
      uiFlags |= SDL_HWACCEL;
    } else {
      std::cout<<"cContext::SetWindowVideoMode Software blit"<<std::endl;
      uiFlags &= ~SDL_HWACCEL;
    }

    // Sets up OpenGL double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // We definitely want the OpenGL flag for SDL_SetVideoMode
    assert(uiFlags & SDL_OPENGL);


    // Create an SDL surface
    std::cout<<"cContext::SetWindowVideoMode Calling SDL_SetVideoMode"<<std::endl;
    pSurface = SDL_SetVideoMode(resolution.width, resolution.height, GetBitsForPixelFormat(resolution.pixelFormat), uiFlags);
    if (pSurface == nullptr) {
      std::cout<<"cContext::SetWindowVideoMode SDL_SetVideoMode FAILED error="<<SDL_GetError()<<std::endl;
      return false;
    }


    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    return true;
  }

  void cContext::SetPerspective()
  {
    const size_t width = resolution.width;
    const size_t height = resolution.height;

    // Protect against a divide by zero
    assert(height != 0);

    // Height / width ratio
    const GLfloat ratio = (GLfloat)width / (GLfloat)height;

    // Setup our viewport
    glViewport(0, 0, (GLint)width, (GLint)height);

    // Change to the projection matrix and set our viewing volume
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set our perspective
    const float fMaximumViewDistance = 1000.0f;
    gluPerspective(45.0f, ratio, 0.1f, fMaximumViewDistance);

    // Set our modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void cContext::ResizeWindow(const cResolution& _resolution)
  {
    assert(bIsRenderingToWindow);

    resolution = _resolution;

    if (!SetWindowVideoMode(false)) {
      std::cout<<"cContext::ResizeWindow Error setting video mode"<<std::endl;
      assert(false);
    }

    SetPerspective();
  }

  void cContext::SetClearColour(const spitfire::math::cColour& _clearColour)
  {
    clearColour = _clearColour;
  }

  void cContext::BeginRendering()
  {
    matProjection.LoadIdentity();
    matModelView.LoadIdentity();
    matTexture.LoadIdentity();


    glClearDepth(1.0);
    glClearColor(clearColour.r, clearColour.g, clearColour.b, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set our default colour
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Set our modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void cContext::EndRendering()
  {
    if (bIsRenderingToWindow) {
      SDL_GL_SwapBuffers();
    }
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


  void cContext::BindTexture(size_t uTextureUnit, const cTexture& texture)
  {
    // Activate the current texture unit
    glActiveTexture(GL_TEXTURE0 + uTextureUnit);
    glClientActiveTexture(GL_TEXTURE0 + uTextureUnit);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture.uiTexture);
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
      std::cout<<"cContext::SetShaderConstant \""<<pCurrentShader->sShaderVertex<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
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
      std::cout<<"cContext::SetShaderConstant \""<<pCurrentShader->sShaderVertex<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
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
      std::cout<<"cContext::SetShaderConstant \""<<pCurrentShader->sShaderVertex<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
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
      std::cout<<"cContext::SetShaderConstant \""<<pCurrentShader->sShaderVertex<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
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
      std::cout<<"cContext::SetShaderConstant \""<<pCurrentShader->sShaderVertex<<"\":\""<<pCurrentShader->IsCompiledFragment()<<"\" Couldn't set \""<<sConstant<<"\" perhaps the constant is not actually used within the shader"<<std::endl;
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


  void cContext::BindStaticVertexBufferObject(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.Bind();
  }

  void cContext::UnBindStaticVertexBufferObject(cStaticVertexBufferObject& staticVertexBufferObject)
  {
    staticVertexBufferObject.Unbind();
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
}
