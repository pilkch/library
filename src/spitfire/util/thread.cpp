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
#include <spitfire/util/thread.h>

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


    // *** cThread

    void cThread::Run()
    {
      WaitToStop();

      soStop.Reset();
      soDone.Reset();

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
};

cDerivedThreadForUnitTest::cDerivedThreadForUnitTest() :
  spitfire::util::cThread("cDerivedThreadForUnitTest")
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
