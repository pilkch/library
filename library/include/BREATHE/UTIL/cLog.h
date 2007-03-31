#ifndef CLOG_H
#define CLOG_H

namespace BREATHE
{
	#define LOG_MAX_LEN 255

	class cLog
	{   
	protected:
		bool WriteLog();

		void t(FILE * f);
		void n(FILE * f);

#ifdef BUILD_DEBUG
		void trace(std::string section);
		void trace(std::string section, std::string text);
#endif //BUILD_DEBUG

		FILE * logfile;

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
		cLog();
		~cLog();

		bool Init(std::string filename="index.html");

		void Newline();
		void Newline(std::string s1);
		void Newline(std::string s1, std::string text);

		void Success(std::string section, std::string text);
		void Error(std::string section, std::string text);
		void Success(std::string section, char *text, ...);
		void Error(std::string section, char *text, ...);
	};
}

extern BREATHE::cLog *pLog;

#endif
