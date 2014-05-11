#ifndef LIBWIN32MM_DYNAMICLIBRARY_H
#define LIBWIN32MM_DYNAMICLIBRARY_H

// Standard headers
#include <iostream>

// libwin32mm headers
#include <libwin32mm/libwin32mm.h>

// Spitfire headers
#include <spitfire/storage/filesystem.h>

namespace win32mm
{
  // ** cDLL

  class cDLL {
  public:
    explicit cDLL(const string_t& sLibraryFilePath);
    ~cDLL();

    bool IsValid() const;

    bool LoadAbsoluteFilePath(const string_t& sLibraryFilePath); // Load the library from the path specified only

    template <class FN>
    bool LoadFunction(const char* szFunction, FN** pFN);

  private:
    void Unload();

    HMODULE handle;
  };

  inline cDLL::cDLL(const string_t& sLibraryFilePath)
  {
    handle = ::LoadLibrary(sLibraryFilePath.c_str());
    #ifndef NDEBUG
    if (handle == NULL) std::wcout<<TEXT("cDLL::cDLL LoadLibrary Cannot load the dll: ")<<sLibraryFilePath<<std::endl;
    #endif
  }

  inline cDLL::~cDLL()
  {
    Unload();
  }

  inline bool cDLL::IsValid() const
  {
    return (handle != NULL);
  }

  inline bool cDLL::LoadAbsoluteFilePath(const string_t& sLibraryFilePath)
  {
    Unload();

    ASSERT(spitfire::filesystem::FileExists(sLibraryFilePath.c_str()));
    handle = ::LoadLibraryEx(sLibraryFilePath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (handle == NULL) std::wcout<<TEXT("cDLL::LoadAbsoluteFilePath Cannot load the dll: ")<<sLibraryFilePath<<std::endl;
    return (handle != NULL);
  }

  inline void cDLL::Unload()
  {
    if (handle != NULL) {
      ::FreeLibrary(handle);
      handle = NULL;
    }
  }

  template <class FN>
  inline bool cDLL::LoadFunction(const char* szFunction, FN** pFN)
  {
    ASSERT(handle != NULL);
    *pFN = (FN*)::GetProcAddress(handle, szFunction);
    if (*pFN == NULL) std::wcout<<TEXT("cDLL::LoadFunction Cannot find the library function: ")<<szFunction<<std::endl;
    return (*pFN != NULL);
  }
}

#endif // LIBWIN32MM_DYNAMICLIBRARY_H
