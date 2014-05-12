#ifndef LIBWIN32MM_TASKDIALOG_H
#define LIBWIN32MM_TASKDIALOG_H

// libwin32mm headers
#include <libwin32mm/libwin32mm.h>
#include <libwin32mm/window.h>

namespace win32mm
{
  // Win32 has a define for ERROR.  Thanks Microsoft.
  #ifdef ERROR
  #undef ERROR
  #endif

  class cTaskDialogItem
  {
  public:
    int iCommandID;
    string_t sText;
    string_t sTextDescription;
  };

  class cTaskDialogAlertDialogSettings
  {
  public:
    cTaskDialogAlertDialogSettings();

    enum class TYPE {
      INFORMATION,
      WARNING,
      ERROR
    };

    TYPE type;
    string_t sCaption;
    string_t sInstructions;
    string_t sContent;

    string_t sYesCaption;
    string_t sYesDescription;
    string_t sNoCaption;
    string_t sNoDescription;
  };

  class cTaskDialogProgressDialogSettings
  {
  public:
    string_t sCaption;
    string_t sInstructions;
    string_t sContent;
  };

  class cTaskDialogSettings
  {
  public:
    cTaskDialogSettings();

    void SetAlertDialogSettings(const cTaskDialogAlertDialogSettings& alertDialogSettings);
    void SetProgressDialogSettings(const cTaskDialogProgressDialogSettings& progressDialogSettings);

    enum class TYPE {
      NORMAL,
      INFORMATION,
      WARNING,
      ERROR
    };

    bool bCloseButtonText;
    TYPE type;
    string_t sCaption;
    string_t sInstructions;
    string_t sContent;

    std::vector<cTaskDialogItem> items;
    int iDefaultItemCommandID;

    bool bIsProgressBarVisible;

    bool bIsCheckBoxVisible;
    bool bIsCheckBoxTickedInitially;
    string_t sCheckBoxTitle;
  };

  class cTaskDialogListener
  {
  public:
    virtual ~cTaskDialogListener() {}

    virtual void OnCreate() {}
    virtual void OnDestroy() {}
    virtual void OnHelp(HWND hwndWindow) {}
  };

  class cTaskDialog
  {
  public:
    cTaskDialog();

    void SetListener(cTaskDialogListener& listener);

    int Run(cWindow& parent, const cTaskDialogSettings& settings);

    void Close();

    void SetCaption(const string_t& sCaption);
    void SetInstructions(const string_t& sInstructions);
    void SetContent(const string_t& sContent);

    void SetCancellable(bool bCancellable);

    void SetProgress0To100(float fProgress0To100);
    void SetProgressIndeterminateOn();
    void SetProgressIndeterminateOff();
    void SetProgressError();

    bool IsCheckBoxTicked() const;

  private:
    HRESULT TaskDialogCallbackProc(HWND hwndWindow, UINT uNotification, WPARAM wParam, LPARAM lParam);

    static HRESULT __stdcall _TaskDialogCallbackProc(HWND hwndWindow, UINT uNotification, WPARAM wParam, LPARAM lParam, LONG_PTR dwUserData);

    cTaskDialogListener* pListener;

    HWND hwndWindow;

    bool bIsCheckBoxTicked;
  };


  // ** Inlines

  inline void cTaskDialog::SetListener(cTaskDialogListener& listener)
  {
    pListener = &listener;
  }
}

#endif // LIBWIN32MM_TASKDIALOG_H
