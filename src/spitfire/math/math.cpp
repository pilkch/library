#include <cstdlib>
#include <cmath>

#include <vector>
#include <limits>

// Spitfire Includes
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>

#ifdef max
#undef max
#endif

namespace spitfire
{
  namespace math
  {
    const float_t cINFINITY = std::numeric_limits<float>::max(); //1e30f //std::numeric_limits<float>::infinity();

    cScopedPredictableRandom::cScopedPredictableRandom(uint32_t seed)
    {
      // Make random numbers and put them into the buffer
      uint32_t s = seed;
      for (size_t i = 0; i < 5; i++) {
        s = (s * 29943829) - 1;
        x[i] = s;
      }

      // Randomise some more
      //for (size_t i = 0; i < 19; i++) GetRandomNumber();
    }
  }
}

#ifdef BUILD_SPITFIRE_UNITTEST

#include <spitfire/util/unittest.h>

class cMathMathUnitTest : protected spitfire::util::cUnitTestBase
{
public:
  cMathMathUnitTest () :
    cUnitTestBase(TEXT("cMathMathUnitTest"))
  {
  }

void TestInteger(float fValue, int iExpectedInteger)
{
   const int iInteger = spitfire::math::GetIntegerPart(fValue);
   ASSERT_TRUE(iInteger == iExpectedInteger);
}

void TestIntegerAndFraction(float fValue, int iExpectedInteger, float fApproximateExpectedFraction)
{
   int iInteger = 0;
   float fFraction = 0;
   spitfire::math::GetIntegerAndFractionParts(fValue, iInteger, fFraction);
   ASSERT_TRUE(iInteger == iExpectedInteger);
   ASSERT_TRUE(spitfire::math::IsApproximatelyEqual(fFraction, fApproximateExpectedFraction));
}

virtual void Test() override
{
   TestInteger(-1.0f, -1);
   TestInteger(0.0f, 0);
   TestInteger(1.000001f, 1);
   TestInteger(1.51f, 1);
   TestInteger(1.9999f, 1);
   TestInteger(2.0f, 2);

   TestIntegerAndFraction(-1.0f, -1, 0.0f);
   TestIntegerAndFraction(0.0f, 0, 0.0f);
   TestIntegerAndFraction(1.000001f, 1, 0.0f);
   TestIntegerAndFraction(1.51f, 1, 0.51f);
   TestIntegerAndFraction(1.9999f, 1, 0.9999f);
   TestIntegerAndFraction(2.0f, 2, 0.0f);
}

};

cMathMathUnitTest gMathMathUnitTest;

#endif // BUILD_SPITFIRE_UNITTEST
