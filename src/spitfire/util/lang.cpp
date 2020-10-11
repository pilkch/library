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
#include <spitfire/util/log.h>

namespace spitfire
{
  class cLangContext
  {
  public:
    cLangContext();

    void SetPlayerName(const string_t& playername);
    void Update(durationms_t currentTime);

  private:
    string_t playername;

    bool bUsesKMH;
    bool bUsesCelsius;

    util::cDateTime datetime;
  };

  cLangContext::cLangContext() :
    playername(TEXT("UNKOWN NAME")),
    bUsesKMH(true),
    bUsesCelsius(true)
  {
  }

  void cLangContext::Update(durationms_t currentTime)
  {
    datetime.SetFromUniversalTimeNow();
  }


  cLangContext context;

  void Update(durationms_t currentTime)
  {
    context.Update(currentTime);
  }


  std::map<std::string, std::string> langtags_utf8;

  namespace util
  {
    bool LastCharacterIsQuotationMark(const std::string& source)
    {
      return (source[source.length() - 1] != '\"');
    }

    void LoadLanguageFile(const string_t& sFilename)
    {
      if (!filesystem::FileExists(sFilename)) return;

      std::vector<std::string> contents;
      storage::ReadText(sFilename, contents);

      std::string tag;
      std::string value;

      std::string line;

      const size_t n = contents.size();
      for (size_t i = 0; i < n; i++) {
        line = spitfire::string::StripLeadingWhiteSpace(contents[i]);

        // If we have enough characters for a comment string and the first 2 characters
        // are comment slashes then skip this line
        if ((line.length() >= 2) && (line[0] == '/') && (line[1] == '/')) continue;

        tag = spitfire::string::StripAfterInclusive(line, " \"");

        // Get a quotation marked value that can span multiple lines
        value = spitfire::string::StripBeforeInclusive(line, " \"");
        if (LastCharacterIsQuotationMark(line)) {
          i++;
          while ((i < n) && !line.empty() && LastCharacterIsQuotationMark(line)) {
            line = contents[i];
            value.append("\n" + line);
            i++;
          }

          i--;
        }

        value = spitfire::string::StripTrailing(value, "\"");

        // Add tag
        //CONSOLE<<"LoadLanguageFile Tag \""<<spitfire::string::ToString(tag)<<"\"=\""<<spitfire::string::ToString(value)<<"\""<<std::endl;
        langtags_utf8[tag] = value;
      }
    }

    void LoadLanguageFiles(const string_t& sDirectory)
    {
      langtags_utf8.clear();

      LoadLanguageFile(filesystem::MakeFilePath(sDirectory, TEXT("lang.txt")));
      LoadLanguageFile(filesystem::MakeFilePath(sDirectory, TEXT("shared_lang.txt")));
    }
  }

  void LoadLanguageFiles()
  {
    util::LoadLanguageFiles("data");
  }

  string_t LookUpLangTag(const std::string& tag)
  {
    std::map<std::string, std::string>::iterator iter = langtags_utf8.find(tag);
    if (iter != langtags_utf8.end()) {
      //CONSOLE<<"LANG["<<tag<<"]=\""<<spitfire::string::ToString(iter->second)<<"\""<<std::endl;
      return  spitfire::string::ToString(iter->second);
    }

    storage::AppendText(TEXT("data/lang.txt"), tag + " \"AUTOMATICALLY GENERATED LANGTAG\"" + "\n");
    return TEXT("LANG TAG NOT FOUND ") + spitfire::string::ToString(tag);
  }

  string_t LANG(const string_t& tag)
  {
    return LookUpLangTag(spitfire::string::ToUTF8(tag));
  }



