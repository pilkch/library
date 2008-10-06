#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include <list>
#include <vector>
#include <map>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <breathe/breathe.h>
#include <breathe/util/cString.h>

#include <breathe/storage/filesystem.h>
#include <breathe/storage/xml.h>

#ifndef FIRESTARTER
#include <breathe/util/log.h>
#endif


namespace breathe
{
#ifdef BUILD_SETTINGS_GLOBAL
  // Shared between applications, per user
  // Uses IPC, the first application starts the IPC server and then other applications query the IPC server
  // Stored in "~/.breathe/preferences.xml"
  // ie.
  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, bool value)
  {
  }

  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, int value)
  {
  }

  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& value)
  {
  }

  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool value)
  {
  }

  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int value)
  {
  }

  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, const string_t& value)
  {
  }


  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, bool& value)
  {
    return false;
  }

  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, int& value)
  {
    return false;
  }

  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, string_t& value)
  {
    return false;
  }

  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool& value)
  {
    return false;
  }

  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int& value)
  {
    return false;
  }

  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, string_t& value)
  {
    return false;
  }


  void RemoveGlobalUserSetting(const string_t& section, const string_t& subsection)
  {
  }

  void RemoveGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection)
  {
  }
#endif

  // Per application, per user
  // Stored in "~/.breathe/$application/preferences.xml"
  // ie. Resolution, Fullscreen, Draw Distance, Language
  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, bool value)
  {
  }

  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, int value)
  {
  }

  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& value)
  {
  }

  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool value)
  {
  }

  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int value)
  {
  }

  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, const string_t& value)
  {
  }


  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, bool& value)
  {
    return false;
  }

  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, int& value)
  {
    return false;
  }

  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, string_t& value)
  {
    return false;
  }

  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool& value)
  {
    return false;
  }

  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int& value)
  {
    return false;
  }

  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, string_t& value)
  {
    return false;
  }


  void RemoveApplicationUserSetting(const string_t& section, const string_t& subsection)
  {
  }

  void RemoveApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection)
  {
  }

#ifdef BUILD_SETTINGS_PROFILES
  // Per profile
  // Stored in "~/.breathe/$application/profiles/$profile.xml"
  // ie. Does the user like to be left or right handed?  What gender is the user?  Name, colour, keys
  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, bool value)
  {
  }

  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, int value)
  {
  }

  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& value)
  {
  }

  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool value)
  {
  }

  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int value)
  {
  }

  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, const string_t& value)
  {
  }

  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, bool& value)
  {
    return false;
  }

  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, int& value)
  {
    return false;
  }

  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, string_t& value)
  {
    return false;
  }

  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool& value)
  {
    return false;
  }

  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int& value)
  {
    return false;
  }

  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, string_t& value)
  {
    return false;
  }


  void RemoveApplicationProfileSetting(const string_t& section, const string_t& subsection)
  {
    return false;
  }

  void RemoveApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection)
  {
    return false;
  }
#endif // BUILD_SETTINGS_PROFILES
}
