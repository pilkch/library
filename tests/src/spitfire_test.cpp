// Standard headers
#include <time.h>

#include <cassert>
#include <cmath>
#include <ctime>

#include <vector>
#include <list>

#include <string>
#include <sstream>

#include <iostream>
#include <fstream>
#include <iomanip>

#include <spitfire/spitfire.h>

#ifndef __WIN__
#include <sys/time.h>
#include <unistd.h>
#endif

#include <gtest/gtest.h>

#include <spitfire/util/string.h>
#include <spitfire/util/timer.h>

TEST(Spitfire, TestCountOf)
{
  char text[14];
  ASSERT_EQ(14, countof(text));

  int stuff[9];
  ASSERT_EQ(9, countof(stuff));
}

TEST(Spitfire, TestSafeDelete)
{
  int* x = new int;
  spitfire::SAFE_DELETE(x);
  ASSERT_EQ(nullptr, x);
}

TEST(Spitfire, TestSafeDeleteArray)
{
  int* y = new int[10];
  spitfire::SAFE_DELETE_ARRAY(y);
  ASSERT_EQ(nullptr, y);
}
