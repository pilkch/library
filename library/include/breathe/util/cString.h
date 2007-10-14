#ifndef CSTRING_H
#define CSTRING_H

namespace breathe
{
	namespace string
	{
#ifdef PLATFORM_WINDOWS
		const size_t size_wchar_t = 2;
#else
		const size_t size_wchar_t = 4;
#endif

#ifdef UNICODE
		typedef wchar_t unicode_char;
		typedef std::wstring string_t;
#else
		typedef char unicode_char;
		typedef std::string string_t;
#endif

		bool IsWhiteSpace(breathe::string::unicode_char c);
		
		std::string Replace(const std::string& source, const std::string& find, const std::string& replace);
		std::string StripLeading(const std::string& source, const std::string& find);
		std::string StripTrailing(const std::string& source, const std::string& find);
		std::string StripBefore(const std::string& source, const std::string& find);
		std::string StripAfter(const std::string& source, const std::string& find);
		std::string StripBeforeInclusive(const std::string& source, const std::string& find);
		std::string StripAfterInclusive(const std::string& source, const std::string& find);

		std::string HTMLDecode(const std::string& source);
		std::string HTMLEncode(const std::string& source);

		std::string ToLower(const std::string& source);
		std::string ToUpper(const std::string& source);


		inline std::wstring ToWchar_t(const std::string& source)
		{
			const size_t len = source.length();
			wchar_t* pTemp = new wchar_t[len];
			mbstowcs(pTemp, source.data(), len * size_wchar_t);
			std::wstring temp(pTemp);
			SAFE_DELETE(pTemp);

			return temp;
		}
		
		inline std::string ToUTF8(const std::string& source)
		{
			return source;
		}

		inline std::string ToUTF8(const std::wstring& source)
		{
			const size_t len = source.length();
			char* pTemp = new char[len];
			wcstombs(pTemp, source.data(), len * size_wchar_t);
			std::string temp(pTemp);
			SAFE_DELETE(pTemp);

			return temp;
		}

#ifdef UNICODE
		inline string_t ToString_t(const std::string& source) { return ToWchar_t(source); }
		inline string_t ToString_t(const std::wstring& source) { return source; }
#else
		inline string_t ToString_t(const std::string& source) { return source; }
		inline string_t ToString_t(const std::wstring& source) { return ToUTF8(source); }
#endif
	}
}

#endif //CSTRING_H
