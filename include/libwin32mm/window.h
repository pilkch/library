#ifndef LIBWIN32MM_WINDOW_H
#define LIBWIN32MM_WINDOW_H

// Win32 headers
#include <windows.h>
#include <commctrl.h>

// libwin32mm headers
#include <libwin32mm/libwin32mm.h>
#include <libwin32mm/keys.h>

namespace win32mm
{
  const int ID_STATUS_BAR = 10101;

  class cMenu;
  class cPopupMenu;
  class cStatusBar;

  class cWindow
  {
  public:
    cWindow();

    HWND GetWindowHandle() const;
    void SetWindowHandle(HWND hwndWindow);

    void SetMenu(cMenu& menu);

    void EnableMenu();
    void DisableMenu();
    void EnableMenuItem(int idItem, bool bEnable);
    void CheckMenuItem(int idItem, bool bCheck);

    void CreateStatusBar(cStatusBar& statusBar);

    void DisplayPopupMenu(cPopupMenu& popupMenu);

  private:
    HWND hwndWindow;
  };

  class cPopupMenu;

  class cMenu
  {
  public:
    cMenu();

    void CreateMenu();
    void AppendPopupMenu(cPopupMenu& popupMenu, const string_t& sText);

    HMENU hmenu;

  private:
    string_t MenuPrepareTextForMenu(const string_t& sText) const;
  };

  class cStatusBar
  {
  public:
    cStatusBar();

    void SetWidths(const int* widths, size_t nWidths);
    void SetText(size_t segment, const string_t& sText);

    void Resize();

    HWND hStatusBar;
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
    hwndWindow(NULL)
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

  inline void cWindow::CreateStatusBar(cStatusBar& statusBar)
  {
    statusBar.hStatusBar = ::CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, hwndWindow, (HMENU)ID_STATUS_BAR, GetModuleHandle(NULL), NULL);
  }

  inline void cWindow::DisplayPopupMenu(cPopupMenu& popupMenu)
  {
    CURSORINFO ci;
    ci.cbSize = sizeof(CURSORINFO);
    ::GetCursorInfo(&ci);

    const POINT& point = ci.ptScreenPos;
    ::TrackPopupMenu(popupMenu.hmenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, 0, hwndWindow, NULL);
  }


  // ** cMenu

  inline cMenu::cMenu() :
    hmenu(NULL)
  {
  }

  inline void cMenu::CreateMenu()
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


  // ** cStatusBar

  inline cStatusBar::cStatusBar() :
    hStatusBar(NULL)
  {
  }

  inline void cStatusBar::SetWidths(const int* widths, size_t nWidths)
  {
    ::SendMessage(hStatusBar, SB_SETPARTS, (WPARAM)nWidths, (LPARAM)widths);
  }

  inline void cStatusBar::SetText(size_t segment, const string_t& sText)
  {
    ::SendMessage(hStatusBar, SB_SETTEXT, (WPARAM)(INT)segment, (LPARAM)(LPSTR)sText.c_str());
  }

  inline void cStatusBar::Resize()
  {
    // Send a dummy resize event to update the statusbar position
    ::SendMessage(hStatusBar, WM_SIZE, 0, 0);
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
