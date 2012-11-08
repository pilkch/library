#ifndef CAPPLICATION_H
#define CAPPLICATION_H

// Standard headers
#include <stack>

// Spitfire headers
#include <spitfire/math/math.h>

// Breathe headers
#include <breathe/audio/audio.h>
#include <breathe/gui/cManager.h>
#include <breathe/gui/cRenderer.h>
#include <breathe/render/cContext.h>
#include <breathe/render/cFont.h>
#include <breathe/render/cSystem.h>
#include <breathe/render/cVertexBufferObject.h>
#include <breathe/render/cWindow.h>

namespace breathe
{
  namespace util
  {
    class cApplication;

    // ** cState

    class cState : public gui::cWidgetEventListener
    {
    public:
      explicit cState(cApplication& application);
      virtual ~cState();

      void Pause() { _OnPause(); }
      void Resume() { _OnResume(); }

      void OnWindowEvent(const gui::cWindowEvent& event) { _OnWindowEvent(event); }
      void OnMouseEvent(const gui::cMouseEvent& event) { _OnMouseEvent(event); }
      void OnKeyboardEvent(const gui::cKeyboardEvent& event) { _OnKeyboardEvent(event); }

      void Update(const spitfire::math::cTimeStep& timeStep) { _Update(timeStep); }
      void UpdateInput(const spitfire::math::cTimeStep& timeStep) { _UpdateInput(timeStep); }
      void Render(const spitfire::math::cTimeStep& timeStep) { _Render(timeStep); }

    protected:
      render::cSystem& system;
      render::cWindow* pWindow;
      render::cContext* pContext;

      audio::cManager* pAudioManager;

      gui::cManager* pGuiManager;
      gui::cRenderer* pGuiRenderer;

      gui::cLayer* pLayer;

    private:
      virtual void _OnEnter() {}
      virtual void _OnExit() {}
      virtual void _OnPause() {}
      virtual void _OnResume() {}

      virtual void _OnWindowEvent(const gui::cWindowEvent& event) {}
      virtual void _OnMouseEvent(const gui::cMouseEvent& event) {}
      virtual void _OnKeyboardEvent(const gui::cKeyboardEvent& event) {}

      virtual void _Update(const spitfire::math::cTimeStep& timeStep) {}
      virtual void _UpdateInput(const spitfire::math::cTimeStep& timeStep) {}
      virtual void _Render(const spitfire::math::cTimeStep& timeStep) {}


      virtual void _OnStateMouseEvent(const gui::cMouseEvent& event) {}
      virtual void _OnStateKeyboardEvent(const gui::cKeyboardEvent& event) {}

      virtual gui::EVENT_RESULT _OnWidgetEvent(const gui::cWidgetEvent& event) { return gui::EVENT_RESULT::NOT_HANDLED_PERCOLATE; }
    };


    // ** cApplication

    class cApplication : public opengl::cWindowEventListener, public opengl::cInputEventListener
    {
    public:
      friend class cState;

      cApplication();
      ~cApplication();

      void Run();

      void PushStateSoon(cState* pState);
      void PopStateSoon();

    protected:
      // State management
      const cState* GetState() const;
      cState* GetState();
      void PushState(cState* pState);
      void PopState();

      bool bIsDone;

      opengl::cSystem system;
      opengl::cWindow* pWindow;
      opengl::cContext* pContext;

      breathe::audio::cManager* pAudioManager;

    private:
      bool Create();
      void Destroy();

      void MainLoop();

      virtual bool _Create() { return true; }
      virtual void _Destroy() {}

      void _OnWindowEvent(const opengl::cWindowEvent& event);
      void _OnMouseEvent(const opengl::cMouseEvent& event);
      void _OnKeyboardEvent(const opengl::cKeyboardEvent& event);

      // State event management (Transitioning between states)
      void ProcessStateEvents();


      std::stack<cState*> states;

      // State change event queue
      typedef cState* cStateEvent;
      std::vector<cState*> stateEvents;
    };
  }
}

#endif // CAPPLICATION_H


