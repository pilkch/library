#include <cstdlib>
#include <cmath>

#include <vector>
#include <limits>

#include <gtest/gtest.h>

// Spitfire Includes
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cMat4.h>

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


TEST(SpitfireMath, Test_cMat4)
{
   spitfire::math::cMat4 mat;

   // Expect identify matrix
   EXPECT_FLOAT_EQ(1.0f, mat[0]);  EXPECT_FLOAT_EQ(0.0f, mat[1]);  EXPECT_FLOAT_EQ(0.0f, mat[2]);  EXPECT_FLOAT_EQ(0.0f, mat[3]);
   EXPECT_FLOAT_EQ(0.0f, mat[4]);  EXPECT_FLOAT_EQ(1.0f, mat[5]);  EXPECT_FLOAT_EQ(0.0f, mat[6]);  EXPECT_FLOAT_EQ(0.0f, mat[7]);
   EXPECT_FLOAT_EQ(0.0f, mat[8]);  EXPECT_FLOAT_EQ(0.0f, mat[9]);  EXPECT_FLOAT_EQ(1.0f, mat[10]); EXPECT_FLOAT_EQ(0.0f, mat[11]);
   EXPECT_FLOAT_EQ(0.0f, mat[12]); EXPECT_FLOAT_EQ(0.0f, mat[13]); EXPECT_FLOAT_EQ(0.0f, mat[14]); EXPECT_FLOAT_EQ(1.0f, mat[15]);

   // Check that we can set and get values
   for (size_t i = 0; i < 16; i++) {
      mat[i] = float(i);
      EXPECT_FLOAT_EQ(float(i), mat[i]);
   }

   // Check that we can set and get values with x,y positions
   // NOTE: Column major to match glm
   for (size_t y = 0; y < 4; y++) {
      for (size_t x = 0; x < 4; x++) {
         const size_t i = ((x * 4) + y);
         const float value = float(100 + i);
         mat.SetValue(x, y, value);
         EXPECT_FLOAT_EQ(value, mat.GetValue(x, y));
         EXPECT_FLOAT_EQ(value, mat[i]);
      }
   }
}
