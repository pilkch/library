// Standard includes
#include <cassert>
#include <cctype>
#include <cmath>

#include <cstring>
#include <string>
#include <codecvt>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <sstream>
#include <iterator>    // for back_inserter

#include <iostream>
#include <fstream>
#include <iomanip>

#include <locale>

#ifdef PLATFORM_LINUX_OR_UNIX
#include <iconv.h>
#endif
#include <errno.h>

// Spitfire includes
#include <spitfire/spitfire.h>
#include <spitfire/util/string.h>
#include <spitfire/util/log.h>

// Test strings
// http://www.columbia.edu/kermit/utf8.html

namespace spitfire
{
  namespace string
  {
    #define WHITE_SPACE "\t\v\r\n"
    #define LWHITE_SPACE L"\t\v\r\n"

    const char szWhiteSpace[] = {
      0x09, // \t
      0x0A, // \n
      0x0B,
      0x0C,
      0x0D, // \r
      0x20, // Space
      0
    };

    const wchar_t wszWhiteSpace[] = {
      0x0009, // <control-0009> to <control-000D>
      0x000A,
      0x000B,
      0x000C,
      0x000D,
      0x0020, // Space
      0x0085, // <control-0085>
      0x00A0, // No-Break Space
      0x1680, // Ogham Space Mark
      0x180E, // Mongolian Vowel Separator
      0x2000, // En Quad to Hair Space
      0x2001,
      0x2002,
      0x2003,
      0x2004,
      0x2005,
      0x2006,
      0x2007,
      0x2008,
      0x2009,
      0x200A,
      0x200C, // Zero Width Non-Joiner
      0x2028, // Line Separator
      0x2029, // Paragraph Separator
      0x202F, // Narrow No-Break Space
      0x205F, // Medium Mathematical Space
      0x3000, // Ideographic Space
      0
    };


    class cLocalisedStringTransformer
    {
    public:
      cLocalisedStringTransformer() :
        tmput_char(std::use_facet<std::time_put<char>>(std::locale("")))
      {
      }

      // Other Possible functions
      // bool isspace(c, loc);
      // bool isalpha(c, loc);
      // bool isdigit(c, loc);
      // bool ispunct(c, loc);
      // bool isxdigit(c, loc);
      // bool isalnum(c, loc);

      template <class T, class C, class O>
      inline T FormatTime(uint32_t hours, uint32_t minutes, uint32_t seconds) const
      {
        O o;

        /*
        if (hours < 10) o<<"0";
        o<<hours;

        o<<":";
        if (minutes < 10) o<<"0";
        o<<minutes;

        o<<":";
        if (seconds < 10) o<<"0";
        o<<seconds;
        */

        // http://wiki.forum.nokia.com/index.php/CS001143_-_Converting_date_and_time_to_string_in_Open_C%2B%2B

        tm now;

        // We don't care about the other fields for this formatting, perhaps we should set them too though just in case?
        now.tm_hour = hours;
        now.tm_min = minutes;
        now.tm_sec = seconds;

        // http://www.cplusplus.com/reference/clibrary/ctime/strftime/
        tmput_char.put(o, o, ' ', &now, 'X');

        return o.str();
      }

    private:
      const std::time_put<char>& tmput_char;
    };


    cLocalisedStringTransformer gLocalisedStringTransformer;


    // http://en.wikipedia.org/wiki/Whitespace_%28computer_science%29#Unicode

    // (' '), tab ('\t'), newline ('\n'), linefeed ('\r'), formfeed ('\f') and vertical tab ('\x0b').
    // U+0009–U+000D (control characters, containing Tab, CR and LF)

    // U+0020 SPACE
    // U+0085 NEL (control character next line)
    // U+00A0 NBSP (NO-BREAK SPACE)
    // U+1680 OGHAM SPACE MARK
    // U+180E MONGOLIAN VOWEL SEPARATOR
    // U+2000–U+200A (different sorts of spaces)
    // U+2028 LS (LINE SEPARATOR)
    // U+2029 PS (PARAGRAPH SEPARATOR)
    // U+202F NNBSP (NARROW NO-BREAK SPACE)
    // U+205F MMSP (MEDIUM MATHEMATICAL SPACE)
    // U+3000 IDEOGRAPHIC SPACE

    bool IsWhiteSpace(char c)
    {
      // We regard space, tab, new line and carriage return characters as white space
      return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r');
    }


