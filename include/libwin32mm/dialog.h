#ifndef LIBWIN32MM_DIALOG_H
#define LIBWIN32MM_DIALOG_H

// libwin32mm headers
#include <libwin32mm/libwin32mm.h>
#include <libwin32mm/window.h>

namespace win32mm
{
  class cHGLOBAL;

  class cDialog : public cWindow
  {
  public:
    bool RunNonResizable(cWindow& parent, const string_t& sCaption);
    bool RunNonResizable(cWindow& parent, const string_t& sCaption, int iWidthDialogUnits, int iHeightDialogUnits);
    bool RunResizable(cWindow& parent, const string_t& sCaption);
    bool RunResizable(cWindow& parent, const string_t& sCaption, int iWidthDialogUnits, int iHeightDialogUnits);

    void ShowOkButton(bool bShow);
    void ShowCancelButton(bool bShow);

    // This simulate clicking one of the buttons (async queued)
    void DoOKButton() { PostMessage(hwndWindow, WM_COMMAND, IDOK, 0); }
    void DoCancelButton() { PostMessage(hwndWindow, WM_COMMAND, IDCANCEL, 0); }

  protected:
    LRESULT DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK _DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  private:
    void CreateDialogResource(cHGLOBAL& hglobal, int iWidthDU, int iHeightDU, int iMenu, const string_t& sCaption, DWORD uStyle, DWORD uExtStyle) const;

    bool CreateModelessDialog(cWindow& parent);

    BOOL EvWmSize(WPARAM wParam, LPARAM lParam);
    BOOL EvWmSizing(WPARAM wParam, LPARAM lParam);

    virtual void OnInit() = 0;
    virtual void OnInitFinished() {}
    virtual void OnDestroy() {}

    virtual void OnClose(int iResult) override;
    virtual bool OnOk() { return true; }
    virtual bool OnCancel() { return true; }
    virtual void OnHelp() {}

    class cDialogCreationFlags {
    public:
      cDialogCreationFlags();

      string_t sCaption;
      bool bResizable;
      bool bMinimizable;
      bool bMaximizable;
    };

    cDialogCreationFlags creationFlags; // Initial flags that are setup after the dialog has been created
  };
}

#endif // LIBWIN32MM_DIALOG_H

