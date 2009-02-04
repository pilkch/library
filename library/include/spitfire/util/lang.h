#ifndef LANG_H
#define LANG_H

// Standard global tags: breathe::LANG("L__MainMenu")
// Application specific tags: breathe::LANG("L_Application")
// Note: Double __ is global, single _ is local to the application

// Internally we use wstring and then we return string_t

// Style Guide
//
// I like the gnome guidelines
// http://library.gnome.org/devel/gdp-style-guide/stable/gnome-glossary-user-actions.html.en
// http://library.gnome.org/devel/gdp-style-guide/stable/gnome-glossary-button-types.html.en
// http://library.gnome.org/devel/gdp-style-guide/stable/gnome-glossary-generic-terms.html.en

// Breathe Style Guide
//
// Use:
// Folder
// Yes
// Log On
// Log Out
//
// Do not use:
// Ok
// Directory
// Login

namespace breathe
{
  // Can pass in either UTF8 or unicode, however, you will always get back unicode
  string_t LANG(const std::string& tag);
  string_t LANG(const std::wstring& tag);

  namespace util
  {
    void LoadLanguageFiles();
  }
}

#endif // LANG_H