    bool IsWhiteSpace(wchar_t c)
    {
      // We regard space, tab, new line and carriage return characters as white space
      // TODO: Use boost::is_space?
      return (c == L' ') || (c == L'\t') || (c == L'\n') || (c == L'\r');
    }

    // TODO: Use boost::is_alnum
    // TODO: Use boost::is_alpha
    // TODO: Use boost::is_lower
    // TODO: Use boost::is_print
    // TODO: Use boost::is_upper


    size_t CountOccurrences(const std::string& source, const std::string& sFind)
    {
      size_t count = 0;

      for (size_t i = 0, j = 0; (j = source.find(sFind, i)) != std::string::npos; i = j + 1, count++)
        ;

      return count;
    }

    size_t CountOccurrences(const std::wstring& source, const std::wstring& sFind)
    {
      size_t count = 0;

      for (size_t i = 0, j = 0; (j = source.find(sFind, i)) != std::wstring::npos; i = j + 1, count++)
        ;

      return count;
    }

    void Split(const std::string& source, char sFind, std::vector<std::string>& vOut)
    {
      vOut.clear();

      std::stringstream stm(source);
      std::string field;

      while (getline(stm, field, sFind)) vOut.push_back(Trim(field));
    }

    void Split(const std::wstring& source, wchar_t sFind, std::vector<std::wstring>& vOut)
    {
      vOut.clear();

      std::wstringstream stm(source);
      std::wstring field;

      while (getline(stm, field, sFind)) vOut.push_back(Trim(field));
    }

    bool Split(const std::string& source, const std::string& find, std::string& before, std::string& after)
    {
      std::string::size_type i = source.find(find);

      // If we found this string then return everything after the found string
      if (std::string::npos != i) {
        before = source.substr(0, i);
        after = source.substr(i + find.length());
        return true;
      }

      return false;
    }

    bool Split(const std::wstring& source, const std::wstring& find, std::wstring& before, std::wstring& after)
    {
      std::wstring::size_type i = source.find(find);

      // If we found this string then return everything after the found string
      if (std::wstring::npos != i) {
        before = source.substr(0, i);
        after = source.substr(i + find.length());
        return true;
      }

      return false;
    }

    void SplitOnNewLines(const std::string& source, std::vector<std::string>& vOut)
    {
      vOut.clear();

      std::stringstream stm(source);
      std::string field;

      while (getline(stm, field)) vOut.push_back(Trim(field));
    }

    void SplitOnNewLines(const std::wstring& source, std::vector<std::wstring>& vOut)
    {
      vOut.clear();

      std::wstringstream stm(source);
      std::wstring field;

      while (getline(stm, field)) vOut.push_back(Trim(field));
    }

    std::string Trim(const std::string& source)
    {
      std::string::size_type start = source.find_first_not_of(WHITE_SPACE);
      std::string::size_type end = source.find_last_not_of(WHITE_SPACE);
      if (start != std::string::npos) {
        if (end != std::string::npos) return source.substr(start, end + 1);

        return source.substr(start, source.length());
      } else if (end != std::string::npos) return source.substr(end);

      return "";
    }

    std::wstring Trim(const std::wstring& source)
    {
      std::wstring::size_type start = source.find_first_not_of(LWHITE_SPACE);
      std::wstring::size_type end = source.find_last_not_of(LWHITE_SPACE);
      if (start != std::string::npos) {
        if (end != std::string::npos) return source.substr(start, end + 1);

        return source.substr(start, source.length());
      } else if (end != std::string::npos) return source.substr(end);

      return L"";
    }

    bool Find(const std::string& source, const std::string& find, size_t& indexOut)
    {
      indexOut = source.find(find);
      return (indexOut != std::string::npos);
    }

    bool Find(const std::wstring& source, const std::wstring& find, size_t& indexOut)
    {
      indexOut = source.find(find);
      return (indexOut != std::wstring::npos);
    }

    std::string Replace(const std::string& source, const std::string& sFind, const std::string& sReplace)
    {
      std::string temp(source);
      for (size_t j = 0; (j = temp.find(sFind)) != std::string::npos;) {
        temp.replace(j, sFind.length(), sReplace);
      }
      return temp;
    }

    std::wstring Replace(const std::wstring& source, const std::wstring& sFind, const std::wstring& sReplace)
    {
      std::wstring temp(source);
      for (size_t j = 0; (j = temp.find(sFind)) != std::wstring::npos;) {
        temp.replace(j, sFind.length(), sReplace);
      }
      return temp;
    }

