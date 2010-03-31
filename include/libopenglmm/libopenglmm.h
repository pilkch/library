/*************************************************************************
 *                                                                       *
 * libopenglmm Library, Copyright (C) 2009 Onwards Chris Pilkington         *
 * All rights reserved.  Web: http://chris.iluo.net                      *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2.1 of the  *
 * License, or (at your option) any later version. The text of the GNU   *
 * General Public License is included with this library in the           *
 * file license.txt.                                                     *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 * See the file GPL.txt for more details.                                *
 *                                                                       *
 *************************************************************************/

// This is a simple wrapper around OpenGL to make it a little bit more modern and easier to work with

#ifndef LIBOPENGLMM_H
#define LIBOPENGLMM_H

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cColour.h>

//#include <libopenglmm/cTexture.h>
//#include <libopenglmm/cShader.h>
//#include <libopenglmm/cVertexBufferObject.h>
//#include <libopenglmm/cWindow.h>

struct SDL_Surface;

namespace opengl
{
  const size_t MAX_TEXTURE_UNITS = 3;
  const size_t MAX_TEXTURE_SIZE = 1024;
  const size_t MAX_LIGHTS = 8;

  enum class CUBE_MAP_FACE {
    CUBE_MAP_FACE_POSITIVE_X,
    CUBE_MAP_FACE_NEGATIVE_X,
    CUBE_MAP_FACE_POSITIVE_Y,
    CUBE_MAP_FACE_NEGATIVE_Y,
    CUBE_MAP_FACE_POSITIVE_Z,
    CUBE_MAP_FACE_NEGATIVE_Z
  };

  enum class PIXELFORMAT {
    R8G8B8A8,
    R8G8B8,
    R5G6B5
    //float etc.
  };

  size_t GetBitsForPixelFormat(PIXELFORMAT pixelFormat);

  class cResolution
  {
  public:
    cResolution();

    size_t width;
    size_t height;
    PIXELFORMAT pixelFormat;
  };

  class cCapabilities
  {
  public:
    cCapabilities() { Clear(); }

    void Clear();

    const cResolution& GetCurrentResolution() const { return currentResolution; }
    void SetCurrentResolution(const cResolution& resolution) { currentResolution = resolution; }
    const std::vector<cResolution>& GetResolutions() const { return resolutions; }
    void AddResolution(const cResolution& resolution) { resolutions.push_back(resolution); }

    bool bIsOpenGLTwoPointZeroOrLaterSupported;
    bool bIsOpenGLThreePointZeroOrLaterSupported;
    bool bIsShadersTwoPointZeroOrLaterSupported;
    bool bIsVertexBufferObjectSupported;
    bool bIsFrameBufferObjectSupported;
    bool bIsShadowsSupported;
    bool bIsCubemappingSupported;
    size_t nTextureUnits;
    size_t iMaxTextureSize;
    bool bIsFSAASupported;
    size_t nMaxFSAALevels;

  private:
    cResolution currentResolution;
    std::vector<cResolution> resolutions;
  };

  class cContext;
  class cWindow;

  class cSystem
  {
  public:
    cSystem();
    ~cSystem();

    bool FindExtension(const std::string& sExt) const;

    void UpdateResolutions();
    void UpdateCapabilities();
    const cCapabilities& GetCapabilities() const { return capabilities; }

    cWindow* CreateWindow(const cResolution& resolution, bool bIsFullScreen);
    void DestroyWindow(cWindow* pWindow);

    cContext* CreateSharedContextFromWindow(const cWindow& window);
    cContext* CreateSharedContextFromContext(const cContext& context);
    void DestroyContext(cContext* pContext);

  private:
    float GetShaderVersion() const;

    cCapabilities capabilities;
  };

  class cWindowEventListener;
  class cInputEventListener;

  class cWindow
  {
  public:
    cWindow(cSystem& system, const cResolution& resolution, bool bIsFullScreen);
    ~cWindow();

    const cContext* GetContext() const { return pContext; }
    cContext* GetContext() { return pContext; }

    size_t GetWidth() const { return resolution.width; }
    size_t GetHeight() const { return resolution.height; }
    PIXELFORMAT GetPixelFormat() const { return resolution.pixelFormat; }
    cResolution GetResolution() const { return resolution; }
    bool IsFullScreen() const { return false; }

    void SetWindowEventListener(cWindowEventListener& listener);
    void SetInputEventListener(cInputEventListener& listener);

    std::string GetCaption() const;
    void SetCaption(const std::string& sCaption);

    void ShowCursor(bool bShow);

    void UpdateEvents();

  private:
    void OnResizeWindow(const cResolution& resolution, bool bIsFullScreen);

    void ReloadResources();

    cSystem& system;
    cResolution resolution;
    cContext* pContext;
    cWindowEventListener* pWindowEventListener;
    cInputEventListener* pInputEventListener;

    std::string sCaption;
  };


  class cTexture;
  class cShader;
  class cVertexBufferObject;

  class cContext
  {
  public:
    friend class cWindow;

    cContext(cSystem& system, const cWindow& window); // Created for a window
    cContext(cSystem& system, const cResolution& resolution); // Created for an offscreen context
    ~cContext();

    bool IsValid() const { return false; }

