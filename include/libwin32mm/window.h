#ifndef LIBWIN32MM_WINDOW_H
#define LIBWIN32MM_WINDOW_H

// Win32 headers
#include <windows.h>

// libwin32mm headers
#include <libwin32mm/libwin32mm.h>
#include <libwin32mm/keys.h>

namespace win32mm
{
  class cMenu;
  class cPopupMenu;

  class cWindow
  {
  public:
    explicit cWindow(HWND hwndWindow);

    HWND GetWindowHandle() const;

    void SetMenu(cMenu& menu);

    void DisplayPopupMenu(cPopupMenu& popupMenu);

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

  inline cWindow::cWindow(HWND _hwndWindow) :
    hwndWindow(_hwndWindow)
  {
  }

  inline HWND cWindow::GetWindowHandle() const
  {
    return hwndWindow;
  }

  inline void cWindow::SetMenu(cMenu& menu)
  {
    ::SetMenu(hwndWindow, menu.hmenu);
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
