// Standard headers
#include <iostream>
#include <sstream>

#include <gtest/gtest.h>

// Spitfire headers
#include <spitfire/algorithm/algorithm.h>
#include <spitfire/math/math.h>

namespace {
  
bool CompareWithUserData(int lhs, int rhs, bool& userData)
{
  userData = true;
  return lhs < rhs;
}

}


TEST(SpitfireAlgorithm, TestVector)
{
  std::vector<int> v;

  // Add 7, 7, 7
  spitfire::vector::push_back(v, 3, 7);

  ASSERT_EQ(3, v.size());
  ASSERT_TRUE(v == std::vector<int>( { 7, 7, 7 }));

  // Add 5, 5
  spitfire::vector::push_back(v, 2, 5);

  ASSERT_EQ(5, v.size());
  ASSERT_TRUE(v == std::vector<int>( { 7, 7, 7, 5, 5 } ));
}

TEST(SpitfireAlgorithm, TestSortWithUserData)
{
  std::vector<int> v = { 8, 4, 5, 2, 8, 0, 9, 3, 7 };
  bool calledCompare = false;
  spitfire::algorithm::SortWithUserData(v.begin(), v.end(), CompareWithUserData, calledCompare);
  ASSERT_TRUE(calledCompare);
  ASSERT_TRUE(v == std::vector<int>({ 0, 2, 3, 4, 5, 7, 8, 8, 9 }));
}

TEST(SpitfireAlgorithm, TestContainer2D)
{
  spitfire::cContainer2D<int, 5, 10> values;

  ASSERT_EQ(5, values.GetWidth());
  ASSERT_EQ(10, values.GetHeight());
  ASSERT_EQ(50, values.size());

  for (size_t i = 0; i < 50; i++) values[i] = int(i);

  for (size_t i = 0; i < 50; i++) {
    ASSERT_EQ(int(i), values[i]);
    ASSERT_EQ(int(i), values.GetElement(i));
    const size_t x = i % 5;
    const size_t y = i / 5;
    ASSERT_EQ(int(i), values.GetElement(x, y));
  }
}

void CheckDynamicContainer2D(spitfire::cDynamicContainer2D<int>& values, size_t width, size_t height)
{
  const size_t n = width * height;

  values.RemoveAllEntriesAndSetNewSize(width, height);

  ASSERT_EQ(width, values.GetWidth());
  ASSERT_EQ(height, values.GetHeight());
  ASSERT_EQ(n, values.size());

  for (size_t i = 0; i < n; i++) values[i] = int(i);

  for (size_t i = 0; i < n; i++) {
    ASSERT_EQ(int(i), values[i]);
    ASSERT_EQ(int(i), values.GetElement(i));
    const size_t x = i % width;
    const size_t y = i / width;
    ASSERT_EQ(int(i), values.GetElement(x, y));
  }
}

TEST(SpitfireAlgorithm, TestDynamicContainer2D)
{
  spitfire::cDynamicContainer2D<int> values(5, 10);

  // First width and height
  CheckDynamicContainer2D(values, 5, 10);

  // Resize to another width and height
  {
    const size_t width = 9;
    const size_t height = 8;

    values.RemoveAllEntriesAndSetNewSize(width, height);

    CheckDynamicContainer2D(values, width, height);
  }
}

TEST(SpitfireAlgorithm, TestRandomBucket)
{
  spitfire::cRandomBucket<int> bucket;
  bucket.AddItem(1);

  ASSERT_EQ(1, bucket.GetPossibleItems().size());

  // This shouldn't do anything
  bucket.ResetCurrentPool();

  spitfire::math::cRand rng;

  for (size_t i = 0; i < 20; i++) {
    ASSERT_EQ(1, bucket.GetRandomItem(rng));
  }

  // This shouldn't do anything
  bucket.ResetCurrentPool();

  bucket.AddItem(2);
  bucket.AddItem(3);
  bucket.AddItem(4);
  bucket.AddItem(5);

  // This shouldn't do anything
  bucket.ResetCurrentPool();

  ASSERT_EQ(5, bucket.GetPossibleItems().size());

  // Add 8 three times
  bucket.AddItems(8, 3);

  ASSERT_EQ(8, bucket.GetPossibleItems().size());

  for (size_t i = 0; i < 20; i++) {
    bucket.GetRandomItem(rng);
  }

  bucket.ResetCurrentPool();

  bucket.ClearPossibleItemsAndResetCurrentPool();

  ASSERT_EQ(0, bucket.GetPossibleItems().size());
}

TEST(SpitfireAlgorithm, TestBinaryCodedDecimal)
{
  // Conversion from 12 to 0x12 for example
  for (uint8_t i = 0; i < 255; i++) {
    const uint8_t x = spitfire::algorithm::ToBinaryCodedDecimal(i);

    std::stringstream ss;
    ss<<std::hex<<int(x);
    uint32_t y = 0;
    ss>>y;

    //std::cout<<"i="<<int(i)<<", ss="<<ss.str()<<", y="<<y<<", x="<<int(x)<<std::endl;
    ASSERT_EQ(uint32_t(x), y);
  }

  // Conversion from 0x12 to 12 for example
  for (uint8_t i = 0; i < 128; i++) {
    ASSERT_EQ(i, spitfire::algorithm::FromBinaryCodedDecimal(spitfire::algorithm::ToBinaryCodedDecimal(i)));
  }
}
