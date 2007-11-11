#if defined (_MSC_VER) && _MSC_VER <= 1200 // MSVC++ 6.0
# pragma warning(disable: 4786)
#endif

#include <cstdarg>
#include <cassert>

#include <list>
#include <string>
#include <vector>
#include <map>

#include <sstream>

// writing on a text file
#include <iostream>
#include <fstream>
#include <iomanip>

#include <breathe/breathe.h>

#if defined(BUILD_DEBUG) && defined(__WIN__)
	#include <windows.h>
#endif //BUILD_DEBUG && __WIN__

#include <breathe/util/cString.h>
#include <breathe/util/log.h>
#include <breathe/util/filesystem.h>

#include <breathe/util/cTimer.h>
#include <breathe/gui/cWidget.h>
#include <breathe/gui/cWindowManager.h>
#include <breathe/util/app.h>



#define t "\t"

const int CONSOLE_MAXLINES = 50;


breathe::logging::cLog LOG;
breathe::logging::cConsole CONSOLE;
breathe::logging::cScreen SCREEN;

namespace breathe
{
	namespace logging
	{
		// ***********************************************LOG*******************************************************

		cLog::cLog() :
			strfilename(TEXT("log/index.html"))
		{
			scol=0;
			ecol=0;
			hash=35;
			starttable=t t t "<table border=\"0\" cellspacing=\"0\">";
			startline=t t t t "<tr>";
			startsuccesscolumn[0]="<td bgcolor=\"" + hash + "006600\" width=\"*\">";
			startsuccesscolumn[1]="<td bgcolor=\"" + hash + "005500\">";
			starterrorcolumn[0]="<td bgcolor=\"" + hash + "660000\">";
			starterrorcolumn[1]="<td bgcolor=\"" + hash + "550000\">";
			endcolumn="</td>";
			endline="</td></tr>\n";
			newline="<br>\n";
			endtable=t t t "</table>\n";



			breathe::filesystem::CreateDirectory(TEXT("log"));
			breathe::filesystem::CreateDirectory(TEXT("log/mem"));
			breathe::filesystem::CreateFile(strfilename);

			CreateLog();

#ifdef BUILD_DEBUG
			Success("Build", "Debug");
#else
			Success("Build", "Release");
#endif //BUILD_DEBUG
		}

		cLog::~cLog()
		{
			logfile.open(breathe::string::ToUTF8(strfilename).c_str(), std::ios::out | std::ios::app);
			
			if(!logfile.is_open())
				return;

			logfile << endtable << "\t\t</center>\n\t</body>" << std::endl << "</html>";
			logfile.close();
		}

		bool cLog::CreateLog()
		{ 
			logfile.open(breathe::string::ToUTF8(strfilename).c_str(), std::ios::out);

			if(!logfile.is_open())
				return false;

			logfile << "<html>" << std::endl;
			logfile << t << "<head>" << std::endl;
			logfile << t << t << "<title>Log</title>" << std::endl;
			logfile << t << t << "<style type=\"text/css\">" << std::endl;
			logfile << t << t << "<!--" << std::endl;
			logfile << t << t << t << "td { font-family: Tahoma; font-size: 12px; } " << std::endl;
			logfile << t << t << "-->" << std::endl;
			logfile << t << t << "</style>" << std::endl;
			logfile << t << "</head>" << std::endl;
			logfile << t << "<body font face=\"Tohama\" size=\"2\" bgcolor=\"" << hash.data()[0] << "000000\" text=\"" << hash.data()[0] << "FFFFFF\">" << std::endl;
			logfile << t << t << "<center>" << std::endl;
			logfile << t << t << t << starttable << std::endl;
			logfile << t << t << t << startline << "<td bgcolor=\"" << hash.data()[0] << "0000FF\">Component" << endcolumn << "<td bgcolor=\"" << hash.data()[0] << "0000FF\">Event" << endline;

			//Close the file, return success
			logfile.close();

			return true;
		}

#ifdef BUILD_DEBUG
		void cLog::trace(const std::string& section)
		{
			breathe::string::string_t s = TEXT("<!> ") + breathe::string::ToString_t(section) + TEXT("\n");
#ifdef __WIN__
			OutputDebugString(s.c_str());
#else
			printf(breathe::string::ToUTF8(s).c_str());
#endif
		}