    std::string StripLeading(const std::string& source, const std::string& find)
    {
      std::string::size_type i = source.find_first_not_of(find);
      if (std::string::npos != i) {
        std::string temp(source);
        return temp.erase(0, i);
      }

      return "";
    }

    std::wstring StripLeading(const std::wstring& source, const std::wstring& find)
    {
      std::wstring::size_type i = source.find_first_not_of(find);
      if (std::wstring::npos != i) {
        std::wstring temp(source);
        return temp.erase(0, i);
      }

      return L"";
    }

    std::string StripTrailing(const std::string& source, const std::string& find)
    {
      std::string::size_type iEndOfContent = source.find_last_not_of(find);
      if (std::string::npos != iEndOfContent) {
        std::string temp(source);
        return temp.erase(iEndOfContent + 1);
      }

      return source;
    }

    std::wstring StripTrailing(const std::wstring& source, const std::wstring& find)
    {
      std::wstring::size_type iEndOfContent = source.find_last_not_of(find);
      if (std::wstring::npos != iEndOfContent) {
        std::wstring temp(source);
        return temp.erase(iEndOfContent + 1);
      }

      return source;
    }


    std::string StripLeadingWhiteSpace(const std::string& source)
    {
      return StripLeading(source, WHITE_SPACE);
    }

    std::wstring StripLeadingWhiteSpace(const std::wstring& source)
    {
      return StripLeading(source, LWHITE_SPACE);
    }

    std::string StripTrailingWhiteSpace(const std::string& source)
    {
      return StripTrailing(source, WHITE_SPACE);
    }

    std::wstring StripTrailingWhiteSpace(const std::wstring& source)
    {
      return StripTrailing(source, LWHITE_SPACE);
    }

    std::string StripBefore(const std::string& source, const std::string& find)
    {
      std::string::size_type i = source.find_first_of(find);
      if (std::string::npos != i) {
        std::string temp(source);
        return temp.erase(0, i);
      }

      return source;
    }

    std::wstring StripBefore(const std::wstring& source, const std::wstring& find)
    {
      std::wstring::size_type i = source.find_first_of(find);
      if (std::wstring::npos != i) {
        std::wstring temp(source);
        return temp.erase(0, i);
      }

      return source;
    }

    std::string StripAfter(const std::string& source, const std::string& find)
    {
      std::string::size_type i = source.find_last_of(find);
      if (std::string::npos != i) {
        std::string temp(source);
        return temp.erase(i);
      }

      return source;
    }

    std::string StripAfterLast(const std::string& source, const std::string& find)
    {
      std::string::size_type i = source.find_last_of(find);
      if (std::string::npos != i) return source.substr(0, i + find.length());

      return source;
    }

    std::wstring StripAfterLast(const std::wstring& source, const std::wstring& find)
    {
      std::wstring::size_type i = source.find_last_of(find);
      if (std::wstring::npos != i) return source.substr(0, i + find.length());

      return source;
    }

    std::string StripBeforeInclusive(const std::string& source, const std::string& find)
    {
      std::string::size_type i = source.find(find);

      // If we found this string then return everything after the found string
      if (std::string::npos != i) return source.substr(i + find.length());

      return source;
    }

    std::wstring StripBeforeInclusive(const std::wstring& source, const std::wstring& find)
    {
      std::wstring::size_type i = source.find(find);

      // If we found this string then return everything after the found string
      if (std::wstring::npos != i) return source.substr(i + find.length());

      return source;
    }

    std::string StripAfterInclusive(const std::string& source, const std::string& find)
    {
      std::string::size_type i = source.find(find);

      // If we found this string then return up to this point
      if (std::string::npos != i) return source.substr(0, i);

      return source;
    }

    std::wstring StripAfterInclusive(const std::wstring& source, const std::wstring& find)
    {
      std::wstring::size_type i = source.find(find);

      // If we found this string then return up to this point
      if (std::wstring::npos != i) return source.substr(0, i);

      return source;
    }

    std::string StripAfterLastInclusive(const std::string& source, const std::string& find)
    {
      std::string::size_type i = source.find_last_of(find);
      if (std::string::npos != i) return source.substr(0, i);

      return source;
    }

