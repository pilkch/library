#include <cstdarg>
#include <cassert>
#include <cmath>

#include <list>
#include <vector>
#include <map>
#include <bitset>

#include <string>
#include <sstream>

// Reading and writing text files
#include <iostream>
#include <fstream>
#include <iomanip>

// Boost headers

// SDL headers
#include <SDL2/SDL.h>

//#if defined(BUILD_DEBUG) && defined(__WIN__)
//#include <windows.h>
//#endif

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/string.h>
#include <spitfire/util/log.h>
#include <spitfire/util/timer.h>

#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/xml.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/geometry.h>

#define t "  "

const int CONSOLE_MAXLINES = 50;


spitfire::logging::cLog gLog;
spitfire::logging::cConsole CONSOLE;
spitfire::logging::cScreen SCREEN;

#ifdef UNICODE
#define cout_t std::wcout
#else
#define cout_t std::cout
#endif

namespace spitfire
{
  namespace logging
  {
    bool bIsLogging = true;

    bool IsLogging()
    {
      return bIsLogging;
    }

    void TurnOnLogging()
    {
      bIsLogging = true;
    }

    void TurnOffLogging()
    {
      bIsLogging = false;
    }


    // ***********************************************LOG*******************************************************

    cLog::cLog() :
      strfilename(TEXT("log/index.html"))
    {
      scol = 0;
      ecol = 0;
      hash = 35;
      starttable = TEXT(t) TEXT(t) TEXT(t) TEXT("<table border=\"0\" cellspacing=\"0\">");
      startline = TEXT(t) TEXT(t) TEXT(t) TEXT(t) TEXT("<tr>");
      startsuccesscolumn[0] = TEXT("<td bgcolor=\"") + hash + TEXT("006600\" width=\"*\">");
      startsuccesscolumn[1] = TEXT("<td bgcolor=\"") + hash + TEXT("005500\">");
      starterrorcolumn[0] = TEXT("<td bgcolor=\"") + hash + TEXT("660000\">");
      starterrorcolumn[1] = TEXT("<td bgcolor=\"") + hash + TEXT("550000\">");
      endcolumn = TEXT("</td>");
      endline = TEXT("</td></tr>");
      newline = TEXT("<br>\n");
      endtable = TEXT(t) TEXT(t) TEXT(t) TEXT("</table>\n");


      filesystem::CreateDirectory(TEXT("log"));
      filesystem::CreateFile(strfilename);

      CreateLog();
    }

    cLog::~cLog()
    {
      std::ofstream logfile;

      logfile.open(spitfire::string::ToUTF8(strfilename).c_str(), std::ios::out | std::ios::app);

      if (!logfile.is_open()) return;

      logfile<<spitfire::string::ToUTF8(endtable)<<t<<t<<"</center>"<<std::endl;
      logfile<<t<<"</body>"<<std::endl;
      logfile<<"</html>"<<std::endl;
      logfile.close();
    }

    bool cLog::CreateLog()
    {
      std::ofstream logfile;

      logfile.open(spitfire::string::ToUTF8(strfilename).c_str(), std::ios::out);

      if (!logfile.is_open()) return false;

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
      logfile << t << t << t << spitfire::string::ToUTF8(starttable) << std::endl;
      logfile << t << t << t << spitfire::string::ToUTF8(startline) << "<td bgcolor=\"" << hash.data()[0] << "0000FF\">Component" << spitfire::string::ToUTF8(endcolumn) << "<td bgcolor=\"" << hash.data()[0] << "0000FF\">Event" << string::ToUTF8(endline)<<std::endl;

      //Close the file, return success
      logfile.close();

      return true;
    }

//#ifdef BUILD_DEBUG
    void cLog::trace(const string_t& _section)
    {
      if (!bIsLogging) return;

      string_t s = TEXT("<!> ") + spitfire::string::ToString_t(_section);
#ifdef __WIN__
      s += TEXT("\n");
      OutputDebugString(s.c_str());
#else
      cout_t<<s<<std::endl;
#endif
    }

    void cLog::trace(const string_t& _section, const string_t& text)
    {
      trace(_section + TEXT(" - ") + text);
    }
//#endif //BUILD_DEBUG

    void cLog::Newline()
    {
      if (!bIsLogging) return;

      std::ofstream logfile;

      logfile.open(spitfire::string::ToUTF8(strfilename).c_str(), std::ios::out | std::ios::app);

      if (!logfile.is_open()) return;

      logfile << string::ToUTF8(startline)<< "<td bgcolor=\"" <<hash.data()[0] << "0000CC\">&nbsp;" <<string::ToUTF8(endcolumn)<< "<td bgcolor=\"" << hash.data()[0] << "0000CC\">&nbsp;" <<string::ToUTF8(endline)<<std::endl;
      logfile.close();
    }

