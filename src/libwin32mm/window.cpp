// Windows headers
#include <windowsx.h>

// libwin32mm headers
#include <libwin32mm/controls.h>
#include <libwin32mm/window.h>

namespace win32mm
{
  void cWindow::InitWindowProc()
  {
    // Set our user data for this window
    ::SetProp(hwndWindow, TEXT("cWindowThis"), (HANDLE)this);

    // Handle the messages for this window
    PreviousWindowProc = (WNDPROC)::SetWindowLongPtr(hwndWindow, GWLP_WNDPROC, (LONG_PTR)_WindowProc);
  }

  void cWindow::DestroyWindowProc()
  {
    // Restore the previous window proc for this window
    ::SetWindowLongPtr(hwndWindow, GWLP_WNDPROC, (LONG_PTR)PreviousWindowProc);

    // Clear the user data for this window
    ::RemoveProp(hwndWindow, TEXT("cWindowThis"));
  }

  void cWindow::AddHandler(HWND control, cWindowProcHandler& handler)
  {
    handlers[control] = &handler;
  }

  void cWindow::RemoveHandler(HWND control)
  {
    std::map<HWND, cWindowProcHandler*>::iterator iter = handlers.find(control);
    if (iter != handlers.end()) handlers.erase(iter);
  }

  void cWindow::Close()
  {
    CloseCancel();
  }

  void cWindow::CloseSoon()
  {
    ::PostMessage(hwndWindow, WM_COMMAND, IDCANCEL, 0);
  }

  void cWindow::SetCaption(const string_t& sCaption)
  {
    ::SetWindowText(hwndWindow, sCaption.c_str());
  }

  void cWindow::SetResizable(bool bResizable)
  {
    LONG iStyle = ::GetWindowLong(hwndWindow, GWL_STYLE);
    if (bResizable) {
      iStyle |= (WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX);
    } else {
      iStyle &= ~(WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX);
    }
    ::SetWindowLong(hwndWindow, GWL_STYLE, iStyle);

    hwndThumb = ::CreateWindow(TEXT("scrollbar"), TEXT(""), WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP, 0, 0, 0, 0, hwndWindow, (HMENU)-1, NULL, 0);
  }

  void cWindow::SetMinimizable(bool bMinimizable)
  {
    LONG iStyle = ::GetWindowLong(hwndWindow, GWL_STYLE);
    if (bMinimizable) {
      iStyle |= WS_MINIMIZEBOX;
    } else {
      iStyle &= ~WS_MINIMIZEBOX;
    }
    ::SetWindowLong(hwndWindow, GWL_STYLE, iStyle);
  }

  void cWindow::SetMaximizable(bool bMaximizable)
  {
    LONG iStyle = ::GetWindowLong(hwndWindow, GWL_STYLE);
    if (bMaximizable) {
      iStyle |= WS_MAXIMIZEBOX;
    } else {
      iStyle &= ~WS_MAXIMIZEBOX;
    }
    ::SetWindowLong(hwndWindow, GWL_STYLE, iStyle);
  }

  bool cWindow::IsMinimized(HWND hwndWindow)
  {
    return (::IsIconic(hwndWindow) != 0);
  }

  bool cWindow::IsMaximized(HWND hwndWindow)
  {
    return (::IsZoomed(hwndWindow) != 0);
  }

  void cWindow::Maximize()
  {
    ::ShowWindow(hwndWindow, SW_MAXIMIZE);
  }

  void cWindow::Restore()
  {
    ::ShowWindow(hwndWindow, SW_RESTORE);
  }

  void cWindow::CenterOnScreen()
  {
    RECT rWindow;
    ::GetWindowRect(hwndWindow, &rWindow);

    RECT rDesktop;
    ::GetWindowRect(::GetDesktopWindow(), &rDesktop);

    const int iWidth = rWindow.right - rWindow.left;
    const int iHeight = rWindow.bottom - rWindow.top;

    int PixelsX = (rDesktop.right - iWidth) / 2;
    if (PixelsX < 0) PixelsX = 10;
    int PixelsY = (rDesktop.bottom - iHeight) / 2;
    if (PixelsY < 0) PixelsY = 10;

    ::MoveWindow(hwndWindow, PixelsX, PixelsY, iWidth, iHeight, TRUE);
  }

