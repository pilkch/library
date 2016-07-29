// Standard headers
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <cassert>
#include <cmath>
#include <ctime>

#include <vector>
#include <list>

#include <string>
#include <sstream>

#include <iostream>
#include <fstream>
#include <iomanip>

#include <spitfire/spitfire.h>

#include <spitfire/util/string.h>

namespace spitfire
{
#ifndef NDEBUG
  uint32_t LogGetTimeStamp()
  {
    struct timeval time;

    gettimeofday(&time, NULL);

    long seconds  = time.tv_sec;
    long useconds = time.tv_usec;

    return ((seconds) * 1000 + useconds / 1000.0) + 0.5;
  }

  std::string LogGetTime()
  {
    static uint32_t start = LogGetTimeStamp();

    uint32_t total = LogGetTimeStamp() - start;

    const uint32_t milliseconds = total % 1000;
    total /= 1000;
    const uint32_t seconds = total % 60;
    total /= 60;
    const uint32_t minutes = total % 60;
    total /= 60;
    const uint32_t hours = total % 60;

    std::ostringstream o;
    o<<hours;
    o<<":";
    o<<std::setw(2)<<std::setfill('0')<<minutes;
    o<<":";
    o<<std::setw(2)<<std::setfill('0')<<seconds;
    o<<":";
    o<<std::setw(3)<<std::setfill('0')<<milliseconds;
    o<<" ";

    return o.str();
  }

  void InformativeAssert(bool bIsResultTrue, const char* szAssert, const char* szFile, const char* szFunction, size_t line)
  {
    if (!bIsResultTrue) {
      //bool bIsLogging = logging::IsLogging();

      // Make sure that we log this assert even if logging is off
      //logging::TurnOnLogging();

      std::cout<<"ASSERTION FAILED "<<szAssert<<" "<<szFile<<" "<<szFunction<<":"<<line<<std::endl;
      std::cerr<<"ASSERTION FAILED "<<szAssert<<" "<<szFile<<" "<<szFunction<<":"<<line<<std::endl;
      assert(bIsResultTrue);

      // If logging was off then keep it off
      //if (!bIsLogging) logging::TurnOffLogging();
    }
  }
#endif
}


#ifdef BUILD_SPITFIRE_UNITTEST

#include <spitfire/util/unittest.h>

class cSpitfireUnitTest : protected spitfire::util::cUnitTestBase
{
public:
  cSpitfireUnitTest() :
    cUnitTestBase(TEXT("cSpitfireUnitTest"))
  {
  }

  void TestCountOf()
  {
    char text[14];
    ASSERT_TRUE(countof(text) == 14);

    int stuff[9];
    ASSERT_TRUE(countof(stuff) == 9);
  }

  void TestSafeDelete()
  {
    int* x = new int;
    spitfire::SAFE_DELETE(x);
    ASSERT_TRUE(x == nullptr);
  }

  void TestSafeDeleteArray()
  {
    int* y = new int[10];
    spitfire::SAFE_DELETE_ARRAY(y);
    ASSERT_TRUE(y == nullptr);
  }

  void Test()
  {
    TestCountOf();
    TestSafeDelete();
    TestSafeDeleteArray();
  }
};

cSpitfireUnitTest gSpitfireUnitTest;

#endif // BUILD_SPITFIRE_UNITTEST