    void cLog::Newline(const string_t& s1)
    {
      if (!bIsLogging) return;

      section = s1;

      std::ofstream logfile;

      logfile.open(spitfire::string::ToUTF8(strfilename).c_str(), std::ios::out | std::ios::app);

      if (!logfile.is_open()) return;

      logfile << string::ToUTF8(startline) << "<td bgcolor=\"" << hash.data()[0] << "0000CC\">" <<string::ToUTF8(s1)<< string::ToUTF8(endcolumn) << "<td bgcolor=\"" << hash.data()[0] << "0000CC\">&nbsp;" << string::ToUTF8(endline)<<std::endl;

      logfile.close();

  //#ifdef BUILD_DEBUG
      trace(s1);
  //#endif //BUILD_DEBUG
    }

    void cLog::Newline(const string_t& s1, const string_t& text)
    {
      if (!bIsLogging) return;

      section = s1;

      std::ofstream logfile;

      logfile.open(spitfire::string::ToUTF8(strfilename).c_str(), std::ios::out | std::ios::app);

      if (!logfile.is_open()) return;

      logfile << string::ToUTF8(startline) << "<td bgcolor=\"" << hash.data()[0] << "0000CC\">" << string::ToUTF8(s1) << string::ToUTF8(endcolumn) << "<td bgcolor=\"" << hash.data()[0]<< "0000CC\">" << string::ToUTF8(text) << string::ToUTF8(endline)<<std::endl;
      logfile.close();

  //#ifdef BUILD_DEBUG
      trace(s1, text);
  //#endif
    }

    void cLog::Success(const string_t& _section, const string_t& text)
    {
      if (!bIsLogging) return;

      std::ofstream logfile;

      logfile.open(spitfire::string::ToUTF8(strfilename).c_str(), std::ios::out | std::ios::app);

      if (!logfile.is_open()) return;

      logfile << string::ToUTF8(startline) <<string::ToUTF8(startsuccesscolumn[scol])<< string::ToUTF8(_section) << string::ToUTF8(endcolumn) <<string::ToUTF8(startsuccesscolumn[scol])<< string::ToUTF8(text) << string::ToUTF8(endline)<<std::endl;

      //Close the file
      logfile.close();

      scol=!scol;

  //#ifdef BUILD_DEBUG
      trace(_section, text);
  //#endif //BUILD_DEBUG
    }

    void cLog::Error(const string_t& _section, const string_t& text)
    {
      if (!bIsLogging) return;

      std::ofstream logfile;

      logfile.open(spitfire::string::ToUTF8(strfilename).c_str(), std::ios::out | std::ios::app);

      if (!logfile.is_open()) return;

      logfile <<string::ToUTF8(startline)<<string::ToUTF8(starterrorcolumn[ecol])<<string::ToUTF8(_section)<<string::ToUTF8(endcolumn)<<string::ToUTF8(starterrorcolumn[ecol])<<string::ToUTF8(text)<<string::ToUTF8(endline)<<std::endl;

      logfile.close();

      ecol=!ecol;

  //#ifdef BUILD_DEBUG
      trace(_section, text);
  //#endif //BUILD_DEBUG
    }


    // //TODO: Do we need this one?
    // void cLog::precision(unsigned long p)
    // {
    //   logfile << std::setprecision(p);
    // }


    // ***********************************************CONSOLE*******************************************************

    cConsole::cConsole()
    {
    }

    cConsole::~cConsole()
    {
    }

    /*void cConsole::Newline()
    {
      if (lLine.size()>CONSOLE_MAXLINES)
        lLine.pop_front();

      lLine.push_back("");
    }

    void cConsole::Newline(const std::string& text)
    {
      if (lLine.size()>CONSOLE_MAXLINES)
        lLine.pop_front();

      lLine.push_back(text);
    }

    void cConsole::Newline(const std::string& s1, const std::string& text)
    {
      if (lLine.size()>CONSOLE_MAXLINES)
        lLine.pop_front();

      lLine.push_back(text);
    }

    void cConsole::Success(const std::string& section, const std::string& text)
    {
      if (lLine.size()>CONSOLE_MAXLINES)
        lLine.pop_front();

      lLine.push_back(text);
    }

    void cConsole::Error(const std::string& section, const std::string& text)
    {
      if (lLine.size()>CONSOLE_MAXLINES)
        lLine.pop_front();

      lLine.push_back(text);
    }

    cConsole& cConsole::operator<<(const std::ostringstream& o)
    {
      //logfile << t;
      LOG.Success(LOG.section, o.str());

      if (bIsLogging) cout_t<<o.str()<<std::endl;

      return *this;
    }*/

