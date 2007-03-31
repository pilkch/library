#if defined (_MSC_VER) && _MSC_VER <= 1200 // MSVC++ 6.0
# pragma warning(disable: 4786)
#endif

#include <stdarg.h>

#include <string>
#include <vector>

#include <BREATHE/cBreathe.h>

#if defined(BUILD_DEBUG) && defined(PLATFORM_WINDOWS)
	#include <windows.h>
#endif //BUILD_DEBUG && PLATFORM_WINDOWS

#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/UTIL/cFileSystem.h>

BREATHE::cLog *pLog;


namespace BREATHE
{
	cLog::cLog()
	{
		strfilename="log/index.html"; 

		scol=0;
		ecol=0;
		hash=35;
		starttable="\t\t\t<table border=\"0\" cellspacing=\"0\">";
		startline="\t\t\t\t<tr>";
		startsuccesscolumn[0]="<td bgcolor=\"" + hash + "006600\" width=\"*\">";
		startsuccesscolumn[1]="<td bgcolor=\"" + hash + "005500\">";
		starterrorcolumn[0]="<td bgcolor=\"" + hash + "660000\">";
		starterrorcolumn[1]="<td bgcolor=\"" + hash + "550000\">";
		endcolumn="</td>";
		endline="</td></tr>\n";
		newline="<br>\n";
		endtable="\t\t\t</table>\n";
	}

	cLog::~cLog()
	{
		if(logfile=fopen(strfilename.c_str(), "a+"))
		{
			fprintf(logfile, "%s\t\t</center>\n\t</body>\n</html>", endtable.c_str());
			fclose(logfile);
		}
	}

	bool cLog::WriteLog()
	{ 
		//Clear the log file contents
		if((logfile=fopen(strfilename.c_str(), "wb"))==NULL)
			return false;

		fclose(logfile);

		if(logfile=fopen(strfilename.c_str(), "a+"))
		{
			fprintf(logfile, "<html>");n(logfile);
			t(logfile);fprintf(logfile, "<head>");n(logfile);
			t(logfile);t(logfile);fprintf(logfile, "<title>Log</title>");n(logfile);
			t(logfile);t(logfile);fprintf(logfile, "<style type=\"text/css\">");n(logfile);
			t(logfile);t(logfile);fprintf(logfile, "<!--");n(logfile);
			t(logfile);t(logfile);t(logfile);fprintf(logfile, "td { font-family: Tahoma; font-size: 12px; } ");n(logfile);
			t(logfile);t(logfile);fprintf(logfile, "-->");n(logfile);
			t(logfile);t(logfile);fprintf(logfile, "</style>");n(logfile);
			t(logfile);fprintf(logfile, "</head>");n(logfile);
			t(logfile);fprintf(logfile, "<body font face=\"Tohama\" size=\"2\" bgcolor=\"%c000000\" text=\"%cFFFFFF\">", hash.data()[0], hash.data()[0]);n(logfile);
			t(logfile);t(logfile);fprintf(logfile, "<center>");n(logfile);
			fprintf(logfile, "%s", starttable.c_str());n(logfile);
			fprintf(logfile, "%s<td bgcolor=\"%c0000FF\">Component%s<td bgcolor=\"%c0000FF\">Event%s", startline.c_str(), hash.data()[0], endcolumn.c_str(), hash.data()[0], endline.c_str());

			//Close the file, return success
			fclose (logfile);
		}
		else
			return false;

		return true;
	}

	void cLog::t(FILE * f)
	{
		fputc('\t', f);
	}

	void cLog::n(FILE * f)
	{
		fputc('\n', f);
	}

#ifdef BUILD_DEBUG
	void cLog::trace(std::string section)
	{
		std::string t="<!> ";
		t+=section;
		t+="\n";
#ifdef PLATFORM_WINDOWS
		OutputDebugString(t.c_str());
#else
		printf(t.c_str());
#endif
	}

	void cLog::trace(std::string section, std::string text)
	{
		std::string t="<!> ";
		t+=section;
		t+=" - ";
		t+=text;
		t+="\n";
#ifdef PLATFORM_WINDOWS
		OutputDebugString(t.c_str());
#else
		printf(t.c_str());
#endif
	}
#endif //BUILD_DEBUG

	bool cLog::Init(std::string filename)
	{
		strfilename="log/";
		strfilename+=filename;

		BREATHE::FILESYSTEM::CreateDirectory("log");
		BREATHE::FILESYSTEM::CreateDirectory("log/mem");
		BREATHE::FILESYSTEM::CreateFile(strfilename);

		WriteLog();

#ifdef BUILD_DEBUG
		pLog->Success("Build", "Debug");
#else
		pLog->Success("Build", "Release");
#endif //BUILD_DEBUG

		return true;
	}

