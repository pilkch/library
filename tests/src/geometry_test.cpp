#include <cstdlib>
#include <cmath>

#include <vector>
#include <list>
#include <string>
#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>

#include <gtest/gtest.h>

// Spitfire Includes
#include <spitfire/spitfire.h>

#include <spitfire/util/string.h>
#include <spitfire/util/log.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/geometry.h>

TEST(SpitfireMath, TestGeometrySphere)
{
  spitfire::math::cSphere a;
  a.position.Set(3.0f, 1.0f, 1.0f);
  a.SetRadius(1.0f);

  spitfire::math::cSphere b;
  b.position.Set(7.0f, 1.0f, 1.0f);
  b.SetRadius(1.0f);

  float f = 0.0f;

  // Should be approximately 2.0f
  f = a.GetDistance(b);
  ASSERT_TRUE(spitfire::math::IsApproximatelyEqual(f, 2.0f));

  // Should be approximately 4.0f
  f = a.GetDistanceCentreToCentre(b);
  ASSERT_TRUE(spitfire::math::IsApproximatelyEqual(f, 4.0f));


  // Should not collide
  a.SetRadius(2.9f);
  ASSERT_TRUE(!a.Collide(b));

  // Should collide
  a.SetRadius(3.5f);
  ASSERT_TRUE(a.Collide(b));

  // Should collide
  a.SetRadius(4.0f);
  ASSERT_TRUE(a.Collide(b));

  // Should collide
  a.SetRadius(4.5f);
  ASSERT_TRUE(a.Collide(b));

  // Should collide
  a.SetRadius(100.0f);
  ASSERT_TRUE(a.Collide(b));
}



