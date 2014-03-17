#ifndef LIBWIN32MM_WINDOW_H
#define LIBWIN32MM_WINDOW_H

// Win32 headers
#include <windows.h>

// libwin32mm headers
#include <libwin32mm/libwin32mm.h>

namespace win32mm
{
  class cMenu;

  class cWindow
  {
  public:
    explicit cWindow(HWND hwndWindow);

    void SetMenu(cMenu& menu);

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

    void CreatePopupMenu();
    void AppendPopupMenuItem(int iCommandID, const string_t& sText);

    HMENU hmenu;
  };


  // ** cWindow

  inline cWindow::cWindow(HWND _hwndWindow) :
    hwndWindow(_hwndWindow)
  {
  }

  inline void cWindow::SetMenu(cMenu& menu)
  {
    ::SetMenu(hwndWindow, menu.hmenu);
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
  }

  inline void cPopupMenu::CreatePopupMenu()
  {
    hmenu = ::CreatePopupMenu();
  }

  inline void cPopupMenu::AppendPopupMenuItem(int iCommandID, const string_t& sText)
  {
    ::AppendMenu(hmenu, MF_STRING, iCommandID, sText.c_str());
  }
}

#endif // LIBWIN32MM_WINDOW_H
