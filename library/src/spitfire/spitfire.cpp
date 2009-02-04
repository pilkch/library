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
  void InformativeAssert(bool bResult, const char* szAssert, const char* szFile, int line, const char* szFunction)
  {
    if (bResult) return;

    LOG<<"ASSERTION FAILED "<<szAssert<<" "<<szFile<<" "<<szFunction<<":"<<line<<std::endl;
    assert(bResult);
  }
#endif
}
