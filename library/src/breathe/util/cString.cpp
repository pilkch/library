#include <string>
#include <cctype> // for toupper/tolower
#include <algorithm>

#include <breathe/breathe.h>
#include <breathe/util/cString.h>

namespace breathe
{
	namespace STRING
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

		std::string CutLeading(std::string sSource, std::string f)
		{
			return sSource.erase(0, sSource.find_first_not_of(f));
		}
		
		std::string CutTrailing(std::string sSource, std::string f)
		{
			std::string::size_type iEndOfContent=sSource.find_last_not_of(f);
			if(std::string::npos != iEndOfContent)
			{
				return sSource.erase(iEndOfContent + 1);
			}

			return sSource;
		}

		
		std::string ToLower(std::string sSource)
		{
			std::string sOut(sSource);

			transform(sOut.begin(), sOut.end(), sOut.begin(), std::tolower);

			return sOut;
		}

		std::string ToUpper(std::string sSource)
		{
			std::string sOut(sSource);

      transform(sOut.begin(), sOut.end(), sOut.begin(), std::toupper);

			return sOut;
		}

		
		std::string HTMLDecode(std::string sSource)
		{
			std::string sOut;

			return sOut;
		}

		std::string HTMLEncode(std::string sSource)
		{
			std::string sOut;

			return sOut;
		}
	}
}
