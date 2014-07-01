#ifndef LIBWIN32MM_H
#define LIBWIN32MM_H

#undef interface
#define interface __STRUCT__

#include <windows.h>
#include <shlwapi.h>
#include <shobjidl.h>

#undef interface
#define interface Interface

// Spitfire headers
#include <spitfire/util/string.h>

namespace win32mm
{
  using spitfire::char_t;
  using spitfire::string_t;
  using spitfire::ostringstream_t;

  class cWindow;

  void Init();
  void Destroy();

  HINSTANCE GetHInstance();

  void OpenWebPage(const cWindow& parent, const string_t& sWebPage);


  // ** Inlines

  inline HINSTANCE GetHInstance()
  {
    return GetModuleHandle(NULL);
  }
}

#endif // LIBWIN32MM_H
