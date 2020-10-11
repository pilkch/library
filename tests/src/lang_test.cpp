#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include <iostream>
#include <fstream>

#include <list>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stack>

// Standard headers
#include <chrono>

#include <gtest/gtest.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/string.h>
#include <spitfire/util/lang.h>
#include <spitfire/util/datetime.h>

#ifndef FIRESTARTER
#include <spitfire/util/log.h>
#endif

#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/file.h>
#include <spitfire/util/datetime.h>
#include <spitfire/util/log.h>

TEST(SpitfireUtil, TestLangHumanReadableTime)
{
  const spitfire::util::cDateTime now(2015, 6, 5, 4, 3);

  struct cTestPair {
    const std::chrono::system_clock::duration duration;
    const spitfire::string_t sExpected;
  };
  
  const cTestPair pairs[] = {
    { - (std::chrono::hours(240) + std::chrono::minutes(1)), TEXT("About 10 days ago") },
    { - (std::chrono::hours(48) + std::chrono::minutes(1)), TEXT("About 2 days ago") },
    { - (std::chrono::hours(24) + std::chrono::minutes(1)), TEXT("About 1 day ago") },
    { -std::chrono::hours(24), TEXT("About 1 day ago") },
    { - (std::chrono::hours(23) + std::chrono::minutes(59)), TEXT("About 23 hours ago") },
    { - (std::chrono::hours(1) + std::chrono::minutes(1)), TEXT("About 1 hour ago") },
    { -std::chrono::hours(1), TEXT("About 1 hour ago") },
    { - (std::chrono::minutes(59) + std::chrono::seconds(59)), TEXT("About 59 minutes ago") },
    { - (std::chrono::minutes(2) + std::chrono::seconds(1)), TEXT("About 2 minutes ago") },
    { -std::chrono::minutes(2), TEXT("About 2 minutes ago") },
    { - (std::chrono::minutes(1) + std::chrono::seconds(58)), TEXT("About 1 minute ago") },
    { -std::chrono::minutes(1), TEXT("About 1 minute ago") },
    { -std::chrono::seconds(59), TEXT("About 1 minute ago") },
    { -std::chrono::seconds(31), TEXT("About 1 minute ago") },
    { -std::chrono::seconds(30), TEXT("30 seconds ago") },
    { -std::chrono::seconds(29), TEXT("29 seconds ago") },
    { -std::chrono::seconds(2), TEXT("2 seconds ago") },
    { -std::chrono::seconds(1), TEXT("1 second ago") },
    { std::chrono::seconds(0), TEXT("Now") },
    { std::chrono::seconds(1), TEXT("In 1 second") },
    { std::chrono::seconds(2), TEXT("In 2 seconds") },
    { std::chrono::seconds(29), TEXT("In 29 seconds") },
    { std::chrono::seconds(30), TEXT("In 30 seconds") },
    { std::chrono::seconds(31), TEXT("In about 1 minute") },
    { std::chrono::seconds(59), TEXT("In about 1 minute") },
    { std::chrono::minutes(1), TEXT("In about 1 minute") },
    { std::chrono::minutes(1) + std::chrono::seconds(58), TEXT("In about 1 minute") },
    { std::chrono::minutes(2), TEXT("In about 2 minutes") },
    { std::chrono::minutes(2) + std::chrono::seconds(1), TEXT("In about 2 minutes") },
    { std::chrono::minutes(59) + std::chrono::seconds(59), TEXT("In about 59 minutes") },
    { std::chrono::hours(1), TEXT("In about 1 hour") },
    { std::chrono::hours(1) + std::chrono::minutes(1), TEXT("In about 1 hour") },
    { std::chrono::hours(23) + std::chrono::minutes(59), TEXT("In about 23 hours") },
    { std::chrono::hours(24), TEXT("In about 1 day") },
    { std::chrono::hours(24) + std::chrono::minutes(1), TEXT("In about 1 day") },
    { std::chrono::hours(48) + std::chrono::minutes(1), TEXT("In about 2 days") },
    { std::chrono::hours(240) + std::chrono::minutes(1), TEXT("In about 10 days") },
  };

  const size_t n = countof(pairs);
  for (size_t i = 0; i < n; i++) {
    EXPECT_STREQ(pairs[i].sExpected.c_str(), spitfire::util::LangHumanReadableTime(now, spitfire::util::cDateTime(now + pairs[i].duration)).c_str());
  }
}