	void cLog::Newline()
	{
		if((logfile=fopen(strfilename.c_str(), "a+"))==NULL)
			return;
		
		fprintf(logfile, "%s<td bgcolor=\"%c0000CC\"> %s<td bgcolor=\"%c0000CC\"> %s", startline.c_str(), hash.data()[0], endcolumn.c_str(), hash.data()[0], endline.c_str());
		
		fclose(logfile);
	}

	void cLog::Newline(std::string s1)
	{
		if((logfile=fopen(strfilename.c_str(), "a+"))==NULL)
			return;

		fprintf(logfile, "%s<td bgcolor=\"%c0000CC\">%s%s<td bgcolor=\"%c0000CC\">%s", startline.c_str(), hash.data()[0], s1.c_str(), endcolumn.c_str(), hash.data()[0], endline.c_str());
		
		fclose(logfile);

#ifdef BUILD_DEBUG
		trace(s1);
#endif //BUILD_DEBUG
	}

	void cLog::Newline(std::string s1, std::string text)
	{
		if((logfile=fopen(strfilename.c_str(), "a+"))==NULL)
			return;

		fprintf(logfile, "%s<td bgcolor=\"%c0000CC\">%s%s<td bgcolor=\"%c0000CC\">%s%s", startline.c_str(), hash.data()[0], s1.c_str(), endcolumn.c_str(), hash.data()[0], text.c_str(), endline.c_str());
		fclose(logfile);

#ifdef BUILD_DEBUG
		trace(s1, text);
#endif
	}

	void cLog::Success(std::string section, std::string text)
	{
		//Open the file for append
		if((logfile=fopen(strfilename.c_str(), "a+"))==NULL)
			return;

		fprintf(logfile, "%s%s%s%s%s%s%s", startline.c_str(), startsuccesscolumn[scol].c_str(), section.c_str(), endcolumn.c_str(), startsuccesscolumn[scol].c_str(), text.c_str(), endline.c_str());
		
		//Close the file
		fclose(logfile);

		scol=!scol;

#ifdef BUILD_DEBUG
		trace(section, text);
#endif //BUILD_DEBUG
	}

	void cLog::Error(std::string section, std::string text)
	{
		//Open the file for append
		if((logfile=fopen(strfilename.c_str(), "a+"))==NULL)
			return;

		fprintf(logfile, "%s%s%s%s%s%s%s", startline.c_str(), starterrorcolumn[ecol].c_str(), section.c_str(), endcolumn.c_str(), starterrorcolumn[ecol].c_str(), text.c_str(), endline.c_str());
		
		fclose(logfile);

		ecol=!ecol;

#ifdef BUILD_DEBUG
		trace(section, text);
#endif //BUILD_DEBUG
	}

	void cLog::Success(std::string section, char *text, ...)
	{
		va_list arg_list;
		va_start(arg_list, text);

		//Open the file for append
		if((logfile=fopen(strfilename.c_str(), "a+"))==NULL)
			return;

		if(scol==true || scol==false)
			fprintf(logfile, "%s%s%s%s%s", startline.c_str(), startsuccesscolumn[scol].c_str(), section.c_str(), endcolumn.c_str(), startsuccesscolumn[scol].c_str());
		else
		{
			scol=true;
			fprintf(logfile, "%s%s%s%s%s", startline.c_str(), startsuccesscolumn[scol].c_str(), section.c_str(), endcolumn.c_str(), startsuccesscolumn[scol].c_str());
		}

		vfprintf(logfile, text, arg_list);
		fprintf(logfile, endline.c_str());

		fclose(logfile);

#ifdef BUILD_DEBUG
		trace(section, text);
#endif //BUILD_DEBUG

		va_end(arg_list);

		scol=!scol;
	}

	void cLog::Error(std::string section, char *text, ...)
	{
		va_list arg_list;
		va_start(arg_list, text);

		//Open the file for append
		if((logfile=fopen(strfilename.c_str(), "a+"))==NULL)
			return;

		if(ecol==true || ecol==false)
			fprintf(logfile, "%s%s%s%s%s", startline.c_str(), starterrorcolumn[ecol].c_str(), section.c_str(), endcolumn.c_str(), starterrorcolumn[ecol].c_str());
		else
		{
			ecol=true;
			fprintf(logfile, "%s%s%s%s%s", startline.c_str(), starterrorcolumn[ecol].c_str(), section.c_str(), endcolumn.c_str(), starterrorcolumn[ecol].c_str());
		}
		vfprintf(logfile, text, arg_list);
		fprintf(logfile, endline.c_str());
		
		fclose(logfile);

#ifdef BUILD_DEBUG
		trace(section, text);
#endif //BUILD_DEBUG

		va_end(arg_list);

		ecol=!ecol;
	}
}