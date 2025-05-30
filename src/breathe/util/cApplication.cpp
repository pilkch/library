/*

<config>
<input>
<onfoot>
<key code="w" action="forward" repeat="true"/>
<key code="a" action="left" repeat="true"/>
</onfoot>
<invehicle>
<key code="a" action="gearup" repeat="false"/>
<key code="up" action="forward" repeat="true"/>
</invehicle>
</input>
</config>

And then console is handled automatically, not configurable.  Cannot override tilde key for console toggle on/off.


#define EPSILON 0.000001

int intersect_triangle(
      const math::cVec3& orig, const math::cVec3& dir,
      const math::cVec3& vert0, const math::cVec3& vert1, const math::cVec3& vert2,
      double *t, double *u, double *v)
{
  // Find vectors for two edges sharing vert0

  math::cVec3 edge1 = vert1 - vert0;
  math::cVec3 edge2 = vert2 - vert0;

  // Begin calculating determinant - also used to calculate U parameter
  math::cVec3 pvec = dir.CrossProduct(edge2);

  // if determinant is near zero, ray lies in plane of triangle
  double det = edge1.DotProduct(pvec);

  math::cVec3 tvec;
  math::cVec3 qvec;

#ifdef TEST_CULL
  if (det < EPSILON) return 0;

  // Calculate distance from vert0 to ray origin
  tvec = orig - vert0;

  // calculate U parameter and test bounds
  *u = tvec.DotProduct(pvec);
  if (*u < 0.0 || *u > det) return 0;


  // prepare to test V parameter
  qvec = tvec.CrossProduct(edge1);

  // calculate V parameter and test bounds

  *v = dir.DotProduct(qvec);
  if (*v < 0.0 || *u + *v > det) return 0;

  // calculate t, scale parameters, ray intersects triangle
  *t = edge2.DotProduct(qvec);

  double inv_det = 1.0 / det;
  *t *= inv_det;
  *u *= inv_det;
  *v *= inv_det;


#else                    // the non-culling branch
   if (det > -EPSILON && det < EPSILON) return 0;

   double inv_det = 1.0 / det;


   // calculate distance from vert0 to ray origin
   tvec = orig - vert0;

   // calculate U parameter and test bounds
   *u = tvec.DotProduct(pvec) * inv_det;

   if (*u < 0.0 || *u > 1.0) return 0;

   // prepare to test V parameter
   qvec = tvec.CrossProduct(edge1);

   // calculate V parameter and test bounds

   *v = dir.DotProduct(qvec) * inv_det;

   if (*v < 0.0 || *u + *v > 1.0) return 0;

   // calculate t, ray intersects triangle

   *t = edge2.DotProduct(qvec) * inv_det;
#endif

   return 1;
}

*/

// Standard headers
#include <iostream>

// Spitfire headers
#include <spitfire/util/log.h>

// Breathe headers
#include <breathe/util/cApplication.h>

namespace breathe
{
  namespace util
  {
    // ** cState

    cState::cState(cApplication& application) :
      system(application.system),
      pWindow(application.pWindow),
      pContext(application.pContext),
      pAudioManager(application.pAudioManager)
    {
    }

    cState::~cState()
    {
    }


    // ** cApplication

    cApplication::cApplication(int argc, const char* const* argv) :
      spitfire::cConsoleApplication(argc, argv),

      pWindow(nullptr),
      pContext(nullptr),

      pAudioManager(nullptr),

      fFramesPerSecond(60.0f)
    {
      LOG("\"", argv[0], "\"");
    }

    cApplication::~cApplication()
    {
      Destroy();
    }

    void cApplication::_PrintHelp() const
    {
      std::cout<<"Usage: "<<spitfire::string::ToUTF8(GetApplicationName())<<" [OPTIONS]"<<std::endl;
      std::cout<<std::endl;
      std::cout<<" -help, --help Display this help and exit"<<std::endl;
      std::cout<<" -version, --version Display version information and exit"<<std::endl;
    }

    string_t cApplication::_GetVersion() const
    {
      ostringstream_t o;
      o<<GetApplicationName();
      o<<" "<<BUILD_APPLICATION_VERSION_STRING;
      return o.str();
    }

    bool cApplication::Create()
    {
      const opengl::cCapabilities& capabilities = system.GetCapabilities();

      // Use a 16:9 windowed resolution
      const size_t width = 1080;
      const size_t height = 1920;

      opengl::cResolution resolution = capabilities.GetCurrentResolution();
      if ((resolution.width < width) || (resolution.height < height) || ((resolution.pixelFormat != opengl::PIXELFORMAT::R8G8B8) && (resolution.pixelFormat != opengl::PIXELFORMAT::R8G8B8A8))) {
        LOGERROR("Current screen resolution is not adequate ", resolution.width, "x", resolution.height);
        return false;
      }

      resolution.width = width;
      resolution.height = height;
      resolution.pixelFormat = opengl::PIXELFORMAT::R8G8B8A8;

      pWindow = system.CreateWindow(TEXT(SPITFIRE_APPLICATION_NAME), resolution, false);
      if (pWindow == nullptr) {
        LOGERROR("Window could not be created");
        return false;
      }

      pContext = pWindow->GetContext();
      if (pContext == nullptr) {
        LOGERROR("Context could not be created");
        return false;
      }

      // Setup our event listeners
      pWindow->SetWindowEventListener(*this);
      pWindow->SetInputEventListener(*this);

      // Register our joystick event listener
      joystickManager.SetEventListener(*this);

      pAudioManager = breathe::audio::Create(breathe::audio::DRIVER::DEFAULT);

      // Call our overridden create function
      return _Create();
    }

    void cApplication::Destroy()
    {
      // Call our overridden destroy function
      _Destroy();


      if (pAudioManager != nullptr) {
        breathe::audio::Destroy(pAudioManager);
        pAudioManager = nullptr;
      }

      // Invalidate our joystick event listener
      joystickManager.InvalidateEventListener();

      pContext = nullptr;

      if (pWindow != nullptr) {
        system.DestroyWindow(pWindow);
        pWindow = nullptr;
      }
    }

    void cApplication::_HandleSDLEvent(const SDL_Event& event)
    {
      joystickManager.HandleSDLEvent(event);
    }

    #ifdef __WIN__
    LRESULT cApplication::_HandleWin32Event(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
      cState* pState = GetState();
      if (pState != nullptr) return pState->HandleWin32Event(hwnd, uMsg, wParam, lParam);

      return FALSE;
    }
    #endif

    void cApplication::_OnWindowEvent(const opengl::cWindowEvent& event)
    {
      LOG("");

      // The resources are probably going to become invalid when the window is resized, so we need to handle destroying and reloading them ourselves
      if (event.IsAboutToResize()) {
        _DestroyResources();
      } else if (event.IsResized()) {
        _LoadResources();
      }

      // Notify the derived application
      OnApplicationWindowEvent(event);

      // Notify the current state
      cState* pState = GetState();
      if (pState != nullptr) pState->OnWindowEvent(event);
    }

    void cApplication::_OnMouseEvent(const opengl::cMouseEvent& event)
    {
      cState* pState = GetState();
      if (pState != nullptr) pState->OnMouseEvent(event);
    }

    void cApplication::_OnKeyboardEvent(const opengl::cKeyboardEvent& event)
    {
      cState* pState = GetState();
      if (pState != nullptr) pState->OnKeyboardEvent(event);
    }

    void cApplication::OnJoystickEvent(const util::cJoystickEvent& event)
    {
      cState* pState = GetState();
      if (pState != nullptr) pState->OnJoystickEvent(event);
    }


    void cApplication::PushStateSoon(cState* pState)
    {
      cStateEvent event = pState;
      stateEvents.push_back(event);
    }

    void cApplication::PopStateSoon()
    {
      cStateEvent event = nullptr; // A null state event means "Pop the current state"
      stateEvents.push_back(event);
    }

    void cApplication::PushState(cState* pState)
    {
      ASSERT(pState != nullptr);

      cState* pOldState = GetState();
      if (pOldState != nullptr) pOldState->Pause();

      states.push_back(pState);

      pState->Resume();

      // Force an update so that textures etc. are loaded
      const durationms_t currentTime = SDL_GetTicks();
      const spitfire::math::cTimeStep timeStep(currentTime, 33);
      pState->Update(timeStep);
    }

