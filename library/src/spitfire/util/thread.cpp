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
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/util/thread.h>

namespace spitfire
{
  namespace util
  {
    void cThread::Run()
    {
      cThread* pThis = this;
      pThread = new boost::thread(boost::bind(&cThread::RunThreadFunction, pThis));
    }
  }
}

#ifdef BUILD_DEBUG
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>
#include <spitfire/util/unittest.h>

class cDerivedThreadForUnitTest : public spitfire::util::cThread
{
public:

private:
  void ThreadFunction();
};

void cDerivedThreadForUnitTest::ThreadFunction()
{
  for (size_t i = 0; i < 10; i++) {
    std::cout<<"cDerivedThreadForUnitTest::ThreadFunction "<<i<<std::endl;
    spitfire::util::SleepThisThread(1000);
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
      spitfire::util::SleepThisThread(500);
    }

    thread.WaitUntilFinished();
  }
};

cThreadUnitTest gThreadUnitTest;
#endif // BUILD_DEBUG