TEST(SpitfireUtil, TestLangHumanReadableDuration)
{
  struct cTestPair {
    const std::chrono::system_clock::duration duration;
    const spitfire::string_t sExpected;
  };

  const cTestPair pairs[] = {
    { std::chrono::seconds(1), TEXT("1 second") },
    { std::chrono::seconds(2), TEXT("2 seconds") },
    { std::chrono::seconds(29), TEXT("29 seconds") },
    { std::chrono::seconds(30), TEXT("30 seconds") },
    { std::chrono::seconds(31), TEXT("About 1 minute") },
    { std::chrono::seconds(59), TEXT("About 1 minute") },
    { std::chrono::minutes(1), TEXT("About 1 minute") },
    { std::chrono::minutes(1) + std::chrono::seconds(58), TEXT("About 1 minute") },
    { std::chrono::minutes(2), TEXT("About 2 minutes") },
    { std::chrono::minutes(2) + std::chrono::seconds(1), TEXT("About 2 minutes") },
    { std::chrono::minutes(59) + std::chrono::seconds(59), TEXT("About 59 minutes") },
    { std::chrono::hours(1), TEXT("About 1 hour") },
    { std::chrono::hours(1) + std::chrono::minutes(1), TEXT("About 1 hour") },
    { std::chrono::hours(23) + std::chrono::minutes(59), TEXT("About 23 hours") },
    { std::chrono::hours(24), TEXT("About 1 day") },
    { std::chrono::hours(24) + std::chrono::minutes(1), TEXT("About 1 day") },
    { std::chrono::hours(48) + std::chrono::minutes(1), TEXT("About 2 days") },
    { std::chrono::hours(240) + std::chrono::minutes(1), TEXT("About 10 days") },
  };

  const size_t n = countof(pairs);
  for (size_t i = 0; i < n; i++) {
    EXPECT_STREQ(pairs[i].sExpected.c_str(), spitfire::util::LangHumanReadableDuration(pairs[i].duration).c_str());
  }
}

TEST(SpitfireUtil, TestLangLangTag)
{
  spitfire::filesystem::CreateDirectory("data");

  // Create a blank lang tags file
  spitfire::storage::WriteTextContents("data/lang.txt", "");
  spitfire::LoadLanguageFiles();

  // Test non-existent lang tag
  {
    const spitfire::string_t sLangTag = TEXT("%1 something %2");
    EXPECT_STREQ(TEXT("LANG TAG NOT FOUND first something 2"), spitfire::LANG(sLangTag, "first", 2).c_str());
  }

  // Test valid tags
  spitfire::storage::WriteTextContents("data/lang.txt",
    "%1 something %2 \"%1 something %2\"\n"
    "And a different order %2 and %1 \"And a different order %2 and %1\"\n"
    "No arguments \"No arguments\"\n"
    "Arguments with no replacement \"Arguments with no replacement\"\n"
    "Multiple of the same %1 argument %1 \"Multiple of the same %1 argument %1\"\n"
    "Rearranged arguments first: %1, second: %2 \"Rearranged arguments first: %2, second: %1\"\n"
  );
  spitfire::LoadLanguageFiles();

  {
    const spitfire::string_t sLangTag = TEXT("%1 something %2");
    EXPECT_STREQ(TEXT("first something 2"), spitfire::LANG(sLangTag, "first", 2).c_str());
  }
  {
    const spitfire::string_t sLangTag = TEXT("And a different order %2 and %1");
    EXPECT_STREQ(TEXT("And a different order 2 and first"), spitfire::LANG(sLangTag, "first", 2).c_str());
  }
  {
    const spitfire::string_t sLangTag = TEXT("No arguments");
    EXPECT_STREQ(TEXT("No arguments"), spitfire::LANG(sLangTag).c_str());
  }
  {
    const spitfire::string_t sLangTag = TEXT("Arguments with no replacement");
    EXPECT_STREQ(TEXT("Arguments with no replacement"), spitfire::LANG(sLangTag, 3, "something").c_str());
  }
  {
    const spitfire::string_t sLangTag = TEXT("Multiple of the same %1 argument %1");
    EXPECT_STREQ(TEXT("Multiple of the same something argument something"), spitfire::LANG(sLangTag, "something", 2, 3).c_str());
  }
  {
    const spitfire::string_t sLangTag = TEXT("Rearranged arguments first: %1, second: %2");
    EXPECT_STREQ(TEXT("Rearranged arguments first: second, second: first"), spitfire::LANG(sLangTag, "first", "second").c_str());
  }
}
