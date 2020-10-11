#include <cstdlib>
#include <cmath>

#include <vector>
#include <limits>

#include <gtest/gtest.h>

// Spitfire Includes
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>

#ifdef max
#undef max
#endif

namespace {

void TestInteger(float fValue, int iExpectedInteger)
{
   const int iInteger = spitfire::math::GetIntegerPart(fValue);
   ASSERT_EQ(iExpectedInteger, iInteger);
}

void TestIntegerAndFraction(float fValue, int iExpectedInteger, float fApproximateExpectedFraction)
{
   int iInteger = 0;
   float fFraction = 0;
   spitfire::math::GetIntegerAndFractionParts(fValue, iInteger, fFraction);
   ASSERT_EQ(iExpectedInteger, iInteger);
   ASSERT_TRUE(spitfire::math::IsApproximatelyEqual(fFraction, fApproximateExpectedFraction));
}

}

TEST(SpitfireMath, TestMath)
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
