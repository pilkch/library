#ifndef CSTRING_H
#define CSTRING_H

namespace breathe
{
	#ifdef UNICODE
	typedef wchar_t char_t;
	typedef std::wstring string_t;
	#else
	typedef char char_t;
	typedef std::string string_t;
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
		std::string StripBeforeInclusive(const std::string& source, const std::string& find);
		std::string StripAfterInclusive(const std::string& source, const std::string& find);
		void Split(const std::string& source, char find, std::vector<std::string>& vOut);
		void SplitOnNewLines(const std::string& source, std::vector<std::string>& vOut);
		std::string Trim(const std::string& source);

		size_t CountOccurrences(const std::wstring& source, const std::wstring& find);
		std::wstring Replace(const std::wstring& source, const std::wstring& find, const std::wstring& replace);
		std::wstring StripLeading(const std::wstring& source, const std::wstring& find);
		std::wstring StripTrailing(const std::wstring& source, const std::wstring& find);
		std::wstring StripBefore(const std::wstring& source, const std::wstring& find);
		std::wstring StripAfter(const std::wstring& source, const std::wstring& find);
		std::wstring StripBeforeInclusive(const std::wstring& source, const std::wstring& find);
		std::wstring StripAfterInclusive(const std::wstring& source, const std::wstring& find);
		void Split(const std::wstring& source, wchar_t find, std::vector<std::wstring>& vOut);
		void SplitOnNewLines(const std::wstring& source, std::vector<std::wstring>& vOut);
		std::wstring Trim(const std::wstring& source);

		std::string HTMLDecode(const std::string& source);
		std::string HTMLEncode(const std::string& source);

		std::string ToLower(const std::string& source);
		std::string ToUpper(const std::string& source);


		std::wstring ToWchar_t(const std::string& source);
		inline std::wstring ToWchar_t(const std::wstring& source) { return source; }
		std::string ToUTF8(const std::wstring& source);
		inline std::string ToUTF8(const std::string& source) { return source; }

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
