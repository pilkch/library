// Standard headers
#include <iostream>
#include <sstream>

// Spitfire headers
#include <spitfire/algorithm/algorithm.h>


#ifdef BUILD_SPITFIRE_UNITTEST

#include <spitfire/util/unittest.h>

class cAlgorithmUnitTest : protected spitfire::util::cUnitTestBase
{
public:
  cAlgorithmUnitTest() :
    cUnitTestBase(TEXT("cAlgorithmUnitTest"))
  {
  }

  void TestVector()
  {
    std::vector<int> v;

    // Add 7, 7, 7
    spitfire::vector::push_back(v, 3, 7);

    ASSERT_TRUE(v.size() == 3);
    ASSERT_TRUE(v == std::vector<int>( { 7, 7, 7 } ));

    // Add 5, 5
    spitfire::vector::push_back(v, 2, 5);

    ASSERT_TRUE(v.size() == 5);
    ASSERT_TRUE(v == std::vector<int>( { 7, 7, 7, 5, 5 } ));
  }

  static bool CompareWithUserData(int lhs, int rhs, bool& userData)
  {
    userData = true;
    return lhs < rhs;
  }

  void TestSortWithUserData()
  {
    std::vector<int> v = { 8, 4, 5, 2, 8, 0, 9, 3, 7 };
    bool calledCompare = false;
    spitfire::algorithm::SortWithUserData(v.begin(), v.end(), CompareWithUserData, calledCompare);
    ASSERT_TRUE(calledCompare);
    ASSERT_TRUE(v == std::vector<int>({ 0, 2, 3, 4, 5, 7, 8, 8, 9 }));
  }

  void TestContainer2D()
  {
    spitfire::cContainer2D<int, 5, 10> values;

    ASSERT_TRUE(values.GetWidth() == 5);
    ASSERT_TRUE(values.GetHeight() == 10);
    ASSERT_TRUE(values.size() == 50);

    for (size_t i = 0; i < 50; i++) values[i] = i;

    for (size_t i = 0; i < 50; i++) {
      ASSERT_TRUE(values[i] == int(i));
      ASSERT_TRUE(values.GetElement(i) == int(i));
      const size_t x = i % 5;
      const size_t y = i / 5;
      ASSERT_TRUE(values.GetElement(x, y) == int(i));
    }
  }

  void CheckDynamicContainer2D(spitfire::cDynamicContainer2D<int>& values, size_t width, size_t height)
  {
    const size_t n = width * height;

    values.RemoveAllEntriesAndSetNewSize(width, height);

    ASSERT_TRUE(values.GetWidth() == width);
    ASSERT_TRUE(values.GetHeight() == height);
    ASSERT_TRUE(values.size() == n);

    for (size_t i = 0; i < n; i++) values[i] = i;

    for (size_t i = 0; i < n; i++) {
      ASSERT_TRUE(values[i] == int(i));
      ASSERT_TRUE(values.GetElement(i) == int(i));
      const size_t x = i % width;
      const size_t y = i / width;
      ASSERT_TRUE(values.GetElement(x, y) == int(i));
    }
  }

  void TestDynamicContainer2D()
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

  void TestRandomBucket()
  {
    spitfire::cRandomBucket<int> bucket;
    bucket.AddItem(1);

    ASSERT_TRUE(bucket.GetPossibleItems().size() == 1);

    // This shouldn't do anything
    bucket.ResetCurrentPool();

    for (size_t i = 0; i < 20; i++) {
      ASSERT_TRUE(bucket.GetRandomItem() == 1);
    }

    // This shouldn't do anything
    bucket.ResetCurrentPool();

    bucket.AddItem(2);
    bucket.AddItem(3);
    bucket.AddItem(4);
    bucket.AddItem(5);

    // This shouldn't do anything
    bucket.ResetCurrentPool();

    ASSERT_TRUE(bucket.GetPossibleItems().size() == 5);

    // Add 8 three times
    bucket.AddItems(8, 3);

    ASSERT_TRUE(bucket.GetPossibleItems().size() == 8);

    for (size_t i = 0; i < 20; i++) {
      bucket.GetRandomItem();
    }

    bucket.ResetCurrentPool();

    bucket.ClearPossibleItemsAndResetCurrentPool();

    ASSERT_TRUE(bucket.GetPossibleItems().size() == 0);
  }

  void TestBinaryCodedDecimal()
  {
    // Conversion from 12 to 0x12 for example
    for (uint8_t i = 0; i < 255; i++) {
      const uint8_t x = spitfire::algorithm::ToBinaryCodedDecimal(i);

      std::stringstream ss;
      ss<<std::hex<<int(x);
      uint32_t y = 0;
      ss>>y;

      //std::cout<<"i="<<int(i)<<", ss="<<ss.str()<<", y="<<y<<", x="<<int(x)<<std::endl;
      ASSERT_TRUE(y == uint32_t(x));
    }

    // Conversion from 0x12 to 12 for example
    for (uint8_t i = 0; i < 128; i++) {
      ASSERT_TRUE(spitfire::algorithm::FromBinaryCodedDecimal(spitfire::algorithm::ToBinaryCodedDecimal(i)) == i);
    }
  }

  void Test()
  {
    TestVector();
    TestSortWithUserData();
    TestContainer2D();
    TestDynamicContainer2D();
    TestRandomBucket();
    TestBinaryCodedDecimal();
  }
};

cAlgorithmUnitTest gAlgorithmUnitTest;

#endif // BUILD_SPITFIRE_UNITTEST
