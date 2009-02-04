#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>

// writing on a text file
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

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/util/cString.h>

#ifndef FIRESTARTER
#include <breathe/util/log.h>
#endif

#include <breathe/storage/filesystem.h>
#include <breathe/storage/file.h>

#include <breathe/util/lang.h>
#include <breathe/util/datetime.h>

namespace breathe
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
        line = breathe::string::StripLeadingWhiteSpace(contents[i]);
        CONSOLE<<"LoadLanguageFile contents["<<i<<"]=\""<<line<<"\""<<std::endl;

        // If we have enough characters for a comment string and the first 2 characters
        // are comment slashes then skip this line
        if ((line.length() >= 2) && (line[0] == L'/') && (line[1] == L'/')) continue;

        CONSOLE<<"LoadLanguageFile Strip"<<std::endl;
        tag = breathe::string::StripAfterInclusive(line, L" \"");

        CONSOLE<<"LoadLanguageFile Read quoted text"<<std::endl;

        // Get a quotation marked value that can span multiple lines
        value = breathe::string::StripBeforeInclusive(line, L" \"");
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
        value = breathe::string::StripTrailing(value, L"\"");

        // Add tag
        CONSOLE<<"LoadLanguageFile Tag \""<<tag<<"\"=\""<<value<<"\""<<std::endl;
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
    std::map<std::wstring, std::wstring>::iterator iter = langtags.find(breathe::string::ToWchar_t(tag));
    if (iter != langtags.end()) {
      CONSOLE<<"LANG["<<tag<<"]=\""<<breathe::string::ToString_t(iter->second)<<"\""<<std::endl;
      return  breathe::string::ToString_t(iter->second);
    }

    string_t sFilename;
    filesystem::FindFile(TEXT("lang.txt"), sFilename);
    storage::AppendText(sFilename, breathe::string::ToWchar_t(tag) + L" \"AUTOMATICALLY GENERATED LANGTAG\"" + WCRLF);
    return TEXT("LANG TAG NOT FOUND ") + breathe::string::ToString_t(tag);
  }

  string_t LANG(const std::wstring& tag)
  {
    std::map<std::wstring, std::wstring>::iterator iter = langtags.find(breathe::string::ToWchar_t(tag));
    if (iter != langtags.end()) {
      CONSOLE<<"LANG["<<tag<<"]=\""<<breathe::string::ToString_t(iter->second)<<"\""<<std::endl;
      return  breathe::string::ToString_t(iter->second);
    }

    string_t sFilename;
    filesystem::FindFile(TEXT("lang.txt"), sFilename);
    storage::AppendText(sFilename, breathe::string::ToWchar_t(tag) + L" \"AUTOMATICALLY GENERATED LANGTAG\"" + WCRLF);
    return TEXT("LANG TAG NOT FOUND ") + breathe::string::ToString_t(tag);
  }
}
