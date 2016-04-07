#ifndef LANG_H
#define LANG_H

// Standard global tags: spitfire::LANG("L__MainMenu")
// Application specific tags: spitfire::LANG("L_Application")
// Note: Double __ is global, single _ is local to the application
// 
// Basic arguments:
// Lang tag L_A_Something_B "%1 something %2"
// LANG("L_A_Something_B", "first", 2);
//
// Switched arguments:
// Lang tag L_A_Something_B_Switched "%1 something %2" OR "%2 something %1" depending on the language
// LANG("L_A_Something_B_Switched", "first", 2);
//
// Missing arguments:
// Lang tag L_A_Something_B_Missing "%1 something %2" OR "Something %1" depending on the language
// LANG("L_A_Something_B_Missing", 3, "something");
//
// Duplicated arguments:
// Lang tag L_A_Something_B_Duplicated "%1 something %2, %3" OR "Something %1, again %1, %2, %3" depending on the language
// LANG("L_A_Something_B_Duplicated", "something", 2, 3);
//
// Internally we use wstring and then we return string_t
//
// British Empire vs. American spelling
// http://en.wikipedia.org/wiki/American_and_British_English_spelling_differences
//
// Style Guide
//
// I like the gnome guidelines
// http://library.gnome.org/devel/gdp-style-guide/stable/gnome-glossary-user-actions.html.en
// http://library.gnome.org/devel/gdp-style-guide/stable/gnome-glossary-button-types.html.en
// http://library.gnome.org/devel/gdp-style-guide/stable/gnome-glossary-generic-terms.html.en
//
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

#include <spitfire/util/string.h>

namespace spitfire
{
  void LoadLanguageFiles();


  inline void ParseArgument(const string_t& sLangTag, size_t index)
  {
    (void)sLangTag;
    (void)index;
  }

  template<typename Argument, typename... OtherArguments>
  inline void ParseArgument(const string_t& sLangTag, size_t index, const Argument& argument, const OtherArguments&... otherArguments)
  {
    // Find something like %1 in the lang tag and replace it with our argument
    string::Replace(sLangTag, TEXT("%") + string::ToString(index + 1), string::ToString(argument));

    // Now we can increment our counter and process the remaining arguments
    index++;
    ParseArgument(sLangTag, index, otherArguments...);
  }

  template<typename Argument, typename... OtherArguments>
  inline string_t LANG(const string_t& _sLangTag, const Argument& argument, const OtherArguments&... otherArguments)
  {
    // Collect our arguments
    string_t sLangTag = _sLangTag;
    size_t index = 1; // Start at 1 so that the parameters are 1..n+1
    ParseArgument(sLangTag, index, argument, otherArguments...);

    // Our lang tag should now be complete
    return sLangTag;
  }

  namespace util
  {
    class cDateTime;

    string_t LangHumanReadableTime(const cDateTime& dateTimeNow, const cDateTime& dateTime);
    string_t LangHumanReadableDuration(const std::chrono::system_clock::duration& duration);
  }
}

#endif // LANG_H
