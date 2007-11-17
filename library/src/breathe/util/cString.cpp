#include <cctype> // for toupper/tolower

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

#include <breathe/breathe.h>
#include <breathe/util/cString.h>

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
			for(;(j = source.find(sFind, i)) != std::string::npos; i = j + 1, n++)
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

		std::string Trim(const std::string& source)
		{
			std::string::size_type start = source.find_first_not_of(" \t\v");
			return source.substr(start, std::string::npos);
		}

		std::string Replace(const std::string& source, const std::string& sFind, const std::string& sReplace)
		{
			size_t j;
			std::string temp(source);
			for(;(j = temp.find(sFind)) != std::string::npos;)
			{
				temp.replace(j, sFind.length(), sReplace);
			}
			return temp;
		}

		std::string StripLeading(const std::string& source, const std::string& find)
		{
			std::string::size_type i = source.find_first_not_of(find);
			if(std::string::npos != i) {
				std::string temp(source);
				return temp.erase(0, i);
			}

			return source;
		}
		
		std::string StripTrailing(const std::string& source, const std::string& find)
		{
			std::string::size_type iEndOfContent = source.find_last_not_of(find);
			if(std::string::npos != iEndOfContent) {
				std::string temp(source);
				return temp.erase(iEndOfContent + 1);
			}
			
			return source;
		}

		std::string StripBefore(const std::string& source, const std::string& find)
		{
			std::string::size_type i = source.find_first_of(find);
			if(std::string::npos != i) {
				std::string temp(source);
				return temp.erase(0, i);
			}

			return source;
		}

		std::string StripAfter(const std::string& source, const std::string& find)
		{
			std::string::size_type i = source.find_last_of(find);
			if(std::string::npos != i) {
				std::string temp(source);
				return temp.erase(i);
			}

			return source;
		}

		std::string StripBeforeInclusive(const std::string& source, const std::string& find)
		{
			std::string::size_type i = source.find(find);
			if(std::string::npos != i) 
				return std::string(source.begin() + i + find.length(), source.end());

			return source;
		}

		std::string StripAfterInclusive(const std::string& source, const std::string& find)
		{
			std::string::size_type i = source.find_first_of(find);
			if(std::string::npos != i) {
				std::string copy(source);
				std::string::size_type len = find.length();
				return copy.erase(i);
			}

			return source;
		}
		
		std::string ToLower(const std::string& source)
		{
			std::string sOut(source);

			transform(sOut.begin(), sOut.end(), sOut.begin(), std::tolower);

			return sOut;
		}

		std::string ToUpper(const std::string& source)
		{
			std::string sOut(source);

      transform(sOut.begin(), sOut.end(), sOut.begin(), std::toupper);

			return sOut;
		}

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
