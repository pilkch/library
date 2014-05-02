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

  void Init();

  HINSTANCE GetHInstance();


  // ** Inlines

  inline HINSTANCE GetHInstance()
  {
    return GetModuleHandle(NULL);
  }
}

#endif // LIBWIN32MM_H
