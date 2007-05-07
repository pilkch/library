#if defined (_MSC_VER) && _MSC_VER <= 1200 // MSVC++ 6.0
# pragma warning(disable: 4786)
#endif

#include <cstdarg>

#include <list>
#include <string>
#include <vector>

// writing on a text file
#include <iostream>
#include <fstream>
#include <iomanip>

#include <BREATHE/cBreathe.h>

#if defined(BUILD_DEBUG) && defined(PLATFORM_WINDOWS)
	#include <windows.h>
#endif //BUILD_DEBUG && PLATFORM_WINDOWS

#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/UTIL/cFileSystem.h>




#define t "\t"

#define CONSOLE_MAXLINES 50


BREATHE::LOGGING::cLog LOG;
BREATHE::LOGGING::cConsole CONSOLE;
BREATHE::LOGGING::cScreen SCREEN;

namespace BREATHE
{
	namespace LOGGING
	{
		// ***********************************************LOG*******************************************************

		cLog::cLog()
		{
			strfilename="log/index.html"; 

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



			BREATHE::FILESYSTEM::CreateDirectory("log");
			BREATHE::FILESYSTEM::CreateDirectory("log/mem");
			BREATHE::FILESYSTEM::CreateFile(strfilename);

			CreateLog();

#ifdef BUILD_DEBUG
			Success("Build", "Debug");
#else
			Success("Build", "Release");
#endif //BUILD_DEBUG
		}

		cLog::~cLog()
		{
			logfile.open(strfilename.c_str(), std::ios::out | std::ios::app);
			
			if(!logfile.is_open())
				return;

			logfile << endtable << "\t\t</center>\n\t</body>" << std::endl << "</html>";
			logfile.close();
		}

		bool cLog::CreateLog()
		{ 
			logfile.open(strfilename.c_str(), std::ios::out);

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
		void cLog::trace(std::string section)
		{
			std::string s="<!> " + section + "\n";
	#ifdef PLATFORM_WINDOWS
			OutputDebugString(s.c_str());
	#else
			printf(s.c_str());
	#endif
		}

		void cLog::trace(std::string section, std::string text)
		{
			std::string s="<!> " + section + " - " + text + "\n";
	#ifdef PLATFORM_WINDOWS
			OutputDebugString(s.c_str());
	#else
			printf(s.c_str());
	#endif
		}
	#endif //BUILD_DEBUG

		void cLog::Newline()
		{
			logfile.open(strfilename.c_str(), std::ios::out | std::ios::app);
			
			if(!logfile.is_open()) 
				return;
			
			logfile << startline << "<td bgcolor=\"" << hash.data()[0] << "0000CC\">&nbsp;" << endcolumn << "<td bgcolor=\"" << hash.data()[0] << "0000CC\">&nbsp;" << endline;
			logfile.close();
		}

		void cLog::Newline(std::string s1)
		{
			section = s1;

			logfile.open(strfilename.c_str(), std::ios::out | std::ios::app);
			
			if(!logfile.is_open()) 
				return;

			logfile << startline << "<td bgcolor=\"" << hash.data()[0] << "0000CC\">" << s1 << endcolumn << "<td bgcolor=\"" << hash.data()[0] << "0000CC\">&nbsp;" << endline;
			
			logfile.close();

	#ifdef BUILD_DEBUG
			trace(s1);
	#endif //BUILD_DEBUG
		}

		void cLog::Newline(std::string s1, std::string text)
		{
			section = s1;

			logfile.open(strfilename.c_str(), std::ios::out | std::ios::app);
			
			if(!logfile.is_open()) 
				return;

			logfile << startline << "<td bgcolor=\"" << hash.data()[0] << "0000CC\">" << s1 << endcolumn << "<td bgcolor=\"" << hash.data()[0]<< "0000CC\">" << text << endline;
			logfile.close();

	#ifdef BUILD_DEBUG
			trace(s1, text);
	#endif
		}

		void cLog::Success(std::string section, std::string text)
		{
			logfile.open(strfilename.c_str(), std::ios::out | std::ios::app);
			
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

		void cLog::Error(std::string section, std::string text)
		{
			logfile.open(strfilename.c_str(), std::ios::out | std::ios::app);
			
			if(!logfile.is_open()) 
				return;

			logfile << startline << starterrorcolumn[ecol] << section << endcolumn << starterrorcolumn[ecol] << text << endline;
			
			logfile.close();

			ecol=!ecol;

	#ifdef BUILD_DEBUG
			trace(section, text);
	#endif //BUILD_DEBUG
		}

		
		void cLog::precision ( unsigned long p )
		{
			logfile << std::setprecision( p );
		}


		// ***********************************************CONSOLE*******************************************************

		cConsole::cConsole()
		{
			uiCursorBlink=0;
			uiCursorPosition=0;
		}

		cConsole::~cConsole()
		{

		}

		void cConsole::Newline()
		{
			if(lLine.size()>CONSOLE_MAXLINES)
				lLine.pop_front(); 
			
			lLine.push_back("");
		}

		void cConsole::Newline(std::string text)
		{
			if(lLine.size()>CONSOLE_MAXLINES)
				lLine.pop_front(); 
			
			lLine.push_back(text);
		}

		void cConsole::Newline(std::string s1, std::string text)
		{
			if(lLine.size()>CONSOLE_MAXLINES)
				lLine.pop_front(); 
			
			lLine.push_back(text);
		}

		void cConsole::Success(std::string section, std::string text)
		{
			if(lLine.size()>CONSOLE_MAXLINES)
				lLine.pop_front(); 
			
			lLine.push_back(text);
		}

		void cConsole::Error(std::string section, std::string text)
		{
			if(lLine.size()>CONSOLE_MAXLINES)
				lLine.pop_front(); 
			
			lLine.push_back(text);
		}


		// ***********************************************SCREEN*******************************************************
		cScreen::cScreen()
		{

		}

		cScreen::~cScreen()
		{

		}
			
		void cScreen::Newline()
		{

		}

		void cScreen::Newline(std::string s1)
		{

		}

		void cScreen::Newline(std::string s1, std::string text)
		{

		}


		void cScreen::Success(std::string section, std::string text)
		{

		}

		void cScreen::Error(std::string section, std::string text)
		{

		}
	}
}
