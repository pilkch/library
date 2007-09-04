#ifndef CLOG_H
#define CLOG_H

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
			
			virtual void Newline()= 0;
			virtual void Newline(std::string s1)= 0;
			virtual void Newline(std::string s1, std::string text)= 0;

			virtual void Success(std::string section, std::string text)= 0;
			virtual void Error(std::string section, std::string text)= 0;
		};

		class cLog : protected cLogBase
		{
		protected:
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


		public:

			std::string section;

			cLog();
			~cLog();

			template<typename T> cLog& operator<<(const T& t)
			{
				//Success(section, t);
				logfile << t;
				return *this;
			}
			
			cLog& operator<<(std::ostream& (*func)(std::ostream&))
			{ 
				logfile << func; 
				return *this; 
			}

			void precision ( unsigned long p );

			void Newline();
			void Newline(std::string s1);
			void Newline(std::string s1, std::string text);

			void Success(std::string section, std::string text);
			void Error(std::string section, std::string text);
		};
	}
}

extern BREATHE::LOGGING::cLog LOG;


namespace BREATHE
{
	namespace LOGGING
	{
		class cConsole : protected cLogBase
		{
		public:
			cConsole();
			~cConsole();
			
			unsigned int uiCursorBlink;
			unsigned int uiCursorPosition;
			std::list<std::string>lLine;
			std::string sLine;

			
			void Newline();
			void Newline(std::string s1);
			void Newline(std::string s1, std::string text);

			void Success(std::string section, std::string text);
			void Error(std::string section, std::string text);

			template<typename T> cConsole& operator<<(const T& t)
			{
				//logfile << t;
				std::ostringstream o;
				o<<t;
				Success(LOG.section, o.str());

				std::cout<<o.str()<<std::endl;
				return *this;
			}
			
			cConsole& operator<<(std::ostream& (*func)(std::ostream&))
			{ 
				//logfile << func;
				//Success(LOG.section, func);
				return *this; 
			}

			void precision ( unsigned long p );
		};
	}
}

extern BREATHE::LOGGING::cConsole CONSOLE;

namespace BREATHE
{
	namespace LOGGING
	{
		class cScreen : protected cLogBase
		{
		public:
			cScreen();
			~cScreen();
			
			void Newline();
			void Newline(std::string s1);
			void Newline(std::string s1, std::string text);

			void Success(std::string section, std::string text);
			void Error(std::string section, std::string text);
		};
	}
}

extern BREATHE::LOGGING::cScreen SCREEN;

#endif
