#ifndef XML_H
#define XML_H

#include <spitfire/storage/document.h>

// http://en.wikipedia.org/wiki/XML

/*
TODO:
Identify and replace these in XML:
&amp;  &
&lt;  <
&gt;  >
&quot;  "
&apos;  '
change comments <!-- --> to comment object (bComment = true).
*/

namespace spitfire
{
  namespace xml
  {
    class reader
    {
    public:
      bool ReadFromFile(document::cDocument& doc, const string_t& filename) const;
      bool ReadFromString(document::cDocument& doc, const std::string& input) const;
      bool ReadFromString(document::cDocument& doc, const std::wstring& input) const { return ReadFromString(doc, spitfire::string::ToUTF8(input)); }
    };


    class writer
    {
    public:
      bool WriteToFile(const document::cDocument& doc, const string_t& filename) const;
      bool WriteToString(const document::cDocument& doc, std::string& output) const;
      bool WriteToString(const document::cDocument& doc, std::wstring& output) const;
    };

    inline bool writer::WriteToString(const document::cDocument& doc, std::string& output) const
    {
      std::wstring temp(spitfire::string::ToWchar_t(output));
      bool bResult = WriteToString(doc, temp);
      output = spitfire::string::ToUTF8(temp);
      return bResult;
    }
  }
}

#endif // XML_H