    void cApplication::PopState()
    {
      cState* pOldState = GetState();
      if (pOldState != nullptr) {
        pOldState->Pause();

        // Delete and remove the old state
        delete pOldState;
        states.pop_back();

        // Prepare the previous state
        cState* pCurrentState = GetState();
        if (pCurrentState != nullptr) pCurrentState->Resume();
      }
    }

    cState* cApplication::GetState()
    {
      cState* pState = nullptr;
      if (!states.empty()) {
        pState = states.back();
        ASSERT(pState != nullptr);
      }
      return pState;
    }

    void cApplication::ProcessStateEvents()
    {
      const size_t n = stateEvents.size();
      for (size_t i = 0; i < n; i++) {
        cState* pState = stateEvents[i];
        if (pState != nullptr) PushState(pState);
        else PopState(); // A null state event means "Pop the current state"
      }

      stateEvents.clear();
    }

    void cApplication::MainLoop()
    {
      assert(pContext != nullptr);
      assert(pContext->IsValid());

      uint32_t T0 = 0;
      uint32_t Frames = 0;

      uint32_t currentTime = SDL_GetTicks();
      uint32_t lastTime = SDL_GetTicks();

      uint32_t lastUpdateTime = SDL_GetTicks();

      // Updates are locked at 60 fps
      const float fUpdateTimeStep = 1000.0f / 60.0f;

      while (!states.empty()) {
        ProcessStateEvents();
        if (states.empty()) break;

        // Update window events
        pWindow->ProcessEvents();

        // Update state
        lastTime = currentTime;
        currentTime = SDL_GetTicks();
        {
          const spitfire::math::cTimeStep timeStep(currentTime, currentTime - lastTime);
          cState* pState = GetState();
          assert(pState != nullptr);
          pState->UpdateInput(timeStep);
        }

        // Perform an Update
        if ((currentTime - lastUpdateTime) > fUpdateTimeStep) {
          const spitfire::math::cTimeStep timeStep(currentTime, fUpdateTimeStep);
          cState* pState = GetState();
          assert(pState != nullptr);
          pState->Update(timeStep);
          lastUpdateTime = currentTime;
        }

        // Update audio
        const spitfire::math::cVec3 listenerPosition;
        const spitfire::math::cVec3 listenerTarget;
        const spitfire::math::cVec3 listenerUp(0.0f, 0.0f, 1.0f);
        pAudioManager->Update(currentTime, listenerPosition, listenerTarget, listenerUp);

        // Render a frame
        {
          const spitfire::math::cTimeStep timeStep(currentTime, currentTime - lastTime);
          cState* pState = GetState();
          assert(pState != nullptr);
          pState->Render(timeStep);
        }

        // Gather our frames per second
        Frames++;
        {
          const uint32_t t = SDL_GetTicks();
          // TODO: Try changing this to 1000
          if (t - T0 >= 5000) {
            const float seconds = (t - T0) / 1000.0f;
            fFramesPerSecond = Frames / seconds;
            LOG(Frames, " frames in ", seconds, " seconds = ", fFramesPerSecond, " FPS");
            T0 = t;
            Frames = 0;
          }
        }
      };
    }

    bool cApplication::_Run()
    {
      const bool bIsSuccess = Create();
      if (bIsSuccess) MainLoop();

      Destroy();

      return bIsSuccess;
    }
  }
}



#if 0

  render::material::cMaterialRef pGaussianBlurMaterial;
  render::material::cMaterialRef pHDRBloomMaterial;

  cApplication::cApplication(int argc, const char* const* argv) :

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
    bStepPhysics(false),
    bUpdatePhysics(true),
#endif
    bDone(false),

    pContext(nullptr),
    pWindow(nullptr),
    pResourceManager(nullptr),

    pFont(nullptr),

    pAudioManager(nullptr),
    pWorld(nullptr),

#ifdef BUILD_DEBUG
    bDebug(true),
#endif

    bIsBlurPostRenderEffect(false),
    bIsHDRBloomPostRenderEffect(false),

    bActive(true),
    bReturnCode(true),

    bPopCurrentStateSoon(false),
    pPushThisStateSoon(nullptr)

#ifdef BUILD_DEBUG
    , pGameUnitTest(nullptr)
#endif
  {
    // Set our cout to use the system locale (What does it use by default?)
    std::cout.imbue(std::locale(""));

    CONSOLE.SetApp(this);
    filesystem::SetThisExecutable(breathe::string::ToString_t(argv[0]));

    // Make sure that we are in the directory we think we are in
    filesystem::ChangeToDirectory(filesystem::GetThisApplicationDirectory());

    // Set our main thread identifier so that when we call IsMainThread later it will work correctly
    spitfire::util::SetMainThread();
    spitfire::util::cRunOnMainThreadQueue::Create();

#ifdef BUILD_DEBUG
    SanityCheck();
#endif

    filesystem::Create();

    _LoadSearchDirectories();

    spitfire::util::LoadLanguageFiles();

    _InitArguments(argc, argv);

    srand(time(NULL));

    LOG("This is printed to the log");
    CONSOLE<<"This is printed to the console"<<std::endl;
    SCREEN<<"This is printed to the screen"<<std::endl;

    CONSOLE<<"1"<<std::endl;
    CONSOLE<<"2"<<std::endl;
    CONSOLE<<"1"<<"2"<<"3"<<"4"<<std::endl;

    // get all files ending in .dll
    CONSOLE<<"FileSystem Test"<<std::endl;

    breathe::filesystem::path dir_path(TEXT("."));
    /*directory_iterator end_it;
    for (directory_iterator it(dir_path); it != end_it; ++it) {
      if (!is_directory(it->status()) && extension(it->path()) == ".dll" ) {
        it->path().string();
      }
    }*/



    breathe::network::Init();


    //CheckIfNeedToUpdateApplication();


    SDL_ShowCursor(SDL_DISABLE);

#ifdef BUILD_LEVEL
    pLevel = new cLevel();
#endif

    AddKeyNoRepeat(SDLK_ESCAPE);
    AddKeyNoRepeat(SDLK_BACKQUOTE);
    AddKeyNoRepeat(SDLK_RETURN);

#ifdef BUILD_DEBUG
    AddKeyNoRepeat(SDLK_F1);
    AddKeyNoRepeat(SDLK_F2);
    AddKeyNoRepeat(SDLK_F3);
    AddKeyNoRepeat(SDLK_F4);
    AddKeyNoRepeat(SDLK_F5);
    AddKeyNoRepeat(SDLK_F6);
    AddKeyNoRepeat(SDLK_F7);
    AddKeyNoRepeat(SDLK_F8);
    AddKeyNoRepeat(SDLK_F9);
    AddKeyNoRepeat(SDLK_F10);
    AddKeyNoRepeat(SDLK_F11);
#endif
  }

  cApplication::~cApplication()
  {
    assert(states.empty());

    std::map<unsigned int, cKey*>::iterator iter = mKey.begin();
    const std::map<unsigned int, cKey*>::iterator iterEnd = mKey.end();
    while (iter != iterEnd) {
      SAFE_DELETE(iter->second);
      iter++;
    }
    mKey.clear();
    assert(mKey.empty());

    SAFE_DELETE(pFont);

    TTF_Quit();

    LOG.Success("Destroy", "Audio");
    breathe::audio::Destroy(pAudioManager);
    pAudioManager = nullptr;

    LOG.Success("Destroy", "Joystick");
    const size_t nJoysticks = vJoystick.size();
    for (size_t i = 0; i < nJoysticks; i++) SDL_JoystickClose(vJoystick[i]);

    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);


#ifdef BUILD_LEVEL
    LOG.Success("Delete", "Level");
    SAFE_DELETE(pLevel);
#endif

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
    LOG.Success("Destroy", "Physics");
    breathe::physics::Destroy(pWorld);
    pWorld = nullptr;