  namespace util
  {
    string_t LangHumanReadableTime(const cDateTime& dateTimeNow, const cDateTime& dateTime)
    {
      const std::chrono::system_clock::duration duration = dateTime - dateTimeNow;

      int64_t milliseconds = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(duration).count();

      // Handle dates in the past
      // NOTE: This is the same code as the positive path just with "About x hours ago" instead of "In about x hours"
      if (milliseconds < 0) {
        // Make the milliseconds positive
        milliseconds = -milliseconds;

        // Now
        if (milliseconds < 1000) return TEXT("Now");

        // Seconds
        const uint64_t seconds = milliseconds / 1000;
        if (seconds <= 1) return TEXT("1 second ago");
        else if (seconds <= 30) return string::ToString(seconds) + TEXT(" seconds ago");

        // Minutes
        const uint64_t minutes = seconds / 60;
        if (minutes <= 1) return TEXT("About 1 minute ago");
        else if (minutes < 60) return TEXT("About ") + string::ToString(minutes) + TEXT(" minutes ago");

        // Hours
        const uint64_t hours = minutes / 60;
        if (hours <= 1) return TEXT("About ") + string::ToString(hours) + TEXT(" hour ago");
        else if (hours < 24) return TEXT("About ") + string::ToString(hours) + TEXT(" hours ago");

        // Days
        const uint64_t days = hours / 24;
        if (days <= 1) return TEXT("About 1 day ago");

        return TEXT("About ") + string::ToString(days) + TEXT(" days ago");
      }

      // Now
      if (milliseconds < 1000) return TEXT("Now");

      // Seconds
      const uint64_t seconds = milliseconds / 1000;
      if (seconds <= 1) return TEXT("In 1 second");
      else if (seconds <= 30) return TEXT("In ") + string::ToString(seconds) + TEXT(" seconds");

      // Minutes
      const uint64_t minutes = seconds / 60;
      if (minutes <= 1) return TEXT("In about 1 minute");
      else if (minutes < 60) return TEXT("In about ") + string::ToString(minutes) + TEXT(" minutes");

      // Hours
      const uint64_t hours = minutes / 60;
      if (hours <= 1) return TEXT("In about ") + string::ToString(hours) + TEXT(" hour");
      else if (hours < 24) return TEXT("In about ") + string::ToString(hours) + TEXT(" hours");

      // Days
      const uint64_t days = hours / 24;
      if (days <= 1) return TEXT("In about 1 day");

      return TEXT("In about ") + string::ToString(days) + TEXT(" days");
    }

    string_t LangHumanReadableDuration(const std::chrono::system_clock::duration& duration)
    {
      const int64_t milliseconds = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(duration).count();

      // We don't handle negative durations
      if (milliseconds < 0) return TEXT("0 seconds");

      // Seconds
      const uint64_t seconds = milliseconds / 1000;
      if (seconds <= 1) return TEXT("1 second");
      else if (seconds <= 30) return string::ToString(seconds) + TEXT(" seconds");

      // Minutes
      const uint64_t minutes = seconds / 60;
      if (minutes <= 1) return TEXT("About 1 minute");
      else if (minutes < 60) return TEXT("About ") + string::ToString(minutes) + TEXT(" minutes");

      // Hours
      const uint64_t hours = minutes / 60;
      if (hours <= 1) return TEXT("About ") + string::ToString(hours) + TEXT(" hour");
      else if (hours < 24) return TEXT("About ") + string::ToString(hours) + TEXT(" hours");

      // Days
      const uint64_t days = hours / 24;
      if (days <= 1) return TEXT("About 1 day");
      
      return TEXT("About ") + string::ToString(days) + TEXT(" days");
    }
  }
}








#if 0
"lives" "5"
"ammo" "24"
"name" "Chris"
"team" "Counter Terrorists"

class cContext
{
public:
   std::map<string_t, string_t> variables;
};


class cLangEntry
{
public:
   string_t sValue;
   bool bIsContainsVariables;
};

class cLangManager
{
public:
   void AddEntry(const string_t& sKey, const string_t& sValue);
   string_t GetLangEntry(const string_t& sKey) const;

private:
   string_t Parse(const string_t& sValue) const;

   std::map<string_t, cLangEntry> entries;
   cContext context;
};

void cLangManager::AddEntry(const string_t& sKey, const string_t& sValue)
{
   cLangEntry entry;
   entry.sValue = sValue;
   entry.bIsContainsVariables = (contains % that is not a %%);

   entries[sKey] = entry;
}

string_t cLangManager::GetLangEntry(const string_t& sKey) const
{
   const cLangEntry& entry = entries[sKey];
   if (entry.bIsContainsVariable) return Parse(entry.sValue);

   return entry.sValue;
}

string_t cLangManager::Parse(const string_t& sValue) const
{
   ostringstream_t o;

   find "%"
   if (next character == %) {
      o<<"%";
   } else {
      find "%"
      o<<context.GetValueOfVariable(variable);
   }

   return sParsed;
}


void UnitTest()
{
   add context string

   get lang tag
   check that %% is present
   check that %ammo% is correct
}

#endif
