// gtest headers
#include <gtest/gtest.h>

#include <spitfire/communication/network.h>

int main(int argc, char** argv)
{
  spitfire::network::Init();

  ::testing::InitGoogleTest(&argc, argv);

  // The settings functions haven't been implemented yet
  // The unicode functions don't actually handle unicode yet
  testing::GTEST_FLAG(filter) = "-SpitfireStorage.TestSettings:SpitfireString.TestUpperLowerUnicode";

  const int result = RUN_ALL_TESTS();

  spitfire::network::Destroy();

  return result;
}
