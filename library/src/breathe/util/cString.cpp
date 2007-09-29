#include <string>
#include <cctype> // for toupper/tolower
#include <algorithm>

#include <breathe/breathe.h>
#include <breathe/util/cString.h>

namespace breathe
{
	namespace string
	{
		bool IsWhiteSpace(breathe::unicode_char c)
		{
			// We regard space, tab, new line and carriage return characters as white space
			return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r');
		}
		
		std::string Replace(std::string sSource, std::string sFind, std::string sReplace)
		{
			size_t j;
			for(;(j = sSource.find(sFind)) != std::string::npos;)
			{
				sSource.replace(j, sFind.length(), sReplace);
			}
			return sSource;
		}

		std::string StripLeading(std::string source, std::string find)
		{
			std::string::size_type i = source.find_first_not_of(find);
			if(std::string::npos != i) return source.erase(0, i);

			return source;
		}
		
		std::string StripTrailing(std::string source, std::string find)
		{
			std::string::size_type iEndOfContent = source.find_last_not_of(find);
			if(std::string::npos != iEndOfContent) return source.erase(iEndOfContent + 1);
			
			return source;
		}

		std::string StripBefore(std::string source, std::string find)
		{
			std::string::size_type i = source.find_first_of(find);
			if(std::string::npos != i) return source.erase(0, i);

			return source;
		}

		std::string StripAfter(std::string source, std::string find)
		{
			std::string::size_type i = source.find_last_of(find);
			if(std::string::npos != i) return source.erase(i);

			return source;
		}

		std::string StripBeforeInclusive(std::string source, std::string find)
		{
			std::string::size_type i = source.find(find);
			if(std::string::npos != i) 
				return std::string(source.begin() + i + find.length(), source.end());

			return source;
		}

		std::string StripAfterInclusive(std::string source, std::string find)
		{
			std::string::size_type i = source.find_first_of(find);
			if(std::string::npos != i) {
				std::string copy(source);
				std::string::size_type len = find.length();
				return copy.erase(i);
			}

			return source;
		}
		
		std::string ToLower(std::string source)
		{
			std::string sOut(source);

			transform(sOut.begin(), sOut.end(), sOut.begin(), std::tolower);

			return sOut;
		}

		std::string ToUpper(std::string source)
		{
			std::string sOut(source);

      transform(sOut.begin(), sOut.end(), sOut.begin(), std::toupper);

			return sOut;
		}

		
		std::string HTMLDecode(std::string source)
		{
			std::string sOut(source);

			return sOut;
		}

		std::string HTMLEncode(std::string source)
		{
			std::string sOut(source);

			return sOut;
		}
	}
}