    std::wstring StripAfterLastInclusive(const std::wstring& source, const std::wstring& find)
    {
      std::wstring::size_type i = source.find_last_of(find);
      if (std::wstring::npos != i) return source.substr(0, i);

      return source;
    }

    bool StartsWith(const std::string& source, const std::string& find)
    {
      if (source.length() < find.length()) return false;

      return (source.substr(0, find.length()) == find);
    }

    // TODO: Use boost::starts_with
    // TODO: Use boost::istarts_with
    // TODO: Use boost::ends_with
    // TODO: Use boost::iends_with

    bool StartsWith(const std::wstring& source, const std::wstring& find)
    {
      if (source.length() < find.length()) return false;

      return (source.substr(0, find.length()) == find);
    }

    bool EndsWith(const std::string& source, const std::string& find)
    {
      if (source.length() < find.length()) return false;

      return (source.substr(source.length() - find.length()) == find);
    }

    bool EndsWith(const std::wstring& source, const std::wstring& find)
    {
      if (source.length() < find.length()) return false;

      return (source.substr(source.length() - find.length()) == find);
    }


    // *** Case Conversion Functions
    //
    // NOTE: This only handles ASCII, it does not handle other languages such as German, Russian, Swedish, Greek
    // http://www.cplusplus.com/faq/sequences/strings/case-conversion/

    std::string ToUpper(const std::string& text)
    {
      std::string output(text);
      transform(output.begin(), output.end(), output.begin(), ::toupper);
      return output;
    }

    std::wstring ToUpper(const std::wstring& text)
    {
      std::wstring output(text);
      transform(output.begin(), output.end(), output.begin(), ::toupper);
      return output;
    }

    std::string ToLower(const std::string& text)
    {
      std::string output(text);
      transform(output.begin(), output.end(), output.begin(), ::tolower);
      return output;
    }

    std::wstring ToLower(const std::wstring& text)
    {
      std::wstring output(text);
      transform(output.begin(), output.end(), output.begin(), ::tolower);
      return output;
    }


    // *** Comparison functions
    bool CompareChar(char c1, char c2)
    {
      return (c1 == c2) || (std::toupper(c1) == std::toupper(c2));
    }

    bool CompareChar(wchar_t c1, wchar_t c2)
    {
      return (c1 == c2) || (std::toupper(c1) == std::toupper(c2));
    }

    bool IsEqualInsensitive(const std::string& a, const std::string& b)
    {
      return (
        (a.size() == b.size()) &&
        std::equal(a.begin(), a.end(), b.begin(), b.end(), [](auto&& l, auto&& r) { return CompareChar(l, r); })
      );
    }

    bool IsEqualInsensitive(const std::wstring& a, const std::wstring& b)
    {
      return (
        (a.size() == b.size()) &&
        std::equal(a.begin(), a.end(), b.begin(), b.end(), [](auto&& l, auto&& r) { return CompareChar(l, r); })
      );
    }


    // *** Formatting functions

    string_t FormatTime(uint32_t hours, uint32_t minutes, uint32_t seconds)
    {
      return gLocalisedStringTransformer.FormatTime<string_t, char_t, ostringstream_t>(hours, minutes, seconds);
    }



    // UTF8 Surrogate Pairs
    // http://en.wikipedia.org/wiki/UTF-8#Description
    //
    // UTF Conversion
    // http://gears.googlecode.com/svn/trunk/third_party/convert_utf/ConvertUTF.c
    //
    // Index into the table below with the first byte of a UTF-8 sequence to get the number of trailing bytes that are supposed to follow it.
    // Note that *legal* UTF-8 values can't have 4 or 5-bytes.
    // The table is left as-is for anyone who may want to do such conversion, which was allowed in earlier algorithms.
    const size_t trailingBytesUTF8[256] = {
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
      2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
    };