#endif

    LOG.Success("Destroy", "Network");
    breathe::network::Destroy();

    LOG.Success("Delete", "Resource Manager");
    system.DestroyResourceManager(pResourceManager);
    pResourceManager = nullptr;

    LOG.Success("Delete", "Render");
    system.DestroyContextAndWindow(pContext, pWindow);
    pContext = nullptr;
    pWindow = nullptr;

    system.Destroy();


    LOG.Success("Delete", "Log");
    // Log is not actually deleted here, we were previously doing stuff like sending stats to the log

    LOG.Success("Delete", "Filesystem");
    filesystem::Destroy();

    spitfire::util::cRunOnMainThreadQueue::Destroy();

    LOG.Success("Main", "Successfully exited");
    LOG.Newline("Main", "return " + bReturnCode ? "true" : "false");

    SDL_Quit();
  }

  void cApplication::_InitArguments(int argc, const char* const* argv)
  {
    int i = 1;
    std::string s;
    if (i < argc) {
      vArgs.push_back(argv[i]);
      s = argv[i];
    }

    for (i = 2; i < argc; i++) {
      vArgs.push_back(argv[i]);
      s += " " + std::string(argv[i]);
    }

    LOG.Success("Arguments", s);
  }

  void cApplication::_LoadSearchDirectories()
  {
    // Now load all the rest from the config file
    breathe::xml::cNode root(TEXT("config.xml"));
    breathe::xml::cNode::iterator iter(root);

    if (!iter.IsValid()) return;

    iter.FindChild("config");
    if (!iter.IsValid()) return;

    iter.FindChild("directory");
    if (!iter.IsValid()) return;

    while (iter.IsValid()) {
      std::string sDirectory;
      if (iter.GetAttribute("path", sDirectory)) {
        LOG("Adding Directory ", sDirectory);
        breathe::filesystem::AddDirectory(breathe::string::ToString_t(sDirectory));
      }

      iter.Next("directory");
    };
  }



  // Physics 2D has width, height
  // Physics 3D has width, depth and infinite height
#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
  float physics_width = 500.0f;
  float physics_height = 500.0f;
#endif
#if defined(BUILD_PHYSICS_3D)
  float physics_depth = 500.0f;
#endif

  void cApplication::LoadConfigXML(render::cResolution& resolution)
  {
    LOG.Success("Init", "Loading config.xml");

    breathe::xml::cNode root(TEXT("config.xml"));
    breathe::xml::cNode::iterator iter(root);

    if (!iter.IsValid()) {
      bReturnCode = false;
      return;
    }

    iter.FindChild("config");
    if (!iter.IsValid()) return;

    breathe::xml::cNode::iterator config(iter);

    iter.FindChild("render");
    while (iter.IsValid()) {
      std::ostringstream t;
      unsigned int uiValue;

      if (iter.GetAttribute("width", uiValue)) {
        t.str("");
        t<<"width = ";
        t<<uiValue;
        LOG.Success("Config", t.str());
        resolution.SetWidth(uiValue);
      }

      if (iter.GetAttribute("height", uiValue)) {
        t.str("");
        t<<"height = ";
        t<<uiValue;
        LOG.Success("Config", t.str());
        resolution.SetHeight(uiValue);
      }

      if (iter.GetAttribute("depth", uiValue)) {
        t.str("");
        t<<"depth = ";
        t<<uiValue;
        LOG.Success("Config", t.str());
        resolution.SetColourDepth(uiValue);
      }

      bool bFullscreen;

      if (iter.GetAttribute("fullscreen", bFullscreen)) {
        LOG.Success("Config", std::string("fullscreen = ") + (bFullscreen ? "true" : "false"));
        resolution.SetFullScreen(bFullscreen);
      }

      iter.Next("render");
    };

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
    iter = config;

    iter.FindChild("physics");
    while (iter.IsValid()) {
      iter.GetAttribute("width", physics_width);
#if defined(BUILD_PHYSICS_2D)
      iter.GetAttribute("height", physics_height);
#else
      iter.GetAttribute("depth", physics_depth);
#endif

      iter.Next("physics");
    };
#endif
  }

  bool cApplication::InitApp()
  {
    render::cResolution resolution;
    resolution.SetWidth(1024);
    resolution.SetHeight(576);
    resolution.SetColourDepth(32);
    resolution.SetWindowed(true);

    LoadConfigXML(resolution);

    // Init SDL
    if (!SDL_Init( SDL_INIT_VIDEO)) {
      LOG.Error("SDL", std::string("SDL_Init for video FAILED error=") + SDL_GetError());
      bReturnCode = false;
      return false;
    }

    // Set our window caption
    SDL_SetWindowTitle(pWindow, breathe::string::ToUTF8(LANG("L_Application")).c_str());

    // Set our window icon
    SDL_Surface* pIcon = IMG_Load("data/icons/application_32x32.png");
    if (pIcon == nullptr) LOGERROR("Could not load data/icons/application_32x32.png");
    SDL_SetWindowIcon(pWindow, pIcon);
    SDL_DestroySurface(pIcon);
    pIcon = nullptr;

    {
      // Joysticks

      if (!SDL_InitSubSystem(SDL_INIT_JOYSTICK)) {
        LOG.Error("SDL", std::string("SDL_InitSubSystem for joysticks FAILED error=") + SDL_GetError());
        bReturnCode = false;
        return false;
      }

      const size_t nJoysticks = SDL_NumJoysticks();

      std::ostringstream t;
      t<<"Joysticks found: "<<nJoysticks;

      if (nJoysticks != 0) {
        LOG.Success("SDL", t.str());
        SDL_JoystickEventState(SDL_ENABLE);

        for (size_t i = 0; i < nJoysticks; i++) {
          t.str("");
          t << "Joystick(";
          t << i;
          t << ") ";
          t << SDL_JoystickNameForIndex(i);
          LOG.Success("SDL", t.str());

          //TODO: Create a list of joysticks, close them at the end of the program
          SDL_Joystick* pJoystick = SDL_JoystickOpen(i);
          ASSERT(pJoystick != nullptr);

          t.str("");
          t << "Buttons=";
          t << SDL_JoystickNumButtons(pJoystick);
          t << ", Axes=";
          t << SDL_JoystickNumAxes(pJoystick);
          t << ", Hats=";
          t << SDL_JoystickNumHats(pJoystick);
          t << ", Balls=";
          t << SDL_JoystickNumBalls(pJoystick);
          LOG.Success("SDL", t.str());

          vJoystick.push_back(pJoystick);
        }

      } else LOG.Error("SDL", t.str());

      /*if (nJoysticks)
      {
        SDL_Event event;
        // Other initializtion code goes here
        // Start main game loop here

        while(SDL_PollEvent(&event))
        {
          switch(event.type)
          {
            case SDL_KEYDOWN:
              // handle keyboard stuff here
              break;

            case SDL_QUIT:

            // Set whatever flags are necessary to
            // end the main game loop here

              break;


            case SDL_JOYAXISMOTION:  // Handle Joystick Motion
              if (( event.jaxis.value < -3200 ) || (event.jaxis.value > 3200 ) )
              {
                // code goes here
              }
              break;

            case SDL_JOYAXISMOTION:  // Handle Joystick Motion
              if (( event.jaxis.value < -3200 ) || (event.jaxis.value > 3200 ) )
              {
                if (event.jaxis.axis == 0)
                {
                  // Left-right movement code goes here
                }

                if (event.jaxis.axis == 1)
                {
                  // Up-Down movement code goes here
                }
              }
              break;

            case SDL_JOYBUTTONDOWN:  // Handle Joystick Button Presses
              if (event.jbutton.button == 0 )
              {
                // code goes here
              }
              break;

            case SDL_JOYHATMOTION:  // Handle Hat Motion
              if (event.jhat.value & SDL_HAT_UP )
              {
                // Do up stuff here
              }
              if (event.jhat.value & SDL_HAT_LEFT )
              {
                // Do left stuff here
              }
              if (event.jhat.value & SDL_HAT_RIGHTDOWN )
              {
                // Do right and down together stuff here
              }
              break;
            }
        }

        // End loop here
      }*/
    }


    system.Create();

    //system.GetAvailableScreenResolutions();

    system.CreateContextAndWindow(resolution);
    pContext = system.GetContext();
    pWindow = system.GetWindow();

    pResourceManager = system.CreateResourceManager();


    TTF_Init();

    pAudioManager = audio::Create(audio::DRIVER::SDLMIXER);

#if defined(BUILD_PHYSICS_2D)
    pWorld = physics::Create(physics::DRIVER::BOX2D, physics_width, physics_height);
#elif defined(BUILD_PHYSICS_BULLET)
    pWorld = physics::Create(physics::DRIVER::BULLET, physics_width, physics_depth, physics_height);
#elif defined(BUILD_PHYSICS_ODE)
    pWorld = physics::Create(physics::DRIVER::ODE, physics_width, physics_depth, physics_height);
#endif

    if (!LoadScene()) return false;


    window_manager.LoadTheme();

#ifdef BUILD_DEBUG
    // Testing Window
    /*unsigned int n = breathe::gui::GenerateID();
    breathe::gui::cWindow* pWindow0 = new breathe::gui::cWindow(breathe::gui::GenerateID(), 0.7f, 0.75f, 0.2f, 0.2f);
    pWindow0->AddChild(new breathe::gui::cWidget_StaticText(n, 0.05f, 0.05f, 0.1f, 0.1f));
    pWindow0->AddChild(new breathe::gui::cWidget_StaticText(breathe::gui::GenerateID(), 0.05f, 0.5f, 0.1f, 0.1f));
    pWindow0->AddChild(new breathe::gui::cWidget_Button(breathe::gui::GenerateID(), 0.5f, 0.05f, 0.1f, 0.1f));
    window_manager.AddChild(pWindow0);*/
#endif

    if (!InitScene()) return false;


    pGaussianBlurMaterial = pResourceManager->AddMaterial(TEXT("postrender/gaussian_blur_two_pass.mat"));
    ASSERT(pGaussianBlurMaterial != nullptr);

    pHDRBloomMaterial = pResourceManager->AddMaterial(TEXT("postrender/bloom.mat"));
    ASSERT(pHDRBloomMaterial != nullptr);



    scenegraph.Create();
    scenegraph2D.Create();

#ifdef BUILD_DEBUG
    {
      // Add the frames per second graph to the scenegraph
      scenegraph2d::cSceneNodeRef pRoot = scenegraph2D.GetRoot();
      scenegraph2d::cGroupNode* pRootAsGroupNode = static_cast<scenegraph2d::cGroupNode*>(pRoot.get());

      const size_t nPoints = 100;
      const float_t fRangeMax = 30.0f;

      {
        pFPSRenderGraph.reset(new scenegraph2d::cGraphNode(nPoints));

        pFPSRenderGraph->SetRangeMax(fRangeMax);

        scenegraph2d::cStateSet& stateset = pFPSRenderGraph->GetStateSet();
        stateset.SetColour(math::cColour(1.0f, 0.0f, 0.0f));

        pRootAsGroupNode->AttachChild(pFPSRenderGraph);
      }

      {
        pFPSUpdateGraph.reset(new scenegraph2d::cGraphNode(nPoints));

        pFPSUpdateGraph->SetRangeMax(fRangeMax);

        scenegraph2d::cStateSet& stateset = pFPSUpdateGraph->GetStateSet();
        stateset.SetColour(math::cColour(0.0f, 1.0f, 0.0f));

        pRootAsGroupNode->AttachChild(pFPSUpdateGraph);
      }

      {
        pFPSPhysicsGraph.reset(new scenegraph2d::cGraphNode(nPoints));

        pFPSPhysicsGraph->SetRangeMax(fRangeMax);

        scenegraph2d::cStateSet& stateset = pFPSPhysicsGraph->GetStateSet();
        stateset.SetColour(math::cColour(0.0f, 0.0f, 1.0f));

        pRootAsGroupNode->AttachChild(pFPSPhysicsGraph);
      }

      {
        pMemoryUsageGraph.reset(new scenegraph2d::cGraphNode(nPoints));

        pMemoryUsageGraph->SetRangeMax(3000.0f);

        scenegraph2d::cStateSet& stateset = pMemoryUsageGraph->GetStateSet();
        stateset.SetColour(math::cColour(1.0f, 1.0f, 0.0f));

        pRootAsGroupNode->AttachChild(pMemoryUsageGraph);
      }
    }
#endif


    // This should be the first state added and it should not be added by the derived class, it should only be added here
    if (!states.empty()) LOG.Error("cApplication::InitApp", "No states should have be pushed yet");
    ASSERT(states.empty());

    cAppState* pAppState = _GetFirstAppState();
    if (pAppState == nullptr) LOG.Error("cApplication::InitApp", "_GetFirstAppState must be overridden and must return a valid state");
    ASSERT(pAppState != nullptr);

    // Add the state now
    PushState(pAppState);


    breathe::audio::GetManager()->StartAll();

    // Setup mouse
    CursorWarpToMiddleOfScreen();

    //SDL_ShowCursor(SDL_DISABLE);

    return true;
  }

  bool cApplication::DestroyApp()
  {
#ifdef BUILD_DEBUG
    if (pGameUnitTest != nullptr) {
      pGameUnitTest->Destroy();
      SAFE_DELETE(pGameUnitTest);
    }
#endif

    CursorShow();

    breathe::audio::GetManager()->StopAll();

    return true;
  }

