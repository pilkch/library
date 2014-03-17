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
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_image.h>

// Spitfire headers
#include <spitfire/util/log.h>

// libopenglmm headers
#include <libopenglmm/cContext.h>
#include <libopenglmm/cSystem.h>
#include <libopenglmm/cWindow.h>
#include <libopenglmm/opengl.h>

namespace opengl
{
  bool IsKeyPrintable(KEY key)
  {
    if ((key >= KEY::A) && (key <= KEY::Z)) return true;

    if ((key >= KEY::SPACE) && (key <= KEY::BACKQUOTE)) return true;

    return false;
  }


  // ** cWindow

  cWindow::cWindow(cSystem& _system, const opengl::string_t& sCaption, const cResolution& _resolution, bool _bIsFullScreen) :
    pWindow(nullptr),
    system(_system),
    resolution(_resolution),
    bIsFullScreen(_bIsFullScreen),
    pContext(nullptr),
    pWindowEventListener(nullptr),
    pInputEventListener(nullptr)
  {
    LOG<<"cWindow::cWindow "<<std::endl;

    pContext = system.CreateSharedContextFromWindow(*this);

    // Set our caption
    SetCaption(sCaption);
  }

  cWindow::~cWindow()
  {
    system.DestroyContext(pContext);
    pContext = nullptr;
  }

  #ifdef __WIN__
  HWND cWindow::GetWindowHandle()
  {
    ASSERT(pWindow != nullptr);

    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    if (!SDL_GetWindowWMInfo(pWindow, &info))  {
      LOGERROR<<"cWindow::GetWindowHandle SDL_GetWindowWMInfo FAILED"<<std::endl;
      return NULL;
    }

    // Store the windows handle
    return info.info.win.window;
  }
  #endif

  void cWindow::SetWindowEventListener(cWindowEventListener& listener)
  {
    pWindowEventListener = &listener;
  }

  void cWindow::SetInputEventListener(cInputEventListener& listener)
  {
    pInputEventListener = &listener;
  }

  const opengl::string_t& cWindow::GetCaption() const
  {
    return sCaption;
  }

  void cWindow::SetCaption(const opengl::string_t& _sCaption)
  {
    // Set the caption
    sCaption = _sCaption;
    SDL_SetWindowTitle(pWindow, opengl::string::ToUTF8(sCaption).c_str());

    // Set the icon
    SDL_Surface* pIcon = IMG_Load("data/icons/application_32x32.png");
    if (pIcon == nullptr) LOGERROR<<"cWindow::SetCaption Could not load data/icons/application_32x32.png"<<std::endl;
    else {
      SDL_SetWindowIcon(pWindow, pIcon);
      SDL_FreeSurface(pIcon);
    }
  }

  void cWindow::ShowCursor(bool bShow)
  {
    SDL_ShowCursor(bShow ? SDL_ENABLE : SDL_DISABLE);
  }

  void cWindow::WarpCursorToMiddleOfScreen()
  {
    SDL_WarpMouseInWindow(pWindow, resolution.width / 2, resolution.height / 2);
  }

  void cWindow::SwapWindowFromContext()
  {
    SDL_GL_SwapWindow(pWindow);
  }

  bool cWindow::IsKeyUp(KEY key) const
  {
    std::map<KEY, STATE>::const_iterator iter = keystates.find(key);
    return (iter != keystates.end()) && (iter->second == STATE::UP);
  }

  bool cWindow::IsKeyDown(KEY key) const
  {
    std::map<KEY, STATE>::const_iterator iter = keystates.find(key);
    return (iter != keystates.end()) && (iter->second == STATE::DOWN);
  }

  bool cWindow::IsKeyHeld(KEY key) const
  {
    std::map<KEY, STATE>::const_iterator iter = keystates.find(key);
    return (iter != keystates.end()) && (iter->second == STATE::HELD);
  }

  bool cWindow::GetKeyState(KEY key) const
  {
    return (IsKeyDown(key) || IsKeyHeld(key));
  }

  void cWindow::OnResizeWindow(const cResolution& _resolution, bool bIsFullScreen)
  {
    (void)bIsFullScreen;

    assert(pContext != nullptr);

    resolution = _resolution;

    pContext->ResizeWindow(*this, resolution);
  }

