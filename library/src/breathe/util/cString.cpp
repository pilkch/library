#include <cctype> // for toupper/tolower

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

#include <breathe/breathe.h>
#include <breathe/util/cString.h>

#define WHITE_SPACE "\t\v\r\n"
#define LWHITE_SPACE L"\t\v\r\n"

namespace breathe
{
	namespace string
	{
		bool IsWhiteSpace(char_t c)
		{
			// We regard space, tab, new line and carriage return characters as white space
			return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r');
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

			while (getline(stm, field)) 
				vOut.push_back(Trim(field));
		}
		
		void SplitOnNewLines(const std::wstring& source, std::vector<std::wstring>& vOut)
		{
			vOut.clear();

			std::wstringstream stm(source);
			std::wstring field;

			while (getline(stm, field)) 
				vOut.push_back(Trim(field));
		}

		std::string Trim(const std::string& source)
		{
			std::string::size_type start = source.find_first_not_of(WHITE_SPACE);
			std::string::size_type end = source.find_last_not_of(WHITE_SPACE);
			return source.substr(start, end + 1);
		}

		std::wstring Trim(const std::wstring& source)
		{
			std::wstring::size_type start = source.find_first_not_of(LWHITE_SPACE);
			std::wstring::size_type end = source.find_last_not_of(LWHITE_SPACE);
			return source.substr(start, end + 1);
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
				std::string::size_type len = find.length();
				return temp.erase(i);
			}

			return source;
		}

		std::wstring StripAfterInclusive(const std::wstring& source, const std::wstring& find)
		{
			std::wstring::size_type i = source.find_first_of(find);
			if (std::wstring::npos != i) {
				std::wstring temp(source);
				std::wstring::size_type len = find.length();
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
			std::string sOut(source);
			std::transform(sOut.begin(), sOut.end(), sOut.begin(), std::tolower);
			return sOut;
		}

		std::string ToUpper(const std::string& source)
		{
			std::string sOut(source);
      std::transform(sOut.begin(), sOut.end(), sOut.begin(), std::toupper);
			return sOut;
		}

		std::wstring ToLower(const std::wstring& source)
		{
			std::wstring sOut(source);
			std::transform(sOut.begin(), sOut.end(), sOut.begin(), std::tolower);
			return sOut;
		}

		std::wstring ToUpper(const std::wstring& source)
		{
			std::wstring sOut(source);
			std::transform(sOut.begin(), sOut.end(), sOut.begin(), std::toupper);
			return sOut;
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
				ct.narrow(source.data(), source.data() + source.size(), '\u00B6', &(*result.begin())); 
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

		float ToFloat(const std::string& source)
		{
			float value = 0.0f;
			std::stringstream stm(source);
			stm >> value;
			return value;
		}
		
		std::string HTMLDecode(const std::string& source)
		{
			std::string sOut(source);

			return sOut;
		}

		std::string HTMLEncode(const std::string& source)
		{
			std::string sOut(source);

			return sOut;
		}
	}
}
