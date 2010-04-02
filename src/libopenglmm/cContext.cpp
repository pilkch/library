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
    pCurrentShader(nullptr)
  {
    std::cout<<"cContext::cContext"<<std::endl;

    unsigned int uiFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE;


    if (window.IsFullScreen()) {
      std::cout<<"cContext::cContext fullscreen"<<std::endl;
      uiFlags |= SDL_FULLSCREEN;
    } else {
      std::cout<<"cContext::cContext window"<<std::endl;
      uiFlags &= ~SDL_FULLSCREEN;
    }



    const SDL_VideoInfo* pVideoInfo = SDL_GetVideoInfo();
    if (pVideoInfo == nullptr) {
      std::cout<<"cContext::cContext SDL_GetVideoInfo FAILED error="<<SDL_GetError()<<std::endl;
      return;
    }


    // This checks to see if surfaces can be stored in memory
    if (pVideoInfo->hw_available) {
      std::cout<<"cContext::cContext Hardware surface"<<std::endl;
      uiFlags |= SDL_HWSURFACE;
      uiFlags &= ~SDL_SWSURFACE;
    } else {
      std::cout<<"cContext::cContext Software surface"<<std::endl;
      uiFlags |= SDL_SWSURFACE;
      uiFlags &= ~SDL_HWSURFACE;
    }

    // This checks if hardware blits can be done
    if (pVideoInfo->blit_hw) {
      std::cout<<"cContext::cContext Hardware blit"<<std::endl;
      uiFlags |= SDL_HWACCEL;
    } else {
      std::cout<<"cContext::cContext Software blit"<<std::endl;
      uiFlags &= ~SDL_HWACCEL;
    }

    // Sets up OpenGL double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // We definitely want the OpenGL flag for SDL_SetVideoMode
    assert(uiFlags & SDL_OPENGL);





    if (!GLeeInit()) {
      std::cout<<"cContext::cContext GLeeInit FAILED error="<<GLeeGetErrorString()<<std::endl;
    } else {
      std::cout<<"cContext::cContext GLeeInit succeeded"<<std::endl;
    }


    // Create an SDL surface
    std::cout<<"cContext::cContext Calling SDL_SetVideoMode"<<std::endl;
    pSurface = SDL_SetVideoMode(resolution.width, resolution.height, GetBitsForPixelFormat(resolution.pixelFormat), uiFlags);
    if (pSurface == nullptr) {
      std::cout<<"cContext::cContext SDL_SetVideoMode FAILED error="<<SDL_GetError()<<std::endl;
      return;
    }


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


  void cContext::BeginRendering()
  {
    const spitfire::math::cColour clearColour(1.0f, 0.0f, 0.0f, 1.0f);
    glClearColor(clearColour.r, clearColour.g, clearColour.b, clearColour.a);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
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
    glLoadMatrixf(matProjection.GetOpenGLMatrixPointer());
  }

  void cContext::SetModelViewMatrix(const spitfire::math::cMat4& matrix)
  {
    matModelView = matrix;

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(matModelView.GetOpenGLMatrixPointer());
  }

  void cContext::SetTextureMatrix(const spitfire::math::cMat4& matrix)
  {
    matTexture = matrix;

    glMatrixMode(GL_TEXTURE);
    glLoadMatrixf(matTexture.GetOpenGLMatrixPointer());
  }


  void cContext::BindTexture(size_t uTextureUnit, const cTexture& texture)
  {
    std::cout<<"cContext::BindTexture unit="<<uTextureUnit<<", texture="<<texture.uiTexture<<std::endl;
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
    std::cout<<"cContext::SetShaderConstant "<<sConstant<<"="<<value<<std::endl;

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
