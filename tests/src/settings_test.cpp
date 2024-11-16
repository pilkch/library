#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include <list>
#include <vector>
#include <map>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <gtest/gtest.h>

// Spitfire Includes
#include <spitfire/spitfire.h>
#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/settings.h>
#include <spitfire/storage/xml.h>
#include <spitfire/util/log.h>
#include <spitfire/util/string.h>

TEST(SpitfireStorage, TestSettings)
{
  // TODO: Fix this test, it is totally broken, none of these functions have been implemented

  const spitfire::string_t sTestProfile(TEXT("TestProfile"));

  const spitfire::string_t sTestSection(TEXT("TestSection"));
  const spitfire::string_t sTestSubSection(TEXT("TestSubSection"));
  const spitfire::string_t sTestSubSubSection(TEXT("TestSubSubSection"));
  const spitfire::string_t sTestValue(TEXT("this is a settings string which is get and set in the settings"));

  bool bResult = false;
  spitfire::string_t sValue;

#ifdef BUILD_SETTINGS_GLOBAL
  spitfire::SetGlobalSettingsSetString(sTestSection, sTestSubSection, sTestSubSubSection, sTestValue);
  sValue.clear();
  bResult = spitfire::GetGlobalSettingsString(sTestSection, sTestSubSection, sTestSubSubSection, sValue);
  ASSERT_TRUE(bResult);
  ASSERT_EQ(sTestValue, sValue);
#endif

  spitfire::SetApplicationUserSetting(sTestSection, sTestSubSection, sTestSubSubSection, sTestValue);
  sValue.clear();
  bResult = spitfire::GetApplicationUserSetting(sTestSection, sTestSubSection, sTestSubSubSection, sValue);
  ASSERT_TRUE(bResult);
  ASSERT_EQ(sTestValue, sValue);

#ifdef BUILD_SETTINGS_PROFILE
  Create profile
  spitfire::SetProfileSettingsString(sProfile, sTestSection, sTestSubSection, sTestSubSubSection, sTestValue);
  sValue.clear();
  bResult = spitfire::GetProfileSettingsString(sProfile, sTestSection, sTestSubSection, sTestSubSubSection, sValue);
  ASSERT_TRUE(bResult);
  ASSERT_EQ(sTestValue, sValue);
#endif
}
