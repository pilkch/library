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

// Standard headers
#include <map>

// SDL headers
#include <SDL2/SDL.h>

// libopenglmm headers
#include <libopenglmm/libopenglmm.h>

struct SDL_Surface;

#ifdef __WIN__
#pragma push_macro("DELETE")
#undef DELETE
#endif

namespace opengl
{
  class cWindowEventListener;
  class cInputEventListener;

  enum KEY {
    // Mapping to SDLK constants
    // The keyboard syms have been cleverly chosen to map to ASCII
    UNKNOWN = SDLK_UNKNOWN,
    BACKSPACE = SDLK_BACKSPACE,
    TAB = SDLK_TAB,
    CLEAR = SDLK_CLEAR,
    RETURN = SDLK_RETURN,
    PAUSE = SDLK_PAUSE,
    ESCAPE = SDLK_ESCAPE,
    SPACE = SDLK_SPACE,
    EXCLAIM = SDLK_EXCLAIM,
    QUOTE_DOUBLE = SDLK_QUOTEDBL,
    HASH = SDLK_HASH,
    CURRENCY = SDLK_DOLLAR,
    AMPERSAND = SDLK_AMPERSAND,
    QUOTE = SDLK_QUOTE,
    PARENTHESES_LEFT = SDLK_LEFTPAREN,
    PARENTHESES_RIGHT = SDLK_RIGHTPAREN,
    ASTERISK = SDLK_ASTERISK,
    PLUS = SDLK_PLUS,
    COMMA = SDLK_COMMA,
    MINUS = SDLK_MINUS,
    PERIOD = SDLK_PERIOD,
    SLASH = SDLK_SLASH,
    NUMBER_0 = SDLK_0,
    NUMBER_1 = SDLK_1,
    NUMBER_2 = SDLK_2,
    NUMBER_3 = SDLK_3,
    NUMBER_4 = SDLK_4,
    NUMBER_5 = SDLK_5,
    NUMBER_6 = SDLK_6,
    NUMBER_7 = SDLK_7,
    NUMBER_8 = SDLK_8,
    NUMBER_9 = SDLK_9,
    COLON = SDLK_COLON,
    SEMICOLON = SDLK_SEMICOLON,
    LESS = SDLK_LESS,
    EQUALS = SDLK_EQUALS,
    GREATER = SDLK_GREATER,
    QUESTION_MARK = SDLK_QUESTION,
    AT = SDLK_AT,
    // Skip uppercase letters
    BRACKET_LEFT = SDLK_LEFTBRACKET,
    BACKSLASH = SDLK_BACKSLASH,
    BRACKET_RIGHT = SDLK_RIGHTBRACKET,
    CARET = SDLK_CARET,
    UNDERSCORE = SDLK_UNDERSCORE,
    BACKQUOTE = SDLK_BACKQUOTE,
    A = SDLK_a,
    B = SDLK_b,
    C = SDLK_c,
    D = SDLK_d,
    E = SDLK_e,
    F = SDLK_f,
    G = SDLK_g,
    H = SDLK_h,
    I = SDLK_i,
    J = SDLK_j,
    K = SDLK_k,
    L = SDLK_l,
    M = SDLK_m,
    N = SDLK_n,
    O = SDLK_o,
    P = SDLK_p,
    Q = SDLK_q,
    R = SDLK_r,
    S = SDLK_s,
    T = SDLK_t,
    U = SDLK_u,
    V = SDLK_v,
    W = SDLK_w,
    X = SDLK_x,
    Y = SDLK_y,
    Z = SDLK_z,
    DELETE = SDLK_DELETE,
    // End of ASCII mapped keysyms

    /*
    // International keyboard syms
    SDLK_WORLD_0        = 160, // 0xA0
    ...
    SDLK_WORLD_95       = 255, // 0xFF
    */

    // Numeric keypad
    /*SDLK_KP0,
    SDLK_KP1,
    SDLK_KP2,
    SDLK_KP3,
    SDLK_KP4,
    SDLK_KP5,
    SDLK_KP6,
    SDLK_KP7,
    SDLK_KP8,
    SDLK_KP9,
    SDLK_KP_PERIOD,
    SDLK_KP_DIVIDE,
    SDLK_KP_MULTIPLY,
    SDLK_KP_MINUS,
    SDLK_KP_PLUS,
    SDLK_KP_ENTER,
    SDLK_KP_EQUALS,*/

    // Arrows + Home/End pad
    UP = SDLK_UP,
    DOWN = SDLK_DOWN,
    RIGHT = SDLK_RIGHT,
    LEFT = SDLK_LEFT,
    INSERT = SDLK_INSERT,
    HOME = SDLK_HOME,
    END = SDLK_END,
    PAGEUP = SDLK_PAGEUP,
    PAGEDOWN = SDLK_PAGEDOWN,

    // Function keys
    F1 = SDLK_F1,
    F2 = SDLK_F2,
    F3 = SDLK_F3,
    F4 = SDLK_F4,
    F5 = SDLK_F5,
    F6 = SDLK_F6,
    F7 = SDLK_F7,
    F8 = SDLK_F8,
    F9 = SDLK_F9,
    F10 = SDLK_F10,
    F11 = SDLK_F11,
    F12 = SDLK_F12,
    F13 = SDLK_F13,
    F14 = SDLK_F14,
    F15 = SDLK_F15,

