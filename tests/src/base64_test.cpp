#include <cassert>
#include <cctype> // for toupper/tolower

#include <string>
#include <sstream>
#include <iterator>    // for back_inserter

#include <iostream>
#include <fstream>
#include <iomanip>

#include <gtest/gtest.h>

#include <spitfire/spitfire.h>
#include <spitfire/algorithm/base64.h>

TEST(SpitfireAlgorithm, TestBase64)
{
  // Test encode
  ASSERT_EQ("YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXo=", spitfire::algorithm::Base64Encode("abcdefghijklmnopqrstuvwxyz"));
  ASSERT_EQ("Z2lyYWZmZQ==", spitfire::algorithm::Base64Encode("giraffe"));
  ASSERT_EQ("QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVphYmNkZWZnaGlqa2xtbm9wcXJzdHV2d3h5ejAxMjM0NTY3ODkrLw==", spitfire::algorithm::Base64Encode("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"));

  // Test decode
  ASSERT_EQ("abcdefghijklmnopqrstuvwxyz", spitfire::algorithm::Base64Decode("YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXo="));
  ASSERT_EQ("giraffe", spitfire::algorithm::Base64Decode("Z2lyYWZmZQ=="));
  ASSERT_EQ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/", spitfire::algorithm::Base64Decode("QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVphYmNkZWZnaGlqa2xtbm9wcXJzdHV2d3h5ejAxMjM0NTY3ODkrLw=="));
}