/*#ifndef CAPPLICATION_H
#define CAPPLICATION_H

#include <breathe/breathe.h>

#include <breathe/audio/audio.h>

#include <breathe/game/scenegraph.h>
#include <breathe/game/scenegraph2d.h>
#include <breathe/game/cGameUnitTest.h>

#include <breathe/gui/cWidget.h>
#include <breathe/gui/cWindow.h>
#include <breathe/gui/cWindowManager.h>

#include <breathe/render/camera.h>

namespace breathe
{
  const float KEY_MIN = 0.1f;

  enum class STATE_RETURN {
    POP_THIS_STATE = 0,
    KEEP_THIS_STATE = 1
  };

  const size_t STATE_CONSOLE = 0;

  namespace render
  {
    class cFont;
  }

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
  namespace physics
  {
    class cWorld;
  }
#endif

  class cApplication
  {
  public:
    cApplication(int argc, const char* const* argv);
    virtual ~cApplication();

    bool InitApp();
    bool DestroyApp();



    bool Run();


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

    void CreateGameUnitTest();
#endif

    bool IsBlurPostRenderEffect() const { return bIsBlurPostRenderEffect; }
    void SetBlurPostRenderEffect(bool _bIsBlurPostRenderEffect);


    bool IsHDRBloomPostRenderEffect() const { return bIsHDRBloomPostRenderEffect; }

    void SetHDRBloomPostRenderEffect(bool _bIsHDRBloomPostRenderEffect);


    render::material::cMaterialRef AddPostRenderEffect(const string_t& sFilename);
    void RemovePostRenderEffect();


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

      void PreRender(sampletime_t currentTime) { _PreRender(currentTime); }
      void RenderScene(sampletime_t currentTime) { _RenderScene(currentTime); }
      void RenderScreenSpace(sampletime_t currentTime) { _RenderScreenSpace(currentTime); }

      void OnMouseEvent(int button, int state, float x, float y) { _OnMouseEvent(button, state, x, y); }



      class cKeyboardEvent
      {
      public:
        bool IsKeyPressed(key_t key) const; // A key down and then a key up event in quick succession

        bool IsKeyDown(key_t key) const;
        bool IsKeyUp(key_t key) const;


      private:
        std::list<key_t> keys; // This can contain for example KEY::A, KEY::SHIFT
      };

      class cMouseEvent
      {
      public:
        bool IsPrimaryButtonDown() const;
        bool IsSecondaryButtonDown() const;

        bool IsTertiaryButtonDown() const;

        float GetX() const;
        float GetY() const;
        float GetZ() const; // Scrolling wheel
      };

      void OnEvent(const cKeyboardEvent& event) { _OnEvent(event); }
      void OnEvent(const cMouseEvent& event) { _OnEvent(event); }

    private:
      virtual void _OnEvent(const cKeyboardEvent& event) {}
      virtual void _OnEvent(const cMouseEvent& event) {}



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

      // Override to create dynamic textures, do last minute changes to the scene graph, etc.
      // For example we can override this function and render to texture for dynamic cubemaps, spheremaps, precalculated shadow maps, etc.
      virtual void _PreRender(sampletime_t currentTime) {}
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

    // Deferred until we get back to the main cApplication loop

    void PushStateSoon(cAppState* state) { ASSERT(pPushThisStateSoon == nullptr); pPushThisStateSoon = state; }
    void PopStateSoon() { ASSERT(bPopCurrentStateSoon == false); bPopCurrentStateSoon = true; }

    // This does both, it defers changing state until we get back to the main cApplication loop,
    // but it also pops the current state and pushes a new one

    void PopStateAndPushNewStateSoon(cAppState* state) { PopStateSoon(); PushStateSoon(state); }

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
    bool bStepPhysics;

    bool bUpdatePhysics;
#endif
    bool bDone;



    SDL_Event event;


#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)

    spitfire::util::cTimer tPhysics;

#endif
    spitfire::util::cTimer tUpdate;
    spitfire::util::cTimer tRender;




    render::cSystem system;

    render::cContext* pContext;

    render::cWindow* pWindow;
    render::cResourceManager* pResourceManager;



    render::cFont* pFont;



    render::cCamera camera;


    scenegraph3d::cSceneGraph scenegraph;

    scenegraph2d::cSceneGraph scenegraph2D;


    breathe::audio::cManager* pAudioManager;


#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)

    breathe::physics::cWorld* pWorld;

#endif



    std::vector<SDL_Joystick*> vJoystick;



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

      void _OnEvent(const gui::cEvent& event);


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



    void LoadConfigXML(render::cResolution& resolution);

    void _ConsoleExecuteSingleCommand(const std::string& s);

    void _InitArguments(int argc, const char* const* argv);

    void _LoadSearchDirectories();



    // Pure virtual functions, these *have* to be overridden in your derived game class

    virtual bool LoadScene() = 0;
    virtual bool InitScene() = 0;

    virtual bool DestroyScene() = 0;


    virtual void FullscreenSwitch()=0;

    virtual bool Execute(const std::string& sCommand)=0;


    // Convert from a float amount to a bool

    static bool _IsKeyDown(float fAmount) { return (fAmount > KEY_MIN || fAmount < -KEY_MIN); }



    void _UpdateKeys(sampletime_t currentTime);

    void _UpdateEvents(sampletime_t currentTime);



    void _OnKeyUp(SDL_keysym* keysym);

    void _OnKeyDown(SDL_keysym* keysym);

    void _OnMouseUp(int button, int x, int y);

    void _OnMouseDown(int button, int x, int y);

    void _OnMouseMove(int button, int x, int y);





    void _Update(sampletime_t currentTime);

    void _UpdateInput(sampletime_t currentTime);

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)

    void _UpdatePhysics(cApplication::cAppState& state, sampletime_t currentTime);

#endif


    // Main rendering function, basically controls the rendering process from rendering to textures, using those textures to render a scene and applying those textures to the screen

    void _Render(cApplication::cAppState& state, sampletime_t currentTime);

    void _RenderScreenSpaceScene(cApplication::cAppState& state, sampletime_t currentTime);





#ifdef BUILD_DEBUG

    bool bDebug;

#endif



    bool bIsBlurPostRenderEffect;

    bool bIsHDRBloomPostRenderEffect;



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



    void CheckIfNeedToUpdateApplication();


    void CreateFBOTextures();



    void RemoveKey(unsigned int code);


    virtual cAppState* _GetFirstAppState() = 0;


    // Forbidden
    void _OnMouseEvent(int button, int state, int x, int y);
    void OnMouse(int button,int state,int x,int y);


    std::list<render::material::cMaterialRef> lPostRenderEffects;
    render::cTextureFrameBufferObjectRef pFrameBuffer0;
    render::cTextureFrameBufferObjectRef pFrameBuffer1;

    render::cTextureFrameBufferObjectRef pHDRBloomExposureFrameBuffer;


    bool bPopCurrentStateSoon;
    cAppState* pPushThisStateSoon;

    class cAppStateConsole : public breathe::cApplication::cAppState
    {
    public:
      cAppStateConsole(cApplication& _app) : breathe::cApplication::cAppState(STATE_CONSOLE), app(_app), pConsoleWindow(nullptr) {}
      virtual ~cAppStateConsole() {}

    private:
      void _OnEntry();
      void _OnExit();

      void _Update(breathe::sampletime_t currentTime);
      void _UpdateInput(breathe::sampletime_t currentTime);
      void _RenderScene(breathe::sampletime_t currentTime) { GetParent()->RenderScene(currentTime); }
      void _RenderScreenSpace(breathe::sampletime_t currentTime) { GetParent()->RenderScreenSpace(currentTime); }

      cApplication& app;
      cConsoleWindow* pConsoleWindow;
    };


#ifdef BUILD_DEBUG
    scenegraph2d::cGraphNodeRef pFPSRenderGraph;
    scenegraph2d::cGraphNodeRef pFPSUpdateGraph;
    scenegraph2d::cGraphNodeRef pFPSPhysicsGraph;
    scenegraph2d::cGraphNodeRef pMemoryUsageGraph;

    friend class cGameUnitTest;
    cGameUnitTest* pGameUnitTest; // Only used if/when the application wants it
#endif
  };
}

#endif // CAPPLICATION_H*/

