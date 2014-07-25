// Standard headers
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include <iostream>
#include <fstream>

#include <list>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stack>

// Boost headers
#include <boost/bind.hpp>

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

      LOG<<"cThread::RunThreadFunction Calling ThreadFunction"<<std::endl;
      pThis->ThreadFunction();
      LOG<<"cThread::RunThreadFunction ThreadFunction returned"<<std::endl;

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
      pThread = new std::thread(boost::bind(&cThread::RunThreadFunction, pThis));
    }
  }
}

#ifdef BUILD_SPITFIRE_UNITTEST
#ifdef BUILD_DEBUG

#include <spitfire/util/string.h>
#include <spitfire/util/log.h>
#include <spitfire/util/unittest.h>

class cDerivedThreadForUnitTest : public spitfire::util::cThread
{
public:
  cDerivedThreadForUnitTest();

private:
  void ThreadFunction();

  spitfire::util::cSignalObject soAction;
};

cDerivedThreadForUnitTest::cDerivedThreadForUnitTest() :
  spitfire::util::cThread(soAction, TEXT("cDerivedThreadForUnitTest")),
  soAction(TEXT("cDerivedThreadForUnitTest_soAction"))
{
}

void cDerivedThreadForUnitTest::ThreadFunction()
{
  for (size_t i = 0; i < 10; i++) {
    std::cout<<"cDerivedThreadForUnitTest::ThreadFunction "<<i<<std::endl;
    spitfire::util::SleepThisThreadMS(1000);
  }
}


class cThreadUnitTest : protected spitfire::util::cUnitTestBase
{
public:
  cThreadUnitTest() :
    cUnitTestBase(TEXT("cThreadUnitTest"))
  {
  }

  void Test()
  {
    cDerivedThreadForUnitTest thread;

    thread.Run();

    for (size_t i = 0; i < 20; i++) {
      std::cout<<"Test "<<i<<std::endl;
      spitfire::util::SleepThisThreadMS(500);
    }

    thread.WaitToStop();
  }
};

cThreadUnitTest gThreadUnitTest;

#endif // BUILD_DEBUG
#endif // BUILD_SPITFIRE_UNITTEST
