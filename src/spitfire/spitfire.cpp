#include <cassert>

#include <vector>
#include <list>

#include <string>
#include <sstream>

#include <iostream>
#include <fstream>
#include <iomanip>

#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

namespace spitfire
{
#ifndef NDEBUG
  void InformativeAssert(bool bIsResultTrue, const char* szAssert, const char* szFile, const char* szFunction, size_t line)
  {
    if (!bIsResultTrue) {
      bool bIsLogging = logging::IsLogging();

      // Make sure that we log this assert even if logging is off
      logging::TurnOnLogging();

      std::cout<<"ASSERTION FAILED "<<szAssert<<" "<<szFile<<" "<<szFunction<<":"<<line<<std::endl;
      LOG<<"ASSERTION FAILED "<<szAssert<<" "<<szFile<<" "<<szFunction<<":"<<line<<std::endl;
      assert(bIsResultTrue);

      // If logging was off then keep it off
      if (!bIsLogging) logging::TurnOffLogging();
    }
  }
#endif
}
