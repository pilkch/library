#ifndef LIBWIN32MM_MAINDIALOG_H
#define LIBWIN32MM_MAINDIALOG_H

#include <libwin32mm/dialog.h>

namespace win32mm
{
  // ** cMainDialog

  class cMainDialog : public cDialog
  {
  public:
    friend class cDialog;

    cMainDialog();

    int Run();

  private:
    virtual void OnDialogCreated() override;

    virtual void OnInit() {}
    virtual void OnInitFinished() {}
    virtual void OnDestroy() {}

    virtual void OnResizing(size_t width, size_t height) { (void)width; (void)height; }
    virtual void OnResize(size_t width, size_t height) { (void)width; (void)height; }

    virtual bool OnQuit() { return true; } // Allow the application to veto quitting the application, return true if the application should quit

    LRESULT MainDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK _MainDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    int iMinWidthPixels;
    int iMinHeightPixels;
  };
}

#endif // LIBWIN32MM_MAINDIALOG_H
