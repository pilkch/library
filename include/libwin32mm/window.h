#ifndef LIBWIN32MM_WINDOW_H
#define LIBWIN32MM_WINDOW_H

// Standard headers
#include <map>

// Win32 headers
#include <windows.h>
#include <commctrl.h>

#undef IsMinimized
#undef IsMaximized

// libwin32mm headers
#include <libwin32mm/libwin32mm.h>
#include <libwin32mm/keys.h>

#ifndef CreateWindow
#define CreateWindow  CreateWindowW
#endif

namespace win32mm
{
  class cWindow;
  class cInputUpDown;
  class cMenu;
  class cPopupMenu;

  // ** cBubbleTip

  class cBubbleTip
  {
  public:
    friend class cWindow;

    cBubbleTip();

    void Show(cWindow& parent, HWND control, const string_t& sText);
    void Hide();

  protected:
    LRESULT HandleWindowMessage(cWindow& window, UINT uMsg, WPARAM wParam, LPARAM lParam);

  private:
    HWND hwndBubbleTip;

    TOOLINFO toolInfo;

    // Keep track of the old position because Windows sends the WM_MOUSEMOVE message continuously when a bubbletip is present
    int iOldX;
    int iOldY;
  };

  // ** cWindowProcHandler

  class cWindowProcHandler
  {
  public:
    // Handle the window message, return true if the message was handled
    virtual LRESULT OnWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
  };

  // ** cWindow

  class cWindow
  {
  public:
    friend class cCheckBox;
    friend class cInput;
    friend class cComboBox;

    cWindow();

    HWND GetWindowHandle() const;
    void SetWindowHandle(HWND hwndWindow);

    void InitWindowProc();
    void DestroyWindowProc();

    void AddHandler(HWND control, cWindowProcHandler& handler);
    void RemoveHandler(HWND control);

    void CloseOk() { OnClose(IDOK); }         // Close the window with an ok result
    void CloseCancel() { OnClose(IDCANCEL); } // Close the window with a cancel result
    void Close();                           // Close the window right now
    void CloseSoon();                       // Close the window as soon as possible

    int GetDPI() const;
    int ScaleByDPI(int iValue);

    void SetCaption(const string_t& sCaption);

    void SetResizable(bool bResizable);
    void SetMinimizable(bool bMinimizable);
    void SetMaximizable(bool bMaximizable);
    
    static bool IsMinimized(HWND hwndWindow);
    static bool IsMaximized(HWND hwndWindow);
    bool IsMinimized() const { return IsMinimized(hwndWindow); }
    bool IsMaximized() const { return IsMaximized(hwndWindow); }
    void Maximize();
    void Restore();

    void CenterOnScreen();
    void CenterOnParent();

    void GetSize(int& iWidth, int& iHeight) const;
    void SetSize(int iWidth, int iHeight);

    void GetClientSize(int& iWidth, int& iHeight) const;
    void SetClientSize(int iWidth, int iHeight);

    void RelayoutControls();

    void SetMenu(cMenu& menu);

    void EnableMenu();
    void DisableMenu();
    void EnableMenuItem(int idItem, bool bEnable);
    void CheckMenuItem(int idItem, bool bCheck);

    void DisplayPopupMenu(cPopupMenu& popupMenu);

    HWND GetControlHandle(int idControl) const;

    void GetControlSize(HWND control, int& iWidth, int& iHeight) const;

    bool IsControlEnabled(HWND control) const;
    void EnableControl(HWND control, bool bEnable);

    void ShowControl(HWND control, bool bShow);
    void MoveControl(HWND control, int x, int y, int width, int height);
    void MoveOkCancelHelp(int iWidth, int iHeight);
    void MoveControlInputUpDown(const cInputUpDown& control, int x, int y, int width);

    // Layout functions for laying out controls on a line
   // http://msdn.microsoft.com/en-us/library/windows/desktop/aa511279.aspx#sizingspacing
    void MoveControlStaticNextToOtherControls(HWND controlStatic, int x, int y, int width);

    int DialogUnitsToPixelsX(int iPixelsX) const;
    int DialogUnitsToPixelsY(int iPixelsY) const;

    int CharacterUnitsToPixelsX(int iPixelsX) const;
    int CharacterUnitsToPixelsY(int iPixelsY) const;

