#include <cassert>
#include <cctype> // for toupper/tolower
#include <cmath>

#include <string>
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

#include <iconv.h>
#include <errno.h>

#include <spitfire/spitfire.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

// Test strings
// http://www.columbia.edu/kermit/utf8.html

#define WHITE_SPACE "\t\v\r\n"
#define LWHITE_SPACE L"\t\v\r\n"

namespace spitfire
{
  namespace string
  {
    class cToUpper
    {
    public:
      explicit cToUpper(const std::locale& l) : loc(l) {}

      char operator() (char c) const { return std::toupper(c,loc); }
      wchar_t operator() (wchar_t c) const { return std::toupper(c,loc); }

    private:
      std::locale const& loc;
    };

    class cToLower
    {
    public:
      explicit cToLower(const std::locale& l) : loc(l) {}

      char operator() (char c) const { return std::tolower(c,loc); }
      wchar_t operator() (wchar_t c) const { return std::tolower(c,loc); }

    private:
      std::locale const& loc;
    };

    class cLocalisedStringTransformer
    {
    public:
      cLocalisedStringTransformer();

      // Other Possible functions
      // bool isspace(c, loc);
      // bool isupper(c, loc);
      // bool islower(c, loc);
      // bool isalpha(c, loc);
      // bool isdigit(c, loc);
      // bool ispunct(c, loc);
      // bool isxdigit(c, loc);
      // bool isalnum(c, loc);

      template <class T>
      inline T ToLower(const T& source) const
      {
        T sOut;
        std::transform(source.begin(), source.end(), std::back_inserter(sOut), lower);
        return sOut;
      }

      template <class T>
      inline T ToUpper(const T& source) const
      {
        T sOut;
        std::transform(source.begin(), source.end(), std::back_inserter(sOut), upper);
        return sOut;
      }

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

        // Get the time_put facet
        const std::time_put<C>& tmput = std::use_facet<std::time_put<C> > (loc);

        tm now;

        // We don't care about the other fields for this formatting, perhaps we should set them too though just in case?
        now.tm_hour = hours;
        now.tm_min = minutes;
        now.tm_sec = seconds;

        // http://www.cplusplus.com/reference/clibrary/ctime/strftime/
        tmput.put(o, o, ' ', &now, 'X');

        return o.str();
      }

    private:
      std::locale loc;

      cToUpper upper;
      cToLower lower;
    };

