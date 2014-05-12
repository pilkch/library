// Standard headers
#include <iostream>

// Windows headers
#include <windows.h>
#include <shellapi.h>

// libwin32mm headers
#include <libwin32mm/com.h>
#include <libwin32mm/filesystem.h>

namespace win32mm
{
  bool MoveFileOrFolderToRecycleBin(const string_t& sFileOrFolderPath)
  {
    // Initialize COM
    cComScope com;

    // Create COM instance of IFileOperation
    IFileOperation* pfo = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pfo));
    if (SUCCEEDED(hr)) {
      ASSERT(pfo != nullptr);

      // Set parameters for current operation
      hr = pfo->SetOperationFlags(
          FOF_SILENT |    // Don't display a progress dialog
          FOF_NOERRORUI   // Don't display any error messages to the user
      );

      if (SUCCEEDED(hr)) {
        // Create IShellItem instance associated to file to delete
        IShellItem* psiFileToDelete = nullptr;
        hr = SHCreateItemFromParsingName(sFileOrFolderPath.c_str(), NULL, IID_PPV_ARGS(&psiFileToDelete));
        if (SUCCEEDED(hr)) {
          ASSERT(psiFileToDelete != nullptr);

          // Declare this shell item (file) to be deleted
          hr = pfo->DeleteItem(psiFileToDelete, NULL);
        }

        // Cleanup file-to-delete shell item
        COM_SAFE_RELEASE(psiFileToDelete);

        if (SUCCEEDED(hr)) {
          // Perform the deleting operation
          hr = pfo->PerformOperations();
        }
      }
    }

    // Cleanup file operation object
    COM_SAFE_RELEASE(pfo);

    if (!SUCCEEDED(hr)) {
      std::wcerr<<"MoveFileOrFolderToRubbishBin Error moving \""<<sFileOrFolderPath<<"\" to the recycle bin"<<std::endl;
      return false;
    }

    return true;
  }
}
