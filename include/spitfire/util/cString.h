#ifndef CSTRING_H
#define CSTRING_H

#include <spitfire/spitfire.h>

// http://www.cppreference.com/wiki/io/io_flags
// http://www.cppreference.com/wiki/c/string/start
// http://www.i18nguy.com/unicode/c-unicode.html

namespace spitfire
{
  typedef char char8_t;
  typedef std::string string8_t;

  typedef std::u16string string16_t;
  typedef std::u32string string32_t;

  //typedef std::u16string string16_t;
  //typedef std::u32string string32_t;

  #ifdef UNICODE
  typedef wchar_t char_t;
  typedef std::wstring string_t;
  typedef std::wostringstream ostringstream_t;
  typedef std::wistringstream istringstream_t;
  //typedef std::wifstream ifstream_t; // This is not correct, the data read is wchar_t, the filename is still char
  #else
  typedef char char_t;
  typedef std::string string_t;
  typedef std::ostringstream ostringstream_t;
  typedef std::istringstream istringstream_t;
  //typedef std::ifstream ifstream_t;
  #endif


  namespace string
  {
    bool IsWhiteSpace(char_t c);

    // http://www.cppreference.com/wiki/c/string/isxdigit
    // returns true if (A-F, a-f, or 0-9)
    inline bool IsHexDigit(char c) { return (isxdigit(c) == 1); }
    inline bool IsHexDigit(wchar_t c) { return (isxdigit(c) == 1); }

    // Is the character for this key in the printable range of ascii characters?
    // http://en.wikipedia.org/wiki/ASCII#ASCII_printable_characters
    inline bool IsPrintableCharacter(char c) { return (c >= 32); }
    inline bool IsPrintableCharacter(wchar_t c) { return (c >= 32) && (c <= 127); } // NOTE: This just takes care of ASCII printable characters, there are many more printable characters after 127 too, that this function does not cover

    size_t CountOccurrences(const std::string& source, const std::string& find);
    bool Find(const std::string& source, const std::string& find, size_t& indexOut);
    std::string Replace(const std::string& source, const std::string& find, const std::string& replace);
    std::string StripLeading(const std::string& source, const std::string& find);
    std::string StripTrailing(const std::string& source, const std::string& find);
    std::string StripLeadingWhiteSpace(const std::string& source);
    std::string StripTrailingWhiteSpace(const std::string& source);
    std::string StripBefore(const std::string& source, const std::string& find);
    std::string StripAfter(const std::string& source, const std::string& find);
    std::string StripAfterLast(const std::string& source, const std::string& find);
    std::string StripBeforeInclusive(const std::string& source, const std::string& find);
    std::string StripAfterInclusive(const std::string& source, const std::string& find);
    std::string StripAfterLastInclusive(const std::string& source, const std::string& find);
    void Split(const std::string& source, char find, std::vector<std::string>& vOut);
    bool Split(const std::string& source, const std::string& find, std::string& before, std::string& after); // Returns true if find is found and puts the first and second parts in before and after.
    void SplitOnNewLines(const std::string& source, std::vector<std::string>& vOut);
    std::string Trim(const std::string& source);
    bool BeginsWith(const std::string& source, const std::string& find);
    bool EndsWith(const std::string& source, const std::string& find);

    size_t CountOccurrences(const std::wstring& source, const std::wstring& find);
    bool Find(const std::wstring& source, const std::wstring& find, size_t& indexOut);
    std::wstring Replace(const std::wstring& source, const std::wstring& find, const std::wstring& replace);
    std::wstring StripLeading(const std::wstring& source, const std::wstring& find);
    std::wstring StripTrailing(const std::wstring& source, const std::wstring& find);
    std::wstring StripLeadingWhiteSpace(const std::wstring& source);
    std::wstring StripTrailingWhiteSpace(const std::wstring& source);
    std::wstring StripBefore(const std::wstring& source, const std::wstring& find);
    std::wstring StripAfter(const std::wstring& source, const std::wstring& find);
    std::wstring StripAfterLast(const std::wstring& source, const std::wstring& find);
    std::wstring StripBeforeInclusive(const std::wstring& source, const std::wstring& find);
    std::wstring StripAfterInclusive(const std::wstring& source, const std::wstring& find);
    std::wstring StripAfterLastInclusive(const std::wstring& source, const std::wstring& find);
    void Split(const std::wstring& source, wchar_t find, std::vector<std::wstring>& vOut);
    bool Split(const std::wstring& source, const std::wstring& find, std::wstring& before, std::wstring& after); // Returns true if find is found and puts the first and second parts in before and after.
    void SplitOnNewLines(const std::wstring& source, std::vector<std::wstring>& vOut);
    std::wstring Trim(const std::wstring& source);
    bool BeginsWith(const std::wstring& source, const std::wstring& find);
    bool EndsWith(const std::wstring& source, const std::wstring& find);

    std::string HTMLDecode(const std::string& source);
    std::string HTMLEncode(const std::string& source);

    std::string ToLower(const std::string& source);
    std::string ToUpper(const std::string& source);

