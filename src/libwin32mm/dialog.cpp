// Standard headers
#include <iostream>

// Windows headers
#include <windows.h>
#include <winuser.h>

// libwin32mm headers
#include <libwin32mm/dialog.h>
#include <libwin32mm/maindialog.h>

// Spitfire headers
#include <spitfire/storage/filesystem.h>

#define WIN32MM_FONT "MS Shell Dlg 2"

#define WIN32MM_WM_USER_INIT_FINISHED (WM_APP + 684)

namespace win32mm
{
  // ** cHGLOBAL
  //
  // HGLOBAL wrapper

  class cHGLOBAL {
  public:
    cHGLOBAL();
    ~cHGLOBAL();

    void Allocate(size_t nSizeBytes);

    void* GetLock();
    HGLOBAL Get();

    void Unlock();

  private:
    void Release();

    HGLOBAL handle;
  };

  inline cHGLOBAL::cHGLOBAL() :
    handle(NULL)
  {
  }

  inline cHGLOBAL::~cHGLOBAL()
  {
    Release();
  }

  inline HGLOBAL cHGLOBAL::Get()
  {
    return handle;
  }

  inline void cHGLOBAL::Allocate(size_t nSizeBytes)
  {
    Release();

    handle = ::GlobalAlloc(GMEM_ZEROINIT, nSizeBytes);
    ASSERT(handle != NULL);
  }

  inline void* cHGLOBAL::GetLock()
  {
    ASSERT(handle != NULL);
    return ::GlobalLock(handle);
  }

  inline void cHGLOBAL::Unlock()
  {
    ASSERT(handle != NULL);
    ::GlobalUnlock(handle);
  }

  inline void cHGLOBAL::Release()
  {
    if (handle != NULL) {
      ::GlobalFree(handle);
      handle = NULL;
    }
  }

  void cDialog::CreateDialogResource(cHGLOBAL& hglobal, int iWidthDU, int iHeightDU, int iMenu, const string_t& sCaption, DWORD uStyle, DWORD uExtStyle) const
  {
    // http://blogs.msdn.com/b/oldnewthing/archive/2005/04/29/412577.aspx

    struct DlgTemplate {
        DLGTEMPLATE dlg;
        WORD dlgmenu[1];
        WORD dlgclass;
        wchar_t dlgcaption[1];
        WORD fontsize;
        wchar_t fontface[30];
    };

    hglobal.Allocate(1024);

    DlgTemplate* pTemplate = (DlgTemplate*)hglobal.GetLock();
    ASSERT(pTemplate != nullptr);

    // Fill out our dialog template
    pTemplate->dlg.style = uStyle | DS_SHELLFONT;
    pTemplate->dlg.dwExtendedStyle = uExtStyle;
    pTemplate->dlg.cdit = 0;
    pTemplate->dlg.x = 0;
    pTemplate->dlg.y = 0;
    pTemplate->dlg.cx = iWidthDU;
    pTemplate->dlg.cy = iHeightDU;

    pTemplate->dlgmenu[0] = 0;
    pTemplate->dlgclass = 0;

    // Copy the caption
    wcscpy_s(pTemplate->dlgcaption, sCaption.c_str());

    pTemplate->fontsize = 8;
    wcscpy_s(pTemplate->fontface, TEXT(WIN32MM_FONT));

    hglobal.Unlock();
  }


  // ** cDialogCreationFlags

  cDialog::cDialogCreationFlags::cDialogCreationFlags() :
    bResizable(false),
    bMinimizable(false),
    bMaximizable(false)
  {
  }

  // ** cDialog

  bool cDialog::RunNonResizable(cWindow& parent, const string_t& sCaption)
  {
    return RunNonResizable(parent, sCaption, 400, 400);
  }

  bool cDialog::RunNonResizable(cWindow& parent, const string_t& sCaption, int iWidthDialogUnits, int iHeightDialogUnits)
  {
    creationFlags.sCaption = sCaption;

    cHGLOBAL hglobal;

    CreateDialogResource(hglobal, short(iWidthDialogUnits), short(iHeightDialogUnits), 0, TEXT(""), WS_POPUP | WS_CLIPCHILDREN | WS_EX_COMPOSITED | WS_VISIBLE | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION | DS_3DLOOK, WS_EX_DLGMODALFRAME);

    LRESULT ret = DialogBoxIndirectParam(NULL, (LPDLGTEMPLATE)hglobal.Get(), parent.GetWindowHandle(), _DialogProc, LPARAM(this));

    ::SetForegroundWindow(parent.GetWindowHandle());
    ::PostMessage(parent.GetWindowHandle(), WM_SHOWWINDOW, 0, 0);

    return (ret == IDOK);
  }