  void cWindow::CenterOnParent()
  {
    // Get the parent handle
    const HWND hwndParent = ::GetParent(hwndWindow);

    // If the window is minimized then we can just center it on the screen
    if ((hwndParent == NULL) || IsMinimized(hwndParent)) {
      CenterOnScreen();
      return;
    }

    // Get parent rect
    RECT rcParent;
    ::GetWindowRect(hwndParent, &rcParent);

    // Get the center point of the parent window
    const int iParentCenterX = (rcParent.left + rcParent.right) / 2;
    const int iParentCenterY = (rcParent.top + rcParent.bottom) / 2;

    // Get the size of the parent window
    RECT rcWindow;
    ::GetWindowRect(hwndWindow, &rcWindow);
    const int iWidth = rcWindow.right - rcWindow.left;
    const int iHeight = rcWindow.bottom - rcWindow.top;

    // Position the child around the center of the parent
    int x = iParentCenterX - (iWidth / 2);
    int y = iParentCenterY - (iHeight / 2);

    // Check the child window top appears on screen
    if (y < 0) y = 0;

    // Set the new window position
    ::SetWindowPos(GetWindowHandle(), HWND_TOP, x, y, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE);
  }

  void cWindow::CallOnResize(int iWidth, int iHeight)
  {
    if (IsMaximized()) {
      ::ShowWindow(hwndThumb, SW_HIDE);  // hide the resize thumb
    } else {
      // show the resize thumb in the bottom right corner
      ::ShowWindow(hwndThumb, SW_SHOW);
      const int iThumbWidth = GetScrollBarWidth();
      const int iThumbHeight = GetScrollBarHeight();
      ::SetWindowPos(hwndThumb, NULL, iWidth - iThumbWidth, iHeight - iThumbHeight, iThumbWidth, iThumbHeight, SWP_NOCOPYBITS | SWP_NOZORDER);
    }

    OnResize(iWidth, iHeight);
  }

  void cWindow::RelayoutControls()
  {
     if (!IsMinimized()) {
       RECT rect;
       ::GetClientRect(hwndWindow, &rect);
       CallOnResize(rect.right, rect.bottom);
     }
  }

  bool cWindow::IsControlEnabled(HWND control) const
  {
    ASSERT(control != NULL);
    return (::IsWindowEnabled(control) == TRUE);
  }

  void cWindow::EnableControl(HWND control, bool bEnable)
  {
    ASSERT(control != NULL);
    ::EnableWindow(control, (bEnable ? TRUE : FALSE));
  }

  void cWindow::ShowControl(HWND control, bool bShow)
  {
    ASSERT(control != NULL);
    ::ShowWindow(control, SW_HIDE);
  }

  void cWindow::GetControlSize(HWND control, int& iWidth, int& iHeight) const
  {
    RECT rect;
    ::GetWindowRect(control, &rect);

    iWidth = rect.right - rect.left;
    iHeight = rect.bottom - rect.top;
  }

  void cWindow::MoveControl(HWND control, int x, int y, int width, int height)
  {
    ::SetWindowPos(control, NULL, x, y, width, height, SWP_NOCOPYBITS | SWP_NOZORDER);
  }

  void cWindow::MoveOkCancelHelp(int iWidth, int iHeight)
  {
    int iX = iWidth - GetMarginWidth();
    const int iY = iHeight - (GetMarginHeight() + GetButtonHeight());

    const int iButtonWidth = DialogUnitsToPixelsX(50);

    HWND controlHelp = GetControlHandle(IDHELP);
    if (controlHelp != NULL) {
      iX -= iButtonWidth;
      MoveControl(controlHelp, iX, iY, iButtonWidth, GetButtonHeight());
      iX -= GetSpacerWidth();
    }

    HWND controlCancel = GetControlHandle(IDCANCEL);
    if (controlCancel != NULL) {
      iX -= iButtonWidth;
      MoveControl(controlCancel, iX, iY, iButtonWidth, GetButtonHeight());
      iX -= GetSpacerWidth();
    }

    HWND controlOk = GetControlHandle(IDOK);
    if (controlOk != NULL) {
      iX -= iButtonWidth;
      MoveControl(controlOk, iX, iY, iButtonWidth, GetButtonHeight());
      iX -= GetSpacerWidth();
    }
  }

