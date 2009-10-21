#ifndef CDYNAMIC_LIBRARY_H
#define CDYNAMIC_LIBRARY_H

#include <spitfire/spitfire.h>

#ifdef PLATFORM_LINUX_OR_UNIX
#include <dlfcn.h>
#endif

namespace spitfire
{
  class cDynamicLibrary
  {
  public:
    cDynamicLibrary();
    ~cDynamicLibrary();

    bool LoadFromAnywhere(const string_t& sDynamicLibraryFile);
    bool LoadFromSpecificPath(const string_t& sDynamicLibraryFullPath);

    bool IsOpen() const { return (handle != nullptr); }

    template <class F>
    bool LoadFunction(const std::string& sFunction, F** pFunction);

  private:
    void Unload();

  #ifdef __WIN__
    HINSTANCE handle;
  #else
    void* handle;
  #endif
  };

  inline cDynamicLibrary::cDynamicLibrary() :
    handle(nullptr)
  {
  }

  inline cDynamicLibrary::~cDynamicLibrary()
  {
    if (handle != nullptr) Unload();
  }


  #ifdef __WIN__
  inline bool cDynamicLibrary::LoadFromAnywhere(const string_t& sDynamicLibraryFile)
  {
    ASSERT(handle == NULL);
    handle = ::LoadLibrary(sDynamicLibraryFile.c_str());
    if (handle == NULL) LOG<<"cDynamicLibrary::LoadFromAnywhere LoadLibrary FAILED to load \""<<sLibraryFile<<"\""<<std::endl;
  }

  inline bool cDynamicLibrary::LoadFromSpecificPath(const string_t& sDynamicLibraryFullPath)
  {
    ASSERT(handle == NULL);
    handle = ::LoadLibraryEx(sDynamicLibraryFile.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (handle == NULL) LOG<<"cDynamicLibrary::LoadFromSpecificPath LoadLibraryEx FAILED to load \""<<sLibraryFile<<"\""<<std::endl;
  }

  inline void cDynamicLibary::Unload()
  {
    ::FreeLibrary(handle);
    handle = NULL;
  }

  template <class F>
  inline bool cDynamicLibary::LoadFunction(const std::string& sFunction, F** pFunction)
  {
    ASSERT(handle != NULL);
    *pFunction = (F*)::GetProcAddress(handle, sFunction.c_str());
    if (*pFunction == nullptr) LOG<<"cDynamicLibary::LoadFunction GetProcAddress FAILED could not find function \""<<sFunction<<"\""<<std::endl;

    return (*pFunction != nullptr);
  }
  #elif defined(PLATFORM_LINUX_OR_UNIX)
  inline bool cDynamicLibrary::LoadFromAnywhere(const string_t& sDynamicLibraryFile)
  {
    ASSERT(handle == NULL);
    handle = dlopen(string::ToUTF8(sDynamicLibraryFile).c_str(), RTLD_LOCAL | RTLD_LAZY);
    if (handle == NULL) LOG<<"cDynamicLibrary::LoadFromSpecificPath dlopen FAILED to load \""<<sDynamicLibraryFile<<"\""<<std::endl;

    return (handle != NULL);
  }

  inline bool cDynamicLibrary::LoadFromSpecificPath(const string_t& sDynamicLibraryFullPath)
  {
    ASSERT(handle == NULL);
    handle = dlopen(string::ToUTF8(sDynamicLibraryFile).c_str(), RTLD_LOCAL | RTLD_LAZY);
    if (handle == NULL) LOG<<"cDynamicLibrary::LoadFromSpecificPath dlopen FAILED to load \""<<sDynamicLibraryFullPath<<"\""<<std::endl;

    return (handle != NULL);
  }

  inline void cDynamicLibary::Unload()
  {
    ASSERT(handle != NULL);
    dlclose(handle);
    handle = NULL;
  }

  template <class F>
  inline bool cDynamicLibary::LoadFunction(const std::string& sFunction, F** pFunction)
  {
    ASSERT(handle != NULL);
    *pFunction = (F*)dlsym(handle, sFunction.c_str());
    if (*pFunction == nullptr) LOG<<"cDynamicLibary::LoadFunction dlsym FAILED could not find function \""<<sFunction<<"\""<<std::endl;

    return (*pFunction != nullptr);
  }
  #endif
}

#endif // CDYNAMIC_LIBRARY_H