    // Get functions for getting control sizing and spacing
    // http://msdn.microsoft.com/en-us/library/windows/desktop/aa511279.aspx#sizingspacing
    int GetMarginWidth() const;
    int GetMarginHeight() const;
    int GetSpacerWidth() const;           // The space between related controls
    int GetSpacerHeight() const;          // The space between related controls
    int GetSpacerParagraphHeight() const; // The space between unrelated controls
    int GetGroupBoxControlOffsetX() const;
    int GetGroupBoxControlOffsetTop() const;
    int GetGroupBoxControlOffsetBottom() const;
    int GetStaticOffsetFromOtherControlY() const; // This is added to the y position of static text controls that are next to other controls
    int GetTextHeight() const;
    int GetInputHeight() const;
    int GetInputUpDownHeight() const;
    int GetButtonHeight() const;
    int GetRadioButtonHeight() const;
    int GetCheckBoxHeight() const;
    int GetPullDownListHeight() const;
    int GetComboBoxHeight() const;
    int GetProgressBarHeight() const;
    int GetScrollBarWidth() const;
    int GetScrollBarHeight() const;

    // Measure functions for getting control sizing and spacing
    int MeasureStaticTextWidth(HWND control) const;
    int MeasureStaticTextHeight(HWND control, int iAreaWidth) const;
    int MeasureButtonWidth(HWND control) const;
    int MeasureOkButtonWidth() const;
    int MeasureCancelButtonWidth() const;
    int MeasureHelpButtonWidth() const;
    int MeasureRadioButtonWidth(HWND control) const;
    int MeasureCheckBoxWidth(HWND control) const;
    int MeasureInputWidth(HWND control) const;
    int MeasureInputUpDownWidth(const cInputUpDown& inputUpDown) const;

    void SetDefaultFont();
    void SetControlDefaultFont(HWND control);

    void SetFocus(HWND control);

    void BubbleTipShow(HWND control, const string_t& sText);
    void BubbleTipHide();

  protected:
    virtual bool OnCommand(int idCommand) { return false; } // Allow the application handle control generated commands, return true if the command was handled

    void UpdateDPI();

    void CallOnResizing(size_t width, size_t height);
    void CallOnResize(size_t width, size_t height);

    static void GetControlText(HWND control, string_t& sText);
    static void SetControlText(HWND control, const string_t& sText);

    LRESULT HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK _WindowProc(HWND, UINT, WPARAM, LPARAM);

    HWND hwndWindow;

  private:
    virtual void OnClose(int iResult) {}

    virtual void OnResizing(size_t width, size_t height) { (void)width; (void)height; }
    virtual void OnResize(size_t width, size_t height) { (void)width; (void)height; }

    int MeasureControlTextWidth(HWND control) const;

    WNDPROC PreviousWindowProc;

    std::map<HWND, cWindowProcHandler*> handlers;

    int iDPI;

    HWND hwndThumb; // Resize control on the bottom right corner of the window

    cBubbleTip bubbleTip;
  };

  class cPopupMenu;

  class cMenu
  {
  public:
    cMenu();

    void Create();
    void AppendPopupMenu(cPopupMenu& popupMenu, const string_t& sText);

    HMENU hmenu;

  private:
    string_t MenuPrepareTextForMenu(const string_t& sText) const;
  };

  class cPopupMenu
  {
  public:
    cPopupMenu();
    ~cPopupMenu();

    void AppendMenuItem(int iCommandID, const string_t& sText);
    void AppendMenuItemWithShortcut(int iCommandID, const string_t& sText, key_t key);
    void AppendSeparator();

    void EnableMenuItem(int iCommandID, bool bEnable);

    HMENU hmenu;
  };


  // ** cWindow

  inline cWindow::cWindow() :
    hwndWindow(NULL),
    PreviousWindowProc(nullptr),
    iDPI(96),
    hwndThumb(NULL)
  {
  }

  inline HWND cWindow::GetWindowHandle() const
  {
    return hwndWindow;
  }

  inline void cWindow::SetWindowHandle(HWND _hwndWindow)
  {
    hwndWindow = _hwndWindow;
  }

  inline void cWindow::UpdateDPI()
  {
    HDC dc = ::GetDC(hwndWindow);
    iDPI = ::GetDeviceCaps(dc, LOGPIXELSX);
    ::ReleaseDC(hwndWindow, dc);
  }

