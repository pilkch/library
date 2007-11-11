#ifndef CAPP_H
#define CAPP_H

namespace breathe
{
	const float KEY_MIN = 0.1f;

	class cApp
	{
	public:
		cApp(int argc, char** argv);
		virtual ~cApp();

		bool InitApp();
		bool DestroyApp();

		bool InitRender();
		bool DestroyRender();

		bool Run();

		bool ToggleFullscreen();
		bool ResizeWindow(unsigned int w, unsigned int h);

		void AddKeyRepeat(unsigned int code); // ie. for a key like wasd.  
		void AddKeyNoRepeat(unsigned int code); // ie. for key like escape, enter, spacebar, etc.  
		void AddKeyToggle(unsigned int code); // ie. tilde for console, either on or off, a press toggles.  

		bool IsKeyDown(unsigned int code);
		bool IsKeyDownReset(unsigned int code);

		bool IsMouseLeftButtonDown() const;
		bool IsMouseRightButtonDown() const;
		bool IsMouseScrollUp() const;
		bool IsMouseScrollDown() const;

		int GetMouseX() const;
		int GetMouseY() const;

		void CursorShow();
		void CursorHide();

		void ConsoleShow();
		void ConsoleHide();
		void ConsoleExecute(const std::string& s);
		
		void SetTitle(const std::string& sTitle);

#ifdef BUILD_DEBUG
		bool IsDebug() const { return bDebug; }
		void ToggleDebug() { bDebug = !bDebug; }
#endif

	protected:
		bool bStepPhysics;
		bool bUpdatePhysics;
		bool bDone;

		SDL_Event event;

		util::cTimer tPhysics;
		util::cTimer tUpdate;
		util::cTimer tRender;
		
		std::vector<SDL_Joystick*>vJoystick;

	private:
		class cConsoleWindow : public gui::cWindow
		{
		public:
			cConsoleWindow();
			~cConsoleWindow();
			
		private:
			void _OnMouseEvent(int button, int state, int x, int y) {}
			void _OnEvent(unsigned int id);
		};
		
		class cKey
		{
		public:
			cKey(unsigned int code, bool variable, bool repeat, bool toggle);
			
			bool IsDown();
			void SetDown(bool bConsole);
			void SetUp(bool bConsole);

			//std::string sCommand;
			//cKey(std::string command);

			bool bVariable;
			bool bRepeat;
			bool bToggle;

			bool bDown;
			bool bCollected;

			unsigned int uiCode;
		};

		class cMouse
		{
		public:
			cMouse::cMouse() : x(0), y(0) { down.reset(); }
			
			bool IsDown(bool bConsole, size_t button) const { assert(button < 10); return down[button]; }
			bool IsUp(bool bConsole, size_t button) const { assert(button < 10); return !down[button]; }

			int GetX() const { return x; }
			int GetY() const { return y; }
			
			std::bitset<10> down;
			int x;
			int y;
		};

		void _ConsoleExecuteSingleCommand(const std::string& s);
		void _InitArguments(int argc, char** argv);
		void _Render(sampletime_t currentTime);


		void _UpdateInput(sampletime_t currentTime);
		bool _IsKeyDown(float fAmount);

		// The render order is managed/automated by this class, so if you want to do anything special like 
		// rendering to an FBO first or adding a timer in, you can do it by overriding these
		virtual void BeginRender(sampletime_t currentTime) {}
		virtual void EndRender(sampletime_t currentTime) {}

		// Pure virtual functions, these *have* to be overridden in your derived game class
		virtual bool LoadScene() = 0;
		virtual bool InitScene() = 0;
		virtual bool DestroyScene() = 0;

		virtual void Update(sampletime_t currentTime) = 0;
		virtual void UpdatePhysics(sampletime_t currentTime) = 0;
		virtual void RenderScene(sampletime_t currentTime) = 0;
		virtual void RenderScreenSpace(sampletime_t currentTime) = 0;
		
		virtual void UpdateInput(sampletime_t currentTime) = 0;

		virtual void FullscreenSwitch()=0;
		virtual bool Execute(const std::string& sCommand)=0;	

		void _UpdateKeys(sampletime_t currentTime);
		void _UpdateEvents(sampletime_t currentTime);
		
		void _OnKeyUp(SDL_keysym* keysym);
		void _OnKeyDown(SDL_keysym* keysym);
		void _OnMouseUp(int button, int x, int y);
		void _OnMouseDown(int button, int x, int y);
		void _OnMouseMove(int button, int x, int y);
    		
#ifdef BUILD_DEBUG
		bool bDebug;
#endif

		bool bActive;
		bool bReturnCode;

		std::string title;
		std::vector<std::string>vArgs;

		std::map<unsigned int, cKey* >mKey;
		cMouse mouse;
		
		gui::cWindowManager window_manager;
		cConsoleWindow* pConsoleWindow;
		
		
	private:
		// Forbidden
		void _OnMouseEvent(int button, int state, int x, int y) {}
		void OnMouse(int button,int state,int x,int y) {}
	};


	// *** Inlines

	inline void cApp::SetTitle(const std::string& sTitle)
	{
		title = sTitle;
	}
	
	// Convert from a float amount to a bool
	inline bool cApp::_IsKeyDown(float fAmount)
	{
		return (fAmount > KEY_MIN || fAmount < -KEY_MIN);
	}

	

	// This doesn't belong here
	// TODO: Move to breathe/algorithm/gen.h
	namespace vector
	{
		template <class T>
		inline void push_back(std::vector<T>& v, size_t n, const T& value)
		{
			v.reserve(n);
			for (size_t i = 0; i != n; i++) v.push_back(value);
		}
	}
}

#endif //CAPP_H
