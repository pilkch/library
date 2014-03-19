// Standard headers
#include <iostream>
#include <sstream>

// libwin32mm headers
#include <libwin32mm/filebrowse.h>
#include <libwin32mm/window.h>

#undef interface

// Win32 headers
#include <Shobjidl.h>
#include <commdlg.h>

namespace win32mm
{
  // ** cWin32mmFolderDialog

  cWin32mmFolderDialog::cWin32mmFolderDialog() :
    type(TYPE::SELECT),
    bSelectMultipleFolders(false)
  {
  }

  bool cWin32mmFolderDialog::Run(const cWindow& parent)
  {
    sSelectedFolder.clear();

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (!SUCCEEDED(hr)) {
      std::wcout<<TEXT("BrowseOpenFolderDialog CoInitializeEx FAILED, returning false")<<std::endl;
      return false;
    }

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

        psi->Release();
      }
    }

    pfd->Release();

    return true;
  }
}
