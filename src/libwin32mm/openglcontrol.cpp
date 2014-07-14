// Standard headers
#include <cassert>
#include <cmath>
#include <cstring>

#include <string>
#include <iostream>
#include <sstream>

#include <algorithm>
#include <map>
#include <vector>
#include <list>

// OpenGL headers
#include <GL/GLee.h>
#include <GL/glu.h>

// Spitfire headers
#include <spitfire/spitfire.h>

// libwin32mm headers
#include <libwin32mm/openglcontrol.h>

#define LIBWIN32MM_OPENGL_CONTROLCLASS "LIBWIN32MM_OPENGL_CONTROL"

namespace win32mm
{
  cKeyEvent::cKeyEvent() :
    key(0),
    bControl(false),
    bAlt(false),
    bShift(false)
  {
  }


  // ** cOpenGLControl

  cOpenGLControl::cOpenGLControl() :
    control(NULL),
    iWidth(0),
    iHeight(0),
    bMouseIn(false)
  {
  }

  cOpenGLControl::~cOpenGLControl()
  {
    std::cout<<"cOpenGLControl::~cOpenGLControl"<<std::endl;
    if (control != NULL) {
      // Clear the user data for this window
      ::RemoveProp(control, TEXT("cOpenGLControlThis"));
    }
  }

  void cOpenGLControl::RegisterClass()
  {
     // Register our window class
     WNDCLASS wc;
     ZeroMemory(&wc, sizeof(WNDCLASS));
     wc.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_OWNDC;
     wc.lpfnWndProc = _WindowProc;
     wc.cbWndExtra = sizeof(cOpenGLControl*);
     wc.hInstance = GetHInstance();
     wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
     wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
     wc.lpszClassName = TEXT(LIBWIN32MM_OPENGL_CONTROLCLASS);
     ::RegisterClass(&wc);
  }

  void cOpenGLControl::Create(cWindow& parent, int idControl)
  {
    // Register our window class
    RegisterClass();

    // Create the OpenGL control
    control = ::CreateWindowEx(0, TEXT(LIBWIN32MM_OPENGL_CONTROLCLASS), TEXT(""), WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP, 0, 0, 0, 0, parent.GetWindowHandle(), (HMENU)idControl, GetHInstance(), (LPVOID)0);

    parent.SetControlDefaultFont(control);

    UpdateSize();

    // Set our user data for this window
    ::SetProp(control, TEXT("cOpenGLControlThis"), (HANDLE)this);
  }

  HWND cOpenGLControl::GetHandle()
  {
    return control;
  }

  int cOpenGLControl::GetWidth() const
  {
    return iWidth;
  }

  int cOpenGLControl::GetHeight() const
  {
    return iHeight;
  }

  bool cOpenGLControl::IsEnabled() const
  {
    return (::IsWindowEnabled(control) == TRUE);
  }

  void cOpenGLControl::SetCapture()
  {
    ::SetCapture(control);
  }

  void cOpenGLControl::ReleaseCapture()
  {
    ::ReleaseCapture();
  }

  bool cOpenGLControl::IsCaptured() const
  {
    return (control == GetCapture());
  }

  bool cOpenGLControl::IsKeyboardFocused() const
  {
    return (control == ::GetFocus());
  }

  void cOpenGLControl::Update()
  {
    ASSERT(control != NULL);
    ::InvalidateRect(control, NULL, FALSE);
  }

  void cOpenGLControl::Update(int x, int y, int width, int height)
  {
    std::cout<<"cOpenGLControl::Update"<<std::endl;
    RECT rc;
    rc.left = x;
    rc.top = y;
    rc.right = rc.left + width;
    rc.bottom = rc.top + height;
    ::InvalidateRect(control, &rc, FALSE);
  }

  void cOpenGLControl::UpdateSize()
  {
    std::cout<<"cOpenGLControl::UpdateSize"<<std::endl;

    RECT rect;
    ::GetClientRect(control, &rect);
    iWidth = rect.right;
    iHeight = rect.bottom;
  }

  void cOpenGLControl::Paint()
  {
    std::cout<<"cOpenGLControl::Paint"<<std::endl;

    PAINTSTRUCT ps;
    ::BeginPaint(control, &ps);

    OnPaint();

    ::EndPaint(control, &ps);
  }

