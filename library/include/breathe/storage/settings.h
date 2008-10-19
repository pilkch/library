#ifndef SETTINGS_H
#define SETTINGS_H

// Bad Example
/*
enum HANDORIENTATION
{
  HANDORIENTATION_RIGHT = 0,
  HANDORIENTATION_LEFT,
  HANDORIENTATION_CENTRE,

  HANDORIENTATION_DEFAULT = HANDORIENTATION_RIGHT
};

inline HANDORIENTATION GetHandOrientation()
{
  int iValue;
  if (GetApplicationProfileSetting(TEXT("View"), TEXT("HandOrientation"), iValue)) {
    if (iValue == 0) return HANDORIENTATION_RIGHT;
    if (iValue == 1) return HANDORIENTATION_LEFT;
    if (iValue == 2) return HANDORIENTATION_CENTRE;
  }
  return HANDORIENTATION_DEFAULT;
}*/

namespace breathe
{
#ifdef BUILD_SETTINGS_GLOBAL
  // Shared between applications, per user
  // Uses IPC, the first application starts the IPC server and then other applications query the IPC server
  // Stored in "~/.breathe/preferences.xml"
  // ie.
  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, bool value);
  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, int value);
  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& value);
  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool value);
  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int value);
  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, const string_t& value);

  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, bool& value);
  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, int& value);
  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, string_t& value);
  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool& value);
  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int& value);
  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, string_t& value);

  bool DoesGlobalSettingExist(const string_t& section, const string_t& subsection);
  bool DoesGlobalSettingExist(const string_t& section, const string_t& subsection, const string_t& subsubsection);

  void RemoveGlobalUserSetting(const string_t& section, const string_t& subsection);
  void RemoveGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection);
#endif

  // Per application, per user
  // Stored in "~/.breathe/$application/preferences.xml"
  // ie. Resolution, Fullscreen, Draw Distance, Language
  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, bool value);
  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, int value);
  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& value);
  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool value);
  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int value);
  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, const string_t& value);

  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, bool& value);
  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, int& value);
  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, string_t& value);
  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool& value);
  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int& value);
  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, string_t& value);

  bool DoesApplicationUserSettingExist(const string_t& section, const string_t& subsection);
  bool DoesApplicationUserSettingExist(const string_t& section, const string_t& subsection, const string_t& subsubsection);

  void RemoveApplicationUserSetting(const string_t& section, const string_t& subsection);
  void RemoveApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection);

#ifdef BUILD_SETTINGS_PROFILES
  // Per profile
  // Stored in "~/.breathe/$application/profiles/$profile.xml"
  // ie. Does the user like to be left or right handed?  What gender is the user?  Name, colour, keys etc.
  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, bool value);
  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, int value);
  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& value);
  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool value);
  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int value);
  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, const string_t& value);

  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, bool& value);
  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, int& value);
  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, string_t& value);
  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool& value);
  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int& value);
  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, string_t& value);

  bool DoesApplicationProfileSettingExist(const string_t& section, const string_t& subsection);
  bool DoesApplicationProfileSettingExist(const string_t& section, const string_t& subsection, const string_t& subsubsection);

  void RemoveApplicationProfileSetting(const string_t& section, const string_t& subsection);
  void RemoveApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection);
#endif // BUILD_SETTINGS_PROFILES
}

#endif // SETTINGS_H
