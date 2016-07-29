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

// Spitfire Includes
#include <spitfire/spitfire.h>
#include <spitfire/util/string.h>

#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/settings.h>
#include <spitfire/storage/xml.h>

#ifndef FIRESTARTER
#include <spitfire/util/log.h>
#endif

namespace spitfire
{
  namespace storage
  {
    void cSettingsDocument::Load()
    {
      assert(spitfire::util::IsMainThread());

      // Read the xml document
      spitfire::xml::reader reader;

      spitfire::util::cProcessInterfaceVoid interface;
      const string_t sFilename = spitfire::filesystem::GetThisApplicationSettingsDirectory() + TEXT("config.xml");
      spitfire::util::PROCESS_RESULT result = reader.ReadFromFile(interface, document, sFilename);
      if (result != spitfire::util::PROCESS_RESULT::COMPLETE) std::cout<<"cSettings::Load \""<<spitfire::string::ToUTF8(sFilename)<<"\" not found"<<std::endl;
    }

    void cSettingsDocument::Save()
    {
      assert(spitfire::util::IsMainThread());

      // Create the directory
      const string_t sFolder = spitfire::filesystem::GetThisApplicationSettingsDirectory();
      spitfire::filesystem::CreateDirectory(sFolder);

      // Write the xml document
      spitfire::xml::writer writer;

      const string_t sFilename = sFolder + TEXT("config.xml");
      if (!writer.WriteToFile(document, sFilename)) {
        std::cout<<"cSettings::Save Error saving to file \""<<spitfire::string::ToUTF8(sFilename)<<"\""<<std::endl;
        return;
      }
    }

    void cSettingsDocument::RemoveValue(const string_t& sSection, const string_t& sItem, const string_t& sAttribute)
    {
      assert(spitfire::util::IsMainThread());

      // Get the config element
      spitfire::document::cNode::iterator iterConfig(document);
      if (!iterConfig.IsValid()) return;

      // Get the config element
      iterConfig.FindChild("config");
      if (!iterConfig.IsValid()) return;

      // Get the section element
      spitfire::document::cNode::iterator iterSection(iterConfig);
      iterSection.FindChild(spitfire::string::ToUTF8(sSection));
      if (!iterSection.IsValid()) return;

      // Get the item element
      spitfire::document::cNode::iterator iterItem(iterSection);
      iterItem.FindChild(spitfire::string::ToUTF8(sItem));
      if (!iterItem.IsValid()) return;

      spitfire::document::element* itemElement = iterItem.Get();

      // Remove the attribute
      itemElement->RemoveAttribute(spitfire::string::ToUTF8(sAttribute));
    }

    void cSettingsDocument::GetListOfValues(const string_t& sSection, const string_t& sItem, const string_t& sAttribute, std::vector<string_t>& values) const
    {
      values.clear();

      // Get the count
      const size_t n = GetValue(sSection, sItem, sAttribute + TEXT("Count"), 0);

      // Reserve space for the number of values we are expecting
      values.reserve(n);

      // Get each path
      string_t sValue;
      for (size_t i = 0; i < n; i++) {
        sValue = GetValue<string_t>(sSection, sItem, sAttribute + spitfire::string::ToString(i), TEXT(""));
        if (!sValue.empty()) values.push_back(sValue);
      }
    }

    void cSettingsDocument::SetListOfValues(const string_t& sSection, const string_t& sItem, const string_t& sAttribute, const std::vector<string_t>& values)
    {
      // Remove the old list
      RemoveListOfValues(sSection, sItem, sAttribute);

      // Set the count
      const size_t n = values.size();
      SetValue(sSection, sItem, sAttribute + TEXT("Count"), n);

      // Add each value
      for (size_t i = 0; i < n; i++) {
        SetValue(sSection, sItem, sAttribute + spitfire::string::ToString(i), values[i]);
      }
    }

    void cSettingsDocument::RemoveListOfValues(const string_t& sSection, const string_t& sItem, const string_t& sAttribute)
    {
      // Get the count
      const size_t n = GetValue(sSection, sItem, sAttribute + TEXT("Count"), 0);

      // Remove each value
      for (size_t i = 0; i < n; i++) {
        RemoveValue(sSection, sItem, sAttribute + spitfire::string::ToString(i));
      }

      // Remove the count
      RemoveValue(sSection, sItem, sAttribute + TEXT("Count"));
    }
  }

#ifdef BUILD_SETTINGS_GLOBAL
  // Shared between applications, per user
  // Uses IPC, the first application starts the IPC server and then other applications query the IPC server
  // Stored in "~/.spitfire/preferences.xml"
  // ie.
  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, bool value)
  {
  }

  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, int value)
  {
  }

  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& value)
  {
  }

  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool value)
  {
  }

  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int value)
  {
  }

  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, const string_t& value)
  {
  }


  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, bool& value)
  {
    return false;
  }

  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, int& value)
  {
    return false;
  }

  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, string_t& value)
  {
    return false;
  }

  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool& value)
  {
    return false;
  }

  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int& value)
  {
    return false;
  }

  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, string_t& value)
  {
    return false;
  }


  void RemoveGlobalUserSetting(const string_t& section, const string_t& subsection)
  {
  }

  void RemoveGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection)
  {
  }
