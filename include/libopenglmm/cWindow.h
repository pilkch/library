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

#ifndef LIBOPENGLMM_WINDOW_H
#define LIBOPENGLMM_WINDOW_H

#include <libopenglmm/libopenglmm.h>

struct SDL_Surface;

namespace opengl
{
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

    cSystem& system;
    cResolution resolution;
    cContext* pContext;
    cWindowEventListener* pWindowEventListener;
    cInputEventListener* pInputEventListener;

    std::string sCaption;
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

#endif // LIBOPENGLMM_WINDOW_H
