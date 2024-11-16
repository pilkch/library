#include <iostream>
#include <cmath>

#include <limits>

#include <gtest/gtest.h>

// Spitfire Includes
#include <spitfire/math/cCurve.h>

TEST(SpitfireMath, TestCurve)
{
  spitfire::math::cCurve curve;
  curve.AddPoint(0.0f, 22.5f); // 22.5 Nm at 0 RPM
  curve.AddPoint(1000.0f, 21.0f); // 21.0 Nm at 1000 RPM
  curve.AddPoint(3000.0f, 9.0f); // 21.0 Nm at 1000 RPM
  curve.AddPoint(3600.0f, 0.0f); // 0 Nm for 3600 RPM and greater

  EXPECT_NEAR(22.5f, curve.GetYAtPointX(0.0f), 0.001f);
  EXPECT_NEAR(22.35f, curve.GetYAtPointX(100.0f), 0.001f);
  EXPECT_NEAR(21.75f, curve.GetYAtPointX(500.0f), 0.001f);
  EXPECT_NEAR(21.0f, curve.GetYAtPointX(1000.0f), 0.001f);
  EXPECT_NEAR(19.5f, curve.GetYAtPointX(1250.0f), 0.001f);
  EXPECT_NEAR(18.0f, curve.GetYAtPointX(1500.0f), 0.001f);
  EXPECT_NEAR(16.5f, curve.GetYAtPointX(1750.0f), 0.001f);
  EXPECT_NEAR(15.0f, curve.GetYAtPointX(2000.0f), 0.001f);
  EXPECT_NEAR(9.0f, curve.GetYAtPointX(3000.0f), 0.001f);
  EXPECT_NEAR(7.5f, curve.GetYAtPointX(3100.0f), 0.001f);
  EXPECT_NEAR(6.0f, curve.GetYAtPointX(3200.0f), 0.001f);
  EXPECT_NEAR(4.5f, curve.GetYAtPointX(3300.0f), 0.001f);
  EXPECT_NEAR(3.0f, curve.GetYAtPointX(3400.0f), 0.001f);
  EXPECT_NEAR(1.5f, curve.GetYAtPointX(3500.0f), 0.001f);
  EXPECT_NEAR(0.0f, curve.GetYAtPointX(3600.0f), 0.001f);
  EXPECT_NEAR(0.0f, curve.GetYAtPointX(3700.0f), 0.001f);
  EXPECT_NEAR(0.0f, curve.GetYAtPointX(5000.0f), 0.001f);
}