  void cWindow::ProcessEvents()
  {
    // Which keys were released this time step
    std::vector<KEY> keysUp;

    // Handle all the events in the queue
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
      switch (sdlEvent.type) {
        case SDL_QUIT: {
          LOG<<"cWindow::ProcessEvents Quit"<<std::endl;
          if (pWindowEventListener != nullptr) {
            cWindowEvent event;
            event.type = TYPE::WINDOW_QUIT;
            pWindowEventListener->OnWindowEvent(event);
          }
          break;
        }

        case SDL_WINDOWEVENT: {
          switch (sdlEvent.window.event) {
            case SDL_WINDOWEVENT_FOCUS_GAINED: {
              LOG<<"cWindow::ProcessEvents Activated"<<std::endl;
              if (pWindowEventListener != nullptr) {
                cWindowEvent event;
                event.type = TYPE::WINDOW_ACTIVATE;
                pWindowEventListener->OnWindowEvent(event);
              }
              break;
            }
            case SDL_WINDOWEVENT_FOCUS_LOST: {
              LOG<<"cWindow::ProcessEvents Deactivated"<<std::endl;
              if (pWindowEventListener != nullptr) {
                cWindowEvent event;
                event.type = TYPE::WINDOW_DEACTIVATE;
                pWindowEventListener->OnWindowEvent(event);
              }
              break;
            }
            case SDL_WINDOWEVENT_RESIZED: {
              LOG<<"cWindow::ProcessEvents Resize"<<std::endl;

              if (pWindowEventListener != nullptr) {
                // Send an about to resize event
                cWindowEvent event;
                event.type = TYPE::WINDOW_ABOUT_TO_RESIZE;
                pWindowEventListener->OnWindowEvent(event);
              }

              cResolution resolution;
              resolution.width = size_t(sdlEvent.window.data1);
              resolution.height = size_t(sdlEvent.window.data2);
              resolution.pixelFormat = GetPixelFormat();
              OnResizeWindow(resolution, IsFullScreen());

              // Send a resized event
              if (pWindowEventListener != nullptr) {
                cWindowEvent event;
                event.type = TYPE::WINDOW_RESIZED;
                pWindowEventListener->OnWindowEvent(event);
              }

              break;
            }
          };

          break;
        }

        case SDL_KEYDOWN: {
          // Update our key state
          KEY key = KEY(sdlEvent.key.keysym.sym);
          keystates[key] = STATE::DOWN;

          if (pInputEventListener != nullptr) {
            cKeyboardEvent event;
            event.type = TYPE::KEY_DOWN;
            event.keyCode = sdlEvent.key.keysym.sym;
            pInputEventListener->OnKeyboardEvent(event);
          }
          break;
        }

        case SDL_KEYUP: {
          // Update our key state and add our key to the list of keys released this time step
          KEY key = KEY(sdlEvent.key.keysym.sym);
          keystates[key] = STATE::UP;
          keysUp.push_back(key);

          if (pInputEventListener != nullptr) {
            cKeyboardEvent event;
            event.type = TYPE::KEY_UP;
            event.keyCode = sdlEvent.key.keysym.sym;
            pInputEventListener->OnKeyboardEvent(event);
          }
          break;
        }

        case SDL_MOUSEBUTTONUP: {
          if (pInputEventListener != nullptr) {
            cMouseEvent event;
            event.type = TYPE::MOUSE_UP;
            event.button = sdlEvent.button.button;
            event.x = sdlEvent.button.x;
            event.y = sdlEvent.button.y;
            pInputEventListener->OnMouseEvent(event);
          }
          break;
        }

        case SDL_MOUSEBUTTONDOWN: {
          if (pInputEventListener != nullptr) {
            cMouseEvent event;
            event.type = TYPE::MOUSE_DOWN;
            event.button = sdlEvent.button.button;
            event.x = sdlEvent.button.x;
            event.y = sdlEvent.button.y;
            pInputEventListener->OnMouseEvent(event);
          }
          break;
        }

        case SDL_MOUSEMOTION: {
          if (pInputEventListener != nullptr) {
            cMouseEvent event;
            event.type = TYPE::MOUSE_MOVE;
            event.x = sdlEvent.button.x;
            event.y = sdlEvent.button.y;
            pInputEventListener->OnMouseEvent(event);
          }
          break;
        }

        default:
          pWindowEventListener->HandleSDLEvent(sdlEvent);
          break;
      }
    }


    // Process keys that were released or held down this time step
    for (std::map<KEY, STATE>::iterator iter = keystates.begin(); iter != keystates.end(); iter++) {
      if (iter->second == STATE::UP) {
        // Handle keys that were up and may now be back to the starting state
        bool bIsKeyFound = false;

        // Find the matching key in keysUp
        const size_t n = keysUp.size();
        for (size_t i = 0; i < n; i++) {
          if (keysUp[i] == iter->first) {
            bIsKeyFound = true;
            break;
          }
        }

        // If the key wasn't changed set it to not found
        if (!bIsKeyFound) iter->second = STATE::NOT_FOUND;
      } else if (iter->second == STATE::DOWN) {
        // Handle keys that were down and may now be into the held state
        bool bIsKeyFound = false;

        // Find the matching key in keysUp
        const size_t n = keysUp.size();
        for (size_t i = 0; i < n; i++) {
          if (keysUp[i] == iter->first) {
            bIsKeyFound = true;
            break;
          }
        }

        // If the key wasn't changed set it to held
        if (!bIsKeyFound) iter->second = STATE::HELD;
      }
    }
  }


  cWindowEvent::cWindowEvent() :
    type(TYPE::WINDOW_QUIT)
  {
  }

  cKeyboardEvent::cKeyboardEvent() :
    type(TYPE::KEY_DOWN),
    keyCode(0)
  {
  }

  cMouseEvent::cMouseEvent() :
    type(TYPE::MOUSE_MOVE),
    button(0),
    x(0.0f),
    y(0.0f)
  {
  }
}
