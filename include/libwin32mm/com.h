#ifndef LIBWIN32MM_COM_H
#define LIBWIN32MM_COM_H

#include <windows.h>
#include <objbase.h>

// libwin32mm headers
#include <libwin32mm/libwin32mm.h>

namespace win32mm
{
  // ** cComScope
  //
  // All calls to COM need to be wrapped in this

  class cComScope {
  public:
    cComScope();
    ~cComScope();

  private:
    bool bInitialised;
  };

  inline cComScope::cComScope() :
    bInitialised(false)
  {
    const HRESULT result = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (result == RPC_E_CHANGED_MODE) std::wcerr<<TEXT("cComScope::cComScope CoInitialize returned RPC_E_CHANGED_MODE which means it was originally initialised for single threaded use")<<std::endl;
    else bInitialised = true;
  }

  inline cComScope::~cComScope()
  {
    if (bInitialised) CoUninitialize();
  }


  // ** COM_SAFE_RELEASE
  //
  // For releasing a COM object and setting it to NULL so that nothing else tries to use it

  template <class T>
  inline void COM_SAFE_RELEASE(T& x)
  {
    ASSERT(x != nullptr);
    x->Release();
    x = nullptr;
  }
}

#endif // LIBWIN32MM_COM_H