#ifdef BUILD_DEBUG
  void cApplication::CreateGameUnitTest()
  {
    LOG("");

    // If we have already created our game unittest then we can reuse it
    if (pGameUnitTest != nullptr) return;

    pGameUnitTest = new cGameUnitTest(*this);

    pGameUnitTest->Init();
  }
#endif

  void cApplication::_Update(durationms_t currentTime)
  {
#ifdef BUILD_DEBUG
    if (pGameUnitTest != nullptr) pGameUnitTest->Update(currentTime);

    pFPSRenderGraph->AddPoint(tRender.GetMPF());
    pFPSUpdateGraph->AddPoint(tUpdate.GetMPF());
    pFPSPhysicsGraph->AddPoint(tPhysics.GetMPF());
    pMemoryUsageGraph->AddPoint(float(spitfire::operatingsystem::GetMemoryUsedByApplicationMB()));
#endif

    // Update our current state
    GetCurrentState().Update(currentTime);

    // NOTE: The current state needs to have set this camera correctly by now
    pContext->SetFrustum(camera.CreateFrustumFromCamera());

    // Now update our other sub systems
    breathe::audio::GetManager()->Update(currentTime, camera.GetEyePosition(), camera.GetLookAtPoint(), camera.GetUpDirection());

    scenegraph.Update(currentTime);
    scenegraph.Cull(currentTime, camera);

    scenegraph2D.Update(currentTime);
    scenegraph2D.Cull(currentTime, camera);
  }



  void cApplication::CreateFBOTextures()
  {
    // If we have never used post render effects before we have to create our FBO textures
    if (pFrameBuffer0 == nullptr) {
      pFrameBuffer0.reset(new render::cTextureFrameBufferObject);
      pFrameBuffer0->Create();
    }

    if (pFrameBuffer1 == nullptr) {
      pFrameBuffer1.reset(new render::cTextureFrameBufferObject);
      pFrameBuffer1->Create();
    }
  }

  render::material::cMaterialRef cApplication::AddPostRenderEffect(const string_t& sFilename)
  {
    render::material::cMaterialRef pMaterial = pResourceManager->AddMaterial(sFilename);
    ASSERT(pMaterial != nullptr);

    lPostRenderEffects.push_back(pMaterial);

    CreateFBOTextures();

    return pMaterial;
  }

  void cApplication::RemovePostRenderEffect()
  {
    if (!lPostRenderEffects.empty()) lPostRenderEffects.pop_back();
  }

  void cApplication::SetBlurPostRenderEffect(bool _bIsBlurPostRenderEffect)
  {
    bIsBlurPostRenderEffect = _bIsBlurPostRenderEffect;

    if (bIsBlurPostRenderEffect) CreateFBOTextures();
  }

  void cApplication::SetHDRBloomPostRenderEffect(bool _bIsHDRBloomPostRenderEffect)
  {
    bIsHDRBloomPostRenderEffect = _bIsHDRBloomPostRenderEffect;

    if (bIsHDRBloomPostRenderEffect) {
      CreateFBOTextures();

      if (pHDRBloomExposureFrameBuffer == nullptr) {
        pHDRBloomExposureFrameBuffer.reset(new render::cTextureFrameBufferObject);
        pHDRBloomExposureFrameBuffer->Create();
      }
    }
  }


  void cApplication::_RenderScreenSpaceScene(cApplication::cAppState& state, durationms_t currentTime)
  {
#ifdef BUILD_DEBUG
    if (pGameUnitTest != nullptr) pGameUnitTest->RenderScreenSpace(currentTime);
#endif


    scenegraph2D.Render(currentTime, *pContext);

    state.RenderScreenSpace(currentTime);

    if (pContext->bIsRenderGui) window_manager.Render();

#ifdef BUILD_DEBUG
    if (IsDebug() && !CONSOLE.IsVisible()) {
      pContext->SetColour(0.0f, 0.0f, 1.0f);

      const float dy = 0.03f;
      float fPositionX = 0.05f;
      float fPositionY = 0.05f;

      pContext->BeginRenderingText();

        pFont->printf(fPositionX, fPositionY += dy, "Log: %s", spitfire::logging::IsLogging() ? "on" : "off");

        const math::cVec3& position = camera.GetEyePosition();
        pFont->printf(fPositionX, fPositionY += dy, "Position: %.03f, %.03f, %.03f", position.x, position.y, position.z);

#ifdef BUILD_PHYSICS_3D
        pFont->printf(fPositionX, fPositionY += dy, "Physics Objects: %d", pWorld->GetNumberOfBodies());
#endif

        fPositionY += dy;
        pFont->printf(fPositionX, fPositionY += dy, "currentTime: %d", currentTime);

        fPositionY += dy;
        pContext->SetColour(1.0f, 0.0f, 0.0f);
        pFont->printf(fPositionX, fPositionY += dy, "fRenderFPS: %.03f", tRender.GetFPS());
        pContext->SetColour(0.0f, 1.0f, 0.0f);
        pFont->printf(fPositionX, fPositionY += dy, "fUpdateFPS: %.03f", tUpdate.GetFPS());
#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
        pContext->SetColour(0.0f, 0.0f, 1.0f);
        pFont->printf(fPositionX, fPositionY += dy, "fPhysicsFPS: %.03f", tPhysics.GetFPS());
#endif

        fPositionY += dy;
        pContext->SetColour(1.0f, 0.0f, 0.0f);
        pFont->printf(fPositionX, fPositionY += dy, "fRenderMPF: %.03f", tRender.GetMPF());
        pContext->SetColour(0.0f, 1.0f, 0.0f);
        pFont->printf(fPositionX, fPositionY += dy, "fUpdateMPF: %.03f", tUpdate.GetMPF());
#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
        pContext->SetColour(0.0f, 0.0f, 1.0f);
        pFont->printf(fPositionX, fPositionY += dy, "fPhysicsMPF: %.03f", tPhysics.GetMPF());
#endif

        fPositionX = 0.55f;
        fPositionY = 0.05f;
        const render::cStatistics& statistics = pContext->GetStatistics();
        pFont->printf(fPositionX, fPositionY += dy, "nStateChanges: %d", statistics.nStateChanges);
        pFont->printf(fPositionX, fPositionY += dy, "nVBOsBound: %d", statistics.nVertexBufferObjectsBound);
        pFont->printf(fPositionX, fPositionY += dy, "nVBOsRendered: %d", statistics.nVertexBufferObjectsRendered);
        pFont->printf(fPositionX, fPositionY += dy, "nTrianglesRendered: %d", statistics.nTrianglesRendered);
        pFont->printf(fPositionX, fPositionY += dy, "nTextureChanges: %d", pContext->uiTextureChanges);
        pFont->printf(fPositionX, fPositionY += dy, "nTextureModeChanges: %d", pContext->uiTextureModeChanges);
      pContext->EndRenderingText();
    }
#endif
  }

  void cApplication::_Render(cApplication::cAppState& state, durationms_t currentTime)
  {
    // TODO: Remove RenderScene entirely and do everything through scenegraphs

    //cHumanEyeExposureControl exposure;

    //exposure.Update(currentTime, fSceneBrightness0To1);

    render::cShaderConstants shaderConstants;
    const math::cColour& ambientColour = scenegraph.GetAmbientColour();
    const math::cVec4 ambientColourAsVec4(ambientColour.r, ambientColour.g, ambientColour.b, ambientColour.a);
    shaderConstants.SetValue(TEXT("ambientColour"), ambientColourAsVec4);

    //const math::cVec3& lightPosition = scenegraph.GetSkySystem()->GetPrimarySunPosition();
    const math::cVec3 lightPosition(100.0f, 100.0f, 100.0f);
    shaderConstants.SetValue(TEXT("lightPosition"), lightPosition);

    pContext->SetShaderConstants(shaderConstants);

    //constants.SetValue(TEXT("scene_brightness"), fSceneBrightness);
    //constants.SetValue(TEXT("eye_perceived_brightness"), exposure.GetPerceivedBrightness0To1());
    //constants.SetValue(TEXT("eye_velocity"), fEyeVelocity);cHumanEyeExposureControl exposure;

    //pContext->SetMaterial(*iter, constants);


#ifdef BUILD_DEBUG
    if (pGameUnitTest != nullptr) pGameUnitTest->PreRender(currentTime);
#endif

    // This can be overridden, allowing the user to create their own render to textures
    state.PreRender(currentTime);

    const size_t n = lPostRenderEffects.size();
    if ((n != 0) || bIsBlurPostRenderEffect || bIsHDRBloomPostRenderEffect) {
      ASSERT(pFrameBuffer0 != nullptr);
      ASSERT(pFrameBuffer1 != nullptr);

      // Ok, we actually want to do some exciting post render effects
      // If there is one rendering effect render to pFrameBuffer0 then to the screen.
      // If there is more than one rendering effect, render to pFrameBuffer0,
      // then render pFrameBuffer0 to pFrameBuffer1, pFrameBuffer1 to pFrameBuffer0 ...
      // until n-1, for the last effect we render whichever FBO we last rendered to, gets rendered to the screen.

      {
        render::cRenderToTexture texture(pFrameBuffer0);

        scenegraph.Render(currentTime, *pContext, pContext->GetFrustum());

#ifdef BUILD_DEBUG
        if (pGameUnitTest != nullptr) pGameUnitTest->RenderScene(currentTime);
#endif

        state.RenderScene(currentTime);
      }


      // HDR Bloom
      if (bIsHDRBloomPostRenderEffect) {
        ASSERT(pHDRBloomExposureFrameBuffer != nullptr);

        // First make a copy to our texture with 1x1 mipmapping
        {
          // Start rendering to the first buffer
          render::cRenderToTexture texture(pHDRBloomExposureFrameBuffer);

          {
            // Draw our texture back to the other texture
            render::cRenderScreenSpace screenspace;

            glBindTexture(GL_TEXTURE_2D, pFrameBuffer0->uiTexture);
            pContext->RenderScreenSpaceRectangleTopLeftIsAt(0.0f, 0.0f, 1.0f, 1.0f);
          }
        }

        // Now apply our HDR bloom
        {
          // Ok, let's swap the fbo pointers over so that at all times pFrameBuffer0 contains the buffer that we are about to render to or have just rendered to
          swap(pFrameBuffer0, pFrameBuffer1);

          // Start rendering to the first buffer
          render::cRenderToTexture texture(pFrameBuffer0);

          {
            // Draw our texture back to the other texture
            render::cRenderScreenSpace screenspace;

            render::ApplyMaterial apply(pHDRBloomMaterial);


            glBindTexture(GL_TEXTURE_2D, pFrameBuffer1->uiTexture);


            // For HDR bloom we need to get the exposure from the second texture
            pContext->SelectTextureUnit1();

            glBindTexture(GL_TEXTURE_2D, pHDRBloomExposureFrameBuffer->uiTexture);

            // Select the lowest mipmap level of detail, this seems to correctly select 1x1
            pHDRBloomExposureFrameBuffer->SelectMipMapLevelOfDetail(10.0f); // Also try -1.0f, +1.0f etc.

            pContext->SelectTextureUnit0();


            pContext->RenderScreenSpaceRectangleTopLeftIsAt(0.0f, 0.0f, 1.0f, 1.0f);
          }
        }
      }


      // Gaussian blur
      if (bIsBlurPostRenderEffect) {
        const float fKernelSize = 0.5f;

        // First blur horizontally (Blur will occur in the horizontal direction)
        {
          // Ok, let's swap the fbo pointers over so that at all times pFrameBuffer0 contains the buffer that we are about to render to or have just rendered to
          swap(pFrameBuffer0, pFrameBuffer1);

          // Start rendering to the first buffer
          render::cRenderToTexture texture(pFrameBuffer0);

          {
            // Draw our texture back to the other texture
            render::cRenderScreenSpace screenspace;

            render::ApplyMaterial apply(pGaussianBlurMaterial);

            pContext->SetShaderConstant("texture_width", 1024.0f);
            pContext->SetShaderConstant("texture_height", 1024.0f);
            pContext->SetShaderConstant("direction", 0.0f); // 0.0 for horizontal pass, 1.0 for vertical
            pContext->SetShaderConstant("kernel_size", fKernelSize);

            glBindTexture(GL_TEXTURE_2D, pFrameBuffer1->uiTexture);
            pContext->RenderScreenSpaceRectangleTopLeftIsAt(0.0f, 0.0f, 1.0f, 1.0f);
          }
        }

        // Now blur vertically (Blur will occur in the vertical direction)
        {
          // Ok, let's swap the fbo pointers over so that at all times pFrameBuffer0 contains the buffer that we are about to render to or have just rendered to
          swap(pFrameBuffer0, pFrameBuffer1);

          // Start rendering to the first buffer
          render::cRenderToTexture texture(pFrameBuffer0);

          {
            // Draw our texture back to the other texture
            render::cRenderScreenSpace screenspace;

            render::ApplyMaterial apply(pGaussianBlurMaterial);

            pContext->SetShaderConstant("texture_width", 1024.0f);
            pContext->SetShaderConstant("texture_height", 1024.0f);
            pContext->SetShaderConstant("direction", 1.0f); // 0.0 for horizontal pass, 1.0 for vertical
            pContext->SetShaderConstant("kernel_size", fKernelSize);

            glBindTexture(GL_TEXTURE_2D, pFrameBuffer1->uiTexture);
            pContext->RenderScreenSpaceRectangleTopLeftIsAt(0.0f, 0.0f, 1.0f, 1.0f);
          }
        }
      }


      // We have just rendered to a texture, loop through the post render chain alternating
      // rendering to pFrameBuffer0 and pFrameBuffer1
      std::list<render::material::cMaterialRef>::iterator iter = lPostRenderEffects.begin();
      for (size_t i = 1; i < n; i++, iter++) {
        // Ok, let's swap the fbo pointers over so that at all times pFrameBuffer0 contains the buffer that we are about to render to or have just rendered to
        swap(pFrameBuffer0, pFrameBuffer1);

        // Start rendering to the first buffer
        render::cRenderToTexture texture(pFrameBuffer0);

        {
          // Draw our texture back to the other texture
          render::cRenderScreenSpace screenspace;

          render::ApplyMaterial apply(*iter);
          glBindTexture(GL_TEXTURE_2D, pFrameBuffer1->uiTexture);
          pContext->RenderScreenSpaceRectangleTopLeftIsAt(0.0f, 0.0f, 1.0f, 1.0f);
        }
      }


      render::cRenderToScreen screen;

      {
        // Finally draw our texture to the screen, we don't end rendering to the screen in this function,
        // from now on in our rendering process we use exactly the same method as non-FBO rendering
        render::cRenderScreenSpace screenspace;

        // If we have one more post render effect to go then we do that now
        if (iter != lPostRenderEffects.end()) pContext->ApplyMaterial(*iter);

        glBindTexture(GL_TEXTURE_2D, pFrameBuffer0->uiTexture);
        pContext->RenderScreenSpaceRectangleTopLeftIsAt(0.0f, 0.0f, 1.0f, 1.0f);

        // Clear up our material
        if (iter != lPostRenderEffects.end()) pContext->UnApplyMaterial(*iter);

        // Now we can render any text, gui, etc. that we want to see over the top of any scene post render effects
        _RenderScreenSpaceScene(state, currentTime);
      }
    } else {
      // Normal rendering, straight to the screen
      render::cRenderToScreen screen;

      scenegraph.Render(currentTime, *pContext, pContext->GetFrustum());

#ifdef BUILD_DEBUG
      if (pGameUnitTest != nullptr) pGameUnitTest->RenderScene(currentTime);
#endif

      state.RenderScene(currentTime);

      {
        render::cRenderScreenSpace screenspace;

        _RenderScreenSpaceScene(state, currentTime);
      }
    }
  }

  void cApplication::_UpdateEvents(durationms_t currentTime)
  {
    // Handle the events in the queue
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_ACTIVEEVENT: {
          bActive = (event.active.gain != 0);
          if (bActive) {
            LOG.Success("Active", "Active");
            system.OnActivateWindow();
          } else {
            LOG.Error("Active", "Inactive");
            system.OnDeactivateWindow();
          }
          break;
        }

        case SDL_VIDEORESIZE: {
          system.OnResizeWindow(event.resize.w, event.resize.h);
          break;
        }

        case SDL_KEYUP: {
          _OnKeyUp(&event.key.keysym);
          break;
        }

        case SDL_KEYDOWN: {
          _OnKeyDown(&event.key.keysym);
          break;
        }

        case SDL_MOUSEBUTTONUP: {
          mouse.x = event.button.x;
          mouse.y = event.button.y;
          mouse.down[event.button.button] = true;
          const int button = event.button.button;
          const int state = event.button.state;
          const float x = event.button.x / float(pContext->uiWidth);
          const float y = event.button.y / float(pContext->uiHeight);
          if (!window_manager.OnMouseEvent(button, state, x, y)) GetCurrentState().OnMouseEvent(button, state, x, y);
          break;
        }

        case SDL_MOUSEBUTTONDOWN: {
          mouse.x = event.button.x;
          mouse.y = event.button.y;
          if (SDL_PRESSED == event.button.state) mouse.down[event.button.button] = true;
          const int button = event.button.button;
          const int state = event.button.state;
          const float x = event.button.x / float(pContext->uiWidth);
          const float y = event.button.y / float(pContext->uiHeight);
          if (!window_manager.OnMouseEvent(button, state, x, y)) GetCurrentState().OnMouseEvent(button, state, x, y);
          break;
        }

        case SDL_MOUSEMOTION: {
          mouse.x = event.button.x;
          mouse.y = event.button.y;
          const int button = event.button.button;
          const int state = event.button.state;
          const float x = event.button.x / float(pContext->uiWidth);
          const float y = event.button.y / float(pContext->uiHeight);
          if (!window_manager.OnMouseEvent(button, state, x, y)) GetCurrentState().OnMouseEvent(button, state, x, y);
          break;
        }

        case SDL_QUIT: {
          LOG.Success("SDL", "SDL_Quit: Quiting");
          bDone = true;
          break;
        }

        default:
          break;
      }
    }
  }

  void cApplication::RemoveKey(unsigned int code)
  {
    std::map<unsigned int, cKey* >::iterator iter = mKey.find(code);

    if (iter != mKey.end()) {
      SAFE_DELETE(iter->second);
      mKey.erase(iter);
    }
  }

  void cApplication::AddKeyRepeat(unsigned int code)
  {
    RemoveKey(code);

    mKey[code] = new cKey(code, true, true, false);
  }

  void cApplication::AddKeyNoRepeat(unsigned int code)
  {
    RemoveKey(code);

    mKey[code] = new cKey(code, false, false, false);
  }

  void cApplication::AddKeyToggle(unsigned int code)
  {
    RemoveKey(code);

    mKey[code] = new cKey(code, false, false, true);
  }

  bool cApplication::IsKeyDown(unsigned int code) const
  {
    std::map<unsigned int, cKey*>::const_iterator iter = mKey.find(code);
    const std::map<unsigned int, cKey*>::const_iterator iterEnd = mKey.end();
    if (iter != iterEnd) return iter->second->IsDown();

    return false;
  }

  void cApplication::_UpdateKeys(durationms_t currentTime)
  {
    const Uint8* key = SDL_GetKeyboardState(NULL);
    cKey* p;

    std::map<unsigned int, cKey*>::iterator iter = mKey.begin();
    const std::map<unsigned int, cKey*>::const_iterator iterEnd = mKey.end();
    while (iter != iterEnd) {
      p = (iter->second);

      //This key is pressed
      if (key[p->uiCode]) {
        //This key can be held down
        if (p->bRepeat) {
          p->bDown=true;
          p->bCollected=false;
        }
        //This key can only be pressed once
        else {
          p->bDown=false;
          p->bCollected=false;
        }
      } else p->bDown = false;

      iter++;
    }


    SDL_GetMouseState(&mouse.x, &mouse.y);
  }

  void cApplication::_OnKeyDown(SDL_keysym *keysym)
  {
    unsigned int code = keysym->sym;

    std::map<unsigned int, cKey* >::iterator iter = mKey.find(code);
    if (iter != mKey.end()) iter->second->SetDown(CONSOLE.IsVisible());

    //if (CONSOLE.IsVisible()) {
    //  // Remove key from list
    //  IsKeyDown(code);
    //
    //  if (!CONSOLE.AddKey(code)) ConsoleHide();
    //}
  }

  void cApplication::_OnKeyUp(SDL_keysym *keysym)
  {
    unsigned int code = keysym->sym;

    std::map<unsigned int, cKey* >::iterator iter = mKey.find(code);
    if (iter != mKey.end()) iter->second->SetUp(CONSOLE.IsVisible());

    //if (CONSOLE.IsVisible()) CONSOLE.AddKey(code);
  }

  void cApplication::_OnMouseUp(int button, int x, int y)
  {

  }

  void cApplication::_OnMouseDown(int button, int x, int y)
  {

  }

  void cApplication::_OnMouseMove(int button, int x, int y)
  {

  }

  void cApplication::_OnWindowEvent(const opengl::cWindowEvent& event)
  {
    LOG("");
  }

  void cApplication::_OnMouseEvent(const opengl::cMouseEvent& event)
  {
    LOG("");
  }

  void cApplication::_OnKeyboardEvent(const opengl::cKeyboardEvent& event)
  {
    LOG("");
  }

  void cApplication::_UpdateInput(durationms_t currentTime)
  {
#ifdef BUILD_DEBUG
    if (IsKeyDown(SDLK_F1)) ToggleDebug();
    if (IsKeyDown(SDLK_F2)) {
      if (spitfire::logging::IsLogging()) spitfire::logging::TurnOffLogging();
      else spitfire::logging::TurnOnLogging();
    }

    if (IsKeyDown(SDLK_F3)) pContext->bIsRenderGui = !pContext->bIsRenderGui;
    if (IsKeyDown(SDLK_F4)) pContext->bIsRenderWireframe = !pContext->bIsRenderWireframe;
    if (IsKeyDown(SDLK_F5)) pContext->bIsRenderWithShadersEnabled = pContext->GetCapabilities().bIsShadersTwoPointZeroOrLaterSupported  && !pContext->bIsRenderWithShadersEnabled;
    if (IsKeyDown(SDLK_F6)) pContext->bIsCubemappingEnabled = !pContext->bIsCubemappingEnabled;
    if (IsKeyDown(SDLK_F7)) pContext->bIsLightingEnabled = !pContext->bIsLightingEnabled;

    if (IsKeyDown(SDLK_F8)) CreateGameUnitTest();

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
    if (IsKeyDown(SDLK_F9)) bUpdatePhysics = !bUpdatePhysics;
    if (IsKeyDown(SDLK_F10)) {
      bUpdatePhysics = false;
      bStepPhysics = true;
    }
#endif

    if (IsKeyDown(SDLK_F11)) pResourceManager->ReloadTextures();
    if (IsKeyDown(SDLK_F12)) spitfire::util::RunUnitTests();
#endif

    if ((event.key.keysym.mod & (KMOD_ALT)) && IsKeyDown(SDLK_RETURN)) {
      system.OnToggleFullScreen();
    }

    if (!CONSOLE.IsVisible() && IsKeyDown(SDLK_BACKQUOTE)) {
      PushStateSoon(new cApplication::cAppStateConsole(*this));
      return;
    }


    GetCurrentState().UpdateInput(currentTime);
  }

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
  void cApplication::_UpdatePhysics(cApplication::cAppState& state, durationms_t currentTime)
  {
#ifdef BUILD_DEBUG
    if (pGameUnitTest != nullptr) pGameUnitTest->UpdatePhysics(currentTime);
#endif

    state.UpdatePhysics(currentTime);
  }
