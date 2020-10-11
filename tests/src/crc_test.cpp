// Standard headers
#include <array>
#include <iostream>
#include <fstream>

// Spitfire headers
#include <spitfire/algorithm/crc.h>

// gtest headers
#include <gtest/gtest.h>

namespace {

template<class T>
spitfire::string_t CalculateCRC(const std::string sText)
{
  spitfire::string_t result;

  T crc;
  crc.CalculateForString(sText.c_str(), result);

  // Make sure that the text encodes as expected
  //std::cout<<"TestCRC result="<<result<<std::endl;
  return result;
}

}

TEST(Spitfire, TestCRC)
{
  // These have been double checked against https://crccalc.com/
  EXPECT_STREQ(TEXT("29B1"), CalculateCRC<spitfire::cCRC16>("123456789").c_str());
  EXPECT_STREQ(TEXT("53E2"), CalculateCRC<spitfire::cCRC16>("abcdefghijklmnopqrstuvwxyz").c_str());
  EXPECT_STREQ(TEXT("CBF43926"), CalculateCRC<spitfire::cCRC32>("123456789").c_str());
  EXPECT_STREQ(TEXT("4C2750BD"), CalculateCRC<spitfire::cCRC32>("abcdefghijklmnopqrstuvwxyz").c_str());
}
