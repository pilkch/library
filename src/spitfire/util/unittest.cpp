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
#include <spitfire/util/string.h>
#include <spitfire/util/log.h>
#include <spitfire/util/unittest.h>
#include <spitfire/util/singleton.h>

namespace spitfire
{
  namespace util
  {
    // *** cUnitTester

    class cUnitTester
    {
    public:
      cUnitTester() {}
      virtual ~cUnitTester();

      void AddUnitTest(cUnitTestBase* unittest);
      void RunUnitTests();

    private:
      NO_COPY(cUnitTester);

      std::list<cUnitTestBase*> unittests;
    };

    cUnitTester::~cUnitTester()
    {
      unittests.clear();
    }

    void cUnitTester::AddUnitTest(cUnitTestBase* unittest)
    {
      unittests.push_back(unittest);
    }

    void cUnitTester::RunUnitTests()
    {
      SCREEN<<"Running unit tests"<<std::endl;

      std::list<cUnitTestBase*>::iterator iter(unittests.begin());
      const std::list<cUnitTestBase*>::iterator iterEnd(unittests.end());

      while (iter != iterEnd) {
        (*iter)->Run();
        iter++;
      }

      SCREEN<<"Finished running unit tests"<<std::endl;
    }

    typedef util::singleton<cUnitTester> cUnitTester_t;


    // *** cUnitTestBase

    cUnitTestBase::cUnitTestBase(const string_t& component) :
      sComponent(component)
    {
      cUnitTester_t::Get().AddUnitTest(this);
    }

    void cUnitTestBase::Run()
    {
      success = true;

      LOG<<"Unit Test Running "<<sComponent<<std::endl;

      Test();

      if (true == success) LOG<<"Unit Test "<<sComponent<<" Successfully completed"<<std::endl;
      else LOGERROR<<"Unit Test "<<sComponent<<" FAILED"<<std::endl;
    }

    void cUnitTestBase::SetFailed(const string_t& error)
    {
      LOGERROR<<"Unit Test "<<sComponent<<" "<<error<<std::endl;
      success = false;
    }


    // *** RunUnitTests

    void RunUnitTests()
    {
      cUnitTester_t::Get().RunUnitTests();
    }
  }
}

#endif // BUILD_DEBUG
