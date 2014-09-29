// Windows headers
#include <windows.h>

// libwin32mm headers
#include <libwin32mm/maindialog.h>

#define WIN32MM_WM_USER_INIT_FINISHED (WM_APP + 684)

namespace win32mm
{
  #define WM_LIBWIN32MM_DESTROY_WINDOW (WM_USER + 1)
  
  cMainDialog::cMainDialog() :
    iMinWidthPixels(400),
    iMinHeightPixels(400)
  {
  }

  void cMainDialog::OnDialogCreated()
  {
    // Set the window property so that we can get it again in the future
    cMainDialog* pThis = this;
    ::SetProp(hwndWindow, TEXT("cMainDialog"), (HANDLE)pThis);
  }

  int cMainDialog::Run()
  {
    cWindow dummyWindow;
    RunResizable(dummyWindow, TEXT(BUILD_APPLICATION_NAME), 300, 300);

    // Main message loop
    MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0)) {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
    }

    return (int)msg.wParam;
  }

  LRESULT cMainDialog::MainDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    switch(uMsg) {
      case WM_DESTROY: {
        // Notify the application
        OnDestroy();

        hwndWindow = NULL;

        // Quit the application
        ::PostQuitMessage(0);

        return FALSE;
      }

      case WM_CLOSE: {
        // Allow the application to veto the quit message
        if (OnQuit()) {
          // Tell our window to destroy itself
          // We use PostMessage so that all other events are processed first
          ::PostMessage(hwndWindow, WM_LIBWIN32MM_DESTROY_WINDOW, 0, 1);
        }
        return TRUE;
      }

      case WM_LIBWIN32MM_DESTROY_WINDOW: {
        // Destroy this window which exits the application
        ::DestroyWindow(hwndWindow);
        return TRUE;
      }

      case WM_SIZE: {
        if (wParam != SIZE_MINIMIZED) CallOnResize(LOWORD(lParam), HIWORD(lParam));

        break;
      }

      case WM_SIZING: {
        LPRECT pRect = (LPRECT)lParam;
        if ((pRect->right - pRect->left) < iMinWidthPixels) {
          if ((wParam == WMSZ_TOPLEFT) || (wParam == WMSZ_BOTTOMLEFT) || (wParam == WMSZ_LEFT)) pRect->left = pRect->right - iMinWidthPixels; 
          else pRect->right = pRect->left + iMinWidthPixels;
        }
        if ((pRect->bottom - pRect->top) < iMinHeightPixels) {
          if ((wParam == WMSZ_TOPLEFT) || (wParam == WMSZ_TOPRIGHT) || (wParam == WMSZ_TOP)) pRect->top = pRect->bottom - iMinHeightPixels;
          else pRect->bottom = pRect->top + iMinHeightPixels;
        }

        if (wParam != SIZE_MINIMIZED) CallOnResizing(pRect->right - pRect->left, pRect->top - pRect->bottom);

        break;
      }
    }

    return cDialog::DialogProc(hwnd, uMsg, wParam, lParam);
  }

  LRESULT CALLBACK cMainDialog::_MainDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    if (uMsg == WM_CREATE) {
      CREATESTRUCT* pCreateStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
      cMainDialog* pThis = static_cast<cMainDialog*>(pCreateStruct->lpCreateParams);

      // Set our window handle
      pThis->hwndWindow = hwnd;

      // Set the window property so that we can get it again in the future
      ::SetProp(hwnd, TEXT("cMainDialog"), (HANDLE)pThis);
    }

    cMainDialog* pThis = static_cast<cMainDialog*>(::GetProp(hwnd, TEXT("cMainDialog")));
    if (pThis != nullptr) return pThis->MainDialogProc(hwnd, uMsg, wParam, lParam);

    return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
}
