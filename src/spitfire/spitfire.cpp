// Standard headers
#include <time.h>

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

#ifndef __WIN__
#include <sys/time.h>
#include <unistd.h>
#endif

#include <spitfire/util/string.h>
#include <spitfire/util/timer.h>

namespace spitfire
{
#ifndef NDEBUG
  std::string LogGetTime()
  {
    static durationms_t start = util::GetTimeMS();

    durationms_t total = util::GetTimeMS() - start;

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
