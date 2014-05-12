// Standard headers
#include <string.h>
#include <iostream>

// Windows headers
#include <windows.h>
#include <commctrl.h>

// libwin32mm headers
#include <libwin32mm/taskdialog.h>

namespace win32mm
{
  // ** cTaskDialogAlertDialogSettings
  
  cTaskDialogAlertDialogSettings::cTaskDialogAlertDialogSettings() :
    type(TYPE::INFORMATION)
  {
  }


  // ** cTaskDialogSettings

  cTaskDialogSettings::cTaskDialogSettings() :
    bCloseButtonText(false),
    type(TYPE::NORMAL),
    iDefaultItemCommandID(0),
    bIsProgressBarVisible(false),
    bIsCheckBoxVisible(false),
    bIsCheckBoxTickedInitially(false)
  {
  }

  void cTaskDialogSettings::SetAlertDialogSettings(const cTaskDialogAlertDialogSettings& alertDialogSettings)
  {
    type = TYPE::INFORMATION;
    if (alertDialogSettings.type == cTaskDialogAlertDialogSettings::TYPE::WARNING) type = TYPE::WARNING;
    else if (alertDialogSettings.type == cTaskDialogAlertDialogSettings::TYPE::ERROR) type = TYPE::ERROR;

    sCaption = alertDialogSettings.sCaption;
    sInstructions = alertDialogSettings.sInstructions;
    sContent = alertDialogSettings.sContent;

    // Add the yes item
    cTaskDialogItem item;
    item.iCommandID = IDYES;
    item.sText = alertDialogSettings.sYesCaption;
    item.sTextDescription = alertDialogSettings.sYesDescription;
    items.push_back(item);

    iDefaultItemCommandID = IDYES;

    // Add the no item
    if (!alertDialogSettings.sNoCaption.empty()) {
      item.iCommandID = IDNO;
      item.sText = alertDialogSettings.sNoCaption;
      item.sTextDescription = alertDialogSettings.sNoDescription;
      items.push_back(item);
      
      iDefaultItemCommandID = IDYES;
    }
  }

  void cTaskDialogSettings::SetProgressDialogSettings(const cTaskDialogProgressDialogSettings& progressDialogSettings)
  {
    type = TYPE::INFORMATION;

    sCaption = progressDialogSettings.sCaption;
    sInstructions = progressDialogSettings.sInstructions;
    sContent = progressDialogSettings.sContent;

    bIsProgressBarVisible = true;
  }


  // ** cTaskDialog

  cTaskDialog::cTaskDialog() :
    hwndWindow(NULL),
    bIsCheckBoxTicked(false),
    pListener(nullptr)
  {
  }

  void cTaskDialog::SetCaption(const string_t& sCaption)
  {
  }

  void cTaskDialog::SetInstructions(const string_t& sInstructions)
  {
    ::SendMessage(hwndWindow, TDM_SET_ELEMENT_TEXT, TDE_MAIN_INSTRUCTION, (LPARAM)sInstructions.c_str());
  }

  void cTaskDialog::SetContent(const string_t& sContent)
  {
    ::SendMessage(hwndWindow, TDM_SET_ELEMENT_TEXT, TDE_CONTENT, (LPARAM)sContent.c_str());
  }

  void cTaskDialog::SetCancellable(bool bCancellable)
  {
    // TODO: This doesn't seem to work?
    ::SendMessage(hwndWindow, TDM_ENABLE_BUTTON, IDCANCEL, bCancellable ? FALSE : TRUE);
  }

  void cTaskDialog::SetProgress0To100(float fProgress0To100)
  {
    // Set the range
    ::SendMessage(hwndWindow, TDM_SET_PROGRESS_BAR_RANGE, 0, MAKELPARAM(0, 100));

    // Set the position
    ::SendMessage(hwndWindow, TDM_SET_PROGRESS_BAR_POS, int(fProgress0To100), 0);
  }

  void cTaskDialog::SetProgressIndeterminateOn()
  {
    //::SendMessage(hwndWindow, TDM_SET_PROGRESS_BAR_STATE, (int)PBST_PAUSE, 0);
  }

  void cTaskDialog::SetProgressIndeterminateOff()
  {
    ::SendMessage(hwndWindow, TDM_SET_PROGRESS_BAR_STATE, (int)PBST_NORMAL, 0);
  }

  void cTaskDialog::SetProgressError()
  {
    ::SendMessage(hwndWindow, TDM_SET_PROGRESS_BAR_STATE, (int)PBST_ERROR, 0);
  }

  void cTaskDialog::Close()
  {
    // Make sure the task dialog is cancellable
    SetCancellable(true);

    // Tell the dialog to close
    ::SendMessage(hwndWindow, WM_CLOSE, 0, 0);
  }

