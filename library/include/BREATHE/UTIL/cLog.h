#ifndef CLOG_H
#define CLOG_H

#include <sstream>

namespace BREATHE
{
	namespace LOGGING
	{
		enum 
		{
			COLOUR_NORMAL = 0,
			COLOUR_RED,
			COLOUR_GREEN,
			COLOUR_BLUE,
		};

		class cLogBase
		{
		public:
			template<typename T>
			cLogBase& operator<<(const T& t)
			{
				std::ostringstream o;
				o<<t;

				line += o.str();

				return *this;
			}

			cLogBase& operator<<(float t)
			{
				std::stringstream o;
				o<<t;

				line += o.str();

				return *this;
			}

			cLogBase& operator<<(bool t)
			{
				line += (t ? "true" : "false");

				return *this;
			}

			cLogBase& operator<<(const std::ostringstream& o)
			{
				line += o.str();

				return *this;
			}
			
			cLogBase& operator<<(std::ostream& (* func)(std::ostream&))
			{ 
				_EndLine(line);

				return *this; 
			}

			//void precision ( unsigned long p );

			void ClearLine() { line.clear(); }

		private:
			virtual void _EndLine(std::string& o) = 0;

			std::string line;
		};


		class cConsole;

		class cLog : public cLogBase
		{
		public:
			cLog();
			~cLog();

			friend class cConsole;

			void Newline();
			void Newline(std::string s1);
			void Newline(std::string s1, std::string text);

			void Success(std::string section, std::string text);
			void Error(std::string section, std::string text);

		private:
			void _EndLine(std::string& o)
			{
				Success(section, o);
				ClearLine();
			}

			bool CreateLog();

	#ifdef BUILD_DEBUG
			void trace(std::string section);
			void trace(std::string section, std::string text);
	#endif //BUILD_DEBUG

			std::ofstream logfile;

			std::string strfilename;

			bool scol;
			bool ecol;
			std::string starttable;
			std::string startline;
			std::string startsuccesscolumn[2];
			std::string starterrorcolumn[2];
			std::string endcolumn;
			std::string endline;
			std::string newline;
			std::string endtable;
			std::string hash;

			std::string section;
		};
	}
}

extern BREATHE::LOGGING::cLog LOG;


namespace BREATHE
{
	namespace LOGGING
	{
		class cScreen;

		class cConsole : public cLogBase
		{
		public:
			cConsole();
			~cConsole();

			friend class cScreen;
			
			unsigned int uiCursorBlink;
			unsigned int uiCursorPosition;
			std::list<std::string>lLine;
			std::string sLine;

			
			/*void Newline();
			void Newline(std::string s1);
			void Newline(std::string s1, std::string text);

			void Success(std::string section, std::string text);
			void Error(std::string section, std::string text);*/

		private:
			void _EndLine(std::string& o)
			{
				// Cascade output to log file
				LOG._EndLine(o);

				std::cout<<o<<std::endl;
				ClearLine();
			}
		};
	}
}

extern BREATHE::LOGGING::cConsole CONSOLE;

namespace BREATHE
{
	namespace LOGGING
	{
		class cScreen : public cLogBase
		{
		public:
			cScreen();
			~cScreen();

			std::list<std::string> lLine;

		private:
			void _EndLine(std::string& o)
			{
				// Cascade output to console
				CONSOLE._EndLine(o);

				// Add line to screen
				//if(lLine.size()>CONSOLE_MAXLINES)
				//	lLine.pop_front();
				
				lLine.push_back(o);
				ClearLine();
			}
		};
	}
}

extern BREATHE::LOGGING::cScreen SCREEN;

#endif
