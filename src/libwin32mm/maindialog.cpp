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

  bool cMainDialog::Create()
  {
    const char_t* szWindowClass = TEXT("libwin32mm_window");

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = _MainDialogProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = GetHInstance();
    wcex.hIcon          = LoadIcon(GetHInstance(), MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if (!::RegisterClassEx(&wcex))
    {
      ::MessageBox(NULL, TEXT("Call to RegisterClassEx failed!"), TEXT("Win32 Guided Tour"), NULL);

      return false;
    }
  
    const char_t* szCaption = TEXT("My Application");

    // The parameters to CreateWindow explained:
    // szWindowClass: the name of the application
    // szTitle: the text that appears in the title bar
    // WS_OVERLAPPEDWINDOW: the type of window to create
    // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
    // 500, 100: initial size (width, length)
    // NULL: the parent of this window
    // NULL: this application does not have a menu bar
    // hInstance: the first parameter from WinMain
    // NULL: not used in this application
    /*hwndWindow = ::CreateWindow(
        szWindowClass,
        szCaption,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500, 100,
        NULL,
        NULL,
        GetHInstance(),
        NULL
    );*/
  
 

    // Initialize the structure members (not shown).
    cMainDialog* pThis = this;
    hwndWindow = ::CreateWindowEx(
      WS_EX_APPWINDOW | WS_EX_WINDOWEDGE | WS_EX_COMPOSITED,                              // Optional window styles.
      szWindowClass,
      szCaption,
      WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,            // Window style

      // Size and position
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

      NULL,       // Parent window
      NULL,       // Menu
      GetHInstance(),
      pThis
    );
    if (hwndWindow == NULL) {
        ::MessageBox(NULL,
            TEXT("Call to CreateWindow failed!"),
            TEXT("Win32 Guided Tour"),
            NULL);

        return false;
    }

    // Set our default font
    SetDefaultFont();

    ::ShowWindow(hwndWindow, SW_NORMAL);
    ::UpdateWindow(hwndWindow);

    return true;
  }

  int cMainDialog::Run()
  {
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
      case WM_CREATE: {
        OnInit();

        // Push a message onto the back of the queue
        ::PostMessage(hwndWindow, WIN32MM_WM_USER_INIT_FINISHED, 0, 0);

        break;
      }

      case WIN32MM_WM_USER_INIT_FINISHED: {
        OnInitFinished();
        break;
      }

      case WM_DESTROY: {
        // Notify the application
        OnDestroy();

        // Quit the application
        ::PostQuitMessage(0);

        return 0;
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
        if (wParam != SIZE_MINIMIZED) OnResize(LOWORD(lParam), HIWORD(lParam));

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

        if (wParam != SIZE_MINIMIZED) OnResizing(pRect->right - pRect->left, pRect->top - pRect->bottom);

        break;
      }
    }

    //return cDialog::DialogProc(hwnd, uMsg, wParam, lParam);
    return cWindow::WindowProc(hwnd, uMsg, wParam, lParam);
  }

  LRESULT CALLBACK cMainDialog::_MainDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    if (uMsg == WM_CREATE) {
      CREATESTRUCT* pCreateStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
      cMainDialog* pThis = static_cast<cMainDialog*>(pCreateStruct->lpCreateParams);

      // Set our window handle
      pThis->hwndWindow = hwnd;

      // Set the window property so that we can get it in the future
      ::SetProp(hwnd, TEXT("cMainDialog"), (HANDLE)pThis);
    }

    cMainDialog* pThis = static_cast<cMainDialog*>(::GetProp(hwnd, TEXT("cMainDialog")));
    if (pThis != nullptr) return pThis->MainDialogProc(hwnd, uMsg, wParam, lParam);

    return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
}