  void cWindow::MoveControlStaticNextToOtherControls(HWND controlStatic, int x, int y, int width)
  {
    MoveControl(controlStatic, x, y + GetStaticOffsetFromOtherControlY(), width, GetTextHeight());
  }

  void cWindow::MoveControlInputUpDown(const cInputUpDown& control, int x, int y, int width)
  {
    const int iUpDownWidth = GetScrollBarWidth();
    const int iInputWidth = width - iUpDownWidth;
    const int iHeight = GetInputUpDownHeight();
    MoveControl(control.GetHandleInput(), x, y, iInputWidth, iHeight);
    x += iInputWidth;
    MoveControl(control.GetHandleUpDown(), x, y, iUpDownWidth, iHeight);
  }

  void cWindow::GetControlText(HWND control, string_t& sText)
  {
    sText.clear();

    int iLength = ::SendMessage(control, WM_GETTEXTLENGTH, 0, 0);
    if (iLength > 0) {
      std::vector<WCHAR> buf(iLength + 1);
      ::SendMessage(control, WM_GETTEXT, buf.size(), LPARAM(static_cast<LPCWSTR>(&buf[0])));
      sText = &buf[0];
    }
  }

  void cWindow::SetControlText(HWND control, const string_t& sText)
  {
    ::SendMessage(control, WM_SETTEXT, 0, LPARAM(sText.c_str()));
  }

  void cWindow::SetDefaultFont()
  {
    // Get the system message box font
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(ncm);

    ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
    HFONT hDlgFont = ::CreateFontIndirect(&(ncm.lfMessageFont));

    // Set the dialog to use the system message box font
    ::SendMessage(hwndWindow, WM_SETFONT, (WPARAM)hDlgFont, MAKELPARAM(FALSE, 0));
  }

  void cWindow::SetControlDefaultFont(HWND control)
  {
    // Get the system message box font
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(ncm);

    ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
    HFONT hDlgFont = ::CreateFontIndirect(&(ncm.lfMessageFont));

    // Set the dialog to use the system message box font
    ::SendMessage(control, WM_SETFONT, (WPARAM)hDlgFont, MAKELPARAM(FALSE, 0));
  }

  void cWindow::SetFocus(HWND control)
  {
    ::PostMessage(hwndWindow, WM_NEXTDLGCTL, (WPARAM)control, TRUE);
  }

  void cWindow::BubbleTipShow(HWND control, const string_t& sText)
  {
    // Set the focus to this control
    SetFocus(control);

    // Show a bubble tip for this control
    bubbleTip.Show(*this, control, sText);
  }

  void cWindow::BubbleTipHide()
  {
    bubbleTip.Hide();
  }

  LRESULT APIENTRY cWindow::HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    // Check if the bubble tip wants to handle this message
    LRESULT result = bubbleTip.HandleWindowMessage(*this, uMsg, wParam, lParam);
    if (result != FALSE) return result;

    // Check if any handlers want to handle this message
    if (lParam != 0) {
      HWND control = HWND(lParam);

      // WM_NOTIFY messages have an actual structure in lParam
      if (uMsg == WM_NOTIFY) control = ((LPNMHDR)lParam)->hwndFrom;

      std::map<HWND, cWindowProcHandler*>::iterator iter = handlers.find(control);
      if (iter != handlers.end()) return iter->second->OnWindowProc(uMsg, wParam, lParam);
    }

