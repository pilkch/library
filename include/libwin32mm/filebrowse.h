#ifndef LIBWIN32MM_FILEBROWSE_H
#define LIBWIN32MM_FILEBROWSE_H

// Standard headers
#include <string>
#include <list>

// Win32 headers
#include <windows.h>

// libwin32mm headers
#include <libwin32mm/libwin32mm.h>

namespace win32mm
{
  class cWindow;

  // Browse for a file with file type filtering
  //
  // cWin32mmFileDialog dialog;
  // dialog.SetType(cWin32mmFileDialog::TYPE::OPEN);
  // dialog.SetCaption(TEXT("Please choose a file"));
  // dialog.SetDefaultFolder(TEXT("/folder"));
  //
  // cFilter filterText;
  // filterText.sName = TEXT("Text files");
  // filterText.mimeTypes.push_back("text/plain");
  //
  // cFilter filterCpp;
  // filterCpp.sName = TEXT("C/C++ files");
  // filterCpp.mimeTypes.push_back("text/x-c");
  // filterCpp.mimeTypes.push_back("text/x-c++");
  // filterCpp.mimeTypes.push_back("text/x-c-header");
  //
  // cFilterList filterList;
  // filterList.AddFilter(filterText);
  // filterList.AddFilter(filterCpp);
  // filterList.AddFilterAllFiles();
  //
  // dialog.SetFilterList(filterList);
  //
  // if (dialog.Run(*this)) {
  // ... set file (dialog.GetSelectedFile());
  // }

  // Browse for multiple files
  //
  // cWin32mmFileDialog dialog;
  // dialog.SetType(cWin32mmFileDialog::TYPE::OPEN);
  // dialog.SetSelectMultipleFiles(true);
  // dialog.SetCaption(TEXT("Please choose some files"));
  // dialog.SetDefaultFolder(TEXT("/folder"));
  // if (dialog.Run(*this)) {
  // const std::list<string_t>& selectedFiles = dialog.GetSelectedFiles();
  // std::list<string_t>::const_iterator iter = selectedFiles.begin();
  // const std::list<string_t>::const_iterator iterEnd = selectedFiles.end();
  // while (iter != iterEnd) {
  // ... add file (*iter)
  //
  // iter++;
  // }
  // }

  // Browse for a Folder
  //
  // cWin32mmFolderDialog dialog;
  // dialog.SetType(cWin32mmFolderDialog::TYPE::SELECT);
  // dialog.SetCaption(TEXT("Please choose a folder"));
  // dialog.SetDefaultFolder(TEXT("/folder"));
  // if (dialog.Run(*this)) {
  // ... set folder (dialog.GetSelectedFolder());
  // }


  class cFilter
  {
  public:
    string_t sName;
    std::vector<std::string> mimeTypes;
    std::vector<std::string> patterns;
  };

  class cFilterList
  {
  public:
    void AddFilter(const cFilter& filter);
    void AddFilterAllFiles();

    std::vector<cFilter> filters;
  };

  class cWin32mmFileDialog
  {
  public:
    cWin32mmFileDialog();

    enum class TYPE {
      OPEN,
      SAVE
    };

    void SetType(TYPE type);
    void SetSelectMultipleFiles(bool bSelectMultipleFiles);
    void SetCaption(const string_t& sCaption);
    void SetDefaultFolder(const string_t& sDefaultFolder);  // The default location the first time the dialog is opened
    void SetOverrideFolder(const string_t& sDefaultFolder); // Ignore the saved last location and force the location to this folder (Not recommended)
    void SetFilterList(const cFilterList& filterList);

    const string_t& GetSelectedFolder() const;
    const string_t& GetSelectedFile() const;
    const std::list<string_t>& GetSelectedFiles() const;

    bool Run(cWindow& parent);

  private:
    TYPE type;
    bool bSelectMultipleFiles;
    string_t sCaption;
    string_t sDefaultFolder;
    cFilterList filterList;

    string_t sSelectedFolder;
    string_t sSelectedFile;
    std::list<string_t> selectedFiles;
  };

  class cWin32mmFolderDialog
  {
  public:
    cWin32mmFolderDialog();

    enum class TYPE {
      SELECT,
      SAVE
    };

    void SetType(TYPE type);
    void SetCaption(const string_t& sCaption);
    void SetDefaultFolder(const string_t& sDefaultFolder);  // The default location the first time the dialog is opened
    void SetOverrideFolder(const string_t& sDefaultFolder); // Ignore the saved last location and force the location to this folder (Not recommended)
    void SetSelectMultipleFolders(bool bSelectMultipleFolders);

    const string_t& GetSelectedFolder() const;
    const std::list<string_t>& GetSelectedFolders() const;

    bool Run(const cWindow& parent);

  private:
    TYPE type;
    bool bSelectMultipleFolders;
    string_t sCaption;
    string_t sDefaultFolder;

    string_t sSelectedFolder;
    std::list<string_t> selectedFolders;
  };
}

#endif // LIBWIN32MM_FILEBROWSE_H
