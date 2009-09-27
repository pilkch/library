#ifndef LANG_H
#define LANG_H

// Standard global tags: spitfire::LANG("L__MainMenu")
// Application specific tags: spitfire::LANG("L_Application")
// Note: Double __ is global, single _ is local to the application

// Internally we use wstring and then we return string_t

// British Empire vs. American spelling
// http://en.wikipedia.org/wiki/American_and_British_English_spelling_differences

// Style Guide
//
// I like the gnome guidelines
// http://library.gnome.org/devel/gdp-style-guide/stable/gnome-glossary-user-actions.html.en
// http://library.gnome.org/devel/gdp-style-guide/stable/gnome-glossary-button-types.html.en
// http://library.gnome.org/devel/gdp-style-guide/stable/gnome-glossary-generic-terms.html.en

// Spitfire Style Guide
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

#include <spitfire/util/cString.h>

namespace spitfire
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