  inline int cWindow::GetDPI() const
  {
    return iDPI;
  }

  inline int cWindow::ScaleByDPI(int iValue)
  {
    return ::MulDiv(iValue, iDPI, 96);
  }

  inline void cWindow::SetSize(int iWidth, int iHeight)
  {
    ::SetWindowPos(hwndWindow, HWND_TOP, 0, 0, iWidth, iHeight, SWP_NOMOVE | SWP_NOZORDER);
  }

  inline void cWindow::SetClientSize(int iWidth, int iHeight)
  {
    RECT rcClient;
    ::GetClientRect(hwndWindow, &rcClient);

    RECT rcWindow;
    ::GetWindowRect(hwndWindow, &rcWindow);

    POINT ptDiff;
    ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
    ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;

    SetSize(iWidth + ptDiff.x, iHeight + ptDiff.y);
  }

  inline void cWindow::GetClientSize(int& iWidth, int& iHeight) const
  {
    RECT rect;
    ::GetClientRect(hwndWindow, &rect);

    iWidth = rect.right - rect.left;
    iHeight = rect.bottom - rect.top;
  }

  inline void cWindow::SetMenu(cMenu& menu)
  {
    ::SetMenu(hwndWindow, menu.hmenu);
  }

  inline void cWindow::EnableMenu()
  {
    // Enable all the top level menu items
    HMENU hmenu = ::GetMenu(hwndWindow);
    const int n = ::GetMenuItemCount(hmenu);
    for (int i = 0; i < n; i++) ::EnableMenuItem(hmenu, i, MF_BYPOSITION | MF_ENABLED);

    // Update the menu
    ::DrawMenuBar(hwndWindow);
  }

  inline void cWindow::DisableMenu()
  {
    // Disable all the top level menu items
    HMENU hmenu = ::GetMenu(hwndWindow);
    const int n = ::GetMenuItemCount(hmenu);
    for (int i = 0; i < n; i++) ::EnableMenuItem(hmenu, i, MF_BYPOSITION | MF_GRAYED);

    // Update the menu
    ::DrawMenuBar(hwndWindow);
  }

  inline void cWindow::EnableMenuItem(int idItem, bool bEnable)
  {
    HMENU hmenu = ::GetMenu(hwndWindow);
    ::EnableMenuItem(hmenu, idItem, MF_BYCOMMAND | (bEnable ? MF_ENABLED : MF_GRAYED));
  }

  inline void cWindow::CheckMenuItem(int idItem, bool bCheck)
  {
    HMENU hmenu = ::GetMenu(hwndWindow);
    ::CheckMenuItem(hmenu, idItem, MF_BYCOMMAND | (bCheck ? MF_CHECKED : MF_UNCHECKED));
  }

  inline void cWindow::DisplayPopupMenu(cPopupMenu& popupMenu)
  {
    CURSORINFO ci;
    ci.cbSize = sizeof(CURSORINFO);
    ::GetCursorInfo(&ci);

    const POINT& point = ci.ptScreenPos;
    ::TrackPopupMenu(popupMenu.hmenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, 0, hwndWindow, NULL);
  }

  inline HWND cWindow::GetControlHandle(int idControl) const
  {
    return ::GetDlgItem(hwndWindow, idControl);
  }

  inline int cWindow::DialogUnitsToPixelsX(int x) const
  {
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = x;
    rect.bottom = 0;
    ::MapDialogRect(hwndWindow, &rect);
    return rect.right;
  }

  inline int cWindow::DialogUnitsToPixelsY(int y) const
  {
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = 0;
    rect.bottom = y;
    ::MapDialogRect(hwndWindow, &rect);
    return rect.bottom;
  }

  inline int cWindow::CharacterUnitsToPixelsX(int iPixelsX) const
  {
    return DialogUnitsToPixelsY(iPixelsX << 3);
  }

  inline int cWindow::CharacterUnitsToPixelsY(int iPixelsY) const
  {
    return DialogUnitsToPixelsY(iPixelsY << 2);
  }

  inline int cWindow::GetMarginWidth() const
  {
    return DialogUnitsToPixelsX(7);
  }

