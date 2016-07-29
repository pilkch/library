#ifndef UNITEST_H
#define UNITEST_H

#ifndef BUILD_SPITFIRE_UNITTEST
#error "BUILD_SPITFIRE_UNITTEST must be defined to use Spitfire unit tests"
#endif

// Standard headers
#include <cassert>

// Spitfire headers
#include <spitfire/util/string.h>

// Use an assert that is similar to GoogleTest
#define ASSERT_TRUE assert

namespace spitfire
{
  namespace util
  {
    class cUnitTestBase
    {
    public:
      explicit cUnitTestBase(const string_t& component);
      virtual ~cUnitTestBase() {}

      const string_t& GetName() const { return sComponent; }

      void Run();

    protected:
      void SetFailed(const string_t& error);

    private:
      virtual void Test() = 0;

      bool success;
      string_t sComponent;

    private:
      cUnitTestBase();
      cUnitTestBase(const cUnitTestBase&);
    };

    void RunSingleUnitTest(const string_t& sUnitTestName);
    void RunAllUnitTests();
  }
}

#endif // UNITEST_H