#ifndef LANG_H
#define LANG_H

namespace breathe
{
	namespace util
	{
		// Can pass in either UTF8 or unicode, however, you will always get back unicode
		std::wstring LANG(const std::string& tag);
		std::wstring LANG(const std::wstring& tag);
	}
}

#endif // LANG_H