    //// Key state modifier keys
    //SDLK_NUMLOCK,
    //SDLK_CAPSLOCK       = 301,
    //SDLK_SCROLLOCK      = 302,

    RSHIFT = SDLK_RSHIFT,
    LSHIFT = SDLK_LSHIFT,
    RCTRL = SDLK_RCTRL,
    LCTRL = SDLK_LCTRL,
    RALT = SDLK_RALT,
    LALT = SDLK_LALT,

    /*SDLK_RMETA      = 309,
    SDLK_LMETA      = 310,
    SDLK_LSUPER     = 311,      // Left "Windows" key
    SDLK_RSUPER     = 312,      // Right "Windows" key
    SDLK_MODE       = 313,      // "Alt Gr" key
    SDLK_COMPOSE,  // Multi-key compose key

    // Miscellaneous function keys
    SDLK_HELP       = 315,
    SDLK_PRINT      = 316,
    SDLK_SYSREQ     = 317,
    SDLK_BREAK      = 318,
    SDLK_MENU       = 319,
    SDLK_POWER      = 320,      // Power Macintosh power key
    SDLK_EURO       = 321,      // Some european keyboards
    SDLK_UNDO       = 322,      // Atari keyboard has Undo*/
  };

  bool IsKeyPrintable(KEY key);

  class cWindow
  {
  public:
    friend class cContext;

    cWindow(cSystem& system, const opengl::string_t& sCaption, const cResolution& resolution, bool bIsFullScreen);
    ~cWindow();

    #ifdef __WIN__
    HWND GetWindowHandle();
    #endif

    const cContext* GetContext() const { return pContext; }
    cContext* GetContext() { return pContext; }

    size_t GetWidth() const { return resolution.width; }
    size_t GetHeight() const { return resolution.height; }
    PIXELFORMAT GetPixelFormat() const { return resolution.pixelFormat; }
    cResolution GetResolution() const { return resolution; }
    bool IsFullScreen() const { return bIsFullScreen; }

    void SetWindowEventListener(cWindowEventListener& listener);
    void SetInputEventListener(cInputEventListener& listener);

    const opengl::string_t& GetCaption() const;
    void SetCaption(const opengl::string_t& sCaption);

    void ShowCursor(bool bShow);
    void WarpCursorToMiddleOfScreen();

    bool IsKeyUp(KEY key) const;
    bool IsKeyDown(KEY key) const;
    bool IsKeyHeld(KEY key) const;
    bool GetKeyState(KEY key) const;

    void ProcessEvents();

    void SwapWindowFromContext(); // Swap the window buffer (Called by cContext, you do not need to call this yourself)

  protected:
    SDL_Window* pWindow;

  private:
    void OnResizeWindow(const cResolution& resolution, bool bIsFullScreen);

    cSystem& system;
    cResolution resolution;
    bool bIsFullScreen;
    cContext* pContext;
    cWindowEventListener* pWindowEventListener;
    cInputEventListener* pInputEventListener;

    opengl::string_t sCaption;

    // For handling key repeating
    enum class STATE {
      NOT_FOUND,
      UP,
      DOWN,
      HELD,
    };
    std::map<KEY, STATE> keystates;
  };


  class cWindowEvent;
  class cKeyboardEvent;
  class cMouseEvent;

  class cWindowEventListener
  {
  public:
    virtual ~cWindowEventListener() {}

    void HandleSDLEvent(const SDL_Event& event) { _HandleSDLEvent(event); } // Allow the application to handle the raw event if nothing else handles it first
    #ifdef __WIN__
    LRESULT HandleWin32Event(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return _HandleWin32Event(hwnd, uMsg, wParam, lParam); }
    #endif

    void OnWindowEvent(const cWindowEvent& event) { _OnWindowEvent(event); }

  private:
    virtual void _HandleSDLEvent(const SDL_Event& event) {}
    #ifdef __WIN__
    virtual LRESULT _HandleWin32Event(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return FALSE; }
    #endif
    virtual void _OnWindowEvent(const cWindowEvent& event) = 0;
  };

  class cInputEventListener
  {
  public:
    virtual ~cInputEventListener() {}

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
    WINDOW_ABOUT_TO_RESIZE,
    WINDOW_RESIZED,
    #ifdef __WIN__
    WINDOW_COMMAND, // Sent when a control or menu command is processed
    #endif
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
    bool IsAboutToResize() const { return (type == TYPE::WINDOW_ABOUT_TO_RESIZE); }
    bool IsResized() const { return (type == TYPE::WINDOW_RESIZED); }
    #ifdef __WIN__
    bool IsCommand() const { return (type == TYPE::WINDOW_COMMAND); }
    int GetCommandID() const { return iCommandID; }
    #endif

  protected:
    TYPE type;
    #ifdef __WIN__
    int iCommandID;
    #endif
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

    bool IsMouseMove() const { return (type == TYPE::MOUSE_MOVE); }
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


  // ** Inlines

  inline bool IsKeyPrintable(KEY key)
  {
    if ((key >= KEY::A) && (key <= KEY::Z)) return true;

    if ((key >= KEY::SPACE) && (key <= KEY::BACKQUOTE)) return true;

    return false;
  }
}

#endif // LIBOPENGLMM_WINDOW_H
