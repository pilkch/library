#ifndef CONSOLE_H
#define CONSOLE_H

namespace BREATHE
{
	#define CONSOLE_MAXLINES 50

	class cConsole
	{
	public:
		unsigned int uiCursorBlink;
		unsigned int uiCursorPosition;
		std::list<std::string>lLine;
		std::string sLine;


		cConsole();
		~cConsole();

		void Add(std::string text);
	};
}

#endif //CONSOLE_H
