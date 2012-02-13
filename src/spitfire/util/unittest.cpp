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

      CONSOLE.Success("Unit Test", "Running " + spitfire::string::ToUTF8(sComponent));

      Test();

      if (true == success) CONSOLE.Success("Unit Test", spitfire::string::ToUTF8(sComponent) + " Successfully completed");
      else CONSOLE.Error("Unit Test", spitfire::string::ToUTF8(sComponent) + " FAILED");
    }

    void cUnitTestBase::SetFailed(const string_t& error)
    {
      CONSOLE.Error("Unit Test", spitfire::string::ToUTF8(sComponent) + " " + spitfire::string::ToUTF8(error));
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
