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
    void Update(sampletime_t currentTime);

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

  void cLangContext::Update(sampletime_t currentTime)
  {
    datetime.SetFromUniversalTimeNow();
  }


  cLangContext context;

  void Update(sampletime_t currentTime)
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
