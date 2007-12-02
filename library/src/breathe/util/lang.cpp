#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>

// writing on a text file
#include <iostream>
#include <fstream>

#include <list>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stack>

#include <breathe/breathe.h>

#include <breathe/util/cString.h>

#ifndef FIRESTARTER
#include <breathe/util/log.h>
#endif

#include <breathe/storage/filesystem.h>
#include <breathe/storage/file.h>

#include <breathe/util/lang.h>

namespace breathe
{
	std::map<std::wstring, std::wstring> langtags;

	namespace util
	{
		bool LastCharacterIsQuotationMark(const std::wstring& source)
		{
			return source[source.length() - 1] != TEXT('\"');
		}

		void LoadLanguageFile(const std::string& filename)
		{
			std::vector<std::wstring> contents;
			storage::ReadText(filesystem::FindFile(breathe::string::ToString_t(filename)), contents);
			
			std::wstring tag;
			std::wstring value;

			std::wstring line;

			size_t i = 0;
			size_t n = contents.size();
			for (i = 0; i < n; i++)
			{
				line = contents[i];
				tag = breathe::string::StripAfterInclusive(line, L" \"");

				// Get a quotation marked value that can span multiple lines
				value = breathe::string::StripBeforeInclusive(line, L" \"");
				if (LastCharacterIsQuotationMark(line)) {
					i++;
					while ((i < n) && !line.empty() && LastCharacterIsQuotationMark(line)) {
						line = contents[i];
						value.append(L"\n" + line);
						i++;
					}

					i--;
				}
				
				value = breathe::string::StripTrailing(value, L"\"");
				
				// Add tag
				CONSOLE<<"Tag \""<<breathe::string::ToUTF8(tag)<<"\"=\""<<breathe::string::ToUTF8(value)<<"\""<<std::endl;
				langtags[tag] = value;
			}
		}

		void LoadLanguageFile()
		{
			assert(langtags.empty());

			LoadLanguageFile("shared_lang.txt");
			LoadLanguageFile("lang.txt");
		}
	}
	
	string_t LANG(const std::string& tag)
	{
		std::map<std::wstring, std::wstring>::iterator iter = langtags.find(breathe::string::ToWchar_t(tag));
		if (iter != langtags.end()) return  breathe::string::ToString_t(iter->second);

		storage::AppendText(filesystem::FindFile(breathe::string::ToString_t(TEXT("lang.txt"))),
			breathe::string::ToWchar_t(tag) + L" \"AUTOMATICALLY GENERATED LANGTAG\"");
		return TEXT("LANG TAG NOT FOUND ") + breathe::string::ToString_t(tag);
	}

	string_t LANG(const std::wstring& tag)
	{
		std::map<std::wstring, std::wstring>::iterator iter = langtags.find(breathe::string::ToWchar_t(tag));
		if (iter != langtags.end()) return  breathe::string::ToString_t(iter->second);

		storage::AppendText(filesystem::FindFile(breathe::string::ToString_t(TEXT("lang.txt"))),
			breathe::string::ToWchar_t(tag) + L" \"AUTOMATICALLY GENERATED LANGTAG\"");
		return TEXT("LANG TAG NOT FOUND ") + breathe::string::ToString_t(tag);
	}
}