		void cLog::trace(const std::string& section, const std::string& text)
		{
			trace(section + " - " + text);
		}
#endif //BUILD_DEBUG

		void cLog::Newline()
		{
			logfile.open(breathe::string::ToUTF8(strfilename).c_str(), std::ios::out | std::ios::app);
			
			if(!logfile.is_open()) 
				return;
			
			logfile << startline << "<td bgcolor=\"" << hash.data()[0] << "0000CC\">&nbsp;" << endcolumn << "<td bgcolor=\"" << hash.data()[0] << "0000CC\">&nbsp;" << endline;
			logfile.close();
		}

		void cLog::Newline(const std::string& s1)
		{
			section = s1;

			logfile.open(breathe::string::ToUTF8(strfilename).c_str(), std::ios::out | std::ios::app);
			
			if(!logfile.is_open()) 
				return;

			logfile << startline << "<td bgcolor=\"" << hash.data()[0] << "0000CC\">" << s1 << endcolumn << "<td bgcolor=\"" << hash.data()[0] << "0000CC\">&nbsp;" << endline;
			
			logfile.close();

	#ifdef BUILD_DEBUG
			trace(s1);
	#endif //BUILD_DEBUG
		}

		void cLog::Newline(const std::string& s1, const std::string& text)
		{
			section = s1;

			logfile.open(breathe::string::ToUTF8(strfilename).c_str(), std::ios::out | std::ios::app);
			
			if(!logfile.is_open()) 
				return;

			logfile << startline << "<td bgcolor=\"" << hash.data()[0] << "0000CC\">" << s1 << endcolumn << "<td bgcolor=\"" << hash.data()[0]<< "0000CC\">" << text << endline;
			logfile.close();

	#ifdef BUILD_DEBUG
			trace(s1, text);
	#endif
		}

		void cLog::Success(const std::string& section, const std::string& text)
		{
			logfile.open(breathe::string::ToUTF8(strfilename).c_str(), std::ios::out | std::ios::app);
			
			if(!logfile.is_open()) 
				return;

			logfile << startline << startsuccesscolumn[scol] << section << endcolumn << startsuccesscolumn[scol] << text << endline;
			
			//Close the file
			logfile.close();

			scol=!scol;

	#ifdef BUILD_DEBUG
			trace(section, text);
	#endif //BUILD_DEBUG
		}

		void cLog::Error(const std::string& section, const std::string& text)
		{
			logfile.open(breathe::string::ToUTF8(strfilename).c_str(), std::ios::out | std::ios::app);
			
			if(!logfile.is_open()) 
				return;

			logfile << startline << starterrorcolumn[ecol] << section << endcolumn << starterrorcolumn[ecol] << text << endline;
			
			logfile.close();

			ecol=!ecol;

	#ifdef BUILD_DEBUG
			trace(section, text);
	#endif //BUILD_DEBUG
		}

		
		/*void cLog::precision ( unsigned long p )
		{
			logfile << std::setprecision( p );
		}*/


		// ***********************************************CONSOLE*******************************************************

		cConsole::cConsole()
		{
		}

		cConsole::~cConsole()
		{

		}

		/*void cConsole::Newline()
		{
			if(lLine.size()>CONSOLE_MAXLINES)
				lLine.pop_front();
			
			lLine.push_back("");
		}

		void cConsole::Newline(const std::string& text)
		{
			if(lLine.size()>CONSOLE_MAXLINES)
				lLine.pop_front();
			
			lLine.push_back(text);
		}

		void cConsole::Newline(const std::string& s1, const std::string& text)
		{
			if(lLine.size()>CONSOLE_MAXLINES)
				lLine.pop_front();
			
			lLine.push_back(text);
		}

		void cConsole::Success(const std::string& section, const std::string& text)
		{
			if(lLine.size()>CONSOLE_MAXLINES)
				lLine.pop_front();
			
			lLine.push_back(text);
		}

		void cConsole::Error(const std::string& section, const std::string& text)
		{
			if(lLine.size()>CONSOLE_MAXLINES)
				lLine.pop_front(); 
			
			lLine.push_back(text);
		}
		
		cConsole& cConsole::operator<<(const std::ostringstream& o)
		{
			//logfile << t;
			LOG.Success(LOG.section, o.str());

			std::cout<<o.str()<<std::endl;
			return *this;
		}*/