    void cConsole::_AddLine(const string_t& o)
    {
      // Cascade output to log file
      gLog._AddLine(o);

      lines.push_back(o);

      if (bIsLogging) cout_t<<o<<std::endl;

      ClearLine();
    }


    // ***********************************************SCREEN*******************************************************
    cScreen::cScreen()
    {
    }

    cScreen::~cScreen()
    {
      {
        std::vector<cGameMessage*>::iterator iter = message.begin();
        std::vector<cGameMessage*>::iterator iterEnd = message.end();
        for (;iter != iterEnd; iter++) SAFE_DELETE(*iter);

        message.clear();
      }

      {
        std::vector<cGameClosedCaption*>::iterator iter = closedCaption.begin();
        std::vector<cGameClosedCaption*>::iterator iterEnd = closedCaption.end();
        for (;iter != iterEnd; iter++) SAFE_DELETE(*iter);

        closedCaption.clear();
      }

      lLine.clear();
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
    // TODO: Console should not be here

    void cConsoleBase::ExecuteCommand(const string_t& command)
    {
      //assert(pApp != nullptr);
      //pApp->ConsoleExecute(command);
    }


    bool cConsoleBase::AddKey(unsigned int uiCode)
    {
      //const Uint8* key = SDL_GetKeyboardState(NULL);

      //key for shift, control, etc. modifiers

      //if (key[SDLK_SHIFT])
      /*
      SDLK_NUMLOCK    = 300,
      SDLK_CAPSLOCK    = 301,
      SDLK_SCROLLOCK    = 302,
      SDLK_RSHIFT    = 303,
      SDLK_LSHIFT    = 304,
      SDLK_RCTRL    = 305,
      SDLK_LCTRL    = 306,
      SDLK_RALT    = 307,
      SDLK_LALT    = 308,
      SDLK_RMETA    = 309,
      SDLK_LMETA    = 310,
      SDLK_LSUPER    = 311,    //Left "Windows" key
      SDLK_RSUPER    = 312,    //Right "Windows" key
      SDLK_MODE    = 313,    //"Alt Gr" key
      SDLK_COMPOSE    = 314,    //Multi-key compose key
      */

      // Early exit
      if ((SDLK_ESCAPE == uiCode) || (SDLK_BACKQUOTE == uiCode)) return false;


      if ((SDLK_RETURN == uiCode) || (SDLK_KP_ENTER == uiCode)) {
        ExecuteCommand(GetCurrentLine());

        ClearCurrent();
        uiCursorPosition = 0;
      } else if (SDLK_DELETE == uiCode) {
        if (uiCursorPosition<current.size())
          current.erase(uiCursorPosition, 1);
      } else if (SDLK_BACKSPACE == uiCode) {
        if (uiCursorPosition > 0)
        {
          current.erase(uiCursorPosition-1, 1);
          uiCursorPosition--;
        }
      } else if (SDLK_LEFT == uiCode) {
        if (uiCursorPosition > 0)
          uiCursorPosition--;
      }

      else if (SDLK_RIGHT == uiCode) {
        if (uiCursorPosition<current.size()) uiCursorPosition++;
      } else if (SDLK_HOME == uiCode) uiCursorPosition = 0;
      else if (SDLK_END == uiCode) uiCursorPosition = current.size();

      else if (SDLK_TAB == uiCode)
        ; //TODO: Autocomplete
      else if (SDLK_CLEAR == uiCode)
        current.clear();
      else if (SDLK_UP == uiCode)
        ; //TODO: History of typed in items
      else if (SDLK_DOWN == uiCode)
        ; //TODO: History of typed in items
      else if (SDLK_PAGEUP == uiCode)
        ; //TODO: History of all console items
      else if (SDLK_PAGEDOWN == uiCode)
        ; //TODO: History of all console items

      else if (uiCode < 300) {
        /*#include <SDL/SDL_keysym.h>

        if ((uiCode<SDLK_a || uiCode>SDLK_z) && (uiCode<SDLK_0 || uiCode>SDLK_9)) {
          std::ostringstream t;
          t<<"unmapped key: "<<uiCode<<" ("<<static_cast<unsigned char>(uiCode)<<")";

          ConsoleAddLine(t.str());
        }*/

        string_t s;
        s += char_t(uiCode);

        current.insert(uiCursorPosition, s);
        uiCursorPosition++;
      }

      //When we press a key we want to see where we are up to
      uiCursorBlink = 0;

      return true;
    }
  }
}
