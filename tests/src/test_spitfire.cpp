// Standard headers
#include <cassert>
#include <cmath>

#include <string>
#include <iostream>
#include <sstream>

#include <algorithm>
#include <map>
#include <vector>
#include <list>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/algorithm/algorithm.h>
#include <spitfire/algorithm/base64.h>
#include <spitfire/algorithm/crc.h>
#include <spitfire/algorithm/md5.h>

#include <spitfire/util/log.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cColour.h>

// Unit test headers
#include "main.h"

void Test_Spitfire_Spitfire()
{
  // Test countof
  {
    char text[14];
    ASSERT_TRUE(countof(text) == 14);

    int stuff[9];
    ASSERT_TRUE(countof(stuff) == 9);
  }

  // Test SAFE_DELETE/SAFE_DELETE_ARRAY
  {
    int* x = new int;
    spitfire::SAFE_DELETE(x);
    ASSERT_TRUE(x == nullptr);

    int* y = new int[10];
    spitfire::SAFE_DELETE_ARRAY(y);
    ASSERT_TRUE(y == nullptr);
  }
}

void Test_Spitfire_Algorithm()
{
  const std::string text = "abcdefghijklmnopqrstuvwxyz";

  // Test Base64
  {
    const std::string result = spitfire::algorithm::Base64Encode(text);
    ASSERT_TRUE(result == "YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXo=");
    ASSERT_TRUE(spitfire::algorithm::Base64Decode(result) == text);
  }

  // Test MD5
  {
    spitfire::algorithm::cMD5 md5;
    md5.CalculateForString(text.c_str());

    //std::cout<<"result="<<md5.GetResultFormatted()<<std::endl;
    //std::cout<<"expect="<<"c3fcd3d76192e4007dfb496cca67e13b"<<std::endl;
    ASSERT_TRUE(md5.GetResultFormatted() == "c3fcd3d76192e4007dfb496cca67e13b");
  }
}

void Test_Spitfire()
{
  Test_Spitfire_Spitfire();
  Test_Spitfire_Algorithm();
}
