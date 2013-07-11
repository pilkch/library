#ifndef LIBGTKMM_FILEBROWSE_H
#define LIBGTKMM_FILEBROWSE_H

// Standard headers
#include <string>
#include <list>

// gtkmm headers
#include <gtkmm.h>

// libgtkmm headers
#include <libgtkmm/libgtkmm.h>

namespace gtkmm
{
  // Browse for a file with file type filtering
  //
  // cGtkmmFileDialog dialog;
  // dialog.SetType(cGtkmmFileDialog::TYPE::OPEN);
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
  //   ... set file (dialog.GetSelectedFile());
  // }

  // Browse for multiple files
  //
  // cGtkmmFileDialog dialog;
  // dialog.SetType(cGtkmmFileDialog::TYPE::OPEN);
  // dialog.SetSelectMultipleFiles(true);
  // dialog.SetCaption(TEXT("Please choose some files"));
  // dialog.SetDefaultFolder(TEXT("/folder"));
  // if (dialog.Run(*this)) {
  //   const std::list<string_t>& selectedFiles = dialog.GetSelectedFiles();
  //   std::list<string_t>::const_iterator iter = selectedFiles.begin();
  //   const std::list<string_t>::const_iterator iterEnd = selectedFiles.end();
  //   while (iter != iterEnd) {
  //     ... add file (*iter)
  //
  //     iter++;
  //   }
  // }

  // Browse for a Folder
  //
  // cGtkmmFolderDialog dialog;
  // dialog.SetType(cGtkmmFolderDialog::TYPE::SELECT);
  // dialog.SetCaption(TEXT("Please choose a folder"));
  // dialog.SetDefaultFolder(TEXT("/folder"));
  // if (dialog.Run(*this)) {
  //   ... set folder (dialog.GetSelectedFolder());
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

  class cGtkmmFileDialog
  {
  public:
    enum class TYPE {
      OPEN,
      SAVE
    };

    void SetType(TYPE type);
    void SetSelectMultipleFiles(bool bSelectMultipleFiles);
    void SetCaption(const string_t& sCaption);
    void SetDefaultFolder(const string_t& sDefaultFolder);
    void SetFilterList(const cFilterList& filterList);

    const string_t& GetSelectedFolder() const;
    const string_t& GetSelectedFile() const;
    const std::list<string_t>& GetSelectedFiles() const;

    bool Run(Gtk::Window& parent);

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

  class cGtkmmFolderDialog
  {
  public:
    enum class TYPE {
      SELECT,
      SAVE
    };

    void SetType(TYPE type);
    void SetCaption(const string_t& sCaption);
    void SetDefaultFolder(const string_t& sDefaultFolder);

    const string_t& GetSelectedFolder() const;

    bool Run(Gtk::Window& parent);

  private:
    TYPE type;
    string_t sCaption;
    string_t sDefaultFolder;

    string_t sSelectedFolder;
  };
}

#endif // LIBGTKMM_FILEBROWSE_H
