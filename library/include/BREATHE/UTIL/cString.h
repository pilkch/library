#ifndef CSTRING_H
#define CSTRING_H

namespace BREATHE
{
	namespace STRING
	{
		/*#define SIZE_TYPE unsigned long int

		const SIZE_TYPE STR_SMALL=256;
		const SIZE_TYPE STR_MEDIUM=2048;
		const SIZE_TYPE STR_LARGE=320000;

		#define STR_SIZE STR_LARGE

		#define BUFFER_SIZE_IN STR_LARGE
		#define BUFFER_SIZE_OUT STR_MEDIUM


		#define STR_END "\r\n"*/



		bool IsWhiteSpace(BREATHE::unicode_char c);
		
		std::string Replace(std::string sSource, std::string f, std::string r);
		std::string CutLeading(std::string sSource, std::string f);
		std::string CutTrailing(std::string sSource, std::string f);

		std::string HTMLDecode(std::string sSource);
		std::string HTMLEncode(std::string sSource);

		std::string ToLower(std::string sSource);
		std::string ToUpper(std::string sSource);
	}
}

#endif //CSTRING_H
