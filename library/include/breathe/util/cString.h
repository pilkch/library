#ifndef CSTRING_H
#define CSTRING_H

namespace breathe
{
	namespace string
	{
		bool IsWhiteSpace(breathe::unicode_char c);
		
		std::string Replace(std::string source, std::string find, std::string replace);
		std::string StripLeading(std::string source, std::string find);
		std::string StripTrailing(std::string source, std::string find);
		std::string StripBefore(std::string source, std::string find);
		std::string StripAfter(std::string source, std::string find);
		std::string StripBeforeInclusive(std::string source, std::string find);
		std::string StripAfterInclusive(std::string source, std::string find);

		std::string HTMLDecode(std::string source);
		std::string HTMLEncode(std::string source);

		std::string ToLower(std::string source);
		std::string ToUpper(std::string source);
	}
}

#endif //CSTRING_H