    size_t GetSurrogatePairCountForMultiByteCharacter(char ch)
    {
      #if 0
      size_t c = size_t(ch);
      ASSERT((c < 128) || (c > 191)); // Second, third, or fourth byte of a multi-byte sequence
      ASSERT((c < 192) || (c > 193)); // Overlong encoding: start of 2-byte sequence, but would encode a code point ≤ 127
      ASSERT((c < 245) || (c > 247)); // Restricted by RFC 3629: start of 4-byte sequence for codepoint above 10FFFF
      ASSERT((c < 248) || (c > 251)); // Restricted by RFC 3629: start of 5-byte sequence
      ASSERT((c < 252) || (c > 253)); // Restricted by RFC 3629: start of 6-byte sequence
      ASSERT((c < 254) || (c > 255)); // Invalid: not defined by original UTF-8 specification

      return 1 + trailingBytesUTF8[c];
      #else
      // Using the array above will sometimes return too many bytes
      // http://stackoverflow.com/questions/2948308/how-do-i-read-utf-8-characters-via-a-pointer?rq=1
      size_t seqlen = 0;
      const unsigned char c = ch;
      if ((c & 0x80) == 0) {
        seqlen = 1;
      } else if ((c & 0xE0) == 0xC0) {
        seqlen = 2;
      } else if ((c & 0xF0) == 0xE0) {
        seqlen = 3;
      } else if ((c & 0xF8) == 0xF0) {
        seqlen = 4;
      } else {
        // Malformed UTF8
        ASSERT(false);
      }

      return seqlen;
      #endif
    }

    size_t GetSurrogatePairCountForMultiByteCharacter(char16_t c)
    {
      return ((c >= 0xD800) && (c <= 0xDBFF)) ? 2 : 1;
    }


    // IsValidUTF8
    // Returns true if the string passed in is valid UTF8
    // http://en.wikipedia.org/wiki/UTF-8

    bool IsValidUTF8(const char* _szUTF8)
    {
      const unsigned char* szUTF8 = (const unsigned char*)_szUTF8;

      const size_t len = strlen(_szUTF8);
      for (size_t i = 0; i < len; i++) {
         // If the current char is ANSI skip it
         if (szUTF8[i] < 0x80) continue;
         else if ((szUTF8[i] ^ 0xDF) <= 0x1F) { //110xxxxx
            // make sure there is at least 1 more byte
            if ((i + 1) >= len) return false;

            // make sure the following byte starts with //10xxxxxx
            unsigned char c = szUTF8[i + 1] ^ 0x80;
            if (c > 0x3F) return false;

            // increment
            i += 1;
         } else if ((szUTF8[i] ^ 0xEF) <= 0x0F) { //1110xxxx
            // make sure there are at least two more bytes
            if ((i + 2) >= len) return false;

            // make sure the following 2 bytes start with //10xxxxxx
            unsigned char c = (szUTF8[i + 1] & szUTF8[i + 2]) ^ 0x80;
            if (c > 0x3F) return false;

            // increment
            i += 2;
         } else if ((szUTF8[i] ^ 0xF7) <= 0x07) { //11110xxx
            // make sure there are at least three more bytes
            if ((i + 3) >= len) return false;

            // make sure the following 3 bytes start with //10xxxxxx
            unsigned char c = (szUTF8[i + 1] & szUTF8[i + 2] & szUTF8[i + 3]) ^ 0x80;
            if (c > 0x3F) return false;

            // increment
            i += 3;
         } else if ((szUTF8[i] ^ 0xFB) <= 0x03) { //111110xx
            // make sure there are at least four more bytes
            if ((i + 4) >= len) return false;

            // make sure the following 4 bytes start with //10xxxxxx
            unsigned char c = (szUTF8[i + 1] & szUTF8[i + 2] & szUTF8[i + 3] & szUTF8[i + 4]) ^ 0x80;
            if (c > 0x3F) return false;

            // increment
            i += 4;
         } else if ((szUTF8[i] ^ 0xBF) <= 0x01) { //1111110x
            // make sure there are at least five more bytes
            if ((i + 5) >= len) return false;

            // make sure the following 5 bytes start with //10xxxxxx
            unsigned char c = (szUTF8[i + 1] & szUTF8[i + 2] & szUTF8[i + 3] & szUTF8[i + 4] & szUTF8[i + 5]) ^ 0x80;
            if (c > 0x3F) return false;

            // increment
            i += 5;
         } else return false;
      }

      return true;
    }
    
    // Utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
    // http://en.cppreference.com/w/cpp/locale/wstring_convert/~wstring_convert
    template<class Facet>
    struct DeletableFacet : public Facet
    {
      template<class ...Args>
      DeletableFacet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
      ~DeletableFacet() {}
    };

    std::wstring ToWchar_t(const std::string& source)
    {
      typedef DeletableFacet<std::codecvt_byname<wchar_t, char, std::mbstate_t>> local_facet_t;
      std::wstring_convert<local_facet_t> convert(new local_facet_t(""));
      return convert.from_bytes(source);
    }

