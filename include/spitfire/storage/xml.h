#ifndef XML_H
#define XML_H

#include <spitfire/storage/document.h>
#include <spitfire/util/process.h>

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
      util::PROCESS_RESULT ReadFromFile(util::cProcessInterface& interface, document::cDocument& doc, const string_t& filename) const;
      util::PROCESS_RESULT ReadFromString(util::cProcessInterface& interface, document::cDocument& doc, const std::string& input) const;
      util::PROCESS_RESULT ReadFromString(util::cProcessInterface& interface, document::cDocument& doc, const std::wstring& input) const { return ReadFromString(interface, doc, spitfire::string::ToUTF8(input)); }
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
