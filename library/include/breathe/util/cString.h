#ifndef CSTRING_H
#define CSTRING_H

namespace breathe
{
  #ifdef UNICODE
  typedef std::wstringstream stringstream_t;
  typedef std::wstring string_t;
  #else
  typedef std::stringstream stringstream_t;
  typedef std::string string_t;
  #endif

  #if SIZEOF_WCHAR_T == 4
  typedef std::wstring string16_t;
  typedef std::basic_string<char32_t> string32_t;
  #else
  typedef std::basic_string<char16_t> string16_t;
  typedef std::wstring string32_t;
  #endif

	namespace string
	{
		bool IsWhiteSpace(breathe::char_t c);
		
		size_t CountOccurrences(const std::string& source, const std::string& find);
		std::string Replace(const std::string& source, const std::string& find, const std::string& replace);
		std::string StripLeading(const std::string& source, const std::string& find);
		std::string StripTrailing(const std::string& source, const std::string& find);
		std::string StripBefore(const std::string& source, const std::string& find);
		std::string StripAfter(const std::string& source, const std::string& find);
		std::string StripAfterLast(const std::string& source, const std::string& find);
		std::string StripBeforeInclusive(const std::string& source, const std::string& find);
		std::string StripAfterInclusive(const std::string& source, const std::string& find);
		std::string StripAfterLastInclusive(const std::string& source, const std::string& find);
		void Split(const std::string& source, char find, std::vector<std::string>& vOut);
		void SplitOnNewLines(const std::string& source, std::vector<std::string>& vOut);
		std::string Trim(const std::string& source);
		bool BeginsWith(const std::string& source, const std::string& find);
		bool EndsWith(const std::string& source, const std::string& find);

		size_t CountOccurrences(const std::wstring& source, const std::wstring& find);
		std::wstring Replace(const std::wstring& source, const std::wstring& find, const std::wstring& replace);
		std::wstring StripLeading(const std::wstring& source, const std::wstring& find);
		std::wstring StripTrailing(const std::wstring& source, const std::wstring& find);
		std::wstring StripBefore(const std::wstring& source, const std::wstring& find);
		std::wstring StripAfter(const std::wstring& source, const std::wstring& find);
		std::wstring StripAfterLast(const std::wstring& source, const std::wstring& find);
		std::wstring StripBeforeInclusive(const std::wstring& source, const std::wstring& find);
		std::wstring StripAfterInclusive(const std::wstring& source, const std::wstring& find);
		std::wstring StripAfterLastInclusive(const std::wstring& source, const std::wstring& find);
		void Split(const std::wstring& source, wchar_t find, std::vector<std::wstring>& vOut);
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

		inline bool ToBool(const std::string& source) { return ("false" != source); }
		float ToFloat(const std::string& source);
	}
}

#endif //CSTRING_H
