#ifndef CAPP_H
#define CAPP_H

namespace breathe
{
	const float KEY_MIN = 0.1f;
  enum STATE_RETURN
  {
    STATE_POP_THIS_STATE = false,
    STATE_KEEP_THIS_STATE = false
  };

	namespace render
	{
		class cFont;
	}

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
		int GetMouseDeltaX() const;
		int GetMouseDeltaY() const;

		void CursorShow();
		void CursorHide();

		void ConsoleShow();
		void ConsoleHide();
		void ConsoleExecute(const std::string& s);

#ifdef BUILD_DEBUG
		bool IsDebug() const { return bDebug; }
		void ToggleDebug() { bDebug = !bDebug; }
#endif

    
    class cAppState
    {
    public:
      cAppState() : result(0), pParent(nullptr) {}

      void SetParent(cAppState* _pParent) { assert(_pParent != nullptr); pParent = _pParent; }
      cAppState* GetParent() { return pParent; }

      void SetResult(int iResult) { result = iResult; }
      int GetResult() const { return result; }

      bool HasParent() const { return pParent != nullptr; }

      // Functions used to transition between states
	    void OnEntry() { _OnEntry(); }
	    void OnExit() { _OnExit(); }

      // Functions that deal with states being parented by other states, 
      // the child runs and then returns a result to the parent
      void OnPause() { _OnPause(); }
      void OnResume(int iResult) { _OnResume(iResult); }

		  void Update(sampletime_t currentTime) { _Update(currentTime); }
		  void UpdateInput(sampletime_t currentTime) { _UpdateInput(currentTime); }
#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
      void UpdatePhysics(sampletime_t currentTime) { _UpdatePhysics(currentTime); }
#endif
		  void RenderScene(sampletime_t currentTime) { _RenderScene(currentTime); }
      void RenderScreenSpace(sampletime_t currentTime) { _RenderScreenSpace(currentTime); }

    private:
	    virtual void _OnEntry() {}
	    virtual void _OnExit() {}
	    virtual void _OnPause() {}
	    virtual void _OnResume(int iResult) {}

      virtual void _Update(sampletime_t currentTime) {}
		  virtual void _UpdateInput(sampletime_t currentTime) {}
#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
		  virtual void _UpdatePhysics(sampletime_t currentTime) {}
#endif
		  virtual void _RenderScene(sampletime_t currentTime) {}
		  virtual void _RenderScreenSpace(sampletime_t currentTime) {}

      int result;
      cAppState* pParent;
    };

	protected:
    cAppState& GetCurrentState();

    // Instantaneous
    void PushState(cAppState* state);
    void PopState();

    // Deferred until we get back to the main cApp loop
    void PushStateSoon(cAppState* state) { assert(pPushThisStateSoon == nullptr); pPushThisStateSoon = state; }
    void PopStateSoon() { assert(bPopCurrentStateSoon == false); bPopCurrentStateSoon = true; }


#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
		bool bStepPhysics;
		bool bUpdatePhysics;
#endif
		bool bDone;

		SDL_Event event;

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
    util::cTimer tPhysics;
#endif
		util::cTimer tUpdate;
		util::cTimer tRender;
		
		std::vector<SDL_Joystick*>vJoystick;

		render::cFont* pFont;

	private:
		class cConsoleWindow : public gui::cWindow
		{
		public:
			cConsoleWindow();
			~cConsoleWindow();

			void InitConsoleWindow();

			gui::cWidget_StaticText& GetPrevious() { return *pPrevious; }
			gui::cWidget_Input& GetInput() { return *pInput; }
			
		private:
			void _OnMouseEvent(int button, int state, int x, int y) {}
			void _OnEvent(gui::id_t id);

			gui::cWidget_StaticText* pPrevious;
			gui::cWidget_Input* pInput;
		};
		
		class cKey
		{
		public:
			cKey(unsigned int code, bool variable, bool repeat, bool toggle);
			
			bool IsDown();
			void SetDown(bool bConsole);
			void SetUp(bool bConsole);

			//std::string sCommand;
			//cKey(const std::string& command);

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
			cMouse() : x(0), y(0) { down.reset(); }
			
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
		void _Update(sampletime_t currentTime);
		void _UpdateInput(sampletime_t currentTime);
		void _Render(cApp::cAppState& state, sampletime_t currentTime);

	  // Convert from a float amount to a bool
		static bool _IsKeyDown(float fAmount) { return (fAmount > KEY_MIN || fAmount < -KEY_MIN); }

		// The render order is managed/automated by this class, so if you want to do anything special like 
		// rendering to an FBO first or adding a timer in, you can do it by overriding these
		virtual void BeginRender(sampletime_t currentTime) {}
		virtual void EndRender(sampletime_t currentTime) {}

		// Pure virtual functions, these *have* to be overridden in your derived game class
		virtual bool LoadScene() = 0;
		virtual bool InitScene() = 0;
		virtual bool DestroyScene() = 0;
		
		virtual void FullscreenSwitch()=0;
		virtual bool Execute(const std::string& sCommand)=0;	

		void _UpdateKeys(sampletime_t currentTime);
		void _UpdateEvents(sampletime_t currentTime);
		
		void _OnKeyUp(SDL_keysym* keysym);
		void _OnKeyDown(SDL_keysym* keysym);
		void _OnMouseUp(int button, int x, int y);
		void _OnMouseDown(int button, int x, int y);
		void _OnMouseMove(int button, int x, int y);

		void _LoadSearchDirectories();
    		
#ifdef BUILD_DEBUG
		bool bDebug;
#endif

		bool bActive;
		bool bReturnCode;

		std::vector<std::string>vArgs;

		std::map<unsigned int, cKey* >mKey;
		cMouse mouse;
		
		gui::cWindowManager window_manager;
		cConsoleWindow* pConsoleWindow;

    std::list<cAppState*> states;

  private:
    bool bPopCurrentStateSoon;
    cAppState* pPushThisStateSoon;
    
		// Forbidden
		void _OnMouseEvent(int button, int state, int x, int y);
		void OnMouse(int button,int state,int x,int y);
	};


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
