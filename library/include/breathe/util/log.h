#ifndef CLOG_H
#define CLOG_H

#include <sstream>

#undef Success
#undef Error

namespace breathe
{
	template <class T>
	class constant_stack
	{
	public:
		typedef std::vector<T> container_t;
		typedef typename container_t::iterator iterator;
		typedef typename container_t::reverse_iterator reverse_iterator;

		constant_stack(size_t n);

		void push_back(const T& rhs);

		iterator begin() { return elements.begin(); }
		iterator end() { return elements.end(); }

		reverse_iterator rbegin() { return elements.rbegin(); }
		reverse_iterator rend() { return elements.rend(); }

		size_t size() const;
		bool empty() const;

		T& operator[](size_t i);

	private:
		container_t elements;
		size_t first;
		size_t n;
	};

	template <class T>
	constant_stack<T>::constant_stack(size_t _n) :
    first(0),
    n(_n)
	{
		elements.reserve(n);
	}

	template <class T>
	void constant_stack<T>::push_back(const T& rhs)
	{
		if (elements.size() < n) elements.push_back(rhs);
		else
		{
			if (first >= n) first = 0;
			elements[first++] = rhs;
		}
	}

	template <class T>
	size_t constant_stack<T>::size() const
	{
		return elements.size();
	}

	template <class T>
	bool constant_stack<T>::empty() const
	{
		return elements.empty();
	}

	template <class T>
	T& constant_stack<T>::operator[](size_t i)
	{
		assert(i < elements.size());
		return elements[(first + i) % n];
	}

	namespace logging
	{
		enum
		{
			COLOUR_NORMAL = 0,
			COLOUR_RED,
			COLOUR_GREEN,
			COLOUR_BLUE
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

				line += breathe::string::ToUTF8(o.str());

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

#ifdef BUILD_DEBUG
			void trace(const std::string& section);
			void trace(const std::string& section, const std::string& text);
#endif // BUILD_DEBUG

			std::ofstream logfile;

			breathe::string_t strfilename;

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

extern breathe::logging::cLog LOG;


namespace breathe
{
	class cApp;

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

			unsigned int uiCursorBlink;
			unsigned int uiCursorPosition;

			void SetApp(cApp* inApp) { pApp = inApp; }

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
			cApp* pApp;

			constant_stack<std::string> lines;
			std::string current;
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
			void _AddLine(const std::string& o)
			{
				// Cascade output to log file
				LOG._AddLine(o);

				lines.push_back(o);
				std::cout<<o<<std::endl;
				ClearLine();
			}
		};
	}
}

extern breathe::logging::cConsole CONSOLE;

namespace breathe
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

		private:
			std::list<std::string> lLine;

			void _AddLine(const std::string& o)
			{
				// Cascade output to console
				CONSOLE._AddLine(o);

				// Add line to screen
				//if (lLine.size()>CONSOLE_MAXLINES)
				//	lLine.pop_front();

				lLine.push_back(o);
				ClearLine();
			}
		};
	}
}

extern breathe::logging::cScreen SCREEN;


#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#ifndef NDEBUG
void InformativeAssert(bool b, const char* szAssert, const char* szFile, int line, const char* szFunction);

#ifdef ASSERT
#undef ASSERT
#endif

#define ASSERT(p) InformativeAssert(p, #p, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#else
#define ASSERT(...)
#endif

#endif // CLOG_H
