#ifndef SETTINGS_H
#define SETTINGS_H

// Standard headers
#include <vector>

// Spitfire headers
#include <spitfire/storage/document.h>
#include <spitfire/util/thread.h>

// Bad Example
/*
enum HANDORIENTATION
{
  HANDORIENTATION_RIGHT = 0,
  HANDORIENTATION_LEFT,
  HANDORIENTATION_CENTRE,

  HANDORIENTATION_DEFAULT = HANDORIENTATION_RIGHT
};

inline HANDORIENTATION GetHandOrientation()
{
  int iValue;
  if (GetApplicationProfileSetting(TEXT("View"), TEXT("HandOrientation"), iValue)) {
    if (iValue == 0) return HANDORIENTATION_RIGHT;
    if (iValue == 1) return HANDORIENTATION_LEFT;
    if (iValue == 2) return HANDORIENTATION_CENTRE;
  }
  return HANDORIENTATION_DEFAULT;
}*/

namespace spitfire
{
  namespace storage
  {
    class cSettingsDocument
    {
    public:
      void Load();
      void Save();

      template <class T>
      T GetValue(const string_t& sSection, const string_t& sItem, const string_t& sAttribute, const T& valueDefault) const;
      template <class T>
      void SetValue(const string_t& sSection, const string_t& sItem, const string_t& sAttribute, const T& value);
      void RemoveValue(const string_t& sSection, const string_t& sItem, const string_t& sAttribute);

      void GetListOfValues(const string_t& sSection, const string_t& sItem, const string_t& sAttribute, std::vector<string_t>& values) const;
      void SetListOfValues(const string_t& sSection, const string_t& sItem, const string_t& sAttribute, const std::vector<string_t>& values);
      void RemoveListOfValues(const string_t& sSection, const string_t& sItem, const string_t& sAttribute);

    private:
      spitfire::document::cDocument document;
    };
  }

#ifdef BUILD_SETTINGS_GLOBAL
  // Shared between applications, per user
  // Uses IPC, the first application starts the IPC server and then other applications query the IPC server
  // Stored in "~/.spitfire/preferences.xml"
  // ie.
  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, bool value);
  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, int value);
  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& value);
  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool value);
  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int value);
  void SetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, const string_t& value);

  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, bool& value);
  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, int& value);
  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, string_t& value);
  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool& value);
  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int& value);
  bool GetGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, string_t& value);

  bool DoesGlobalSettingExist(const string_t& section, const string_t& subsection);
  bool DoesGlobalSettingExist(const string_t& section, const string_t& subsection, const string_t& subsubsection);

  void RemoveGlobalUserSetting(const string_t& section, const string_t& subsection);
  void RemoveGlobalUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection);
#endif

  // Per application, per user
  // Stored in "~/.spitfire/$application/preferences.xml"
  // ie. Resolution, Fullscreen, Draw Distance, Language
  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, bool value);
  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, int value);
  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& value);
  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool value);
  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int value);
  void SetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, const string_t& value);

  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, bool& value);
  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, int& value);
  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, string_t& value);
  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool& value);
  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int& value);
  bool GetApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, string_t& value);

  bool DoesApplicationUserSettingExist(const string_t& section, const string_t& subsection);
  bool DoesApplicationUserSettingExist(const string_t& section, const string_t& subsection, const string_t& subsubsection);

  void RemoveApplicationUserSetting(const string_t& section, const string_t& subsection);
  void RemoveApplicationUserSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection);