    return FALSE;
  }

  LRESULT APIENTRY cWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    // Check if anything wants to handle this message
    LRESULT result = HandleMessage(hwnd, uMsg, wParam, lParam);
    if (result != FALSE) return result;

    switch (uMsg) {
      case WM_COMMAND: {
        OnCommand(LOWORD(wParam));
        break;
      }
    }

    // Let the previous window proc handle the message
    if (PreviousWindowProc != nullptr) {
      //if (uMsg != WM_CREATE) return ::CallWindowProc(PreviousWindowProc, hwnd, uMsg, wParam, lParam);
    }

    // Let the default window proc handle the message
    return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

  LRESULT APIENTRY cWindow::_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    cWindow* pThis = (cWindow*)::GetProp(hwnd, TEXT("cWindowThis"));
    ASSERT(pThis != nullptr);
    return pThis->WindowProc(hwnd, uMsg, wParam, lParam);
  }


  // ** cBubbleTip

  bool g_TrackingMouse = false;

  void cBubbleTip::Show(cWindow& parent, HWND control, const string_t& sText)
  {
    // Hide and destroy any current bubble tip
    Hide();

    if (hwndBubbleTip == NULL) {
      // Create the bubble tip
      hwndBubbleTip = ::CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, TTS_BALLOON | TTS_CLOSE | TTS_USEVISUALSTYLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parent.GetWindowHandle(), NULL, GetHInstance(), NULL);
    }

    // Set the maximum bubble tip width
    ::SendMessage(hwndBubbleTip, TTM_SETMAXTIPWIDTH, 0, 300);

    ZeroMemory(&toolInfo, sizeof(toolInfo));
    toolInfo.cbSize = sizeof(toolInfo);
    toolInfo.uFlags = TTF_TRANSPARENT | TTF_CENTERTIP | TTF_IDISHWND | TTF_PARSELINKS;
    toolInfo.hwnd = control;
    toolInfo.uId = UINT_PTR(control);
    toolInfo.hinst = GetHInstance();
    toolInfo.lpszText = LPWSTR(sText.c_str());

    int iIcon = 1;
    const string_t sCaption = TEXT("Caption");
    if (!sCaption.empty()) ::SendMessage(hwndBubbleTip, TTM_SETTITLE, (WPARAM)(int)iIcon, (LPARAM)(LPCTSTR)sCaption.c_str());

    // Get the control position
    ::GetClientRect(control, &toolInfo.rect);

    // Add the bubble tip
    ::SendMessage(hwndBubbleTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

    g_TrackingMouse = true;

    // Activate the tooltip.
    ::SendMessage(hwndBubbleTip, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&toolInfo);

    //POINT pt = { toolInfo.rect.left, toolInfo.rect.top }; 
    //::ClientToScreen(control, &pt);
    //::SendMessage(hwndBubbleTip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(pt.x + 10, pt.y - 20));
  }

  LRESULT cBubbleTip::HandleWindowMessage(cWindow& window, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    return FALSE;

    switch (uMsg) {
      case WM_MOUSELEAVE: {
        // The mouse pointer has left our window. Deactivate the tooltip
        ::SendMessage(hwndBubbleTip, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&toolInfo);
        g_TrackingMouse = false;
        return FALSE;
      }
      case WM_MOUSEMOVE: {
        if (!g_TrackingMouse) {
          // The mouse has just entered the window
          // Request notification when the mouse leaves
          TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
          tme.hwndTrack = hwndBubbleTip;
          tme.dwFlags = TME_LEAVE;

          ::TrackMouseEvent(&tme);

          // Activate the tooltip.
          ::SendMessage(hwndBubbleTip, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&toolInfo);

          g_TrackingMouse = true;
        }

        const int newX = GET_X_LPARAM(lParam);
        const int newY = GET_Y_LPARAM(lParam);

        // Make sure the mouse has actually moved. The presence of the tooltip 
        // causes Windows to send the message continuously
        if ((newX != iOldX) || (newY != iOldY)) {
          iOldX = newX;
          iOldY = newY;

          // Update the text
          WCHAR coords[30];
          swprintf_s(coords, ARRAYSIZE(coords), L"%d, %d", newX, newY);

          toolInfo.lpszText = coords;
          ::SendMessage(hwndBubbleTip, TTM_SETTOOLINFO, 0, (LPARAM)&toolInfo);

          // Position the tooltip. The coordinates are adjusted so that the tooltip does not overlap the mouse pointer.
          POINT pt = { newX, newY }; 
          ::ClientToScreen(window.GetWindowHandle(), &pt);
          ::SendMessage(hwndBubbleTip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(pt.x + 10, pt.y - 20));
        }

        return FALSE;
      }
    }

    return FALSE;
  }
}