  inline int cWindow::GetMarginHeight() const
  {
    return DialogUnitsToPixelsX(7);
  }

  inline int cWindow::GetSpacerWidth() const
  {
    return DialogUnitsToPixelsX(4);
  }

  inline int cWindow::GetSpacerHeight() const
  {
    return DialogUnitsToPixelsY(4);
  }

  inline int cWindow::GetSpacerParagraphHeight() const
  {
    return DialogUnitsToPixelsY(7);
  }

  inline int cWindow::GetGroupBoxControlOffsetX() const
  {
    return DialogUnitsToPixelsX(6);
  }

  inline int cWindow::GetGroupBoxControlOffsetTop() const
  {
    return DialogUnitsToPixelsY(11);
  }

  inline int cWindow::GetGroupBoxControlOffsetBottom() const
  {
    return DialogUnitsToPixelsY(7);
  }

  inline int cWindow::GetStaticOffsetFromOtherControlY() const
  {
    return DialogUnitsToPixelsY(3);
  }

  inline int cWindow::GetTextHeight() const
  {
    return DialogUnitsToPixelsY(10);
  }

  inline int cWindow::GetInputHeight() const
  {
    return DialogUnitsToPixelsY(12);
  }

  inline int cWindow::GetInputUpDownHeight() const
  {
    return GetInputHeight() + 2;
  }

  inline int cWindow::GetButtonHeight() const
  {
    return DialogUnitsToPixelsY(14);
  }

  inline int cWindow::GetRadioButtonHeight() const
  {
    return DialogUnitsToPixelsY(10);
  }

  inline int cWindow::GetCheckBoxHeight() const
  {
    return DialogUnitsToPixelsY(10);
  }

  inline int cWindow::GetPullDownListHeight() const
  {
    return DialogUnitsToPixelsY(14);
  }

  inline int cWindow::GetComboBoxHeight() const
  {
    return DialogUnitsToPixelsY(14);
  }

  inline int cWindow::GetProgressBarHeight() const
  {
    return DialogUnitsToPixelsY(8);
  }

  inline int cWindow::GetScrollBarWidth() const
  {
    return ::GetSystemMetrics(SM_CXVSCROLL);
  }

  inline int cWindow::GetScrollBarHeight() const
  {
    return ::GetSystemMetrics(SM_CYHSCROLL);
  }

  inline int cWindow::MeasureStaticTextWidth(HWND control) const
  {
    string_t sText;
    GetControlText(control, sText);
    if (sText.empty()) return 0;

    RECT rc;
    rc.left = 0;
    rc.right = 32768;
    rc.top = 0;
    rc.bottom = 32768;

    HDC hdc = GetDC(hwndWindow);
    HFONT hPrevFont = HFONT(SelectObject(hdc, HFONT(SendMessage(control, WM_GETFONT, 0, 0))));
    ::DrawText(hdc, sText.c_str(), -1, &rc, DT_CALCRECT | DT_NOPREFIX);
    SelectObject(hdc, hPrevFont);
    ReleaseDC(hwndWindow, hdc);
    return rc.right;
  }

  inline int cWindow::MeasureStaticTextHeight(HWND control, int iAreaWidth) const
  {
    string_t sText;
    GetControlText(control, sText);
    if (sText.empty()) return 0;

    RECT rc;
    rc.left = 0;
    rc.right = iAreaWidth;
    rc.top = 0;
    rc.bottom = 32768;

    HDC hdc = GetDC(hwndWindow);
    HFONT hPrevFont = HFONT(SelectObject(hdc, HFONT(SendMessage(control, WM_GETFONT, 0, 0))));
    int i = ::DrawText(hdc, sText.c_str(), -1, &rc, DT_CALCRECT | DT_NOPREFIX | DT_WORDBREAK);
    SelectObject(hdc, hPrevFont);
    ReleaseDC(hwndWindow, hdc);
    return i;
  }

  inline int cWindow::MeasureButtonWidth(HWND control) const
  {
    string_t sText;
    GetControlText(control, sText);
    if (sText.empty()) return 0;

    RECT rc;
    rc.left = 0;
    rc.right = 32768;
    rc.top = 0;
    rc.bottom = 32768;

    HDC hdc = GetDC(hwndWindow);
    HFONT hPrevFont = HFONT(SelectObject(hdc, HFONT(::SendMessage(control, WM_GETFONT, 0, 0))));
    ::DrawText(hdc, sText.c_str(), -1, &rc, DT_CALCRECT | DT_NOPREFIX);
    ::SelectObject(hdc, hPrevFont);
    ::ReleaseDC(hwndWindow, hdc);
    return rc.right + CharacterUnitsToPixelsX(2);
  }