#ifdef BUILD_SETTINGS_PROFILES
  // Per profile
  // Stored in "~/.spitfire/$application/profiles/$profile.xml"
  // ie. Does the user like to be left or right handed?  What gender is the user?  Name, colour, keys etc.
  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, bool value);
  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, int value);
  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& value);
  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool value);
  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int value);
  void SetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, const string_t& value);

  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, bool& value);
  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, int& value);
  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, string_t& value);
  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, bool& value);
  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, int& value);
  bool GetApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection, string_t& value);

  bool DoesApplicationProfileSettingExist(const string_t& section, const string_t& subsection);
  bool DoesApplicationProfileSettingExist(const string_t& section, const string_t& subsection, const string_t& subsubsection);

  void RemoveApplicationProfileSetting(const string_t& section, const string_t& subsection);
  void RemoveApplicationProfileSetting(const string_t& section, const string_t& subsection, const string_t& subsubsection);
#endif // BUILD_SETTINGS_PROFILES


  namespace storage
  {
    template <class T>
    inline T cSettingsDocument::GetValue(const string_t& sSection, const string_t& sItem, const string_t& sAttribute, const T& valueDefault) const
    {
      assert(spitfire::util::IsMainThread());

      T value = valueDefault;

      spitfire::document::cNode::const_iterator iterConfig(document);
      if (!iterConfig.IsValid()) return value;

      iterConfig.FindChild("config");
      if (!iterConfig.IsValid()) return value;

      {
        spitfire::document::cNode::const_iterator iter(iterConfig);

        iter.FindChild(spitfire::string::ToUTF8(sSection));
        if (iter.IsValid()) {
          iter.FindChild(spitfire::string::ToUTF8(sItem));
          if (iter.IsValid()) {
            iter.GetAttribute(spitfire::string::ToUTF8(sAttribute), value);
            //std::cout<<"cSettings::GetValue Item \""<<sItem<<"\" found "<<spitfire::string::ToString(value)<<std::endl;
          }
        }
      }

      return value;
    }

    template <class T>
    inline void cSettingsDocument::SetValue(const string_t& sSection, const string_t& sItem, const string_t& sAttribute, const T& value)
    {
      assert(spitfire::util::IsMainThread());

      // Get or create the config element
      spitfire::document::cNode::iterator iterConfig(document);
      if (!iterConfig.IsValid()) {
        spitfire::document::element* configElement = document.CreateElement("config");
        document.AppendChild(configElement);
        iterConfig = document;
        assert(iterConfig.IsValid());
      }

      // Get or create the config element
      iterConfig.FindChild("config");
      if (!iterConfig.IsValid()) {
        spitfire::document::element* configElement = document.CreateElement("config");
        document.AppendChild(configElement);
        iterConfig = document;
        assert(iterConfig.IsValid());
        iterConfig.FindChild("config");
        assert(iterConfig.IsValid());
      }

      // Get or create the section element
      spitfire::document::cNode::iterator iterSection(iterConfig);
      iterSection.FindChild(spitfire::string::ToUTF8(sSection));
      if (!iterSection.IsValid()) {
        spitfire::document::element* sectionElement = document.CreateElement(spitfire::string::ToUTF8(sSection));
        spitfire::document::element* configElement = iterConfig.Get();
        configElement->AppendChild(sectionElement);
        iterSection = iterConfig;
        assert(iterSection.IsValid());
        iterSection.FindChild(spitfire::string::ToUTF8(sSection));
        assert(iterSection.IsValid());
      }

      // Get or create the item element
      spitfire::document::cNode::iterator iterItem(iterSection);
      iterItem.FindChild(spitfire::string::ToUTF8(sItem));
      if (!iterItem.IsValid()) {
        spitfire::document::element* itemElement = document.CreateElement(spitfire::string::ToUTF8(sItem));
        spitfire::document::element* sectionElement = iterSection.Get();
        sectionElement->AppendChild(itemElement);
        iterItem = iterSection;
        assert(iterItem.IsValid());
        iterItem.FindChild(spitfire::string::ToUTF8(sItem));
        assert(iterItem.IsValid());
      }

      spitfire::document::element* itemElement = iterItem.Get();

      // Create and append the item element
      itemElement->SetAttribute(spitfire::string::ToUTF8(sAttribute), value);
    }
  }
}

#endif // SETTINGS_H
