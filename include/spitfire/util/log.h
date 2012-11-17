#ifndef CLOG_H
#define CLOG_H

// Standard headers
#include <sstream>

// Spitfire headers
#include <spitfire/spitfire.h>

#ifdef BUILD_LOGGING

#include <spitfire/util/string.h>
#include <spitfire/algorithm/algorithm.h>

namespace spitfire
{
  namespace logging
  {
    #undef Success
    #undef Error

    #undef ERROR

    bool IsLogging();

    void TurnOnLogging();
    void TurnOffLogging();

    enum class COLOUR {
      NORMAL = 0,
      RED,
      GREEN,
      BLUE
    };

    class cLogBase
    {
    public:
      virtual ~cLogBase() {}

      template<typename T>
      cLogBase& operator<<(const T& t)
      {
        std::ostringstream o;
        o<<t;

        line += o.str();

        return *this;
      }

      cLogBase& operator<<(const std::wstring& t)
      {
        std::wostringstream o;
        o<<t;

        line += spitfire::string::ToUTF8(o.str());

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

      cLogBase& operator<<(std::ostream& (* /*func*/)(std::ostream&))
      {
        _AddLine(line);

        return *this;
      }

      //void precision ( unsigned long p );

      void ClearLine() { line.clear(); }


      virtual void Success(const std::string& section, const std::string& text) = 0;
      virtual void Error(const std::string& section, const std::string& text) = 0;

    private:
      virtual void _AddLine(const std::string& o) = 0;

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
      void Newline(const std::string& s1);
      void Newline(const std::string& s1, const std::string& text);

      void Success(const std::string& section, const std::string& text);
      void Error(const std::string& section, const std::string& text);

    private:
      void _AddLine(const std::string& o)
      {
        Success(section, o);
        ClearLine();
      }

      bool CreateLog();

//#ifdef BUILD_DEBUG
      void trace(const std::string& section);
      void trace(const std::string& section, const std::string& text);
//#endif // BUILD_DEBUG

      string_t strfilename;

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

    private:
      cLog(const cLog&);
    };
  }
}

extern spitfire::logging::cLog LOG;

//#define LOGERROR LOG<<LOG_ERROR
#define LOGERROR LOG


namespace breathe
{
  class cApplication;
}

namespace spitfire
{
  namespace util
  {
    class cConsoleBase
    {
    public:
      cConsoleBase() :
        uiCursorBlink(0),
        uiCursorPosition(0),

        bShow(false),

        pApp(nullptr),

        lines(10)
      {
      }
      virtual ~cConsoleBase() {}

      unsigned int uiCursorBlink;
      unsigned int uiCursorPosition;

      void SetApp(breathe::cApplication* inApp) { pApp = inApp; }

      const constant_stack<std::string>& GetLines() const { return lines; }
      const std::string GetCurrentLine() const { return current; }
      void ClearCurrent() { current = ""; }

      bool AddKey(unsigned int code);
      void ExecuteCommand(const std::string& command);

      bool IsVisible() const { return bShow; }
      void Show()
      {
        bShow = true;
        uiCursorPosition = 0;
        ClearCurrent();
      }

      void Hide()
      {
        bShow = false;
        uiCursorPosition = 0;
        ClearCurrent();
      }

      constant_stack<std::string>::iterator begin() { return lines.begin(); }
      constant_stack<std::string>::iterator end() { return lines.end(); }

      constant_stack<std::string>::reverse_iterator rbegin() { return lines.rbegin(); }
      constant_stack<std::string>::reverse_iterator rend() { return lines.rend(); }

    protected:
      bool bShow;
      breathe::cApplication* pApp;

      constant_stack<std::string> lines;
      std::string current;

    private:
      NO_COPY(cConsoleBase);
    };
  }

  namespace logging
  {
    class cScreen;

    class cConsole : public cLogBase, public util::cConsoleBase
    {
    public:
      cConsole();
      ~cConsole();

      friend class cScreen;

      /*void Newline();
      void Newline(const std::string& s1);
      void Newline(const std::string& s1, const std::string& text);*/

      void Success(const std::string& section, const std::string& text) { _AddLine(section + " " + text); }
      void Error(const std::string& section, const std::string& text) { _AddLine(section + " " + text); }

    private:
      void _AddLine(const std::string& o);
    };
  }
}

extern spitfire::logging::cConsole CONSOLE;

namespace spitfire
{
  namespace logging
  {
    class cScreen : public cLogBase
    {
    public:
      cScreen();
      ~cScreen();

      void Success(const std::string& section, const std::string& text) { _AddLine(section + " " + text); }
      void Error(const std::string& section, const std::string& text) { _AddLine(section + " " + text); }

      const std::list<std::string>& GetLines() const;

      void AddMessageInformative(const string_t& text);
      void AddMessageWarning(const string_t& text);
      void AddMessageError(const string_t& text);

      void AddClosedCaption(const string_t& actor, const string_t& line, uint32_t life); // Which person is speaking, what they are saying

    private:
      enum class GAME_MESSAGE {
        INFORMATIVE,
        WARNING,
        ERROR
      };

      class cGameMessage
      {
      public:
        cGameMessage(GAME_MESSAGE type, const string_t& text, uint32_t life);

        GAME_MESSAGE GetType() const { return type; }
        const string_t& GetText() const { return text; }
        uint32_t GetLifeLeft() const { return life; }

      private:
        GAME_MESSAGE type;
        const string_t text;
        uint32_t life;
      };

      class cGameClosedCaption
      {
      public:
        cGameClosedCaption(const string_t& actor, const string_t& text, uint32_t life);

        const string_t& GetActor() const { return actor; }
        const string_t& GetText() const { return text; }
        uint32_t GetLifeLeft() const { return life; }

      private:
        const string_t actor;
        const string_t text;
        uint32_t life;
      };

      void _AddLine(const std::string& o)
      {
        // Cascade output to console
        CONSOLE._AddLine(o);

        // Add line to screen
        //if (lLine.size()>CONSOLE_MAXLINES)
        //  lLine.pop_front();

        lLine.push_back(o);
        ClearLine();
      }

      std::vector<cGameMessage*> message;
      std::vector<cGameClosedCaption*> closedCaption;

      std::list<std::string> lLine;
    };

    inline void cScreen::AddMessageInformative(const string_t& text)
    {
      cGameMessage* pMessage = new cGameMessage(GAME_MESSAGE::INFORMATIVE, text, 50);
      message.push_back(pMessage);
    }

    inline void cScreen::AddMessageWarning(const string_t& text)
    {
      cGameMessage* pMessage = new cGameMessage(GAME_MESSAGE::WARNING, text, 50);
      message.push_back(pMessage);
    }

    inline void cScreen::AddMessageError(const string_t& text)
    {
      cGameMessage* pMessage = new cGameMessage(GAME_MESSAGE::ERROR, text, 50);
      message.push_back(pMessage);
    }

    inline void cScreen::AddClosedCaption(const string_t& actor, const string_t& line, uint32_t life)
    {
      cGameClosedCaption* pMessage = new cGameClosedCaption(actor, line, life);
      closedCaption.push_back(pMessage);
    }
  }
}

extern spitfire::logging::cScreen SCREEN;

#else

#define CONSOLE std::cout
#define SCREEN std::cout

#endif // BUILD_LOGGING

#endif // CLOG_H
