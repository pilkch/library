#ifndef CSTRING_H
#define CSTRING_H

namespace BREATHE
{
	namespace STRING
	{
		bool IsWhiteSpace(char c);
		
		std::string Replace(std::string sSource, std::string f, std::string r);
		std::string CutLeading(std::string sSource, std::string f);
		std::string CutTrailing(std::string sSource, std::string f);

		std::string HTMLDecode(std::string sSource);
		std::string HTMLEncode(std::string sSource);
	}
}

#endif //CSTRING_H
