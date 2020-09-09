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

      void RunSingleUnitTest(const string_t& sUnitTestName);
      void RunAllUnitTests();

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

    void cUnitTester::RunSingleUnitTest(const string_t& sUnitTestName)
    {
      SCREEN<<"Running unit test \""<<sUnitTestName<<"\""<<std::endl;

      std::list<cUnitTestBase*>::iterator iter(unittests.begin());
      const std::list<cUnitTestBase*>::iterator iterEnd(unittests.end());

      while (iter != iterEnd) {
        if ((*iter)->GetName() == sUnitTestName) {
          (*iter)->Run();
          break;
        }

        iter++;
      }

      SCREEN<<"Finished running unit test \""<<sUnitTestName<<"\""<<std::endl;
    }

    void cUnitTester::RunAllUnitTests()
    {
      SCREEN<<"Running unit tests"<<std::endl;

      std::list<cUnitTestBase*>::iterator iter(unittests.begin());
      const std::list<cUnitTestBase*>::iterator iterEnd(unittests.end());

      while (iter != iterEnd) {
        SCREEN<<"Test: "<<(*iter)->GetName()<<std::endl;
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

      LOG("Unit Test Running ", sComponent);

      Test();

      if (true == success) LOG("Unit Test ", sComponent, " Successfully completed");
      else LOGERROR("Unit Test ", sComponent, " FAILED");
    }

    void cUnitTestBase::SetFailed(const string_t& error)
    {
      LOGERROR("Unit Test ", sComponent, " ", error);
      success = false;
    }


    void RunSingleUnitTest(const string_t& sUnitTestName)
    {
      cUnitTester_t::Get().RunSingleUnitTest(sUnitTestName);
    }

    void RunAllUnitTests()
    {
      cUnitTester_t::Get().RunAllUnitTests();
    }
  }
}

#endif // BUILD_DEBUG
