#ifndef LANG_H
#define LANG_H

// Standard global tags: breathe::LANG("L__MainMenu")
// Application specific tags: breathe::LANG("L_Application")

namespace breathe
{
	// Can pass in either UTF8 or unicode, however, you will always get back unicode
	std::wstring LANG(const std::string& tag);
	std::wstring LANG(const std::wstring& tag);

	namespace util
	{
		void LoadLanguageFile();
	}
}

#endif // LANG_H
