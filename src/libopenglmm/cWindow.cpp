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
#include <SDL3_image/SDL_image.h>

// Spitfire headers
#include <spitfire/util/log.h>

// libopenglmm headers
#include <libopenglmm/cContext.h>
#include <libopenglmm/cSystem.h>
#include <libopenglmm/cWindow.h>
#include <libopenglmm/opengl.h>

namespace opengl
{
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
    LOG("");

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
    return (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(pWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
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
    if (pIcon == nullptr) LOGERROR("Could not load data/icons/application_32x32.png");
    else {
      SDL_SetWindowIcon(pWindow, pIcon);
      SDL_DestroySurface(pIcon);
    }
  }

  bool cWindow::IsActive() const
  {
    // Return true if this window has focus
    uint32_t uFlags = SDL_GetWindowFlags(pWindow);
    return ((uFlags & SDL_WINDOW_INPUT_FOCUS) != 0);
  }

  void cWindow::ShowCursor(bool bShow)
  {
    if (bShow) SDL_ShowCursor();
    else SDL_HideCursor();
  }

  void cWindow::WarpCursorToMiddleOfScreen()
  {
    SDL_WarpMouseInWindow(pWindow, int(resolution.width / 2), int(resolution.height / 2));
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

  void cWindow::OnResizeWindow(const cResolution& _resolution, bool _bIsFullScreen)
  {
    (void)_bIsFullScreen;

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
      // Allow imgui to handle any events first
      if (pWindowEventListener->OnRawSDLEvent(sdlEvent)) {
        // imgui handled the event
        continue;
      }

      switch (sdlEvent.type) {
        case SDL_EVENT_QUIT: {
          LOG("Quit");
          if (pWindowEventListener != nullptr) {
            cWindowEvent event;
            event.type = TYPE::WINDOW_QUIT;
            pWindowEventListener->OnWindowEvent(event);
          }
          break;
        }

        #ifdef __WIN__
        case SDL_SYSWMEVENT: {
          // Allow the application to handle the raw Win32 Event
          if (pWindowEventListener != nullptr) {
            pWindowEventListener->HandleWin32Event(sdlEvent.syswm.msg->msg.win.hwnd, sdlEvent.syswm.msg->msg.win.msg, sdlEvent.syswm.msg->msg.win.wParam, sdlEvent.syswm.msg->msg.win.lParam);
          }

          break;
        }
        #endif

        case SDL_EVENT_WINDOW_FOCUS_GAINED: {
          LOG("Activated");
          if (pWindowEventListener != nullptr) {
            cWindowEvent event;
            event.type = TYPE::WINDOW_ACTIVATE;
            pWindowEventListener->OnWindowEvent(event);
          }
          break;
        }
        case SDL_EVENT_WINDOW_FOCUS_LOST: {
          LOG("Deactivated");
          if (pWindowEventListener != nullptr) {
            cWindowEvent event;
            event.type = TYPE::WINDOW_DEACTIVATE;
            pWindowEventListener->OnWindowEvent(event);
          }
          break;
        }
        case SDL_EVENT_WINDOW_RESIZED: {
          LOG("Resize");

          if (pWindowEventListener != nullptr) {
            // Send an about to resize event
            cWindowEvent event;
            event.type = TYPE::WINDOW_ABOUT_TO_RESIZE;
            pWindowEventListener->OnWindowEvent(event);
          }

          cResolution newResolution;
          newResolution.width = size_t(sdlEvent.window.data1);
          newResolution.height = size_t(sdlEvent.window.data2);
          newResolution.pixelFormat = GetPixelFormat();
          OnResizeWindow(newResolution, IsFullScreen());

          // Send a resized event
          if (pWindowEventListener != nullptr) {
            cWindowEvent event;
            event.type = TYPE::WINDOW_RESIZED;
            pWindowEventListener->OnWindowEvent(event);
          }

          break;
        }

        case SDL_EVENT_KEY_DOWN: {
          // Update our key state
          KEY key = KEY(sdlEvent.key.key);
          keystates[key] = STATE::DOWN;

          if (pInputEventListener != nullptr) {
            cKeyboardEvent event;
            event.type = TYPE::KEY_DOWN;
            event.keyCode = sdlEvent.key.key;
            pInputEventListener->OnKeyboardEvent(event);
          }
          break;
        }

        case SDL_EVENT_KEY_UP: {
          // Update our key state and add our key to the list of keys released this time step
          KEY key = KEY(sdlEvent.key.key);
          keystates[key] = STATE::UP;
          keysUp.push_back(key);

          if (pInputEventListener != nullptr) {
            cKeyboardEvent event;
            event.type = TYPE::KEY_UP;
            event.keyCode = sdlEvent.key.key;
            pInputEventListener->OnKeyboardEvent(event);
          }
          break;
        }

        case SDL_EVENT_MOUSE_WHEEL: {
          if (pInputEventListener != nullptr) {
            cMouseEvent event;

            if (sdlEvent.wheel.y > 0) {
              event.type = TYPE::MOUSE_SCROLL_UP;
            }
            else if (sdlEvent.wheel.y < 0) {
              event.type = TYPE::MOUSE_SCROLL_DOWN;
            }

            // NOTE: We can also check sdlEvent.wheel.x for scrolling left and right

            event.x = sdlEvent.button.x;
            event.y = sdlEvent.button.y;
            pInputEventListener->OnMouseEvent(event);
          }
          break;
        }

        case SDL_EVENT_MOUSE_BUTTON_UP: {
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

        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
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

        case SDL_EVENT_MOUSE_MOTION: {
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
    #ifdef __WIN__
    , iCommandID(0)
    #endif
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
