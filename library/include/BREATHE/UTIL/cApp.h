#ifndef CAPP_H
#define CAPP_H

namespace BREATHE
{
	const float KEY_MIN = 0.1f;

	class cApp
	{
	public:
		cApp(int argc, char **argv);
		~cApp();

		bool Init();
		bool Run();
		
		// We have a default render function, but if you want to do anything special you will have to override this
		virtual void Render(float fCurrentTime);

		// Pure virtual functions, these *have* to be overridden in your derived game class
		virtual bool LoadScene()=0;
		virtual bool InitScene()=0;
		virtual bool DestroyScene()=0;

		virtual void FullscreenSwitch()=0;

		virtual void Update(float fCurrentTime)=0;
		virtual void UpdatePhysics(float fCurrentTime)=0;
		virtual void UpdateInput(float fCurrentTime)=0;
		virtual void RenderScene(float fCurrentTime)=0;
		virtual void RenderScreenSpace(float fCurrentTime)=0;
		virtual void OnMouse(int button,int state,int x,int y)=0;

		virtual bool Execute(std::string sCommand)=0;


		bool InitRender();
		bool DestroyRender();		

		bool ToggleFullscreen();
		bool SetPerspective();
		bool ResizeWindow(unsigned int w, unsigned int h);

		void AddKeyRepeat(unsigned int code); // ie. for a key like wasd.  
		void AddKeyNoRepeat(unsigned int code); // ie. for key like escape, enter, spacebar, etc.  
		void AddKeyToggle(unsigned int code); // ie. tilde for console, either on or off, a press toggles.  

		bool IsKeyDown(unsigned int code);
		bool IsKeyDownReset(unsigned int code);

		void UpdateKeys(float fCurrentTime);
		void UpdateEvents(float fCurrentTime);

		void OnKeyUp(SDL_keysym *keysym);
    
		void ConsoleAddKey(unsigned int code);
		void ConsoleExecute(std::string s);

		void SetTitle(std::string sTitle);

		cVar<std::string>* VarFind(std::string name);

		template <class T>
		void VarSet(std::string name, T value);


#ifdef BUILD_DEBUG
		bool bDebug;
#endif

		bool bConsole;
		bool bActive;
		bool bDone;
		bool bUpdatePhysics;
		bool bStepPhysics;

		bool bReturnCode;

		UTIL::cTimer tPhysics;
		UTIL::cTimer tUpdate;
		UTIL::cTimer tRender;

		std::vector<std::string>vArgs;

		std::map<std::string, cVar<std::string>*>mVar;
		
		std::vector<SDL_Joystick*>vJoystick;
		
		const SDL_VideoInfo *videoInfo;

		SDL_Event event;


		// Information about the current video settings
		SDL_VideoInfo* g_info;

	private:
		void _ConsoleExecuteSingleCommand(std::string s);

		bool _IsKeyDown(float fAmount);
		
		std::string title;

		class cKey
		{
		public:
			//std::string sCommand;
			//cKey(std::string command);

			bool bVariable;
			bool bRepeat;
			bool bToggle;

			bool bDown;
			bool bCollected;

			unsigned int uiCode;

			cKey(unsigned int code, bool variable, bool repeat, bool toggle);
			
			bool IsKeyDown();
			void SetKeyUp(bool bConsole);
		};

		std::map<unsigned int, cKey * >mKey;
	};


	// *** Inlines

	inline void cApp::SetTitle(std::string sTitle)
	{
		title = sTitle;
	}

	template <class T>
	inline void cApp::VarSet(std::string name, T value)
	{
		std::map<std::string, cVar<std::string>*>::iterator iter = mVar.begin();

		std::string s;
		while(iter != mVar.end())
		{
			if(name == iter->first)
			{
				cVar<std::string>* p = iter->second;
				*p = value;
				return;
			}

			iter++;
		};

		mVar[name] = new cVar<std::string>(value);
	}

	
	// Convert from a float amount to a bool
	inline bool cApp::_IsKeyDown(float fAmount)
	{
		return (fAmount > KEY_MIN || fAmount < -KEY_MIN);
	}
}

#endif //CAPP_H