#endif

  // Per application, per user
  // Stored in "~/.spitfire/$application/preferences.xml"
  // ie. Resolution, Fullscreen, Draw Distance, Language
  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, bool value)
  {
  }

  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, int value)
  {
  }

  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& value)
  {
  }

  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool value)
  {
  }

  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int value)
  {
  }

  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, const string_t& value)
  {
  }


  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, bool& value)
  {
    return false;
  }

  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, int& value)
  {
    return false;
  }

  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, string_t& value)
  {
    return false;
  }

  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool& value)
  {
    return false;
  }

  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int& value)
  {
    return false;
  }

  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, string_t& value)
  {
    return false;
  }



  bool DoesApplicationUserSettingExist(const string_t& section, const string_t& subsection)
  {
    return false;
  }

  bool DoesApplicationUserSettingExist(const string_t& section, const string_t& subsection, const string_t& subsubsection)
  {
    return false;
  }


  void RemoveApplicationUserSetting(const string_t& section, const string_t& subsection)
  {
  }

  void RemoveApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection)
  {
  }

#ifdef BUILD_SETTINGS_PROFILES
  // Per profile
  // Stored in "~/.spitfire/$application/profiles/$profile.xml"
  // ie. Does the user like to be left or right handed?  What gender is the user?  Name, colour, keys
  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, bool value)
  {
  }

  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, int value)
  {
  }

  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& value)
  {
  }

  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool value)
  {
  }

  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int value)
  {
  }

  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, const string_t& value)
  {
  }

  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, bool& value)
  {
    return false;
  }

  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, int& value)
  {
    return false;
  }

  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, string_t& value)
  {
    return false;
  }

  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool& value)
  {
    return false;
  }

  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int& value)
  {
    return false;
  }

  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, string_t& value)
  {
    return false;
  }


  void RemoveApplicationProfileSetting(const string_t& section, const string_t& subsection)
  {
    return false;
  }

  void RemoveApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection)
  {
    return false;
  }
#endif // BUILD_SETTINGS_PROFILES
}


#ifdef BUILD_SPITFIRE_UNITTEST

#ifdef BUILD_SPITFIRE_UNITTEST
#include <spitfire/util/unittest.h>
#endif

class cSettingsUnitTest : protected util::cUnitTestBase
{
public:
  cSettingsUnitTest() :
    cUnitTestBase(TEXT("cSettingsUnitTest"))
  {
  }

  void Test()
  {
    // TODO: Fix this test, it is totally broken, none of these functions have been implemented

    const string_t sTestProfile(TEXT("TestProfile"));

    const string_t sTestSection(TEXT("TestSection"));
    const string_t sTestSubSection(TEXT("TestSubSection"));
    const string_t sTestValue(TEXT("this is a settings string which is get and set in the settings"));

    bool bResult = false;
    string_t sValue;

#ifdef BUILD_SETTINGS_GLOBAL
    SetGlobalSettingsSetString(sTestSection, sTestSubSection, sTestValue);
    sValue.clear();
    bResult = GetGlobalSettingsString(sTestSection, sTestSubSection, sValue);
    ASSERT(bResult != false);
    ASSERT(sValue == sTestValue);
#endif

    SetApplicationUserSetting(sTestSection, sTestSubSection, sTestValue);
    sValue.clear();
    bResult = GetApplicationUserSetting(sTestSection, sTestSubSection, sValue);
    ASSERT(bResult != false);
    ASSERT(sValue == sTestValue);

#ifdef BUILD_SETTINGS_PROFILE
    Create profile
    SetProfileSettingsString(sProfile, sTestSection, sTestSubSection, sTestValue);
    sValue.clear();
    bResult = GetProfileSettingsString(sProfile, sTestSection, sTestSubSection, sValue);
    ASSERT(bResult != false);
    ASSERT(sValue == sTestValue);
#endif
  }
};

cSettingsUnitTest gSettingsUnitTest;

#endif // BUILD_SPITFIRE_UNITTEST
