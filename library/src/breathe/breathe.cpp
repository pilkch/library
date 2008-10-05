#include <cassert>

#include <vector>
#include <list>

#include <string>
#include <sstream>

#include <iostream>
#include <fstream>
#include <iomanip>

#include <breathe/breathe.h>

#include <breathe/util/cSmartPtr.h>
#include <breathe/util/cString.h>
#include <breathe/util/log.h>

namespace breathe
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
