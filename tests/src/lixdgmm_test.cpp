// Standard includes
#include <cassert>
#include <cctype>
#include <cmath>

#include <cstring>
#include <string>
#include <iostream>

#include <gtest/gtest.h>

// libxdgmm headers
#include <libxdgmm/libxdgmm.h>

TEST(LibXDGMM, TestLibXDGMM)
{
  xdg::cXdg x;

  ASSERT_TRUE(x.IsValid());

  const char* szHome = getenv("HOME");
  ASSERT_TRUE(szHome != nullptr);

  const std::string sHome = szHome;

  const std::string sHomeDirectory = x.GetHomeDirectory();
  EXPECT_STREQ(sHomeDirectory.c_str(), sHome.c_str());

  const std::string sHomeDataDirectory = x.GetHomeDataDirectory();
  EXPECT_STREQ(sHomeDataDirectory.c_str(), (sHome + "/.local/share").c_str());

  const std::string sHomeConfigDirectory = x.GetHomeConfigDirectory();
  EXPECT_STREQ(sHomeConfigDirectory.c_str(), (sHome + "/.config").c_str());
}
