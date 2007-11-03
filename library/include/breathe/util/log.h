#ifndef CLOG_H
#define CLOG_H

#include <sstream>

namespace breathe
{
	template <class T>
	class constant_stack
	{
	public:
		template <class T>
		class iterator
		{
		public:
			iterator(size_t i);
		};

		constant_stack(size_t n);

		void push_back(const T& rhs);

		iterator<T> begin();
		iterator<T> end();
		
		size_t size() const;

		T& operator[](size_t i);

	private:
		std::vector<T> elements;
		size_t first;
		size_t n;
	};
	
	template <class T>
	constant_stack<T>::constant_stack(size_t _n)
	{
		n = _n;
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
	typename constant_stack<T>::iterator<T> constant_stack<T>::begin()
	{
		return elements.begin();
	}

	template <class T>
	typename constant_stack<T>::iterator<T> constant_stack<T>::end()
	{
		return elements.end();
	}
	
	template <class T>
	size_t constant_stack<T>::size() const
	{
		return elements.size();
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
#endif //BUILD_DEBUG

			std::ofstream logfile;

			breathe::string::string_t strfilename;

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
				bShow(false),

				pApp(nullptr),

				uiCursorBlink(0),
				uiCursorPosition(0),

				lines(10)
			{
			}

			unsigned int uiCursorBlink;
			unsigned int uiCursorPosition;

			void SetApp(cApp* inApp) { pApp = inApp; }
		
			const constant_stack<std::string>& GetLines() const { return lines; }
			const std::string GetCurrentLine() const { return current; }
			void ClearCurrent() { current = ""; }

			void AddKey(unsigned int code);
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
				//if(lLine.size()>CONSOLE_MAXLINES)
				//	lLine.pop_front();
				
				lLine.push_back(o);
				ClearLine();
			}
		};
	}
}

extern breathe::logging::cScreen SCREEN;

#endif
