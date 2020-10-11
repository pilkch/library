// gtest headers
#include <gtest/gtest.h>

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);

  // The settings functions haven't been implemented yet
  testing::GTEST_FLAG(filter) = "-SpitfireStorage.TestSettings";

  return RUN_ALL_TESTS();
}
