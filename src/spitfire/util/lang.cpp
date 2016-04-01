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

// Boost headers
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/microsec_time_clock.hpp>

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


  std::map<std::wstring, std::wstring> langtags;

  const std::wstring WCRLF(L"\r\n");

  namespace util
  {
    bool LastCharacterIsQuotationMark(const std::wstring& source)
    {
      return source[source.length() - 1] != TEXT('\"');
    }

    void LoadLanguageFile(const string_t& sFilename)
    {
      std::vector<std::wstring> contents;
      storage::ReadText(sFilename, contents);

      CONSOLE<<"LoadLanguageFile "<<sFilename<<" Text has been read"<<std::endl;

      std::wstring tag;
      std::wstring value;

      std::wstring line;

      const size_t n = contents.size();
      for (size_t i = 0; i < n; i++) {
        line = spitfire::string::StripLeadingWhiteSpace(contents[i]);
        CONSOLE<<"LoadLanguageFile contents["<<i<<"]=\""<<spitfire::string::ToString_t(line)<<"\""<<std::endl;

        // If we have enough characters for a comment string and the first 2 characters
        // are comment slashes then skip this line
        if ((line.length() >= 2) && (line[0] == L'/') && (line[1] == L'/')) continue;

        CONSOLE<<"LoadLanguageFile Strip"<<std::endl;
        tag = spitfire::string::StripAfterInclusive(line, L" \"");

        CONSOLE<<"LoadLanguageFile Read quoted text"<<std::endl;

        // Get a quotation marked value that can span multiple lines
        value = spitfire::string::StripBeforeInclusive(line, L" \"");
        if (LastCharacterIsQuotationMark(line)) {
          i++;
          while ((i < n) && !line.empty() && LastCharacterIsQuotationMark(line)) {
            line = contents[i];
            value.append(L"\n" + line);
            i++;
          }

          i--;
        }

        CONSOLE<<"LoadLanguageFile Strip trailing"<<std::endl;
        value = spitfire::string::StripTrailing(value, L"\"");

        // Add tag
        CONSOLE<<"LoadLanguageFile Tag \""<<spitfire::string::ToString_t(tag)<<"\"=\""<<spitfire::string::ToString_t(value)<<"\""<<std::endl;
        langtags[tag] = value;
      }

      CONSOLE<<"LoadLanguageFile returning"<<std::endl;
    }

    void LoadLanguageFiles()
    {
      ASSERT(langtags.empty());

      string_t actual_filename;
      filesystem::FindFile(TEXT("lang.txt"), actual_filename);
      LoadLanguageFile(actual_filename);
      filesystem::FindFile(TEXT("shared_lang.txt"), actual_filename);
      LoadLanguageFile(actual_filename);
    }
  }

  string_t LANG(const std::string& tag)
  {
    std::map<std::wstring, std::wstring>::iterator iter = langtags.find(spitfire::string::ToWchar_t(tag));
    if (iter != langtags.end()) {
      CONSOLE<<"LANG["<<tag<<"]=\""<<spitfire::string::ToString_t(iter->second)<<"\""<<std::endl;
      return  spitfire::string::ToString_t(iter->second);
    }

    string_t sFilename;
    filesystem::FindFile(TEXT("lang.txt"), sFilename);
    storage::AppendText(sFilename, spitfire::string::ToWchar_t(tag) + L" \"AUTOMATICALLY GENERATED LANGTAG\"" + WCRLF);
    return TEXT("LANG TAG NOT FOUND ") + spitfire::string::ToString_t(tag);
  }

  string_t LANG(const std::wstring& tag)
  {
    std::map<std::wstring, std::wstring>::iterator iter = langtags.find(spitfire::string::ToWchar_t(tag));
    if (iter != langtags.end()) {
      CONSOLE<<"LANG["<<spitfire::string::ToString_t(tag)<<"]=\""<<spitfire::string::ToString_t(iter->second)<<"\""<<std::endl;
      return  spitfire::string::ToString_t(iter->second);
    }

    string_t sFilename;
    filesystem::FindFile(TEXT("lang.txt"), sFilename);
    storage::AppendText(sFilename, spitfire::string::ToWchar_t(tag) + L" \"AUTOMATICALLY GENERATED LANGTAG\"" + WCRLF);
    return TEXT("LANG TAG NOT FOUND ") + spitfire::string::ToString_t(tag);
  }



  namespace util
  {
    string_t LangHumanReadableTime(const cDateTime& dateTimeNow, const cDateTime& dateTime)
    {
      const boost::posix_time::time_duration duration = dateTime - dateTimeNow;

      int64_t milliseconds = duration.total_milliseconds();

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

    string_t LangHumanReadableDuration(const boost::posix_time::time_duration& duration)
    {
      const int64_t milliseconds = duration.total_milliseconds();

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

// Assert that matches the Google Test definition
#ifdef ASSERT_TRUE
#error "Remove this definition of ASSERT_TRUE"
#endif
#define ASSERT_TRUE assert

    void LangHumanReadableTimeUnitTest()
    {
      const cDateTime now(2015, 6, 5, 4, 3);

      struct cTestPair {
        const boost::posix_time::time_duration duration;
        const string_t sExpected;
      };
      
      const cTestPair pairs[] = {
        { - (boost::posix_time::hours(240) + boost::posix_time::minutes(1)), TEXT("About 10 days ago") },
        { - (boost::posix_time::hours(48) + boost::posix_time::minutes(1)), TEXT("About 2 days ago") },
        { - (boost::posix_time::hours(24) + boost::posix_time::minutes(1)), TEXT("About 1 day ago") },
        { - boost::posix_time::hours(24), TEXT("About 1 day ago") },
        { - (boost::posix_time::hours(23) + boost::posix_time::minutes(59)), TEXT("About 23 hours ago") },
        { - (boost::posix_time::hours(1) + boost::posix_time::minutes(1)), TEXT("About 1 hour ago") },
        { - boost::posix_time::hours(1), TEXT("About 1 hour ago") },
        { - (boost::posix_time::minutes(59) + boost::posix_time::seconds(59)), TEXT("About 59 minutes ago") },
        { - (boost::posix_time::minutes(2) + boost::posix_time::seconds(1)), TEXT("About 2 minutes ago") },
        { - boost::posix_time::minutes(2), TEXT("About 2 minutes ago") },
        { - (boost::posix_time::minutes(1) + boost::posix_time::seconds(58)), TEXT("About 1 minute ago") },
        { - boost::posix_time::minutes(1), TEXT("About 1 minute ago") },
        { - boost::posix_time::seconds(59), TEXT("About 1 minute ago") },
        { - boost::posix_time::seconds(31), TEXT("About 1 minute ago") },
        { - boost::posix_time::seconds(30), TEXT("30 seconds ago") },
        { - boost::posix_time::seconds(29), TEXT("29 seconds ago") },
        { - boost::posix_time::seconds(2), TEXT("2 seconds ago") },
        { - boost::posix_time::seconds(1), TEXT("1 second ago") },
        { boost::posix_time::seconds(0), TEXT("Now") },
        { boost::posix_time::seconds(1), TEXT("In 1 second") },
        { boost::posix_time::seconds(2), TEXT("In 2 seconds") },
        { boost::posix_time::seconds(29), TEXT("In 29 seconds") },
        { boost::posix_time::seconds(30), TEXT("In 30 seconds") },
        { boost::posix_time::seconds(31), TEXT("In about 1 minute") },
        { boost::posix_time::seconds(59), TEXT("In about 1 minute") },
        { boost::posix_time::minutes(1), TEXT("In about 1 minute") },
        { boost::posix_time::minutes(1) + boost::posix_time::seconds(58), TEXT("In about 1 minute") },
        { boost::posix_time::minutes(2), TEXT("In about 2 minutes") },
        { boost::posix_time::minutes(2) + boost::posix_time::seconds(1), TEXT("In about 2 minutes") },
        { boost::posix_time::minutes(59) + boost::posix_time::seconds(59), TEXT("In about 59 minutes") },
        { boost::posix_time::hours(1), TEXT("In about 1 hour") },
        { boost::posix_time::hours(1) + boost::posix_time::minutes(1), TEXT("In about 1 hour") },
        { boost::posix_time::hours(23) + boost::posix_time::minutes(59), TEXT("In about 23 hours") },
        { boost::posix_time::hours(24), TEXT("In about 1 day") },
        { boost::posix_time::hours(24) + boost::posix_time::minutes(1), TEXT("In about 1 day") },
        { boost::posix_time::hours(48) + boost::posix_time::minutes(1), TEXT("In about 2 days") },
        { boost::posix_time::hours(240) + boost::posix_time::minutes(1), TEXT("In about 10 days") },
      };

      const size_t n = countof(pairs);
      for (size_t i = 0; i < n; i++) {
        const string_t sResult = LangHumanReadableTime(now, cDateTime(now + pairs[i].duration));
        ASSERT_TRUE(sResult == pairs[i].sExpected);
      }
    }

    void LangHumanReadableDurationUnitTest()
    {
      struct cTestPair {
        const boost::posix_time::time_duration duration;
        const string_t sExpected;
      };

      const cTestPair pairs[] = {
        { boost::posix_time::seconds(1), TEXT("1 second") },
        { boost::posix_time::seconds(2), TEXT("2 seconds") },
        { boost::posix_time::seconds(29), TEXT("29 seconds") },
        { boost::posix_time::seconds(30), TEXT("30 seconds") },
        { boost::posix_time::seconds(31), TEXT("About 1 minute") },
        { boost::posix_time::seconds(59), TEXT("About 1 minute") },
        { boost::posix_time::minutes(1), TEXT("About 1 minute") },
        { boost::posix_time::minutes(1) + boost::posix_time::seconds(58), TEXT("About 1 minute") },
        { boost::posix_time::minutes(2), TEXT("About 2 minutes") },
        { boost::posix_time::minutes(2) + boost::posix_time::seconds(1), TEXT("About 2 minutes") },
        { boost::posix_time::minutes(59) + boost::posix_time::seconds(59), TEXT("About 59 minutes") },
        { boost::posix_time::hours(1), TEXT("About 1 hour") },
        { boost::posix_time::hours(1) + boost::posix_time::minutes(1), TEXT("About 1 hour") },
        { boost::posix_time::hours(23) + boost::posix_time::minutes(59), TEXT("About 23 hours") },
        { boost::posix_time::hours(24), TEXT("About 1 day") },
        { boost::posix_time::hours(24) + boost::posix_time::minutes(1), TEXT("About 1 day") },
        { boost::posix_time::hours(48) + boost::posix_time::minutes(1), TEXT("About 2 days") },
        { boost::posix_time::hours(240) + boost::posix_time::minutes(1), TEXT("About 10 days") },
      };

      const size_t n = countof(pairs);
      for (size_t i = 0; i < n; i++) {
        const string_t sResult = LangHumanReadableDuration(pairs[i].duration);
        ASSERT_TRUE(sResult == pairs[i].sExpected);
      }
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
