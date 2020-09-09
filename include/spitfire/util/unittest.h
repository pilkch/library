#ifndef UNITEST_H
#define UNITEST_H

#ifndef BUILD_SPITFIRE_UNITTEST
#error "BUILD_SPITFIRE_UNITTEST must be defined to use Spitfire unit tests"
#endif

// Standard headers
#include <cassert>

// Spitfire headers
#include <spitfire/math/math.h>
#include <spitfire/util/log.h>
#include <spitfire/util/string.h>

// Use an assert that is similar to GoogleTest
#define ASSERT_TRUE assert
#define ASSERT_EQ(expected, result) spitfire::util::AssertEqual(expected, result, STRINGIFY(expected), STRINGIFY(result), __FILE__, __LINE__)
#define ASSERT_STREQ(expected, result) spitfire::util::AssertStrEqual(expected, result, STRINGIFY(expected), STRINGIFY(result), __FILE__, __LINE__)
#define ASSERT_FLOAT_EQ(expected, result) spitfire::util::AssertFloatEqual(expected, result, STRINGIFY(expected), STRINGIFY(result), __FILE__, __LINE__)

namespace spitfire
{
  namespace util
  {
    template <typename A, typename B>
    inline void AssertEqual(A expected, B result, const char* expected_string, const char* result_string, const char* file, int line)
    {
      std::wostringstream a;
      a<<expected;

      std::wostringstream b;
      b<<expected;
      
      if (a.str() != b.str()) {
        LOGERROR("ASSERT_EQ FAILED expected: ", expected_string, ", which is: ", expected, ", actual: ", result_string, ", which is: ", result, " in file ", file, " at ", line);
        abort();
      }
    }

    inline void AssertStrEqual(const char* expected, const char* result, const char* expected_string, const char* result_string, const char* file, int line)
    {
      if (strcmp(expected, result) != 0) {
        LOGERROR("ASSERT_STEQ FAILED expected: ", expected_string, ", which is: ", expected, ", actual: ", result_string, ", which is: ", result, " in file ", file, " at ", line);
        abort();
      }
    }

    inline void AssertStrEqual(const wchar_t* expected, const wchar_t* result, const char* expected_string, const char* result_string, const char* file, int line)
    {
      if (wcscmp(expected, result) != 0) {
        LOGERROR("ASSERT_STEQ FAILED expected: ", expected_string, ", which is: ", expected, ", actual: ", result_string, ", which is: ", result, " in file ", file, " at ", line);
        abort();
      }
    }

    inline void AssertFloatEqual(float expected, float result, const char* expected_string, const char* result_string, const char* file, int line)
    {
      if ((expected != result) && !spitfire::math::IsApproximatelyEqual(expected, result)) {
        LOGERROR("ASSERT_STEQ FAILED expected: ", expected_string, ", which is: ", expected, ", actual: ", result_string, ", which is: ", result, " in file ", file, " at ", line);
        abort();
      }
    }


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