  LRESULT cOpenGLControl::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
     switch (uMsg) {
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
        case WM_ENABLE:
          Update();
          break;

        case WM_SIZING:
        case WM_SIZE:
          UpdateSize();
          OnSize();
          Update();
          break;

        case WM_PAINT:
          Paint();
          return FALSE;

        case WM_ERASEBKGND:
          return TRUE;

        case WM_MOUSEMOVE: {
           int x = int((short)LOWORD(lParam));
           int y = int((short)HIWORD(lParam));
           if (!bMouseIn) {
              TRACKMOUSEEVENT tme;
              tme.cbSize = sizeof(TRACKMOUSEEVENT);
              tme.dwFlags = TME_LEAVE | TME_HOVER;
              tme.hwndTrack = hwnd;
              tme.dwHoverTime = HOVER_DEFAULT;
              ::TrackMouseEvent(&tme);
              OnMouseIn();
              bMouseIn = true;
           }
           OnMouseMove(x, y);
        } break;

        case WM_MOUSELEAVE:
           if (bMouseIn) {
              OnMouseOut();
              bMouseIn = false;
           }
           break;

        case WM_MOUSEHOVER:
           if (bMouseIn) {
              OnMouseHover(LOWORD(lParam), HIWORD(lParam));

              // Reset the track mouse event
              TRACKMOUSEEVENT tme;
              tme.cbSize = sizeof(TRACKMOUSEEVENT);
              tme.dwFlags = TME_LEAVE | TME_HOVER;
              tme.hwndTrack = hwnd;
              tme.dwHoverTime = HOVER_DEFAULT;
              ::TrackMouseEvent(&tme);
           }
           break;

        case WM_LBUTTONDOWN:
           OnLButtonDown(int((short)LOWORD(lParam)), int((short)HIWORD(lParam)));
           break;

        case WM_LBUTTONUP:
           OnLButtonUp(int((short)LOWORD(lParam)), int((short)HIWORD(lParam)));
           break;

        case WM_RBUTTONDOWN:
           OnRButtonDown(int((short)LOWORD(lParam)), int((short)HIWORD(lParam)));
           break;

        case WM_RBUTTONUP:
           OnRButtonUp(int((short)LOWORD(lParam)), int((short)HIWORD(lParam)));
           break;

        case WM_MBUTTONDOWN:
           OnMButtonDown(int((short)LOWORD(lParam)), int((short)HIWORD(lParam)));
           break;

        case WM_MBUTTONUP:
           OnMButtonUp(int((short)LOWORD(lParam)), int((short)HIWORD(lParam)));
           break;

        case WM_LBUTTONDBLCLK:
           OnDoubleClick(int((short)LOWORD(lParam)), int((short)HIWORD(lParam)));
           break;

        case WM_MOUSEWHEEL:
           POINT pt;
           pt.x = LOWORD(lParam);
           pt.y = HIWORD(lParam);
           if (::ScreenToClient(hwnd, &pt) == TRUE) OnMouseWheel(pt.x, pt.y, short(HIWORD(wParam)));
           break;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
           if ((lParam & 0x40000000) == 0) {
              cKeyEvent event;
              event.key = wParam;
              event.bControl = ((::GetKeyState(VK_CONTROL) & 0x8000) != 0);
              event.bAlt = ((::GetKeyState(VK_MENU) & 0x8000) != 0);
              event.bShift = ((::GetKeyState(VK_SHIFT) & 0x8000) != 0);
              OnKeyDown(event);
           }
           break;

        case WM_KEYUP:
        case WM_SYSKEYUP:
           if ((lParam & 0x40000000) == 0) {
              cKeyEvent event;
              event.key = wParam;
              event.bControl = ((::GetKeyState(VK_CONTROL) & 0x8000) != 0);
              event.bAlt = ((::GetKeyState(VK_MENU) & 0x8000) != 0);
              event.bShift = ((::GetKeyState(VK_SHIFT) & 0x8000) != 0);
              OnKeyUp(event);
           }
           break;
     }

     return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

  LRESULT CALLBACK cOpenGLControl::_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    // Pass this to the member function WindowProc
    cOpenGLControl* pThis = (cOpenGLControl*)::GetProp(hwnd, TEXT("cOpenGLControlThis"));
    if (pThis == NULL) return DefWindowProc(hwnd, uMsg, wParam, lParam);

    return pThis->WindowProc(hwnd, uMsg, wParam, lParam);
  }
}