    cLocalisedStringTransformer::cLocalisedStringTransformer() :
      loc("C"),
      upper(loc),
      lower(loc)
    {
    }

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
      return (c == L' ') || (c == L'\t') || (c == L'\n') || (c == L'\r');
    }

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

      return source;
                                }

    std::wstring StripLeading(const std::wstring& source, const std::wstring& find)
    {
      std::wstring::size_type i = source.find_first_not_of(find);
      if (std::wstring::npos != i) {
        std::wstring temp(source);
        return temp.erase(0, i);
      }

      return source;
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

    bool BeginsWith(const std::string& source, const std::string& find)
    {
      if (source.length() < find.length()) return false;

      return (source.substr(0, find.length()) == find);
    }

    bool BeginsWith(const std::wstring& source, const std::wstring& find)
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


    // *** Conversion Functions

    std::string ToLower(const std::string& source)
    {
      return gLocalisedStringTransformer.ToLower(source);
    }

    std::wstring ToLower(const std::wstring& source)
    {
      return gLocalisedStringTransformer.ToLower(source);
    }

    std::string ToUpper(const std::string& source)
    {
      return gLocalisedStringTransformer.ToUpper(source);
    }

    std::wstring ToUpper(const std::wstring& source)
    {
      return gLocalisedStringTransformer.ToUpper(source);
    }



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

    size_t GetSurrogatePairCountForMultiByteCharacter(char8_t ch)
    {
      size_t c = size_t(ch);
      ASSERT((c < 128) || (c > 191)); // Second, third, or fourth byte of a multi-byte sequence
      ASSERT((c < 192) || (c > 193)); // Overlong encoding: start of 2-byte sequence, but would encode a code point ≤ 127
      ASSERT((c < 245) || (c > 247)); // Restricted by RFC 3629: start of 4-byte sequence for codepoint above 10FFFF
      ASSERT((c < 248) || (c > 251)); // Restricted by RFC 3629: start of 5-byte sequence
      ASSERT((c < 252) || (c > 253)); // Restricted by RFC 3629: start of 6-byte sequence
      ASSERT((c < 254) || (c > 255)); // Invalid: not defined by original UTF-8 specification

      return 1 + trailingBytesUTF8[c];
    }

    size_t GetSurrogatePairCountForMultiByteCharacter(char16_t c)
    {
      return ((c >= 0xD800) && (c <= 0xDBFF)) ? 2 : 1;
    }

    /*
    // This doesn't seem to work correctly, it needs more debugging and testing

    // http://www.manpagez.com/man/3/iconv_open/
    // http://www.manpagez.com/man/3/iconv/

    template <class I, class O, class OT>
    void ConvertString(const I& sIn, O& sOut, const char* szInFormat, const char* szOutFormat)
    {
      //LOG<<"ConvertString szIn=\""<<sIn<<"\""<<std::endl;

      sOut[0] = 0;

      iconv_t i = iconv_open(szOutFormat, szInFormat);
      if (i == (iconv_t) -1) {
        //if (errno == EINVAL) LOG<<"ConvertString iconv_open FAILED Conversion from ISO-8859-1 to UTF-8 not available"<<std::endl;
        //else LOG<<"ConvertString iconv_open FAILED Unknown reason "<<errno<<std::endl;

        return;
      }

      const size_t inLength = sIn.length();
      size_t fromlen = inLength;

      char* szIn = const_cast<char*>((const char*)sIn.data());

      // Our output list of chunks of text
      std::list<std::vector<OT>*> outChunks;

      size_t nRead = 0;
      while (nRead < inLength) {
        // Temporary buffer for storing each part of the conversion
        const size_t nChunkLength = 80;
        std::vector<OT>* pNewChunk = new std::vector<OT>;
        pNewChunk->resize(nChunkLength);

        char* szOut = (char*)pNewChunk;
        size_t outLength = nChunkLength;

        fromlen = inLength - nRead;

        //LOG<<"ConvertString BEFORE fromlen="<<fromlen<<", outLength="<<outLength<<", szIn=\""<<szIn<<"\", szOut=\""<<szOut<<"\""<<std::endl;

        const size_t iconv_value = iconv(i, &szIn, &fromlen, &szOut, &outLength);
        if (iconv_value == size_t(-1)) {
          if (errno == E2BIG) {
            // We handle this case, so not really an error
            //LOG<<"ConvertString iconv FAILED \"There is not sufficient room at *outbuf\"");
            //break;
          } else if (errno == EILSEQ) {
            LOG<<"ConvertString iconv FAILED \"An invalid multibyte sequence has been encountered in the input\""<<std::endl;
            break;
          } else if (errno == EINVAL) {
            LOG<<"ConvertString iconv FAILED \"An  incomplete  multibyte  sequence  has been encountered in the input\""<<std::endl;
            break;
          } else {
            LOG<<"ConvertString iconv FAILED Could not convert string, error: "<<errno<<std::endl;
            break;
          }
        }

        //LOG<<"ConvertString AFTER fromlen="<<fromlen<<", outLength="<<outLength<<", szIn=\""<<szIn<<"\", szOut=\""<<szOut<<"\""<<std::endl;

        nRead += fromlen;

        pNewChunk->resize(fromlen);
        outChunks.push_back(pNewChunk);
      }

      iconv_close(i);

      ASSERT(fromlen == 0); // The whole string should have been processed

      // Build our output string from the chunks
      size_t n = 0;
      {
        // Get our output buffer size
        std::list<OT*>::const_iterator iter = outChunks.begin();
        const std::list<OT*>::const_iterator iterEnd = outChunks.end();
        while (iter != iterEnd) {
          const OT* pChunk = *iter;

          n += pChunk->size();

          iter++;
        }
      }

      OT szBuffer[n];
      {
        // Build our output buffer
        size_t i = 0;

        std::list<OT*>::iterator iter = outChunks.begin();
        const std::list<OT*>::iterator iterEnd = outChunks.end();
        while (iter != iterEnd) {
          OT* pChunk = *iter;

          memcpy(&szBuffer[i], pChunk->data(), pChunk->size() * sizeof(OT));

          delete [] pChunk;

          iter++;
        }
      }

      assert(szBuffer[n] == 0);

      sOut = szBuffer;
    }

    std::string ToUTF8(const std::wstring& source)
    {
      std::string result;

    #ifdef __WIN__
      ConvertString<std::wstring, std::string, char>(source, result, "UTF-16LE", "UTF-8");
    #else
      ConvertString<std::wstring, std::string, char>(source, result, "UTF-32LE", "UTF-8");
    #endif

      return result;
    }

    std::wstring ToWchar_t(const std::string& source)
    {
      // This doesn't work yet, ConvertString needs to be fixed
      ASSERT(false);

      std::wstring result;

    #ifdef __WIN__
      ConvertString<std::string, std::wstring, wchar_t>(source, result, "UTF-8", "UTF-16LE");
    #else
      ConvertString<std::string, std::wstring, wchar_t>(source, result, "UTF-8", "UTF-32LE");
    #endif

      return result;
    }*/



    std::wstring ToWchar_t(const std::string& source)
    {
      std::wstring result(source.size(), char(0));
      typedef std::ctype<wchar_t> ctype_t;
      const ctype_t& ct = std::use_facet<ctype_t>(std::locale());
      ct.widen(source.data(), source.data() + source.size(), &(*result.begin()));
      return result;
    }


    std::string ToUTF8(const std::wstring& source)
    {
      std::string result(source.size(), char(0));
      typedef std::ctype<wchar_t> ctype_t;
      const ctype_t& ct = std::use_facet<ctype_t>(std::locale());
#ifdef __LINUX__
      ct.narrow(source.data(), source.data() + source.size(), '0', &(*result.begin()));
#else
      ct.narrow(source.data(), source.data() + source.size(), '\u00B6', &(*result.begin()));
#endif
      return result;
    }

    /*std::string ToUTF8(const std::wstring& source)
    {
      std::ostringstream stm;
      const std::ctype<char>& ctfacet = std::use_facet< std::ctype<char> >(stm.getloc());
      const size_t n = source.length();
      for (size_t i = 0; i < n; ++i) {
        wchar_t a = source[i];
        wchar_t c = ctfacet.narrow(a, 0);
        stm<<c;
      }
      return stm.str();

      //std::string temp;
      //temp.reserve(source.length());
      //std::transform(source.begin(), source.end(), temp.begin(), (int(*)(int))std::ios::narrow);

      //const size_t len = source.length() + 1;
      //char* pTemp = new char[len];
      //wcstombs(pTemp, source.data(), len);
      //std::string temp(pTemp);
      //SAFE_DELETE_ARRAY(pTemp);

      //return temp;
  }*/


    template <class T>
    inline string_t ToString(T value)
    {
      ostringstream_t s;
      s<<value;
      return s.str();
    }

    string_t ToString(uint8_t value)
    {
      return ToString<uint8_t>(value);
    }

    string_t ToString(int8_t value)
    {
      return ToString<int8_t>(value);
    }

    string_t ToString(uint16_t value)
    {
      return ToString<uint16_t>(value);
    }

    string_t ToString(int16_t value)
    {
      return ToString<int16_t>(value);
    }

    string_t ToString(uint32_t value)
    {
      return ToString<uint32_t>(value);
    }

    string_t ToString(int32_t value)
    {
      return ToString<int32_t>(value);
    }

    string_t ToString(uint64_t value)
    {
      return ToString<uint64_t>(value);
    }

    string_t ToString(int64_t value)
    {
      return ToString<int64_t>(value);
    }

    string_t ToString(float value)
    {
      return ToString<float>(value);
    }


    template <class T>
    inline T FromStringGeneric(const string_t& source)
    {
      T value = T(0);
      istringstream_t stm(source);
      stm>>value;
      return value;
    }

    void FromString(const string_t& source, uint64_t& value)
    {
      value = FromStringGeneric<uint64_t>(source);
    }

    void FromString(const string_t& source, int64_t& value)
    {
      value = FromStringGeneric<int64_t>(source);
    }

    void FromString(const string_t& source, float& value)
    {
      value = FromStringGeneric<float>(source);
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

    string_t ToHexString(uint32_t value)
    {
      ostringstream_t ss;

      for (size_t i = 7; i > 0; i++) {
        if (value < (uint32_t(0x1) << uint32_t(i))) ss<<TEXT("0");
      }

      ss<<std::hex<<value;

      return ss.str();
    }

    string_t ToHexString(uint8_t red, uint8_t green, uint8_t blue)
    {
      const int r = int(red * 255.0f);
      const int g = int(green * 255.0f);
      const int b = int(blue * 255.0f);

      ostringstream_t ss;

      if (r < 0x10) ss<<TEXT("0");
      ss<<std::hex<<r;

      if (g < 0x10) ss<<TEXT("0");
      ss<<std::hex<<g;

      if (b < 0x10) ss<<TEXT("0");
      ss<<std::hex<<b;

      return ss.str();
    }

    string_t ToHexString(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    {
      const int r = int(red * 255.0f);
      const int g = int(green * 255.0f);
      const int b = int(blue * 255.0f);
      const int a = int(alpha * 255.0f);

      ostringstream_t ss;

      if (r < 0x10) ss<<TEXT("0");
      ss<<std::hex<<r;

      if (g < 0x10) ss<<TEXT("0");
      ss<<std::hex<<g;

      if (b < 0x10) ss<<TEXT("0");
      ss<<std::hex<<b;

      if (a < 0x10) ss<<TEXT("0");
      ss<<std::hex<<a;

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


#ifdef BUILD_DEBUG
#include <spitfire/util/log.h>
#include <spitfire/util/unittest.h>

class cStringUnitTest : protected spitfire::util::cUnitTestBase
{
public:
  cStringUnitTest() :
    cUnitTestBase(TEXT("cStringUnitTest"))
  {
  }

  void Test()
  {
    // IEC Byte Size Formats

    ASSERT(spitfire::string::GetIECStringFromBytes(0) == TEXT("0 Bytes"));
    ASSERT(spitfire::string::GetIECStringFromBytes(1023) == TEXT("1023 Bytes"));

    ASSERT(spitfire::string::GetIECStringFromBytes(1024) == TEXT("1 KiB"));
    ASSERT(spitfire::string::GetIECStringFromBytes(1048575) == TEXT("1023 KiB"));

    ASSERT(spitfire::string::GetIECStringFromBytes(1048576) == TEXT("1 MiB"));
    ASSERT(spitfire::string::GetIECStringFromBytes(1073741823) == TEXT("1023 MiB"));

    ASSERT(spitfire::string::GetIECStringFromBytes(1073741824) == TEXT("1 GiB"));
    ASSERT(spitfire::string::GetIECStringFromBytes(1099511627775) == TEXT("1023 GiB"));

    ASSERT(spitfire::string::GetIECStringFromBytes(1099511627776) == TEXT("1 TiB"));
    ASSERT(spitfire::string::GetIECStringFromBytes(1125899906842623) == TEXT("1023 TiB"));

    ASSERT(spitfire::string::GetIECStringFromBytes(1125899906842624) == TEXT("1 PiB"));
    ASSERT(spitfire::string::GetIECStringFromBytes(1152921504606846975) == TEXT("1023 PiB"));

    ASSERT(spitfire::string::GetIECStringFromBytes(1152921504606846976) == TEXT("1 EiB"));


    // Hex conversions

    ASSERT(spitfire::string::FromHexStringToUint32_t(TEXT("0")) == 0x0);
    ASSERT(spitfire::string::ToHexString(0x0) == TEXT("00000000"));
    ASSERT(spitfire::string::ToHexString(0x0, 0x0, 0x0) == TEXT("000000"));
    ASSERT(spitfire::string::ToHexString(0x0, 0x0, 0x0, 0x0) == TEXT("00000000"));

    ASSERT(spitfire::string::FromHexStringToUint32_t(TEXT("FFFFffff")) == 0xFFFFFFFF);
    ASSERT(spitfire::string::ToHexString(0xFF) == TEXT("FFFFFFFF"));
    ASSERT(spitfire::string::ToHexString(0xFF, 0xFF, 0xFF) == TEXT("FFFFFF"));
    ASSERT(spitfire::string::ToHexString(0xFF, 0xFF, 0xFF, 0xFF) == TEXT("FFFFFFFF"));

    ASSERT(spitfire::string::FromHexStringToUint32_t(TEXT("12345678")) == 0x12345678);
    ASSERT(spitfire::string::ToHexString(0x12345678) == TEXT("12345678"));
    ASSERT(spitfire::string::ToHexString(0x12, 0x34, 0x56) == TEXT("123456"));
    ASSERT(spitfire::string::ToHexString(0x12, 0x34, 0x56, 0x78) == TEXT("12345678"));
  }
};

cStringUnitTest gStringUnitTest;
#endif // BUILD_DEBUG
