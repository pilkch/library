#ifndef CAPP_H
#define CAPP_H

namespace BREATHE
{
	class cAppKey;

	class cApp
	{
		void ConsoleExecuteSingleCommand(std::string s);

	public:

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

		std::map<std::string, cVar *>mVar;
		
		std::map<unsigned int, cAppKey * >mKey;
		std::vector<SDL_Joystick*>vJoystick;
		
		const SDL_VideoInfo *videoInfo;

		SDL_Event event;


		// Information about the current video settings
		SDL_VideoInfo* g_info;


		std::string sTitle;

		cApp(int argc, char **argv);
		~cApp();

		bool Init();
		bool Run();

		bool InitRender();
		bool DestroyRender();		

		bool ToggleFullscreen();
		bool SetPerspective();
		bool ResizeWindow(unsigned int w, unsigned int h);

		void AddKey(unsigned int code, bool repeat);
		bool IsKeyDown(unsigned int code);
		void UpdateKeys(float fCurrentTime);
		void UpdateEvents(float fCurrentTime);

		void OnKeyUp(SDL_keysym *keysym);
    
		void ConsoleAddKey(unsigned int code);
		void ConsoleExecute(std::string s);

		cVar *VarFind(std::string name);
		void VarSet(std::string name, std::string value);

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
		
		virtual void Render(float fCurrentTime);

		virtual bool Execute(std::string sCommand)=0;
	};

	class cAppKey
	{
	public:
		//std::string sCommand;
		//cKey(std::string command);

		bool bRepeat;
		bool bDown;
		bool bCollected;

		unsigned int uiCode;

		cAppKey(unsigned int code, bool repeat=true);
		
		bool IsKeyDown();
		void SetKeyUp(bool bConsole);
	};
}

#endif //CAPP_H
