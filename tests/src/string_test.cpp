// Standard includes
#include <cassert>
#include <cctype>
#include <cmath>

#include <cstring>
#include <string>
#include <codecvt>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <sstream>
#include <iterator>    // for back_inserter

#include <iostream>
#include <fstream>
#include <iomanip>

#include <locale>

#ifdef PLATFORM_LINUX_OR_UNIX
#include <iconv.h>
#endif
#include <errno.h>

#include <gtest/gtest.h>

// Spitfire includes
#include <spitfire/spitfire.h>
#include <spitfire/util/string.h>
#include <spitfire/util/log.h>

TEST(SpitfireString, TestUpperLower)
{
  // Upper and lower case conversions
  EXPECT_STREQ("ABCDEFGHIJKLMNOPQRSTUVWXYZ", spitfire::string::ToUpper("abcdefghijklmnopqrstuvwxyz").c_str());
  EXPECT_STREQ("ABCDEFGHIJKLMNOPQRSTUVWXYZ", spitfire::string::ToUpper("ABCDEFGHIJKLMNOPQRSTUVWXYZ").c_str());
  EXPECT_STREQ("abcdefghijklmnopqrstuvwxyz", spitfire::string::ToLower("abcdefghijklmnopqrstuvwxyz").c_str());
  EXPECT_STREQ("abcdefghijklmnopqrstuvwxyz", spitfire::string::ToLower("ABCDEFGHIJKLMNOPQRSTUVWXYZ").c_str());
}

TEST(SpitfireString, TestIECHumanReadableByteSizes)
{
  // IEC Byte Size Formats

  EXPECT_STREQ(TEXT("0 Bytes"), spitfire::string::GetIECStringFromBytes(0).c_str());
  EXPECT_STREQ(TEXT("1023 Bytes"), spitfire::string::GetIECStringFromBytes(1023).c_str());

  EXPECT_STREQ(TEXT("1 KiB"), spitfire::string::GetIECStringFromBytes(1024).c_str());
  EXPECT_STREQ(TEXT("1023 KiB"), spitfire::string::GetIECStringFromBytes(1048575).c_str());

  EXPECT_STREQ(TEXT("1 MiB"), spitfire::string::GetIECStringFromBytes(1048576).c_str());
  EXPECT_STREQ(TEXT("1023 MiB"), spitfire::string::GetIECStringFromBytes(1073741823).c_str());

  EXPECT_STREQ(TEXT("1 GiB"), spitfire::string::GetIECStringFromBytes(1073741824).c_str());
  EXPECT_STREQ(TEXT("1023 GiB"), spitfire::string::GetIECStringFromBytes(1099511627775).c_str());

  EXPECT_STREQ(TEXT("1 TiB"), spitfire::string::GetIECStringFromBytes(1099511627776).c_str());
  EXPECT_STREQ(TEXT("1023 TiB"), spitfire::string::GetIECStringFromBytes(1125899906842623).c_str());

  EXPECT_STREQ(TEXT("1 PiB"), spitfire::string::GetIECStringFromBytes(1125899906842624).c_str());
  EXPECT_STREQ(TEXT("1023 PiB"), spitfire::string::GetIECStringFromBytes(1152921504606846975).c_str());

  EXPECT_STREQ(TEXT("1 EiB"), spitfire::string::GetIECStringFromBytes(1152921504606846976).c_str());
}

TEST(SpitfireString, TestHex)
{
  // Hex conversions

  EXPECT_EQ(0x0, spitfire::string::FromHexStringToUint32_t(TEXT("0")));
  EXPECT_STREQ(TEXT("0000"), spitfire::string::ToHexString(uint16_t(0x0)).c_str());
  EXPECT_STREQ(TEXT("00000000"), spitfire::string::ToHexString(uint32_t(0x0)).c_str());
  EXPECT_STREQ(TEXT("000000"), spitfire::string::ToHexString(0x0, 0x0, 0x0).c_str());
  EXPECT_STREQ(TEXT("00000000"), spitfire::string::ToHexString(0x0, 0x0, 0x0, 0x0).c_str());

  EXPECT_EQ(0xFFFFFFFF, spitfire::string::FromHexStringToUint32_t(TEXT("FFFFffff")));
  EXPECT_STREQ(TEXT("00FF"), spitfire::string::ToHexString(uint16_t(0xFF)).c_str());
  EXPECT_STREQ(TEXT("000000FF"), spitfire::string::ToHexString(uint32_t(0xFF)).c_str());
  EXPECT_STREQ(TEXT("FFFFFF"), spitfire::string::ToHexString(0xFF, 0xFF, 0xFF).c_str());
  EXPECT_STREQ(TEXT("FFFFFFFF"), spitfire::string::ToHexString(0xFF, 0xFF, 0xFF, 0xFF).c_str());

  EXPECT_EQ(0x12345678, spitfire::string::FromHexStringToUint32_t(TEXT("12345678")));
  EXPECT_STREQ(TEXT("12345678"), spitfire::string::ToHexString(uint32_t(0x12345678)).c_str());
  EXPECT_STREQ(TEXT("123456"), spitfire::string::ToHexString(0x12, 0x34, 0x56).c_str());
  EXPECT_STREQ(TEXT("12345678"), spitfire::string::ToHexString(0x12, 0x34, 0x56, 0x78).c_str());

  // Check leading zeroes on hex numbers
  EXPECT_STREQ(TEXT("0008"), spitfire::string::ToHexString(uint16_t(0x0008)).c_str());
  EXPECT_STREQ(TEXT("0078"), spitfire::string::ToHexString(uint16_t(0x0078)).c_str());
  EXPECT_STREQ(TEXT("0678"), spitfire::string::ToHexString(uint16_t(0x0678)).c_str());
  EXPECT_STREQ(TEXT("5678"), spitfire::string::ToHexString(uint16_t(0x5678)).c_str());

  EXPECT_STREQ(TEXT("00000008"), spitfire::string::ToHexString(uint32_t(0x00000008)).c_str());
  EXPECT_STREQ(TEXT("00000078"), spitfire::string::ToHexString(uint32_t(0x00000078)).c_str());
  EXPECT_STREQ(TEXT("00000678"), spitfire::string::ToHexString(uint32_t(0x00000678)).c_str());
  EXPECT_STREQ(TEXT("00005678"), spitfire::string::ToHexString(uint32_t(0x00005678)).c_str());
  EXPECT_STREQ(TEXT("00045678"), spitfire::string::ToHexString(uint32_t(0x00045678)).c_str());
  EXPECT_STREQ(TEXT("00345678"), spitfire::string::ToHexString(uint32_t(0x00345678)).c_str());
  EXPECT_STREQ(TEXT("02345678"), spitfire::string::ToHexString(uint32_t(0x02345678)).c_str());
  EXPECT_STREQ(TEXT("12345678"), spitfire::string::ToHexString(uint32_t(0x12345678)).c_str());
}