    size_t GetWidth() const { return resolution.width; }
    size_t GetHeight() const { return resolution.height; }
    PIXELFORMAT GetPixelFormat() const { return resolution.pixelFormat; }
    cResolution GetResolution() const { return resolution; }


    cTexture* CreateTexture(const std::string& sFileName);
    cTexture* CreateTexture(size_t width, size_t height, PIXELFORMAT pixelFormat);
    void DestroyTexture(cTexture* pTexture);

    cShader* CreateShader(const std::string& sVertexShaderFileName, const std::string& sFragmentShaderFileName);
    void DestroyShader(cShader* pShader);

    cVertexBufferObject* CreateVertexBufferObject();
    void DestroyVertexBufferObject(cVertexBufferObject* pVertexBufferObject);


    void BeginRendering() {}
    void EndRendering() {}


    void BindTexture(size_t uTextureUnit, const cTexture& texture) {}
    void UnBindTexture(size_t uTextureUnit, const cTexture& texture) {}


    void BindShader(const cShader& shader) {}
    void UnBindShader(const cShader& shader) {}


    const spitfire::math::cMat4& GetProjectionMatrix() const { return matProjection; }
    const spitfire::math::cMat4& GetModelViewMatrix() const { return matModelView; }
    const spitfire::math::cMat4& GetTextureMatrix() const { return matTexture; }

    // A shader must already be bound before these are called
    void SetProjectionMatrix(const spitfire::math::cMat4& matrix);
    void SetModelViewMatrix(const spitfire::math::cMat4& matrix);
    void SetTextureMatrix(const spitfire::math::cMat4& matrix);


    void BindVertexBufferObject(const cVertexBufferObject& vertexBufferObject) {}
    void UnBindVertexBufferObject(const cVertexBufferObject& vertexBufferObject) {}

    void DrawVertexBufferObject(const cVertexBufferObject& vertexBufferObject) {}

  protected:
    void ReloadResources() {}

  private:
    cSystem& system;
    bool bIsValid;
    cResolution resolution;

    SDL_Surface* pSurface;

    // matProjection and matModelView are multiplied together to get the MPV matrix in OpenGL 3.0
    spitfire::math::cMat4 matProjection;
    spitfire::math::cMat4 matModelView;
    spitfire::math::cMat4 matTexture;

    std::map<std::string, cTexture*> textures;
    std::vector<cShader*> shaders;
    std::vector<cVertexBufferObject*> vertexBufferObjects;
  };


  class cTexture
  {
  public:
    bool IsValid() const { return false; }

    bool LoadFromFile(const std::string& sFileName) { return false; }
  };

  class cShader
  {
  public:
    bool IsValid() const { return false; }

    bool LoadVertexShaderFromFile(const std::string& sFileName) { return false; }
    bool LoadFragmentShaderFromFile(const std::string& sFileName) { return false; }
  };


  class cVertexBufferObject
  {
  public:
    bool IsValid() const { return false; }


  };



  class cWindowEvent;
  class cKeyboardEvent;
  class cMouseEvent;

  class cWindowEventListener
  {
  public:
    void OnWindowEvent(const cWindowEvent& event) { _OnWindowEvent(event); }

  private:
    virtual void _OnWindowEvent(const cWindowEvent& event) = 0;
  };

  class cInputEventListener
  {
  public:
    void OnKeyboardEvent(const cKeyboardEvent& event) { _OnKeyboardEvent(event); }
    void OnMouseEvent(const cMouseEvent& event) { _OnMouseEvent(event); }

  private:
    virtual void _OnMouseEvent(const cMouseEvent& event) = 0;
    virtual void _OnKeyboardEvent(const cKeyboardEvent& event) = 0;
  };



  enum class TYPE {
    WINDOW_QUIT,
    WINDOW_ACTIVATE,
    WINDOW_DEACTIVATE,
    WINDOW_RESIZE,
    KEY_DOWN,
    KEY_UP,
    MOUSE_DOWN,
    MOUSE_UP,
    MOUSE_MOVE,
  };

  class cWindowEvent
  {
  public:
    friend class cWindow;

    cWindowEvent();

    bool IsQuit() const { return (type == TYPE::WINDOW_QUIT); }
    bool IsActivated() const { return (type == TYPE::WINDOW_ACTIVATE); }
    bool IsDeactivated() const { return (type == TYPE::WINDOW_DEACTIVATE); }
    bool IsResize() const { return (type == TYPE::WINDOW_RESIZE); }

  protected:
    TYPE type;
  };

  class cKeyboardEvent
  {
  public:
    friend class cWindow;

    cKeyboardEvent();

    bool IsKeyDown() const { return (type == TYPE::KEY_DOWN); }
    bool IsKeyUp() const { return (type == TYPE::KEY_UP); }
    unsigned int GetKeyCode() const { return keyCode; }

  protected:
    TYPE type;
    unsigned int keyCode;
  };

  class cMouseEvent
  {
  public:
    friend class cWindow;

    cMouseEvent();

    bool IsButtonDown() const { return (type == TYPE::MOUSE_DOWN); }
    bool IsButtonUp() const { return (type == TYPE::MOUSE_UP); }
    unsigned int GetButton() const { return button; }
    float GetX() const { return x; }
    float GetY() const { return y; }

  protected:
    TYPE type;
    unsigned int button;
    float x;
    float y;
  };
}

#endif // LIBOPENGLMM_H
