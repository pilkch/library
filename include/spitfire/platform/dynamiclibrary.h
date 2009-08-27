#ifndef DYNAMICLIBRARY_H
#define DYNAMICLIBRARY_H

namespace spitfire
{
  namespace operatingsystem
  {
    class cLibrary
    {
    public:
      explicit cLibrary(const string_t& sPath);
      ~cLibrary();

      bool IsOpen() const { return (pLibrary != nullptr); }

      void* LoadSymbol(const string_t& sSymbol)
      {
        return (library*)dlsym((void*)pLibrary, spitfire::string::ToUTF8(sSymbol).c_str());
      }

    private:
      library* pLibrary;
    };

    cLibrary::cLibrary(const string_t& sPath) :
      pLibrary(nullptr)
    {
      LOG<<"cLibrary::cLibrary Loading library \""<<sPath<<"\""<<std::endl;
      pLibrary = (void*)dlopen(spitfire::string::ToUTF8(sPath).c_str(), RTLD_LAZY);
    }

    cLibrary::~cLibrary()
    {
      LOG<<"cLibrary::cLibrary Unloading library"<<std::endl;
      if (pLibrary != nullptr) {
        dlclose((void*)pLibrary);
        pLibrary = nullptr;
      }
    }
  }
}

#endif // DYNAMICLIBRARY_H
