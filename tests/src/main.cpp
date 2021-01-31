// gtest headers
#include <gtest/gtest.h>

#include <gnutls/gnutls.h>

int main(int argc, char** argv)
{
  gnutls_global_init();

  ::testing::InitGoogleTest(&argc, argv);

  // The settings functions haven't been implemented yet
  // The unicode functions don't actually handle unicode yet
  testing::GTEST_FLAG(filter) = "-SpitfireStorage.TestSettings:SpitfireString.TestUpperLowerUnicode";

  const int result = RUN_ALL_TESTS();

  gnutls_global_deinit();

  return result;
}