TEST(SpitfireMath, TestGeometryLineGenerator)
{
  {
    spitfire::math::cLineGenerator generator;
    generator.GenerateLine(-10, -10, 10, 10);

    std::vector<spitfire::math::cVec2> points;

    points.push_back(spitfire::math::cVec2(-10, -10));
    points.push_back(spitfire::math::cVec2(-9, -9));
    points.push_back(spitfire::math::cVec2(-8, -8));
    points.push_back(spitfire::math::cVec2(-7, -7));
    points.push_back(spitfire::math::cVec2(-6, -6));
    points.push_back(spitfire::math::cVec2(-5, -5));
    points.push_back(spitfire::math::cVec2(-4, -4));
    points.push_back(spitfire::math::cVec2(-3, -3));
    points.push_back(spitfire::math::cVec2(-2, -2));
    points.push_back(spitfire::math::cVec2(-1, -1));
    points.push_back(spitfire::math::cVec2(0, 0));
    points.push_back(spitfire::math::cVec2(1, 1));
    points.push_back(spitfire::math::cVec2(2, 2));
    points.push_back(spitfire::math::cVec2(3, 3));
    points.push_back(spitfire::math::cVec2(4, 4));
    points.push_back(spitfire::math::cVec2(5, 5));
    points.push_back(spitfire::math::cVec2(6, 6));
    points.push_back(spitfire::math::cVec2(7, 7));
    points.push_back(spitfire::math::cVec2(8, 8));
    points.push_back(spitfire::math::cVec2(9, 9));
    points.push_back(spitfire::math::cVec2(10, 10));

    // NOTE: This only works because we have specified exact coordinates
    const std::vector<spitfire::math::cVec2> generated = generator.GetPoints();
    ASSERT_EQ(points.size(), generated.size());

    for (size_t i = 0; i < generated.size(); i++) {
      ASSERT_FLOAT_EQ(generated[i].x, points[i].x);
      ASSERT_FLOAT_EQ(generated[i].y, points[i].y);
    }
  }
  {
    spitfire::math::cLineGenerator generator;
    generator.GenerateLine(10, 10, -10, -10);

    std::vector<spitfire::math::cVec2> points;

    points.push_back(spitfire::math::cVec2(-10, -10));
    points.push_back(spitfire::math::cVec2(-9, -9));
    points.push_back(spitfire::math::cVec2(-8, -8));
    points.push_back(spitfire::math::cVec2(-7, -7));
    points.push_back(spitfire::math::cVec2(-6, -6));
    points.push_back(spitfire::math::cVec2(-5, -5));
    points.push_back(spitfire::math::cVec2(-4, -4));
    points.push_back(spitfire::math::cVec2(-3, -3));
    points.push_back(spitfire::math::cVec2(-2, -2));
    points.push_back(spitfire::math::cVec2(-1, -1));
    points.push_back(spitfire::math::cVec2(0, 0));
    points.push_back(spitfire::math::cVec2(1, 1));
    points.push_back(spitfire::math::cVec2(2, 2));
    points.push_back(spitfire::math::cVec2(3, 3));
    points.push_back(spitfire::math::cVec2(4, 4));
    points.push_back(spitfire::math::cVec2(5, 5));
    points.push_back(spitfire::math::cVec2(6, 6));
    points.push_back(spitfire::math::cVec2(7, 7));
    points.push_back(spitfire::math::cVec2(8, 8));
    points.push_back(spitfire::math::cVec2(9, 9));
    points.push_back(spitfire::math::cVec2(10, 10));

    // NOTE: This only works because we have specified exact coordinates
    const std::vector<spitfire::math::cVec2> generated = generator.GetPoints();
    ASSERT_EQ(points.size(), generated.size());

    for (size_t i = 0; i < generated.size(); i++) {
      ASSERT_FLOAT_EQ(generated[i].x, points[i].x);
      ASSERT_FLOAT_EQ(generated[i].y, points[i].y);
    }
  }
  {
    spitfire::math::cLineGenerator generator;
    generator.GenerateLine(0, 0, 10, 5);

    std::vector<spitfire::math::cVec2> points;

    points.push_back(spitfire::math::cVec2(0, 0));
    points.push_back(spitfire::math::cVec2(1, 0));
    points.push_back(spitfire::math::cVec2(2, 1));
    points.push_back(spitfire::math::cVec2(3, 1));
    points.push_back(spitfire::math::cVec2(4, 2));
    points.push_back(spitfire::math::cVec2(5, 2));
    points.push_back(spitfire::math::cVec2(6, 3));
    points.push_back(spitfire::math::cVec2(7, 3));
    points.push_back(spitfire::math::cVec2(8, 4));
    points.push_back(spitfire::math::cVec2(9, 4));
    points.push_back(spitfire::math::cVec2(10, 5));

    // NOTE: This only works because we have specified exact coordinates
    const std::vector<spitfire::math::cVec2> generated = generator.GetPoints();
    ASSERT_EQ(points.size(), generated.size());

    for (size_t i = 0; i < generated.size(); i++) {
      ASSERT_FLOAT_EQ(generated[i].x, points[i].x);
      ASSERT_FLOAT_EQ(generated[i].y, points[i].y);
    }
  }
  {
    spitfire::math::cLineGenerator generator;
    generator.GenerateLine(0, 0, 5, 10);

    std::vector<spitfire::math::cVec2> points;

    points.push_back(spitfire::math::cVec2(0, 0));
    points.push_back(spitfire::math::cVec2(0, 1));
    points.push_back(spitfire::math::cVec2(1, 2));
    points.push_back(spitfire::math::cVec2(1, 3));
    points.push_back(spitfire::math::cVec2(2, 4));
    points.push_back(spitfire::math::cVec2(2, 5));
    points.push_back(spitfire::math::cVec2(3, 6));
    points.push_back(spitfire::math::cVec2(3, 7));
    points.push_back(spitfire::math::cVec2(4, 8));
    points.push_back(spitfire::math::cVec2(4, 9));
    points.push_back(spitfire::math::cVec2(5, 10));

    // NOTE: This only works because we have specified exact coordinates
    const std::vector<spitfire::math::cVec2> generated = generator.GetPoints();
    ASSERT_EQ(points.size(), generated.size());

    for (size_t i = 0; i < generated.size(); i++) {
      ASSERT_FLOAT_EQ(generated[i].x, points[i].x);
      ASSERT_FLOAT_EQ(generated[i].y, points[i].y);
    }
  }
}