    std::string ToUTF8(const std::wstring& source)
    {
      std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
      return convert.to_bytes(source);
    }


    // TODO: Do a correct conversion
    std::string ToASCII(const std::wstring& source)
    {
      return ToUTF8(source);
    }

    // TODO: Do a correct conversion
    std::string ToASCII(const std::string& source)
    {
      return ToUTF8(source);
    }


    // String to hex
    // Converts a string containing a hexadecimal number to an unsigned integer
    // eg. "FE1234" -> 16650804
    // atoh : ASCII
    // wtoh : UNICODE

    template <class T, class C>
    uint32_t GenericFromHexStringToUint32_t(const T& source)
    {
      uint32_t value = 0;

      const size_t n = source.length();
      for (size_t i = 0; i < n; i++) {
        C c = source[i];
        ASSERT(c != 0);

        uint32_t digit;
        if ((c >= C('0')) && (c <= '9')) digit = uint32_t((c - C('0')));
        else if ((c >= C('a')) && (c <= C('f'))) digit = uint32_t((c - C('a'))) + 10;
        else if ((c >= C('A')) && (c <= C('F'))) digit = uint32_t((c - C('A'))) + 10;
        else break;

        value = (value << 4) + digit;
      }

      return value;
    }

    uint32_t FromHexStringToUint32_t(const std::string& source)
    {
      return GenericFromHexStringToUint32_t<std::string, char>(source);
    }

    uint32_t FromHexStringToUint32_t(const std::wstring& source)
    {
      return GenericFromHexStringToUint32_t<std::wstring, wchar_t>(source);
    }

    string_t ToHexString(uint16_t value)
    {
      // NOTE: If we just print normally with ss<<std::setfill('0')<<std::setw(8)<<std::hex<<value we can get something like "12,345,678" depending on the locale, so we print each octet separately

      ostringstream_t ss;

      ss<<std::uppercase<<std::setfill('0')<<std::setw(2)<<std::hex<<uint32_t((value & 0xFF00) >> 8);
      ss<<std::uppercase<<std::setfill('0')<<std::setw(2)<<std::hex<<uint32_t(value & 0x00FF);

      return ss.str();
    }


    string_t ToHexString(uint32_t value)
    {
      // NOTE: If we just print normally with ss<<std::setfill('0')<<std::setw(8)<<std::hex<<value we can get something like "12,345,678" depending on the locale, so we print each octet separately

      ostringstream_t ss;

      ss<<std::uppercase<<std::setfill('0')<<std::setw(2)<<std::hex<<uint32_t(value >> 24);
      ss<<std::uppercase<<std::setfill('0')<<std::setw(2)<<std::hex<<uint32_t((value & 0x00FF0000) >> 16);
      ss<<std::uppercase<<std::setfill('0')<<std::setw(2)<<std::hex<<uint32_t((value & 0x0000FF00) >> 8);
      ss<<std::uppercase<<std::setfill('0')<<std::setw(2)<<std::hex<<uint32_t(value & 0x000000FF);

      return ss.str();
    }

    string_t ToHexString(uint8_t red, uint8_t green, uint8_t blue)
    {
      ostringstream_t ss;

      if (red < 0x10) ss<<TEXT("0");
      ss<<std::hex<<std::uppercase<<uint32_t(red);

      if (green < 0x10) ss<<TEXT("0");
      ss<<std::hex<<std::uppercase<<uint32_t(green);

      if (blue < 0x10) ss<<TEXT("0");
      ss<<std::hex<<std::uppercase<<uint32_t(blue);

      return ss.str();
    }