  int cTaskDialog::Run(cWindow& parent, const cTaskDialogSettings& settings)
  {
    // Create our task dialog
    TASKDIALOGCONFIG tdc = { 0 };
    tdc.cbSize = sizeof(tdc);
    tdc.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION;
    if (settings.bIsCheckBoxTickedInitially) tdc.dwFlags |= TDF_VERIFICATION_FLAG_CHECKED;
    tdc.dwCommonButtons = (settings.bCloseButtonText ? TDCBF_CLOSE_BUTTON : TDCBF_CANCEL_BUTTON);
    tdc.hwndParent = parent.GetWindowHandle();
    tdc.hInstance = GetHInstance();

    //tdc.hMainIcon = hIcon;

    switch (settings.type) {
      case cTaskDialogSettings::TYPE::INFORMATION: {
        tdc.pszMainIcon = TD_INFORMATION_ICON;
        break;
      }
      case cTaskDialogSettings::TYPE::WARNING: {
        tdc.pszMainIcon = TD_ERROR_ICON;
        break;
      }
      case cTaskDialogSettings::TYPE::ERROR: {
        tdc.pszMainIcon = TD_ERROR_ICON;
        break;
      }
    };

    tdc.pszWindowTitle = settings.sCaption.c_str();
    tdc.pszMainInstruction = settings.sInstructions.c_str();
    tdc.pszContent = settings.sContent.c_str();

    // Add our buttons
    TASKDIALOG_BUTTON* pButtons = nullptr;
    const size_t nButtons = settings.items.size();
    if (!settings.items.empty()) {
      tdc.dwFlags |= TDF_USE_COMMAND_LINKS;

      pButtons = new TASKDIALOG_BUTTON[nButtons];
      for (size_t i = 0; i < nButtons; i++) {
        pButtons[i].nButtonID = settings.items[i].iCommandID;
        string_t sText(settings.items[i].sText);
        if (!settings.items[i].sTextDescription.empty()) {
          sText += TEXT("\n");
          sText += settings.items[i].sTextDescription;
        }

        const size_t nSize = sText.length() + 1;
        pButtons[i].pszButtonText = new wchar_t[nSize];
        wcscpy_s((wchar_t*)(pButtons[i].pszButtonText), nSize, sText.c_str());
      }

      tdc.pButtons = pButtons;
      tdc.cButtons = UINT(nButtons);

      tdc.nDefaultButton = settings.iDefaultItemCommandID;
    }

    /*PCWSTR      pszExpandedInformation;
    PCWSTR      pszExpandedControlText;
    PCWSTR      pszCollapsedControlText;*/

    tdc.pfCallback = _TaskDialogCallbackProc;
    tdc.lpCallbackData = LONG_PTR(this);

    if (settings.bIsProgressBarVisible) {
      tdc.dwFlags |= TDF_CALLBACK_TIMER | TDF_SHOW_PROGRESS_BAR;
    }

    // Checkbox
    if (settings.bIsCheckBoxVisible) {
      tdc.dwFlags |= TDF_EXPAND_FOOTER_AREA;
      tdc.pszVerificationText = settings.sCheckBoxTitle.c_str();
    }

    // Run the task dialog
    int iButton = 0;
    //BOOL bResult = FALSE;
    const HRESULT rResult = TaskDialogIndirect(&tdc, &iButton, NULL, nullptr /*&bResult*/);
    assert(rResult == S_OK);

    // Clear our window handle
    hwndWindow = NULL;

    // Destroy our buttons
    if (pButtons != nullptr) {
      for (size_t i = 0; i < nButtons; i++) delete [] pButtons[i].pszButtonText;
      delete [] pButtons;
    }

    // If there was an error then pretend the user cancelled
    if (rResult != S_OK) iButton = IDCANCEL;
    
    // If no valid selection was made then pretend the user cancelled
    if (iButton < 0) iButton = IDCANCEL;

    // Return the button that was selected
    return iButton;
  }

  HRESULT cTaskDialog::TaskDialogCallbackProc(HWND _hwndWindow, UINT uNotification, WPARAM wParam, LPARAM lParam)
  {
    switch (uNotification) {
      case TDN_DIALOG_CONSTRUCTED: {
        hwndWindow = _hwndWindow;

        // Notify our listener
        if (pListener != nullptr) pListener->OnCreate();

        break;
      }
      case TDN_DESTROYED: {
        // Notify our listener
        if (pListener != nullptr) pListener->OnDestroy();

        break;
      }
      case TDN_TIMER: {
        //std::wcout<<"cTaskDialog::TaskDialogCallbackProc"<<std::endl;
        break;
      }

      case TDN_HELP:
        if (pListener != nullptr) pListener->OnHelp(hwndWindow);
        break;

      case TDN_VERIFICATION_CLICKED:   // 'Show again' check box
        bIsCheckBoxTicked = !bIsCheckBoxTicked;
        break;
    }

    return FALSE;
  }

  HRESULT CALLBACK cTaskDialog::_TaskDialogCallbackProc(HWND hwndWindow, UINT uNotification, WPARAM wParam, LPARAM lParam, LONG_PTR dwUserData)
  {
     if (dwUserData != 0) {
        cTaskDialog* pThis = (cTaskDialog*)(dwUserData);
        return pThis->TaskDialogCallbackProc(hwndWindow, uNotification, wParam, lParam);
     }

     return S_OK;
  }
}
