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
        ostringstream_t o;
        o<<t;

        line += o.str();

        return *this;
      }

      cLogBase& operator<<(const std::string& t)
      {
        line += string::ToString_t(t);

        return *this;
      }

      cLogBase& operator<<(const std::wstring& t)
      {
        line += string::ToString_t(t);

        return *this;
      }

      cLogBase& operator<<(float t)
      {
        ostringstream_t o;
        o<<t;

        line += o.str();

        return *this;
      }

      cLogBase& operator<<(bool t)
      {
        line += (t ? TEXT("true") : TEXT("false"));

        return *this;
      }

      cLogBase& operator<<(const ostringstream_t& o)
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


      virtual void Success(const string_t& section, const string_t& text) = 0;
      virtual void Error(const string_t& section, const string_t& text) = 0;

    private:
      virtual void _AddLine(const string_t& o) = 0;

      string_t line;
    };


    class cConsole;

    class cLog : public cLogBase
    {
    public:
      cLog();
      ~cLog();

      friend class cConsole;

      void Newline();
      void Newline(const string_t& s1);
      void Newline(const string_t& s1, const string_t& text);

      void Success(const string_t& section, const string_t& text);
      void Error(const string_t& section, const string_t& text);

    private:
      void _AddLine(const string_t& o)
      {
        Success(section, o);
        ClearLine();
      }

      bool CreateLog();

//#ifdef BUILD_DEBUG
      void trace(const string_t& section);
      void trace(const string_t& section, const string_t& text);
//#endif // BUILD_DEBUG

      string_t strfilename;

      bool scol;
      bool ecol;
      string_t starttable;
      string_t startline;
      string_t startsuccesscolumn[2];
      string_t starterrorcolumn[2];
      string_t endcolumn;
      string_t endline;
      string_t newline;
      string_t endtable;
      string_t hash;

      string_t section;

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

      size_t uiCursorBlink;
      size_t uiCursorPosition;

      void SetApp(breathe::cApplication* inApp) { pApp = inApp; }

      const constant_stack<string_t>& GetLines() const { return lines; }
      const string_t& GetCurrentLine() const { return current; }
      void ClearCurrent() { current.clear(); }

      bool AddKey(unsigned int code);
      void ExecuteCommand(const string_t& command);

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

      constant_stack<string_t>::iterator begin() { return lines.begin(); }
      constant_stack<string_t>::iterator end() { return lines.end(); }

      constant_stack<string_t>::reverse_iterator rbegin() { return lines.rbegin(); }
      constant_stack<string_t>::reverse_iterator rend() { return lines.rend(); }

    protected:
      bool bShow;
      breathe::cApplication* pApp;

      constant_stack<string_t> lines;
      string_t current;

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
      void Newline(const string_t& s1);
      void Newline(const string_t& s1, const string_t& text);*/

      void Success(const string_t& section, const string_t& text) { _AddLine(section + TEXT(" ") + text); }
      void Error(const string_t& section, const string_t& text) { _AddLine(section + TEXT(" ") + text); }

    private:
      void _AddLine(const string_t& o);
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

      void Success(const string_t& section, const string_t& text) { _AddLine(section + TEXT(" ") + text); }
      void Error(const string_t& section, const string_t& text) { _AddLine(section + TEXT(" ") + text); }

      const std::list<string_t>& GetLines() const;

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

      void _AddLine(const string_t& o)
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

      std::list<string_t> lLine;
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

#ifdef UNICODE
#define CONSOLE std::wcout
#define SCREEN std::wcout
#else
#define CONSOLE std::cout
#define SCREEN std::cout
#endif

#endif // BUILD_LOGGING

#endif // CLOG_H