    std::wstring ToLower(const std::wstring& source);
    std::wstring ToUpper(const std::wstring& source);


    // UTF8 Surrogate Pairs
    // http://en.wikipedia.org/wiki/UTF-8#Description

    // For UTF8 this function will return 1, 2, 3, 4 or 5
    // For UTF16 this function will return 1 or 2
    // For UTF32 this function will always return 1
    size_t GetSurrogatePairCountForMultiByteCharacter(char8_t c);
    size_t GetSurrogatePairCountForMultiByteCharacter(char16_t c);
    inline size_t GetSurrogatePairCountForMultiByteCharacter(char32_t c) { return 1; }

    std::string ToUTF8(const std::wstring& source);
    inline std::string ToUTF8(const std::string& source) { return source; }

    std::wstring ToWchar_t(const std::string& source);
    inline std::wstring ToWchar_t(const std::wstring& source) { return source; }

#ifdef UNICODE
    inline string_t ToString_t(const std::string& source) { return ToWchar_t(source); }
    inline string_t ToString_t(const std::wstring& source) { return source; }
#else
    inline string_t ToString_t(const std::string& source) { return source; }
    inline string_t ToString_t(const std::wstring& source) { return ToUTF8(source); }
#endif

    inline string_t ToString(bool value) { return (value ? TEXT("true") : TEXT("false")); }
    string_t ToString(uint64_t value);
    string_t ToString(int64_t value);
    string_t ToString(float value);

    // returns true for anything other than "false" and "0"
    inline bool ToBool(const string_t& source) { return ((TEXT("false") != source) && (TEXT("0") != source)); }
    uint64_t ToUnsignedInt(const string_t& source);
    int64_t ToInt(const string_t& source);
    float ToFloat(const string_t& source);

    // String to hex
    // Converts a string containing a hexadecimal number to an unsigned integer
    // eg. "FE1234" -> 16650804

    uint32_t FromHexStringToUint32_t(const std::string& source);
    uint32_t FromHexStringToUint32_t(const std::wstring& source);

    // Hex to string
    // Converts an unsigned integer to a string containing a hexadecimal number
    // eg. 16650804 -> "FE1234"

    string_t ToHexString(uint32_t value);
    string_t ToHexString(uint8_t red, uint8_t green, uint8_t blue);
    string_t ToHexString(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);


    template <class T>
    inline T ConvertFromHexDigit(char hex)
    {
      if (isalpha(hex)) return T(hex) - T('0');
      else if (isupper(hex)) return T(hex) - T('a') + T(0xA);

      return T(hex) - T('a') + T(0xa);
    }

    template <class T>
    inline T ConvertFromHexDigit(wchar_t hex)
    {
      if (isalpha(hex)) return T(hex) - T('0');
      else if (isupper(hex)) return T(hex) - T('a') + T(0xA);

      return T(hex) - T('a') + T(0xa);
    }

    template <class T>
    inline char ConvertToHexDigit(T value)
    {
      ASSERT(value < 16);

      if (value < T(10)) return '0' + char(value);

      return 'a' + char(value) - char(10);
    }


    string_t FormatTime(uint32_t hours, uint32_t minutes, uint32_t seconds);


    // http://en.wikipedia.org/wiki/Binary_prefix#IEC_standard_prefixes
    string_t GetIECStringFromBytes(uint64_t nBytes);

    enum class SORT {
      A_IS_EQUAL_TO_B,
      A_IS_LESS_THAN_B,
      A_IS_GREATER_THAN_B,
    };

    // Case is insignificant.
    // Punctuation and symbols are significant for sorting.
    // Digit sub-strings are sorted by numeric value rather than as characters.
    SORT Compare(const string_t& sA, const string_t& sB);





    template <class C, class S>
    class cStringParserTemplate
    {
    public:
      cStringParserTemplate();
      explicit cStringParserTemplate(const S& sString);

      cStringParserTemplate(const cStringParserTemplate& rhs);
      cStringParserTemplate& operator=(const cStringParserTemplate& rhs);

      bool IsEmpty() const;
      bool IsEnd() const { return IsEmpty(); }

      S GetCharacter() const; // Returns a string because UTF8 and UTF16 can have surrogate pairs
      S GetCharacters(size_t nSurrogatePairs) const; // Returns a string of n surrogate pairs
      bool GetToString(const S& sFind, S& sResult) const; // Returns true if sFind is found, else returns false
      S GetToEnd() const; // Returns the remaining string

      S GetCharacterAndSkip(); // Returns a string because UTF8 and UTF16 can have surrogate pairs
      bool GetToStringAndSkip(const S& sFind, S& sResult); // Returns true if sFind is found and skips it, else returns false
      S GetToEndAndSkip(); // Returns the remaining string and skips to the end

      void SkipCharacter(); // Skips ahead one surrogate pair
      void SkipCharacters(size_t nSurrogatePairs); // Skips ahead n surrogate pairs
      bool SkipToString(const S& sFind); // Skips to sFind if found
      bool SkipToStringAndSkip(const S& sFind); // Skips to sFind if found and skips it also
      void SkipToEnd();

    private:
      S sString;
    };

