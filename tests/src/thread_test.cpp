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

#include <gtest/gtest.h>

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/util/log.h>
#include <spitfire/util/string.h>
#include <spitfire/util/thread.h>

class cDerivedThreadForUnitTest : public spitfire::util::cThread
{
public:
  cDerivedThreadForUnitTest();

private:
  virtual void ThreadFunction() override;

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


TEST(SpitfireUtil, TestThreads)
{
  cDerivedThreadForUnitTest thread;

  thread.Run();

  for (size_t i = 0; i < 20; i++) {
    std::cout<<"Test "<<i<<std::endl;
    spitfire::util::SleepThisThreadMS(500);
  }

  thread.WaitToStop();
}
