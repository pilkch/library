#include <cassert>
#include <cctype> // for toupper/tolower

#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <sstream>
#include <iterator>    // for back_inserter

// writing a text file
#include <iostream>
#include <fstream>
#include <iomanip>

#include <locale>

#include <breathe/breathe.h>
#include <breathe/util/cString.h>
#include <breathe/util/log.h>

#define WHITE_SPACE "\t\v\r\n"
#define LWHITE_SPACE L"\t\v\r\n"

namespace breathe
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

    private:
      std::locale loc_c;

      cToUpper upper;
      cToLower lower;
    };

    cLocalisedStringTransformer::cLocalisedStringTransformer() :
      loc_c("C"),
      upper(loc_c),
      lower(loc_c)
    {
    }

    cLocalisedStringTransformer gLocalisedStringTransformer;



    bool IsWhiteSpace(char_t c)
    {
      // We regard space, tab, new line and carriage return characters as white space
      return (c == TEXT(' ')) || (c == TEXT('\t')) || (c == TEXT('\n')) || (c == TEXT('\r'));
    }

    size_t CountOccurrences(const std::string& source, const std::string& sFind)
    {
      size_t n = 0;

      size_t i = 0;
      size_t j;
      for (;(j = source.find(sFind, i)) != std::string::npos; i = j + 1, n++)
        ;

      return n;
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
      size_t j;
      std::string temp(source);
      for (;(j = temp.find(sFind)) != std::string::npos;)
      {
        temp.replace(j, sFind.length(), sReplace);
      }
      return temp;
    }

    std::wstring Replace(const std::wstring& source, const std::wstring& sFind, const std::wstring& sReplace)
    {
      size_t j;
      std::wstring temp(source);
      for (;(j = temp.find(sFind)) != std::wstring::npos;)
      {
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
      if (std::string::npos != i)
        return std::string(source.begin() + i + find.length(), source.end());

      return source;
    }

    std::wstring StripBeforeInclusive(const std::wstring& source, const std::wstring& find)
    {
      std::wstring::size_type i = source.find(find);
      if (std::wstring::npos != i)
        return std::wstring(source.begin() + i + find.length(), source.end());

      return source;
    }

    std::string StripAfterInclusive(const std::string& source, const std::string& find)
    {
      std::string::size_type i = source.find_first_of(find);
      if (std::string::npos != i) {
        std::string temp(source);
        //std::string::size_type len = find.length();
        return temp.erase(i);
      }

      return source;
    }

    std::wstring StripAfterInclusive(const std::wstring& source, const std::wstring& find)
    {
      std::wstring::size_type i = source.find_first_of(find);
      if (std::wstring::npos != i) {
        std::wstring temp(source);
        //std::wstring::size_type len = find.length();
        return temp.erase(i);
      }

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
      size_t n = source.length();
      size_t i = 0;
      for (; i<n ; ++i) {
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
      s << value;
      return s.str();
    }

    string_t ToString(unsigned int value)
    {
      return ToString<unsigned int>(value);
    }

    string_t ToString(size_t value)
    {
      return ToString<size_t>(value);
    }

    string_t ToString(int value)
    {
      return ToString<int>(value);
    }

    string_t ToString(float value)
    {
      return ToString<float>(value);
    }

    unsigned int ToUnsignedInt(const string_t& source)
    {
      unsigned int value = 0;
      istringstream_t stm(source);
      stm >> value;
      return value;
    }

    int ToInt(const string_t& source)
    {
      int value = 0;
      istringstream_t stm(source);
      stm >> value;
      return value;
    }

    float ToFloat(const string_t& source)
    {
      float value = 0.0f;
      istringstream_t stm(source);
      stm >> value;
      return value;
    }

    // text to hex
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


    struct cSpecialCharacter
    {
      std::string text;
      int entityCode;
    };

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

      // TODO: Do stuff :)
      o<<source;

      return o.str();
    }

    std::string cSpecialCharacterEncoder::HTMLEncode(const std::string& source)
    {
      std::ostringstream o;

      // TODO: Do stuff :)
      o<<source;

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
  }
}