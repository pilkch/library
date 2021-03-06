// Standard headers
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include <iostream>
#include <fstream>
#include <functional>

#include <list>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stack>

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/util/log.h>
#include <spitfire/util/thread.h>

#if defined(__WIN__) && defined(BUILD_DEBUG)
const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
  DWORD dwType; // Must be 0x1000.
  LPCSTR szName; // Pointer to name (in user addr space).
  DWORD dwThreadID; // Thread ID (-1 = caller thread).
  DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
#endif

namespace spitfire
{
  namespace util
  {
    std::thread::id idThreadMain;

    void SetMainThread()
    {
      ASSERT(idThreadMain == std::thread::id());
      idThreadMain = std::this_thread::get_id();
    }

    bool IsMainThread()
    {
      ASSERT(idThreadMain != std::thread::id());
      const std::thread::id idThread = std::this_thread::get_id();
      return (idThread == idThreadMain);
    }


    #if defined(__WIN__) && defined(BUILD_DEBUG)
    void SetThreadName(const char* szThreadName)
    {
      const DWORD dwThreadID = DWORD(-1); // Current thread

      THREADNAME_INFO info;
      info.dwType = 0x1000;
      info.szName = szThreadName;
      info.dwThreadID = dwThreadID;
      info.dwFlags = 0;

      __try
      {
        RaiseException( MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
      }
      __except(EXCEPTION_EXECUTE_HANDLER)
      {
      }
    }
    #endif


    // *** cThread

    // Not the most elegant method, but it works
    int cThread::RunThreadFunction(void* pData)
    {
      ASSERT(pData != nullptr);
      cThread* pThis = static_cast<cThread*>(pData);
      ASSERT(pThis != nullptr);

      #if defined(__WIN__) && defined(BUILD_DEBUG)
      // Set the thread name in debug mode
      SetThreadName(spitfire::string::ToUTF8(pThis->sName).c_str());
      #endif

      LOG("Calling ThreadFunction");
      pThis->ThreadFunction();
      LOG("ThreadFunction returned");

      // Tell everyone that the thread has finished
      pThis->soDone.Signal();

      // Tell everyone that something has happened
      pThis->soAction.Signal();

      // We don't really do this correctly, we just return 0 for every thread
      return 0;
    }

    void cThread::Run()
    {
      StopThreadSoon();
      WaitToStop();

      soStop.Reset();
      soDone.Reset();
      soAction.Reset();

      cThread* pThis = this;
      pThread = new std::thread(std::bind(&cThread::RunThreadFunction, pThis));
    }
  }
}
