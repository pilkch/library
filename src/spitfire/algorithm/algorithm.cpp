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

  void Test()
  {
    TestVector();
    TestSortWithUserData();
    TestContainer2D();
  }
};

cAlgorithmUnitTest gAlgorithmUnitTest;

#endif // BUILD_SPITFIRE_UNITTEST