  inline int cWindow::MeasureOkButtonWidth() const
  {
    // Ok, Cancel and Help buttons must always be 50 dialog units wide
    // http://msdn.microsoft.com/en-us/library/windows/desktop/aa511453.aspx#sizing
    return DialogUnitsToPixelsX(50);
  }

  inline int cWindow::MeasureCancelButtonWidth() const
  {
    // Ok, Cancel and Help buttons must always be 50 dialog units wide
    // http://msdn.microsoft.com/en-us/library/windows/desktop/aa511453.aspx#sizing
    return DialogUnitsToPixelsX(50);
  }

  inline int cWindow::MeasureHelpButtonWidth() const
  {
    // Ok, Cancel and Help buttons must always be 50 dialog units wide
    // http://msdn.microsoft.com/en-us/library/windows/desktop/aa511453.aspx#sizing
    return DialogUnitsToPixelsX(50);
  }

  inline int cWindow::MeasureRadioButtonWidth(HWND control) const
  {
    return 99;
  }

  inline int cWindow::MeasureCheckBoxWidth(HWND control) const
  {
    return 99;
  }

  inline int cWindow::MeasureInputWidth(HWND control) const
  {
    return 99;
  }

  inline int cWindow::MeasureInputUpDownWidth(const cInputUpDown& inputUpDown) const
  {
    return 99;
  }


  // ** cMenu

  inline cMenu::cMenu() :
    hmenu(NULL)
  {
  }

  inline void cMenu::Create()
  {
    hmenu = ::CreateMenu();
  }

  inline void cMenu::AppendPopupMenu(cPopupMenu& popupMenu, const string_t& sText)
  {
    ::AppendMenu(hmenu, MF_STRING | MF_POPUP, (UINT)popupMenu.hmenu, MenuPrepareTextForMenu(sText).c_str());
  }

  inline string_t cMenu::MenuPrepareTextForMenu(const string_t& sText) const
  {
    // If it is one of these known menu items then we can use an ampersand to provide a keyboard accelerator
    if (
      (sText == TEXT("File")) ||
      (sText == TEXT("Edit"))
    ) {
      return TEXT("&") + sText;
    }

    return sText;
  }


  // ** cBubbleTip

  inline cBubbleTip::cBubbleTip() :
    hwndBubbleTip(NULL),
    iOldX(0),
    iOldY(0)
  {
  }

  inline void cBubbleTip::Hide()
  {
    ::DestroyWindow(hwndBubbleTip);
    hwndBubbleTip = NULL;
  }


  // ** cPopupMenu

  inline cPopupMenu::cPopupMenu() :
    hmenu(NULL)
  {
    hmenu = ::CreatePopupMenu();
  }

  inline cPopupMenu::~cPopupMenu()
  {
    ::DestroyMenu(hmenu);
  }

  inline void cPopupMenu::AppendMenuItem(int iCommandID, const string_t& sText)
  {
    ::AppendMenu(hmenu, MF_STRING, iCommandID, sText.c_str());
  }

  inline void cPopupMenu::AppendMenuItemWithShortcut(int iCommandID, const string_t& sText, key_t key)
  {
    ASSERT(key != KEY_INVALID);

    // Add the shortcut key to the menu item text
    const string_t sTextWithShortcut = sText + TEXT("\t") + KeyGetDescription(key);
    AppendMenuItem(iCommandID, sTextWithShortcut);
  }

  inline void cPopupMenu::AppendSeparator()
  {
    ::AppendMenu(hmenu, MF_SEPARATOR, UINT_PTR(-1), NULL);
  }

  inline void cPopupMenu::EnableMenuItem(int iCommandID, bool bEnable)
  {
    ::EnableMenuItem(hmenu, iCommandID, MF_BYCOMMAND | (bEnable ? MF_ENABLED : MF_GRAYED));
  }
}

#endif // LIBWIN32MM_WINDOW_H