  bool cDialog::RunResizable(cWindow& parent, const string_t& sCaption)
  {
    return RunResizable(parent, sCaption, 400, 400);
  }

  bool cDialog::RunResizable(cWindow& parent, const string_t& sCaption, int iWidthDialogUnits, int iHeightDialogUnits)
  {
    creationFlags.sCaption = sCaption;

    cHGLOBAL hglobal;

    CreateDialogResource(hglobal, short(iWidthDialogUnits), short(iHeightDialogUnits), 0, TEXT(""), WS_POPUP | WS_CLIPCHILDREN | WS_EX_COMPOSITED | WS_VISIBLE | DS_MODALFRAME | WS_BORDER | WS_CAPTION | DS_3DLOOK | WS_SYSMENU | WS_OVERLAPPED | WS_THICKFRAME, WS_EX_DLGMODALFRAME);

    // TODO: APPARENTLY WE ONLY NEED WS_THICKFRAME SO WE COULD JUST CALL THE OTHER FUNCTION
    // TODO: WORK OUT WHAT FLAGS ARE UNIQUE AND JUST PASS THOSE
    creationFlags.bResizable = true;
    creationFlags.bMinimizable = true;
    creationFlags.bMaximizable = true;

    LRESULT ret = DialogBoxIndirectParam(NULL, (LPDLGTEMPLATE)hglobal.Get(), parent.GetWindowHandle(), _DialogProc, LPARAM(this));

    ::SetForegroundWindow(parent.GetWindowHandle());
    ::PostMessage(parent.GetWindowHandle(), WM_SHOWWINDOW, 0, 0);

    return (ret == IDOK);
  }
  
  bool cDialog::CreateModelessDialog(cWindow& parent)
  {
    cHGLOBAL hglobal;

    CreateDialogResource(hglobal, 0, 0, 0, TEXT(""), DS_3DLOOK | DS_CENTER | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_CLIPCHILDREN | WS_EX_COMPOSITED, 0);

    const HWND hwnd = CreateDialogIndirectParam(NULL, (LPDLGTEMPLATE)hglobal.Get(), parent.GetWindowHandle(), _DialogProc, LPARAM(this));

    ::SetForegroundWindow(parent.GetWindowHandle());

    if (hwnd == NULL) {
      std::cerr<<"cDialog::CreateModelessDialog CreateDialogIndirectParam Failed"<<std::endl;
      return false;
    }

    return true;
  }

  BOOL cDialog::EvWmSize(WPARAM wParam, LPARAM lParam)
  {
    if (wParam != SIZE_MINIMIZED) {
      const int iWidth = LOWORD(lParam);
      const int iHeight = HIWORD(lParam);
      CallOnResize(iWidth, iHeight);
      
      // Invalidate to redrawn any areas without controls
      ::InvalidateRect(hwndWindow, NULL, FALSE);
    }

    return FALSE;
  }

  BOOL cDialog::EvWmSizing(WPARAM wParam, LPARAM lParam)
  {
    const int iMinimumWidth = 100;
    const int iMinimumHeight = 100;

    LPRECT pRect = (LPRECT)lParam;
    if ((pRect->right - pRect->left) < iMinimumWidth) {
      if ((wParam == WMSZ_TOPLEFT) || (wParam == WMSZ_BOTTOMLEFT) || (wParam == WMSZ_LEFT)) pRect->left = pRect->right - iMinimumWidth;
      else pRect->right = pRect->left + iMinimumWidth;
    }
    if ((pRect->bottom - pRect->top) < iMinimumHeight) {
      if ((wParam == WMSZ_TOPLEFT) || (wParam == WMSZ_TOPRIGHT) || (wParam == WMSZ_TOP)) pRect->top = pRect->bottom - iMinimumHeight;
      else pRect->bottom = pRect->top + iMinimumHeight;
    }
    return TRUE;
  }

