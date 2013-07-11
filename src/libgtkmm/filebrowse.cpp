// libgtkmm headers
#include <libgtkmm/filebrowse.h>

namespace gtkmm
{
  // ** cFilterList

  void cFilterList::AddFilter(const cFilter& filter)
  {
    filters.push_back(filter);
  }

  void cFilterList::AddFilterAllFiles()
  {
    cFilter filter;
    filter.sName = TEXT("Any files");
    filter.patterns.push_back("*.*");

    filters.push_back(filter);
  }


  // ** cGtkmmFileDialog

  void cGtkmmFileDialog::SetType(TYPE _type)
  {
    type = _type;
  }

  void cGtkmmFileDialog::SetSelectMultipleFiles(bool _bSelectMultipleFiles)
  {
    bSelectMultipleFiles = _bSelectMultipleFiles;
  }

  void cGtkmmFileDialog::SetCaption(const string_t& _sCaption)
  {
    sCaption = _sCaption;
  }

  void cGtkmmFileDialog::SetDefaultFolder(const string_t& _sDefaultFolder)
  {
    sDefaultFolder = _sDefaultFolder;
  }

  void cGtkmmFileDialog::SetFilterList(const cFilterList& _filterList)
  {
    filterList = _filterList;
  }

  const string_t& cGtkmmFileDialog::GetSelectedFolder() const
  {
    return sSelectedFolder;
  }

  const string_t& cGtkmmFileDialog::GetSelectedFile() const
  {
    return sSelectedFile;
  }

  const std::list<string_t>& cGtkmmFileDialog::GetSelectedFiles() const
  {
    return selectedFiles;
  }

  bool cGtkmmFileDialog::Run(Gtk::Window& parent)
  {
    assert(!filterList.filters.empty());

    sSelectedFolder.clear();
    sSelectedFile.clear();
    selectedFiles.clear();

    // Create our dialog
    Gtk::FileChooserDialog
    dialog(spitfire::string::ToUTF8(sCaption).c_str(), (type == TYPE::SAVE) ? Gtk::FILE_CHOOSER_ACTION_SAVE : Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(parent);
    dialog.set_select_multiple(bSelectMultipleFiles);
    dialog.set_current_folder(spitfire::string::ToUTF8(sDefaultFolder).c_str());

    // Add response buttons the the dialog
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button((type == TYPE::SAVE) ? Gtk::Stock::SAVE : Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

    // Add filters, so that only certain file types can be selected
    const size_t nFilters = filterList.filters.size();
    for (size_t iFilter = 0; iFilter < nFilters; iFilter++) {
      const cFilter& filter = filterList.filters[iFilter];

      Glib::RefPtr<Gtk::FileFilter> fileFilter = Gtk::FileFilter::create();
      fileFilter->set_name(spitfire::string::ToUTF8(filter.sName).c_str());

      const size_t nMimeTypes = filter.mimeTypes.size();
      for (size_t iMimeType = 0; iMimeType < nMimeTypes; iMimeType++) {
        fileFilter->add_mime_type(filter.mimeTypes[iMimeType].c_str());
      }

      const size_t nPatterns = filter.patterns.size();
      for (size_t iPattern = 0; iPattern < nPatterns; iPattern++) {
        fileFilter->add_mime_type(filter.patterns[iPattern].c_str());
      }

      dialog.add_filter(fileFilter);
    }

    bool bResult = false;

    // Run the dialog
    int iResult = dialog.run();
    if (iResult == Gtk::RESPONSE_OK) {
      bResult = true;
      sSelectedFolder = spitfire::string::ToString_t(dialog.get_current_folder());
      sSelectedFile = spitfire::string::ToString_t(dialog.get_filename());
      const std::vector<std::string>& files = dialog.get_filenames();
      const size_t n = files.size();
      for (size_t i = 0; i < n; i++) selectedFiles.push_back(spitfire::string::ToString_t(files[i]));
    }

    return bResult;
  }


  // ** cGtkmmFolderDialog

  void cGtkmmFolderDialog::SetType(TYPE _type)
  {
    type = _type;
  }

  void cGtkmmFolderDialog::SetCaption(const string_t& _sCaption)
  {
    sCaption = _sCaption;
  }

  void cGtkmmFolderDialog::SetDefaultFolder(const string_t& _sDefaultFolder)
  {
    sDefaultFolder = _sDefaultFolder;
  }

  const string_t& cGtkmmFolderDialog::GetSelectedFolder() const
  {
    return sSelectedFolder;
  }

  bool cGtkmmFolderDialog::Run(Gtk::Window& parent)
  {
    sSelectedFolder.clear();

    // Create our dialog
    Gtk::FileChooserDialog
    dialog(spitfire::string::ToUTF8(sCaption).c_str(), Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
    dialog.set_transient_for(parent);
    dialog.set_current_folder(spitfire::string::ToUTF8(sDefaultFolder).c_str());

    // Add response buttons the the dialog
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    if (type == TYPE::SELECT) dialog.add_button("Select", Gtk::RESPONSE_OK);
    else dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

    bool bResult = false;

    // Run the dialog
    int iResult = dialog.run();
    if (iResult == Gtk::RESPONSE_OK) {
      bResult = true;
      sSelectedFolder = spitfire::string::ToString_t(dialog.get_filename());
    }

    return bResult;
  }
}
