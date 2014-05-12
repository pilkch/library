#ifndef LIBWIN32MM_COM_H
#define LIBWIN32MM_COM_H

// Standard headers
#include <iostream>

// Windows headers
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


  // ** cComObject

  template <class T>
  class cComObject {
  public:
    cComObject();
    ~cComObject();

    void CreateObject(REFCLSID rclsid, REFIID riid);

    bool IsValid() const;

    const T& operator*() const;
    T& operator*();
    const T* operator->() const;
    T* operator->();

  private:
    NO_COPY(cComObject<T>);

    void Release();

    T* p;
  };


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



  // ** Inlines

  // ** cComScope

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


  // ** cComObject

  template <class T>
  inline cComObject<T>::cComObject() :
    p(nullptr)
  {
  }

  template <class T>
  inline cComObject<T>::~cComObject()
  {
     Release();
  }

  template <class T>
  inline void cComObject<T>::Release()
  {
     if (p != nullptr) {
        p->Release();
        p = nullptr;
     }
  }

  template <class T>
  inline bool cComObject<T>::IsValid() const
  {
     return (p != nullptr);
  }

  template <class T>
  inline const T& cComObject<T>::operator*() const
  {
     ASSERT(p != nullptr);
     return *p;
  }

  template <class T>
  inline T& cComObject<T>::operator*()
  {
     ASSERT(p != nullptr);
     return *p;
  }

  template <class T>
  inline const T* cComObject<T>::operator->() const
  {
     ASSERT(p != nullptr);
     return p;
  }


  template <class T>
  inline T* cComObject<T>::operator->()
  {
     ASSERT(p != nullptr);
     return p;
  }

  template <class T>
  inline void cComObject<T>::CreateObject(REFCLSID rclsid, REFIID riid)
  {
    // Release any existing object
    Release();

    // Create our new object
    const HRESULT result = CoCreateInstance(rclsid, NULL, CLSCTX_INPROC_SERVER, riid, (void**)&p);
    if (result != S_OK) {
      std::wcerr<<TEXT("cComObject::CreateObject CoCreateInstance FAILED result=")<<result<<TEXT(", last error=")<<GetLastError()<<std::endl;
    }
  }
}

#endif // LIBWIN32MM_COM_H