  LRESULT cDialog::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    // Allow cWindow to handle this message
    LRESULT result = HandleMessage(hwnd, uMsg, wParam, lParam);
    if (result != FALSE) return result;

    switch (uMsg) {
      case WM_INITDIALOG:
        std::cout<<"cWindow::DialogProc call InitDialog"<<std::endl;
        OnInit();

        // Make sure Vista themes are initialised
        ::SendMessage(hwndWindow, WM_THEMECHANGED, 0, 0);

        // Set our font to the system font
        SetDefaultFont();

        // Center our dialog on the parent window
        CenterOnParent();

        //EnableControls();

        // Push a message onto the back of the queue
        ::PostMessage(hwndWindow, WIN32MM_WM_USER_INIT_FINISHED, 0, 0);

        return TRUE;

      case WIN32MM_WM_USER_INIT_FINISHED: {
        OnInitFinished();
        return FALSE;
      }

      case WM_COMMAND: {
        switch (LOWORD(wParam)) {
          case IDOK:
            if (OnOk()) {
              CloseOk();
              return TRUE;
            }
            break;
          case IDCANCEL:
            if (OnCancel()) {
              CloseCancel();
              return TRUE;
            }
            break;
          case IDHELP:
            OnHelp();
            return TRUE;
          default:
            OnCommand(LOWORD(wParam));
            break;
        }
        break;
      }

      case WM_DESTROY:
        OnDestroy();
        hwndWindow = NULL;
        break;

      case WM_SIZE:
        return EvWmSize(wParam, lParam);
      case WM_SIZING:
        return EvWmSizing(wParam, lParam);
    }

    return FALSE;
  }

  LRESULT CALLBACK cDialog::_DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    if (uMsg == WM_INITDIALOG) {
      // Set our window pointer so that we can consistently get the pointer back for all messages later on
      cDialog* pDialog = (cDialog*)lParam;
      if (pDialog != nullptr) {
        ::SetWindowLongPtr(hwnd, DWLP_USER , lParam);
        pDialog->SetWindowHandle(hwnd);

        pDialog->UpdateDPI();

        pDialog->SetCaption(pDialog->creationFlags.sCaption);

        if (pDialog->creationFlags.bResizable) pDialog->SetResizable(true);
        if (pDialog->creationFlags.bMaximizable) pDialog->SetMaximizable(true);
        if (pDialog->creationFlags.bMinimizable) pDialog->SetMinimizable(true);

        // Set the icon for this dialog
        const string_t sFolderPath = spitfire::filesystem::GetThisApplicationDirectory();
        const string_t sFilePath = spitfire::filesystem::MakeFilePath(sFolderPath, TEXT("data\\icons\\windows\\application.ico"));
        ASSERT(spitfire::filesystem::FileExists(sFilePath));
        HICON icon = ::HICON(LoadImage(GetHInstance(), sFilePath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
        ASSERT(icon != NULL);
        ::SendMessage(hwnd, WM_SETICON, ICON_BIG, LPARAM(icon));
        ::SendMessage(hwnd, WM_SETICON, ICON_SMALL, LPARAM(icon));

        pDialog->OnDialogCreated();
      }
    }

    // Allow cMainDialog to handle this message
    cMainDialog* pThis = static_cast<cMainDialog*>(::GetProp(hwnd, TEXT("cMainDialog")));
    if (pThis != nullptr) return pThis->MainDialogProc(hwnd, uMsg, wParam, lParam);

    // Get our window pointer
    cDialog* pDialog = (cDialog*)::GetWindowLongPtr(hwnd, DWLP_USER);
    if ((pDialog == nullptr) || (pDialog->hwndWindow != hwnd)) return FALSE;

    // Handle the message
    return pDialog->DialogProc(hwnd, uMsg, wParam, lParam);
  }

  void cDialog::OnClose(int iResult)
  {
    std::cout<<TEXT("cDialog::OnClose iResult=")<<iResult<<std::endl;

    // Tell the application that the dialog is being destroyed
    OnDestroy();

    // End the dialog and return the result
    HWND hwndWindowTemp = hwndWindow;
    hwndWindow = NULL;
    ::EndDialog(hwndWindowTemp, iResult);
  }
}