    template <class C, class S>
    cStringParserTemplate<C, S>::cStringParserTemplate()
    {
    }

    template <class C, class S>
    cStringParserTemplate<C, S>::cStringParserTemplate(const S& _sString) :
      sString(_sString)
    {
    }

    template <class C, class S>
    cStringParserTemplate<C, S>::cStringParserTemplate(const cStringParserTemplate& rhs) :
      sString(rhs.sString)
    {
    }

    template <class C, class S>
    cStringParserTemplate<C, S>& cStringParserTemplate<C, S>::operator=(const cStringParserTemplate& rhs)
    {
      sString = rhs.sString;
      return *this;
    }

    template <class C, class S>
    bool cStringParserTemplate<C, S>::IsEmpty() const
    {
      return sString.empty();
    }

    template <class C, class S>
    S cStringParserTemplate<C, S>::GetCharacter() const
    {
      ASSERT(!IsEmpty());

      const size_t nElementCount = GetSurrogatePairCountForMultiByteCharacter(sString[0]);
      return sString.substr(0, nElementCount);
    }

    template <class C, class S>
    S cStringParserTemplate<C, S>::GetCharacters(size_t nSurrogatePairs) const
    {
      ASSERT(!IsEmpty());

      size_t nElementCount = 0;
      for (size_t i = 0; i < nSurrogatePairs; i++) {
        // If we are at the end of the string we are finished incrementing our element count
        if (sString[nElementCount] == 0) break;

        nElementCount += GetSurrogatePairCountForMultiByteCharacter(sString[nElementCount]);
      }

      return sString.substr(0, nElementCount);
    }

    template <class C, class S>
    bool cStringParserTemplate<C, S>::GetToString(const S& sFind, S& sResult) const
    {
      ASSERT(!IsEmpty());

      sResult.clear();

      std::string::size_type i = sString.find(sFind);

      // If we found this string then return everything after the found string
      if (std::string::npos != i) {
        sResult = sString.substr(i);
        return true;
      }

      return false;
    }

    template <class C, class S>
    S cStringParserTemplate<C, S>::GetToEnd() const
    {
      ASSERT(!IsEmpty());

      return sString;
    }

    template <class C, class S>
    S cStringParserTemplate<C, S>::GetCharacterAndSkip()
    {
      ASSERT(!IsEmpty());

      const S sResult = GetCharacter();
      sString = sString.substr(sResult.length());
      return sResult;
    }

    template <class C, class S>
    bool cStringParserTemplate<C, S>::GetToStringAndSkip(const S& sFind, S& sResult)
    {
      ASSERT(!IsEmpty());

      bool bResult = GetToString(sFind, sResult);
      sString = sString.substr(sResult.length());

      return bResult;
    }

    template <class C, class S>
    S cStringParserTemplate<C, S>::GetToEndAndSkip()
    {
      ASSERT(!IsEmpty());

      const S sResult = GetToEnd();
      sString = sString.substr(sResult.length());
      return sResult;
    }

    template <class C, class S>
    void cStringParserTemplate<C, S>::SkipCharacter()
    {
      ASSERT(!IsEmpty());

      const size_t nElementCount = GetSurrogatePairCountForMultiByteCharacter(sString[0]);
      sString = sString.substr(nElementCount);
    }

    template <class C, class S>
    void cStringParserTemplate<C, S>::SkipCharacters(size_t nSurrogatePairs)
    {
      ASSERT(!IsEmpty());

      size_t nElementCount = 0;
      for (size_t i = 0; i < nSurrogatePairs; i++) {
        // If we are at the end of the string we are finished incrementing our element count
        if (sString[nElementCount] == 0) break;

        nElementCount += GetSurrogatePairCountForMultiByteCharacter(sString[nElementCount]);
      }

      sString = sString.substr(nElementCount);
    }

    template <class C, class S>
    bool cStringParserTemplate<C, S>::SkipToString(const S& sFind)
    {
      ASSERT(!IsEmpty());

      std::string::size_type i = sString.find(sFind);

      // If we found this string then return everything after the found string
      if (std::string::npos != i) {
        sString = sString.substr(i);
        return true;
      }

      return false;
    }

    template <class C, class S>
    bool cStringParserTemplate<C, S>::SkipToStringAndSkip(const S& sFind)
    {
      ASSERT(!IsEmpty());

      std::string::size_type i = sString.find(sFind);

      // If we found this string then return everything after the found string
      if (std::string::npos != i) {
        sString = sString.substr(i + sFind.length());
        return true;
      }

      return false;
    }

    template <class C, class S>
    void cStringParserTemplate<C, S>::SkipToEnd()
    {
      ASSERT(!IsEmpty());

      sString.clear();
    }


    typedef cStringParserTemplate<char8_t, string8_t> cStringParserUTF8;
    typedef cStringParserTemplate<char16_t, string16_t> cStringParserUTF16;
    typedef cStringParserTemplate<char32_t, string32_t> cStringParserUTF32;

    typedef cStringParserTemplate<char_t, string_t> cStringParser;
  }
}

#endif // CSTRING_H