		// ***********************************************SCREEN*******************************************************
		cScreen::cScreen()
		{

		}

		cScreen::~cScreen()
		{

		}
			
		/*void cScreen::Newline()
		{

		}

		void cScreen::Newline(const std::string& s1)
		{

		}

		void cScreen::Newline(const std::string& s1, const std::string& text)
		{

		}


		void cScreen::Success(const std::string& section, const std::string& text)
		{

		}

		void cScreen::Error(const std::string& section, const std::string& text)
		{

		}*/

	}

	namespace util
	{
		void cConsoleBase::ExecuteCommand(const std::string& command)
		{
			assert(pApp != nullptr);
			pApp->ConsoleExecute(command);
		}

		
		bool cConsoleBase::AddKey(unsigned int uiCode)
		{
			//Uint8* key = SDL_GetKeyState( NULL );
			//key for shift, control, etc. modifiers

			//if(key[SDLK_SHIFT])
			/*
			SDLK_NUMLOCK		= 300,
			SDLK_CAPSLOCK		= 301,
			SDLK_SCROLLOCK		= 302,
			SDLK_RSHIFT		= 303,
			SDLK_LSHIFT		= 304,
			SDLK_RCTRL		= 305,
			SDLK_LCTRL		= 306,
			SDLK_RALT		= 307,
			SDLK_LALT		= 308,
			SDLK_RMETA		= 309,
			SDLK_LMETA		= 310,
			SDLK_LSUPER		= 311,		//Left "Windows" key
			SDLK_RSUPER		= 312,		//Right "Windows" key
			SDLK_MODE		= 313,		//"Alt Gr" key
			SDLK_COMPOSE		= 314,		//Multi-key compose key
			*/

			if((SDLK_RETURN == uiCode) || (SDLK_KP_ENTER == uiCode))
			{
				ExecuteCommand(GetCurrentLine());

				ClearCurrent();
				uiCursorPosition=0;
			}
			else if((SDLK_ESCAPE == uiCode) || (SDLK_BACKQUOTE == uiCode))
				return false;

			else if(SDLK_DELETE == uiCode)
			{
				if(uiCursorPosition<current.size())
					current.erase(uiCursorPosition, 1);
			}
			else if(SDLK_BACKSPACE == uiCode)
			{
				if(uiCursorPosition > 0)
				{
					current.erase(uiCursorPosition-1, 1);
					uiCursorPosition--;
				}
			}
			else if(SDLK_LEFT==uiCode)
			{
				if(uiCursorPosition > 0)
					uiCursorPosition--;
			}

			else if(SDLK_RIGHT==uiCode)
			{
				if(uiCursorPosition<current.size())
					uiCursorPosition++;
			}
			else if(SDLK_HOME==uiCode)
				uiCursorPosition=0;
			else if(SDLK_END==uiCode)
				uiCursorPosition=current.size();

			else if(SDLK_TAB==uiCode)
				; //TODO: Autocomplete
			else if(SDLK_CLEAR==uiCode)
				current="";
			else if(SDLK_UP==uiCode)
				; //TODO: History of typed in items
			else if(SDLK_DOWN==uiCode)
				; //TODO: History of typed in items
			else if(SDLK_PAGEUP==uiCode)
				; //TODO: History of all console items
			else if(SDLK_PAGEDOWN==uiCode)
				; //TODO: History of all console items

			else if(uiCode<300)
			{
				/*#include <SDL/SDL_keysym.h>
					
				if((uiCode<SDLK_a || uiCode>SDLK_z) && (uiCode<SDLK_0 || uiCode>SDLK_9))
				{
					std::ostringstream t;
					t<<"unmapped key: "<<uiCode<<" ("<<static_cast<unsigned char>(uiCode)<<")";				

					ConsoleAddLine(t.str());
				}*/

				std::string s;
				s += char(uiCode);

				current.insert(uiCursorPosition, s);
				uiCursorPosition++;
			}

			//When we press a key we want to see where we are up to
			uiCursorBlink = 0;

			return true;
		}
	}
}