    string_t ToHexString(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    {
      ostringstream_t ss;

      if (red < 0x10) ss<<TEXT("0");
      ss<<std::hex<<std::uppercase<<uint32_t(red);

      if (green < 0x10) ss<<TEXT("0");
      ss<<std::hex<<std::uppercase<<uint32_t(green);

      if (blue < 0x10) ss<<TEXT("0");
      ss<<std::hex<<std::uppercase<<uint32_t(blue);

      if (alpha < 0x10) ss<<TEXT("0");
      ss<<std::hex<<std::uppercase<<uint32_t(alpha);

      return ss.str();
    }


    struct cSpecialCharacter
    {
      std::string text;
      int entityCode;
    };


    // Also note: hex and decimal
    // Only list decimal in source code, generate hex encodings as well
    // std::map<int, std::string> decimalToStringEncodings;
    // std::map<std::string, int> stringToDecimalEncodings;
    //
    // &#38; // Decimal
    // &#x26; // Hex
    //
    // Both represent '&' character
    //
    // int GetDecimalValue()
    // {
    //   value = get value;
    //
    //   if (x) return hexToDecimal(value);
    //
    //   return value;
    // }

    const cSpecialCharacter specialCharactersArray[] = {
      { "quot", 34 },    { "amp", 38 },     { "lt", 60 },      { "gt", 62 },      { "nbsp", 160 },
      { "iexcl", 161 },  { "cent", 162 },   { "pound", 163 },  { "curren", 164 }, { "yen", 165 },
      { "brvbar", 166 }, { "sect", 167 },   { "um", 168 },     { "copy", 169 },   { "ordf", 170 },
      { "laquo", 171 },  { "not", 172 },    { "shy", 173 },    { "reg", 174 },    { "macr", 175 },
      { "deg", 176 },    { "plusmn", 177 }, { "sup2", 178 },   { "sup3", 179 },   { "acute", 180 },
      { "micro", 181 },  { "para", 182 },   { "middot", 183 }, { "cedil", 184 },  { "sup1", 185 },
      { "ordm", 186 },   { "raquo", 187 },  { "frac14", 188 }, { "frac12", 189 }, { "frac34", 190 },
      { "iquest", 191 }, { "Agrave", 192 }, { "Aacute", 193 }, { "Acirc", 194 },  { "Atilde", 195 },
      { "Auml", 196 },   { "Aring", 197 },  { "AElig", 198 },  { "Ccedil", 199 }, { "Egrave", 200 },
      { "Eacute", 201 }, { "Ecirc", 202 },  { "Euml", 203 },   { "Igrave", 204 }, { "Iacute", 205 },
      { "Icirc", 206 },  { "Iuml", 207 },   { "ETH", 208 },    { "Ntilde", 209 }, { "Ograve", 210 },
      { "Oacute", 211 }, { "Ocirc", 212 },  { "Otilde", 213 }, { "Ouml", 214 },   { "times", 215 },
      { "Oslash", 216 }, { "Ugrave", 217 }, { "Uacute", 218 }, { "Ucirc", 219 },  { "Uuml", 220 },
      { "Yacute", 221 }, { "THORN", 222 },  { "szlig", 223 },  { "agrave", 224 }, { "aacute", 225 },
      { "acirc", 226 },  { "atilde", 227 }, { "auml", 228 },   { "aring", 229 },  { "aelig", 230 },
      { "ccedil", 231 }, { "egrave", 232 }, { "eacute", 233 }, { "ecirc", 234 },  { "euml", 235 },
      { "igrave", 236 }, { "iacute", 237 }, { "icirc", 238 },  { "iuml", 239 },   { "eth", 240 },
      { "ntilde", 241 }, { "ograve", 242 }, { "oacute", 243 }, { "ocirc", 244 },  { "otilde", 245 },
      { "ouml", 246 },   { "divide", 247 }, { "oslash", 248 }, { "ugrave", 249 }, { "uacute", 250 },
      { "ucirc", 251 },  { "uuml", 252 },   { "yacute", 253 }, { "thorn", 254 },  { "yuml", 255 }
    };

    class cSpecialCharacterEncoder
    {
    public:
      cSpecialCharacterEncoder();

      std::string HTMLDecode(const std::string& source);
      std::string HTMLEncode(const std::string& source);

    private:
      std::map<int, std::string> entityCodeToString;
      std::map<std::string, int> stringToEntityCode;
    };

    cSpecialCharacterEncoder::cSpecialCharacterEncoder()
    {
      // Build entityCodeToString and stringToEntityCode from specialCharactersArray
      const size_t n = sizeof(specialCharactersArray) / sizeof(cSpecialCharacter);
      for (size_t i = 0; i < n; i++) {
        entityCodeToString[specialCharactersArray[i].entityCode] = specialCharactersArray[i].text;
        stringToEntityCode[specialCharactersArray[i].text] = specialCharactersArray[i].entityCode;
      }
    }

    std::string cSpecialCharacterEncoder::HTMLDecode(const std::string& source)
    {
      std::ostringstream o;

      const size_t n = source.length();
      for (size_t i = 0; i < n; i++) {
        const char c = source[i];
        if (c == '&') o<<"&amp;";
        else if (c == '<') o<<"&lt;";
        else if (c == '>') o<<"&gt;";
        else if (c == '"') o<<"&quot;";
        else if (c == '\'') o<<"&apos;";
        else o<<c;
      }

      return o.str();
    }

    std::string cSpecialCharacterEncoder::HTMLEncode(const std::string& source)
    {
      std::ostringstream o;

      bool bIsBR = false;
      bool bIsP = false;
      char cReturnChar = 0;
      const size_t n = source.length();
      for (size_t i = 0; i < n + 1; i++) {
        const char c = (i < n) ? source[i] : 0;
        if ((c == '\r') || (c == '\n')) {
          if (bIsBR) {
            if (c == cReturnChar) {
              if (bIsP) o<<"<br>";
              else bIsP = true;
            }
          } else {
            cReturnChar = c;
            bIsBR = true;
            bIsP = false;
          }
          continue;
        }
        if (bIsP) {
          o<<"<p>";
          bIsP = false;
          bIsBR = false;
        } else if (bIsBR) {
          o<<"<br>";
          bIsBR = false;
        }
        if (c == 0) break;

        if (c == '&') o<<"&amp;";
        else if (c == '<') o<<"&lt;";
        else if (c == '>') o<<"&gt;";
        else if (c == '"') o<<"&quot;";
        else if (c == '\'') o<<"&apos;";
        else o<<c;
      }

      return o.str();
    }

    cSpecialCharacterEncoder gSpecialCharacterEncoder;


    std::string HTMLDecode(const std::string& source)
    {
      return gSpecialCharacterEncoder.HTMLDecode(source);
    }

    std::string HTMLEncode(const std::string& source)
    {
      return gSpecialCharacterEncoder.HTMLEncode(source);
    }



    // Format nBytes using IEC standard prefixes, KiB, MiB, GiB, ... using multiples of 1024. I.e., binary "bytes".
    // http://en.wikipedia.org/wiki/Binary_prefix#IEC_standard_prefixes
    string_t GetIECStringFromBytes(uint64_t nBytes)
    {
      const string_t suffixes[] = {
        TEXT("Bytes"), TEXT("KiB"), TEXT("MiB"),
        TEXT("GiB"), TEXT("TiB"), TEXT("PiB"),
        TEXT("EiB"), TEXT("ZiB"), TEXT("YiB")
      };

      const size_t nSuffixes = 9;

      uint64_t ix = 0;
      uint64_t factor = 1;

      const uint64_t step = 1024;

      while (((nBytes / factor) >= step) && (ix < nSuffixes)) {
        factor *= step;
        ix++;
      }

      uint64_t integral = nBytes / factor;    // Integral part of reduced nBytes
      uint64_t remainder = nBytes - (integral * factor); // Remainder.
      // Convert remainder to a decimal fraction
      double fraction  = double(remainder) / double(factor);

      ostringstream_t o;

      o<<ToString(integral);

      // TODO: Do we even use this?
      if (remainder == 0) {
        //o<<TEXT(".0");
      } else if (integral < 10) { // Append two digits from the fraction
        int dec = int(fraction * 100);
        o<<TEXT(".");
        o<<((dec < 10) ? TEXT("0") : TEXT(""));
        o<<ToString(dec);
      } else if (integral < 100) { // Append one digit from the fraction
        int dec = int(fraction * 10);
        o<<TEXT(".");
        o<<ToString(dec);
      }

      o<<" ";
      o<<suffixes[ix];

      return o.str();
    }



    /*// Case is insignificant.
    // Punctuation and symbols are significant for sorting.
    // Digit sub-strings are sorted by numeric value rather than as characters.
    SORT Compare(const string_t& sA, const string_t& sB)
    {
      const string_t sLowerA = ToLower(sA);
      const string_t sLowerB = ToLower(sB);

      if (sLowerA == sLowerB) return SORT_A_IS_EQUAL_TO_B;

      sort = SORT_A_IS_LESS_THAN_B;

      const size_t nA = sLowerA.length();
      const size_t nB = sLowerB.length();
      for (size_t i = 0, size_t j = 0; (i < nA) && (j < nB); i++, j++) {
        const char_t cA = sLowerA[i];
        const char_t cB = sLowerB[j];

        if ( != sLowerB[j]) {
          ...
        }
        ...
      }

      return SORT_A_IS_LESS_THAN_B;
    }*/
  }
}
