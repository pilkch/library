#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>

// Reading and writing text files
#include <iostream>
#include <fstream>

#include <list>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stack>

// Spitfire headers
#include <spitfire/spitfire.h>

#ifdef BUILD_DEBUG
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>
#include <spitfire/util/unittest.h>
#include <spitfire/util/cSmartPtr.h>

class cSmartPtrTestClass
{
public:
  cSmartPtrTestClass(int iValue)
  {
    value = iValue;
  }

  int value;

  void FunctionA()
  {
    std::cout<<"cSmartPtrTestClass::FunctionA"<<std::endl;
  }

  void FunctionB(std::string sParameter)
  {
    std::cout<<"cSmartPtrTestClass::FunctionB sParameter="<<sParameter<<std::endl;
  }
};

class cSmartPtrUnitTest : protected spitfire::util::cUnitTestBase
{
public:
  cSmartPtrUnitTest() :
    cUnitTestBase("cSmartPtrUnitTest")
  {
  }

  void Test()
  {
    spitfire::util::cSmartPtr<cSmartPtrTestClass> c(new cSmartPtrTestClass(100));
    c->FunctionA();
    c->FunctionB("Chris");



    {
      //spitfire::util::cSmartPtr<cSmartPtrTestClass>::reference ref(c);
    }
  }
};

cSmartPtrUnitTest gSmartPtrUnitTest;
#endif // BUILD_DEBUG