#endif

  void cApplication::CursorShow()
  {
    SDL_ShowCursor(SDL_ENABLE);
  }

  void cApplication::CursorHide()
  {
    SDL_ShowCursor(SDL_DISABLE);
  }

  void cApplication::CursorWarpToMiddleOfScreen()
  {
    SDL_WarpMouse(pContext->uiWidth / 2, pContext->uiHeight / 2);
  }

  void cApplication::ConsoleExecute(const std::string& command)
  {
    Execute(command);
  }

  //This is for executing one single line, cannot have ";"
  void cApplication::_ConsoleExecuteSingleCommand(const std::string& command)
  {
    CONSOLE<<command<<std::endl;

    std::string s(command);
    std::string full(s);
    std::vector<std::string> args;

    {
      size_t whitespace;
      std::string parameter;

      //We have a line, find the individual parts
      while(s.length())
      {
        whitespace=s.find(" ");

        if (std::string::npos==whitespace)
        {
          parameter = s;
          s = "";
        }
        else
        {
          parameter=s.substr(0, whitespace);
          s = s.substr(whitespace+1);
        }

        if (parameter.length())
          args.push_back(parameter);
      };
    }


    if (!args.empty())
    {
      if ("quit"==full || "exit"==full) bDone = true;
      else if ("var"==args[0]) var::PrintAll();
      else if (Execute(full)) {

      }

#ifdef BUILD_DEBUG
      else if (bDebug) {
        std::string c="Line: [";

        const size_t n = args.size();
        for (size_t a = 0;a < n; a++) c+="(" + args[a] + ")";

        c+="]";

        CONSOLE<<c<<std::endl;
      }
#endif
    }
  }

  /*//This is for executing multiple lines of commands, seperated by ";"
  void cApplication::ConsoleExecute(const std::string& command)
  {
    std::string s(command);
    if ("" != s)
    {
      //Take out all \n, each line should be finished with a ";" or it is not a valid line
      s=string::Replace(s, "\n", " ");

      //Split into std::strings that are all ended with ";"
      size_t endofline;
      std::string line;
      std::string parameter;

      while(s.length())
      {
        endofline = s.find(";");

        if (std::string::npos==endofline)
        {
          line = s;
          s = "";
        }
        else
        {
          line = s.substr(0, endofline);
          s = s.substr(endofline+1);
        }

        if (line.length())
          _ConsoleExecuteSingleCommand(line);
      };
    }
  }*/


  bool cApplication::Run()
  {
    LOG.Newline("Run");

    bReturnCode = InitApp();
    if (!bReturnCode) return bReturnCode;

    LOG.Newline("MainLoop");

    durationms_t currentTime = spitfire::util::GetTimeMS();

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
    size_t uiPhysicsHz = pWorld->GetFrequencyHz();
#endif
    unsigned int uiUpdateHz = 30;
    unsigned int uiRenderHz = 60;

    float fEventsDelta = 1000.0f / 30.0f; // Should be once every single loop?
    float fInputDelta = 1000.0f / 30.0f;
#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
    float fPhysicsDelta = 1000.0f / uiPhysicsHz;
#endif
    float fUpdateDelta = 1000.0f / uiUpdateHz;
    float fRenderDelta = 1000.0f / uiRenderHz;

    float fEventsNext = 0.0f;
    float fInputNext = 0.0f;
#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
    float fPhysicsNext = 0.0f;
#endif
    float fUpdateNext = 0.0f;
    float fRenderNext = 0.0f;

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
    tPhysics.InitWithLockedFPS(uiPhysicsHz);
#endif
    tUpdate.InitWithLockedFPS(uiUpdateHz);
    tRender.InitWithLockedFPS(uiRenderHz);

    //TODO: Activate window so that it is on top as soon as we start
    while (!bDone) {
      // If this fails we have a problem.  At all times we should either have
      // bDone == true OR states has one or more states
      assert(!states.empty());

      currentTime = spitfire::util::GetTimeMS();

      if (currentTime > fEventsNext) {
        _UpdateEvents(currentTime);
        fEventsNext = currentTime + fEventsDelta;
      }

      if (currentTime > fInputNext) {
        // These have to be in this order or nothing gets collected
        _UpdateInput(currentTime);
        _UpdateKeys(currentTime);
        fInputNext = currentTime + fInputDelta;
      }

      // We can change state during _UpdateInput
      if (bPopCurrentStateSoon) {
        PopState();
        continue;
      }

      if (pPushThisStateSoon != nullptr) {
        PushState(pPushThisStateSoon);
        continue;
      }

      // State is constant from here on
      cApplication::cAppState& state = GetCurrentState();

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
      if (bStepPhysics || (bUpdatePhysics && currentTime > fPhysicsNext)) {
        tPhysics.Begin();
          _UpdatePhysics(state, currentTime);
        tPhysics.End();
        fPhysicsNext = currentTime + fPhysicsDelta;
      }
#endif

      if (currentTime > fUpdateNext) {
        tUpdate.Begin();
          _Update(currentTime);
        tUpdate.End();
        fUpdateNext = currentTime + fUpdateDelta;

        //spitfire::util::cRunOnMainThreadQueue::UpdateFromMainThread();
      }

      bool bPushOrPopState = bPopCurrentStateSoon || (pPushThisStateSoon != nullptr);

      // We can change state during _Update
      if (bPopCurrentStateSoon) PopState();
      if (pPushThisStateSoon != nullptr) PushState(pPushThisStateSoon);

      // If we changed state then we can go straight to the next iteration of this loop
      if (bPushOrPopState) continue;

      // TODO: Do we need this?
      //&& (currentTime > fRenderNext)) {
      if (bActive && !bDone) {
        if (currentTime > fRenderNext) {
          tRender.Begin();
          _Render(state, currentTime);
          tRender.End();
          fRenderNext = currentTime + fRenderDelta;
        }
      }

      spitfire::util::YieldThisThread();
      //spitfire::util::SleepThisThreadMS(3);
    }


    // This is just a sanity check
    bDone = false;


    // Get rid of any states that we do have as they may try and operate on destructed/destructing objects later on
    while (!states.empty()) states.pop_back();

    LOG.Newline("DestroyScene");
    DestroyScene();

    LOG.Newline("DestroyApp");
    DestroyApp();

    return bReturnCode;
  }



  // *** cConsoleWindow
  cApplication::cConsoleWindow::cConsoleWindow() :
    gui::cModelessWindow(breathe::gui::GenerateID(), 0.05f, 0.05f, 0.4f, 0.4f, LANG("L__Console"), nullptr),
    pPrevious(nullptr),
    pInput(nullptr)
  {
  }

  void cApplication::cConsoleWindow::InitConsoleWindow()
  {
    SetVisible(false);
    SetResizable(true);

    const math::cColour red(1.0f, 0.0f, 0.0f);
    pPrevious = new gui::cWidget_StaticText(breathe::gui::GenerateID(), 0.05f, 0.05f, 0.40f, 0.9f);
    pPrevious->SetColour(red);

    const math::cColour green(0.0f, 1.0f, 0.0f);
    pInput = new gui::cWidget_Input(breathe::gui::GenerateID(), 0.55f, 0.05f, 0.40f, 0.9f);
    pInput->SetColour(green);

    AddChildToContainer(pPrevious);
    AddChildToContainer(pInput);
  }

  void cApplication::cConsoleWindow::_OnEvent(const gui::cEvent& event)
  {
    LOG("");
  }


  // *** cKey
  cApplication::cKey::cKey(unsigned int code, bool variable, bool repeat, bool toggle) :
    bVariable(variable),
    bRepeat(repeat),
    bToggle(toggle),

    uiCode(code),

    bDown(false),
    bCollected(false)
  {
  }

  bool cApplication::cKey::IsDown() const
  {
    if (bDown) {
      // If we are repeating this key or we have not collected it before then set ourselves to collected and return true
      if (bRepeat || !bCollected) {
        bCollected = true;
        return true;
      }

      bDown = false;
      bCollected = true;
    }

    return false;
  }

  void cApplication::cKey::SetDown(bool bConsole)
  {
    bDown = true;
    bCollected = false;
  }

  void cApplication::cKey::SetUp(bool bConsole)
  {
    bDown = false;
    bCollected = false;

    /*if (bRepeat)
    {
      bDown = false;
      bCollected = false;
    }
    else if (!bConsole)
    {
      bDown = true;
      bCollected = false;
    }*/
  }



  bool cApplication::IsMouseLeftButtonDown() const
  {
    return mouse.down[0];
  }

  bool cApplication::IsMouseRightButtonDown() const
  {
    return mouse.down[2];
  }

  bool cApplication::IsMouseScrollDown() const
  {
    return mouse.down[4];
  }

  bool cApplication::IsMouseScrollUp() const
  {
    return mouse.down[5];
  }

  int cApplication::GetMouseX() const
  {
    return mouse.GetY();
  }

  int cApplication::GetMouseY() const
  {
    return mouse.GetX();
  }

  cApplication::cAppState& cApplication::GetCurrentState() const
  {
    assert(!states.empty());
    return *states.back();
  }

  void cApplication::PushState(cApplication::cAppState* state)
  {
    assert(state != nullptr);

    bPopCurrentStateSoon = false;
    pPushThisStateSoon = nullptr;

    if (!states.empty()) {
      cAppState* pParent = states.back();
      pParent->OnPause();
      state->SetParent(pParent);
    }

    states.push_back(state);

    CONSOLE<<"cApplication::PushState States="<<states.size()<<std::endl;

    state->OnEntry();
  }

  void cApplication::PopState()
  {
    assert(!states.empty());

    bPopCurrentStateSoon = false;

    cAppState* pTemp = states.back();
    pTemp->OnExit();
    int iResult = pTemp->GetResult();
    SAFE_DELETE(pTemp);

    states.pop_back();

    CONSOLE<<"cApplication::PopState States="<<states.size()<<std::endl;

    if (states.empty()) bDone = true;
    else GetCurrentState().OnResume(iResult);
  }




  // *** cAppStateConsole

  void breathe::cApplication::cAppStateConsole::_OnEntry()
  {
    pConsoleWindow = new cConsoleWindow;
    app.window_manager.AddChild(pConsoleWindow);
    pConsoleWindow->InitConsoleWindow();

    pConsoleWindow->SetVisible(true);
    CONSOLE.Show();
    app.CursorShow();
  }

  void breathe::cApplication::cAppStateConsole::_OnExit()
  {
    assert(pConsoleWindow != nullptr);
    pConsoleWindow->SetVisible(false);
    app.window_manager.RemoveChild(pConsoleWindow);
    CONSOLE.Hide();
    app.CursorHide();
  }

  // Left and right shifts the cursor on the current typing in line
  // Page up and down scrolls the view up and down
  // Up and down shows history
  // Auto complete with tab
  // ma<tab>
  // map <space has been inserted too>
  // if more than one option then show all options as in linux

  void breathe::cApplication::cAppStateConsole::_Update(breathe::durationms_t currentTime)
  {
    std::string s;
    /*breathe::constant_stack<std::string>::iterator iter = CONSOLE.begin();
    breathe::constant_stack<std::string>::iterator iterEnd = CONSOLE.end();

    while (iter != iterEnd) {
    s.append((*iter) + "\n");
    iter++;
    }*/

    s = "This is the console";
    pConsoleWindow->GetPrevious().SetText(breathe::string::ToString_t(s));
  }

  void breathe::cApplication::cAppStateConsole::_UpdateInput(breathe::durationms_t currentTime)
  {
    if (app.IsKeyDown(SDLK_BACKQUOTE) || app.IsKeyDown(SDLK_ESCAPE)) {
      app.PopStateSoon();
      return;
    }
  }
}

#endif

