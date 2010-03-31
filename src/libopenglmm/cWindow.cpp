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

// SDL headers

// libopenglmm headers
#include <libopenglmm/cContext.h>
#include <libopenglmm/cSystem.h>
#include <libopenglmm/cWindow.h>

namespace opengl
{
  cWindow::cWindow(cSystem& _system, const cResolution& _resolution, bool bIsFullScreen) :
    system(_system),
    resolution(_resolution),
    pContext(nullptr),
    pWindowEventListener(nullptr),
    pInputEventListener(nullptr)
  {
    // Enable unicode
    SDL_EnableUNICODE(1);

    // Enable key repeat
    SDL_EnableKeyRepeat(200, 20);

    pContext = system.CreateSharedContextFromWindow(*this);
  }

  cWindow::~cWindow()
  {
    system.DestroyContext(pContext);
    pContext = nullptr;
  }

  void cWindow::SetWindowEventListener(cWindowEventListener& listener)
  {
    pWindowEventListener = &listener;
  }

  void cWindow::SetInputEventListener(cInputEventListener& listener)
  {
    pInputEventListener = &listener;
  }

  std::string cWindow::GetCaption() const
  {
    return sCaption;
  }

  void cWindow::SetCaption(const std::string& _sCaption)
  {
    sCaption = _sCaption;
    SDL_WM_SetCaption(sCaption.c_str(), "app.ico");
  }

  void cWindow::ShowCursor(bool bShow)
  {
    if (bShow) SDL_ShowCursor(SDL_DISABLE);
    else SDL_ShowCursor(SDL_ENABLE);
  }

  void cWindow::OnResizeWindow(const cResolution& _resolution, bool bIsFullScreen)
  {
    // TODO: Resize the window, reinit opengl etc.
    resolution = _resolution;

    ReloadResources();
  }

  void cWindow::ReloadResources()
  {
    assert(pContext != nullptr);
    pContext->ReloadResources();
  }


  void cWindow::UpdateEvents()
  {
    // Handle all the events in the queue
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
      switch (sdlEvent.type) {
        case SDL_QUIT: {
          std::cout<<"cWindow::UpdateEvents Quit"<<std::endl;
          cWindowEvent event;
          event.type = TYPE::WINDOW_QUIT;
          if (pWindowEventListener != nullptr) pWindowEventListener->OnWindowEvent(event);
          break;
        }

        case SDL_ACTIVEEVENT: {
          cWindowEvent event;
          bool bActivated = (sdlEvent.active.gain != 0);
          if (bActivated) {
            std::cout<<"cWindow::UpdateEvents Activated"<<std::endl;
            event.type = TYPE::WINDOW_ACTIVATE;
          } else {
            std::cout<<"cWindow::UpdateEvents Deactivated"<<std::endl;
            event.type = TYPE::WINDOW_DEACTIVATE;
          }
          if (pWindowEventListener != nullptr) pWindowEventListener->OnWindowEvent(event);
          break;
        }

        case SDL_VIDEORESIZE: {
          std::cout<<"cWindow::UpdateEvents Resize"<<std::endl;
          cResolution resolution;
          resolution.width = sdlEvent.resize.w;
          resolution.height = sdlEvent.resize.h;
          resolution.pixelFormat = GetPixelFormat();
          OnResizeWindow(resolution, IsFullScreen());

          cWindowEvent event;
          event.type = TYPE::WINDOW_RESIZE;
          if (pWindowEventListener != nullptr) pWindowEventListener->OnWindowEvent(event);
          break;
        }

        case SDL_KEYDOWN: {
          cKeyboardEvent event;
          event.type = TYPE::KEY_DOWN;
          event.keyCode = sdlEvent.key.keysym.sym;
          if (pInputEventListener != nullptr) pInputEventListener->OnKeyboardEvent(event);
          break;
        }

        case SDL_KEYUP: {
          cKeyboardEvent event;
          event.type = TYPE::KEY_UP;
          event.keyCode = sdlEvent.key.keysym.sym;
          if (pInputEventListener != nullptr) pInputEventListener->OnKeyboardEvent(event);
          break;
        }

        case SDL_MOUSEBUTTONUP: {
          cMouseEvent event;
          event.type = TYPE::MOUSE_UP;
          event.x = sdlEvent.button.x;
          event.y = sdlEvent.button.y;
          if (pInputEventListener != nullptr) pInputEventListener->OnMouseEvent(event);
          break;
        }

        case SDL_MOUSEBUTTONDOWN: {
          cMouseEvent event;
          event.type = TYPE::MOUSE_DOWN;
          event.x = sdlEvent.button.x;
          event.y = sdlEvent.button.y;
          if (pInputEventListener != nullptr) pInputEventListener->OnMouseEvent(event);
          break;
        }

        case SDL_MOUSEMOTION: {
          cMouseEvent event;
          event.type = TYPE::MOUSE_MOVE;
          event.x = sdlEvent.button.x;
          event.y = sdlEvent.button.y;
          if (pInputEventListener != nullptr) pInputEventListener->OnMouseEvent(event);
          break;
        }

        default:
          break;
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
