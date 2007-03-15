#if defined (_MSC_VER) && _MSC_VER <= 1200  // MSVC++ 6.0
#  pragma warning(disable: 4786)  
#endif

#include <string>
#include <list>
#include <map>
#include <iterator>
#include <vector>






#include <BREATHE/UTIL/cVar.h>
#include <BREATHE/UTIL/cConsole.h>
#include <BREATHE/UTIL/cLog.h>

namespace BREATHE
{
	cConsole::cConsole()
	{
		//uiScrollFromBottom=0;
		
		uiCursorBlink=0;
		uiCursorPosition=0;
	}

	cConsole::~cConsole()
	{

	}

	void cConsole::Add(std::string text)
	{
		if(lLine.size()>CONSOLE_MAXLINES)
			lLine.pop_front(); 
		
		lLine.push_back(text);
	}
}
