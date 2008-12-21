#ifndef CAPP_H
#define CAPP_H

#include <breathe/game/scenegraph.h>
#include <breathe/game/scenegraph2d.h>

namespace breathe
{
  const float KEY_MIN = 0.1f;
  enum STATE_RETURN
  {
    STATE_POP_THIS_STATE = 0,
    STATE_KEEP_THIS_STATE = 1
  };

  const size_t STATE_CONSOLE = 0;

  namespace render
  {
    class cFont;
  }

  class cApp
  {
  public:
    cApp(int argc, const char** argv);
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

    bool IsKeyDown(unsigned int code) const;
    bool IsKeyDownReset(unsigned int code) const;

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
    void CursorWarpToMiddleOfScreen();

    void ConsoleExecute(const std::string& s);

#ifdef BUILD_DEBUG
    bool IsDebug() const { return bDebug; }
    void ToggleDebug() { bDebug = !bDebug; }
#endif

    gui::cWindowManager window_manager;

    class cAppState
    {
    public:
      explicit cAppState(size_t _uiState) : uiState(_uiState), result(0), pParent(nullptr) {}
      virtual ~cAppState() {}

      size_t GetID() const { return uiState; }

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

      void OnMouseEvent(int button, int state, float x, float y) { _OnMouseEvent(button, state, x, y); }

    protected:
      void AddMessageInformative(const string_t& text) { SCREEN.AddMessageInformative(text); }
      void AddMessageWarning(const string_t& text) { SCREEN.AddMessageWarning(text); }
      void AddMessageError(const string_t& text) { SCREEN.AddMessageError(text); }

      // Which person is speaking, what they are saying
      void AddClosedCaption(const string_t& actor, const string_t& line, uint32_t life) { SCREEN.AddClosedCaption(actor, line, life); }

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

      virtual void _OnMouseEvent(int button, int state, float x, float y) {}

      size_t uiState;
      int result;
      cAppState* pParent;
    };

  protected:
    cAppState& GetCurrentState() const;

    // Instantaneous
    void PushState(cAppState* state);
    void PopState();

    // Deferred until we get back to the main cApp loop
    void PushStateSoon(cAppState* state) { ASSERT(pPushThisStateSoon == nullptr); pPushThisStateSoon = state; }
    void PopStateSoon() { ASSERT(bPopCurrentStateSoon == false); bPopCurrentStateSoon = true; }

    // This does both, it defers changing state until we get back to the main cApp loop,
    // but it also pops the current state and pushes a new one
    void PopStateAndPushNewStateSoon(cAppState* state) { PopStateSoon(); PushStateSoon(state); }


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

    scenegraph3d::cSceneGraph scenegraph;
    scenegraph2d::cSceneGraph scenegraph2D;

  private:
    class cConsoleWindow : public gui::cModelessWindow
    {
    public:
      cConsoleWindow();

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

      bool IsDown() const;
      void SetDown(bool bConsole);
      void SetUp(bool bConsole);

      //std::string sCommand;
      //cKey(const std::string& command);

      bool bVariable;
      bool bRepeat;
      bool bToggle;

      unsigned int uiCode;

      mutable bool bDown;
      mutable bool bCollected;
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

    void LoadConfigXML();
    void _ConsoleExecuteSingleCommand(const std::string& s);
    void _InitArguments(int argc, const char** argv);
    void _Update(sampletime_t currentTime);
    void _UpdateInput(sampletime_t currentTime);
#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
    void _UpdatePhysics(cApp::cAppState& state, sampletime_t currentTime);
#endif
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

    // TODO: Change to std::stack?
    std::list<cAppState*> states;

  private:
#ifdef BUILD_DEBUG
    void SanityCheck();
#endif

    void RemoveKey(unsigned int code);

    virtual cAppState* _GetFirstAppState() = 0;

    // Forbidden
    void _OnMouseEvent(int button, int state, int x, int y);
    void OnMouse(int button,int state,int x,int y);

    bool bPopCurrentStateSoon;
    cAppState* pPushThisStateSoon;

    class cAppStateConsole : public breathe::cApp::cAppState
    {
    public:
      cAppStateConsole(cApp& _app) : breathe::cApp::cAppState(STATE_CONSOLE), app(_app), pConsoleWindow(nullptr) {}
      virtual ~cAppStateConsole() {}

    private:
      void _OnEntry();
      void _OnExit();

      void _Update(breathe::sampletime_t currentTime);
      void _UpdateInput(breathe::sampletime_t currentTime);
      void _RenderScene(breathe::sampletime_t currentTime) { GetParent()->RenderScene(currentTime); }
      void _RenderScreenSpace(breathe::sampletime_t currentTime) { GetParent()->RenderScreenSpace(currentTime); }

      cApp& app;
      cConsoleWindow* pConsoleWindow;
    };
  };
}

#endif // CAPP_H
