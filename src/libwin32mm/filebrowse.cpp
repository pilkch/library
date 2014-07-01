// Standard headers
#include <iostream>
#include <sstream>

// Win32 headers
#include <windows.h>
#include <commdlg.h>

// libwin32mm headers
#include <libwin32mm/com.h>
#include <libwin32mm/filebrowse.h>
#include <libwin32mm/window.h>

namespace win32mm
{
  // ** cFolderDialog

  cFolderDialog::cFolderDialog() :
    type(TYPE::SELECT),
    bSelectMultipleFolders(false)
  {
  }

  bool cFolderDialog::Run(const cWindow& parent)
  {
    sSelectedFolder.clear();

    // Initialize COM
    cComScope com;

    IFileDialog* pfd = nullptr;
    if (!SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd)))) {
      std::wcout<<TEXT("BrowseOpenFolderDialog CoCreateInstance FAILED, returning false")<<std::endl;
      return false;
    }

    DWORD dwOptions = 0;
    if (SUCCEEDED(pfd->GetOptions(&dwOptions))) {
      pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
    }

    if (SUCCEEDED(pfd->Show(parent.GetWindowHandle()))) {
      IShellItem* psi = nullptr;
      if (SUCCEEDED(pfd->GetResult(&psi))) {
        PWSTR pszFilePath;
        if(!SUCCEEDED(psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pszFilePath))) {
          std::wcout<<TEXT("BrowseOpenFolderDialog GetDisplayName FAILED")<<std::endl;
        } else {
          std::wcout<<TEXT("BrowseOpenFolderDialog GetDisplayName returned \"")<<pszFilePath<<TEXT("\"")<<std::endl;
        }

        sSelectedFolder = pszFilePath;

        CoTaskMemFree(pszFilePath);

        COM_SAFE_RELEASE(psi);
      }
    }

    COM_SAFE_RELEASE(pfd);

    return true;
  }
}
