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


#include <cmath>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <iostream>
#include <fstream>
#include <sstream>

#include <list>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <bitset>

// Boost includes
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/asio.hpp>

//FreeType Headers
#include <freetype/ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

// OpenGL headers
#include <GL/GLee.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>


// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>
#include <spitfire/util/cTimer.h>
#include <spitfire/util/unittest.h>
#include <spitfire/util/lang.h>
#include <spitfire/util/process.h>
#include <spitfire/util/thread.h>

#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/xml.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/geometry.h>

#include <spitfire/communication/network.h>
#include <spitfire/communication/uri.h>
#include <spitfire/communication/http.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/util/app.h>
#include <breathe/util/cVar.h>
#include <breathe/util/base.h>

#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>
#include <breathe/render/cFont.h>

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
#include <breathe/physics/physics.h>
#endif

#ifdef BUILD_LEVEL
#include <breathe/game/cLevel.h>
#endif

#include <breathe/game/scenegraph.h>
#include <breathe/game/skysystem.h>

#include <breathe/audio/audio.h>

/*
Automatic updates:
Settings:
Radio buttons:
  () Check for updates
  () Don''t check for updates

If an update is found:
Radio buttons:
  () Download and apply it automatically
  () Ask me if I would like to download and apply it

  At program startup check for updates.
  If an update is found start downloading it.


  <require_package name="shared" version="1"/>

  <add_folder name="data"/>
  <add_file name="data/hello.txt"/>

  <remove_file name="data/hello.txt"/>
  <remove_folder_if_empty name="data"/>
*/


namespace breathe
{
  class cApplicationUpdaterListener;

  class cApplicationUpdater : protected util::cThread
  {
  public:
    enum RESULT
    {
      // Generic
      RESULT_UNKNOWN = 0,
      RESULT_CONNECTION_FAILED,

      // Checking version only
      RESULT_VERSION_IS_UP_TO_DATE,
      RESULT_VERSION_IS_OUT_OF_DATE,

      // Downloading an update only
      RESULT_DOWNLOAD_FAILED,
      RESULT_DOWNLOAD_COMPLETE
    };

    cApplicationUpdater();

    void SetListener(cApplicationUpdaterListener* pListener);

    // This starts a thread that downloads the file from the server and checks the result to see if there is a new version
    // If there is a new version then pListener will be notified
    void CheckIfUpdateIsAvailableInBackground();

    // This just copies this executable to a new folder in the temp directory and places a .txt file there with the application location there
    void DownloadUpdateInBackground();

  private:
    RESULT CheckIfUpdateIsAvailable();
    RESULT DownloadUpdate();

    void _ThreadFunction();

    util::cMutex mutex;
    bool bModeIsCheckIfUpdateIsAvailable; // This is for telling which mode we are in within the thread
    cApplicationUpdaterListener* pListener;
  };

  cApplicationUpdater::cApplicationUpdater() :
    pListener(nullptr)
  {
  }

  void cApplicationUpdater::SetListener(cApplicationUpdaterListener* _pListener)
  {
    util::cLockObject lock(mutex);
    pListener = _pListener;
  }

  void cApplicationUpdater::CheckIfUpdateIsAvailableInBackground()
  {
    if (IsRunning()) cThread::StopNow();

    cThread::Run();
  }

  /*cApplicationUpdater::RESULT cApplicationUpdater::CheckIfUpdateIsAvailable()
  {
    // Connect
    network::cDownloadHTTP download;

    download.Open();
    if (!download.IsConnected()) return RESULT_CONNECTION_FAILED;

    // It is only expected to be a small file so even 128 will probably do
    char buffer[128];

    while (download.IsConnected()) {
      download.Read(buffer);

      TODO: Parse the file here
      {
        return RESULT_THERE_IS_A_NEW_VERSION_AVAILABLE;
      }

      util::YieldThisThread();
    };

    return RESULT_VERSION_IS_OUT_OF_DATE;
  }

  cApplicationUpdater::RESULT cApplicationUpdater::DownloadUpdate()
  {
    // Connect
    cConnectionTCP connection;

    connection.Open();
    if (!connection.IsConnected()) return RESULT_CONNECTION_FAILED;

    uint8_t buffer[1024];

    while (connection.IsConnected()) {
      connection.Read(buffer);

      Download the file here
      {
        return RESULT_DOWNLOAD_COMPLETE;
      }

      Sleep();
    };

    return RESULT_DOWNLOAD_FAILED;
  }

  void cApplicationUpdater::_ThreadFunction()
  {
    LOG<<"cApplicationUpdater::_ThreadFunction"<<std::endl;

    // Get the listener and the mode
    cApplicationUpdaterListener* pTempListener = nullptr;
    bool bTempModeIsCheckIfUpdateIsAvailable = true;
    {
      util::cLockObject lock(mutex);
      pTempListener = pListener;
      bTempModeIsCheckIfUpdateIsAvailable = bModeIsCheckIfUpdateIsAvailable;
    }
    ASSERT(pTempListener != nullptr);

    RESULT result = RESULT_UNKNOWN;
    if (bTempModeIsCheckIfUpdateIsAvailable) result = CheckIfUpdateIsAvailable();
    else result = DownloadUpdate();

    // Notify the listener
    if (bTempModeIsCheckIfUpdateIsAvailable) pTempListener->_OnUpdateThreadFinished(result);
    else pTempListener->_OnDownloadThreadFinished(result);

    LOG<<"cApplicationUpdater::_ThreadFunction returning"<<std::endl;
  }


  class cApplicationUpdaterListener
  {
  public:
    virtual ~cApplicationUpdaterListener() {}

    void OnUpdateThreadFinished(cApplicationUpdater::RESULT result) { _OnUpdateThreadFinished(result); }
    void OnDownloadThreadFinished(cApplicationUpdater::RESULT result) { _OnDownloadThreadFinished(result); }

  private:
    virtual void _OnUpdateThreadFinished(cApplicationUpdater::RESULT result) = 0;
    virtual void _OnDownloadThreadFinished(cApplicationUpdater::RESULT result) = 0;
  };

  class cConsoleApplicationUpdateListener : public cApplicationUpdateListener
  {
  public:
    cConsoleApplicationUpdateListener() : bIsVersionOutOfDate(true) {}

    bool IsVersionOutOfDate() const { return result == cApplicationUpdater::RESULT_VERSION_IS_OUT_OF_DATE; }

  private:
    void _OnUpdateThreadFinished(cApplicationUpdater::RESULT _result) { result = _result; }
    void _OnDownloadThreadFinished(cApplicationUpdater::RESULT _result) { result = _result; }

    cApplicationUpdater::RESULT result;
  };



  void CheckIfNeedToUpdateApplication()
  {
    if (check for newer version setting is true) {
      cConsoleApplicationUpdateListener listener;
      cApplicationUpdater updater;

      listener.ClearSuccess();
      updater.SetListener(&listener);

      updater.CheckIfUpdateIsAvailableInBackground();

      std::cout<<"Checking for updates"<<std::endl;
      while (updater.IsRunning()) {
        std::cout<<".";
        util::PauseThisThread(1000);
      };

      std::cout<<std::endl;

      if (listener.IsVersionOutOfDate()) {
        std::cout<<"The current version is out of date"<<std::endl;

        if (download the latest version automatically setting is true)
            // TODO:
            // Download the latest version to /tmp
            // Extract the files to a folder within /tmp
            // Run the extracted executable

            // We can now safely return, the newly extracted executable is running and will install itself
          return;
      }
    } else std::cout<<"The current version is up to date"<<std::endl;
  }
  */

  render::material::cMaterialRef pGaussianBlurMaterial;
  render::material::cMaterialRef pHDRBloomMaterial;

  cApplication::cApplication(int argc, const char* const* argv) :
#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
    bStepPhysics(false),
    bUpdatePhysics(true),
#endif
    bDone(false),
    pFont(nullptr),

    pWorld(nullptr),

#ifdef BUILD_DEBUG
    bDebug(true),
#endif

    bIsBlurPostRenderEffect(false),
    bIsHDRBloomPostRenderEffect(false),

    bActive(true),
    bReturnCode(breathe::GOOD),

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

    // Set our main thread identifier so that when we call IsMainThread later it will work correctly
    spitfire::util::SetMainThread();
    spitfire::util::cRunOnMainThreadQueue::Create();

#ifdef BUILD_DEBUG
    SanityCheck();
#endif

    filesystem::Create();

    string_t sFullPath;
    if (filesystem::GetFile(TEXT("Current"), sFullPath)) LOG<<"File exists at "<<breathe::string::ToUTF8(sFullPath).c_str()<<std::endl;
    else LOG<<"File does not exists"<<std::endl;

    _LoadSearchDirectories();

    spitfire::util::LoadLanguageFiles();

    _InitArguments(argc, argv);

    srand(time(NULL));

    LOG<<"This is printed to the log"<<std::endl;
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

    pRender = new render::cRender();

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
    breathe::audio::Destroy();


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
    breathe::physics::Destroy();
    pWorld = nullptr;
#endif

    LOG.Success("Destroy", "Network");
    breathe::network::Destroy();

    LOG.Success("Delete", "Render");
    SAFE_DELETE(pRender);

    LOG.Success("Delete", "Log");
    // Log is not actually deleted here, we were previously doing stuff like sending stats to the log

    LOG.Success("Delete", "Filesystem");
    filesystem::Destroy();

    spitfire::util::cRunOnMainThreadQueue::Destroy();

    LOG.Success("Main", "Successfully exited");
    LOG.Newline("Main", "return " + bReturnCode ? "true" : "false");

    SDL_Quit();
  }

#ifdef BUILD_DEBUG
  void cApplication::SanityCheck()
  {
    ASSERT(sizeof(int) == 4);
    ASSERT(sizeof(float) == 4);
    // This test fails because on gcc/linux mathdefs.h defines float_t as double
    //ASSERT(sizeof(float_t) == sizeof(float));
    ASSERT(sizeof(double) >= 4);

    ASSERT(sizeof(int8_t) == 1);
    ASSERT(sizeof(uint8_t) == 1);
    ASSERT(sizeof(uint16_t) == 2);
    ASSERT(sizeof(int16_t) == 2);
    ASSERT(sizeof(int32_t) == 4);
    ASSERT(sizeof(uint32_t) == 4);
    ASSERT(sizeof(int64_t) == 8);
    ASSERT(sizeof(uint64_t) == 8);

    ASSERT(sizeof(char) == 1);
#ifdef __WIN__
    ASSERT(sizeof(wchar_t) == 2);
#else
    ASSERT(sizeof(wchar_t) == 4);
#endif
    ASSERT(sizeof(char16_t) == 2);
    ASSERT(sizeof(char32_t) == 4);

    // Smart pointers that have not been initialised should point to nullptr
    cSmartPtr<int> a;
    ASSERT(a == nullptr);
  }
#endif

  class cDownloadUpdateListener : public network::http::cDownloadListener
  {
  public:

  private:
    void _OnTextContentReceived(const std::string& sContent);
  };

  void cDownloadUpdateListener::_OnTextContentReceived(const std::string& sContent)
  {
    LOG<<"Content: \""<<sContent<<"\""<<std::endl;
  }

  void cApplication::CheckIfNeedToUpdateApplication()
  {
    LOG<<"cApplication::CheckIfNeedToUpdateApplication"<<std::endl;

    network::http::cDownloadHTTP download;
    cDownloadUpdateListener listener;

    download.Download("http://chris.iluo.net/projects/drive/info.php", network::http::METHOD::GET, listener);

    LOG<<"cApplication::CheckIfNeedToUpdateApplication returning"<<std::endl;


    // http://chris.iluo.net/projects/drive/get_state.php
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
        LOG<<"Adding Directory "<<sDirectory<<std::endl;
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

  void cApplication::LoadConfigXML()
  {
    LOG.Success("Init", "Loading config.xml");

    breathe::xml::cNode root(TEXT("config.xml"));
    breathe::xml::cNode::iterator iter(root);

    if (!iter.IsValid()) {
      bReturnCode=breathe::BAD;
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
        pRender->uiWidth = uiValue;
      }

      if (iter.GetAttribute("height", uiValue)) {
        t.str("");
        t<<"height = ";
        t<<uiValue;
        LOG.Success("Config", t.str());
        pRender->uiHeight = uiValue;
      }

      if (iter.GetAttribute("depth", uiValue)) {
        t.str("");
        t<<"depth = ";
        t<<uiValue;
        LOG.Success("Config", t.str());
        pRender->uiDepth = uiValue;
      }

      bool bFullscreen;

      if (iter.GetAttribute("fullscreen", bFullscreen)) {
        LOG.Success("Config", std::string("fullscreen = ") + (bFullscreen ? "true" : "false"));
        pRender->bFullscreen = bFullscreen;
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
    LoadConfigXML();

    // Init SDL
    if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0) {
      LOG.Error("SDL", std::string("SDL_Init for video FAILED error=") + SDL_GetError());
      bReturnCode = breathe::BAD;
      return breathe::BAD;
    }

    // Enable unicode
    SDL_EnableUNICODE(1);

    // Enable key repeat
    SDL_EnableKeyRepeat(200, 20);

    // Set our window caption and possibly the icon as well
    SDL_WM_SetCaption(breathe::string::ToUTF8(LANG("L_Application")).c_str(), "app.ico");

    {
      // Joysticks

      if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0) {
        LOG.Error("SDL", std::string("SDL_InitSubSystem for joysticks FAILED error=") + SDL_GetError());
        bReturnCode = breathe::BAD;
        return breathe::BAD;
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
          t << SDL_JoystickName(i);
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

    if (breathe::BAD == InitRender()) return breathe::BAD;

    TTF_Init();

    audio::Init(audio::DRIVER::DRIVER_SDLMIXER);

#if defined(BUILD_PHYSICS_2D)
    physics::Init(physics::DRIVER::DRIVER_BOX2D, physics_width, physics_height);
#elif defined(BUILD_PHYSICS_BULLET)
    physics::Init(physics::DRIVER::DRIVER_BULLET, physics_width, physics_depth, physics_height);
#elif defined(BUILD_PHYSICS_ODE)
    physics::Init(physics::DRIVER::DRIVER_ODE, physics_width, physics_depth, physics_height);
#endif
    pWorld = physics::GetWorld();

    if (breathe::BAD == LoadScene()) return breathe::BAD;


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

    if (breathe::BAD == InitScene()) return breathe::BAD;


    pGaussianBlurMaterial = pRender->AddMaterial(TEXT("postrender/gaussian_blur_two_pass.mat"));
    ASSERT(pGaussianBlurMaterial != nullptr);

    pHDRBloomMaterial = pRender->AddMaterial(TEXT("postrender/bloom.mat"));
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

    return breathe::GOOD;
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

    return breathe::GOOD;
  }

  bool cApplication::InitRender()
  {
    if (breathe::BAD == pRender->PreInit()) {
      bReturnCode = breathe::BAD;
      return breathe::BAD;
    }

    pRender->SetPerspective();

    if (breathe::BAD == pRender->Init()) return breathe::BAD;

    return breathe::GOOD;
  }

  bool cApplication::DestroyRender()
  {
    LOG.Success("Delete", "Frame Buffer Objects");
    pFrameBuffer0.reset();
    pFrameBuffer1.reset();

    pHDRBloomExposureFrameBuffer.reset();

    pRender->Destroy();

    return breathe::GOOD;
  }

  bool cApplication::ToggleFullscreen()
  {
    // Destroy the old render
    if (breathe::BAD == DestroyRender()) {
      bReturnCode=breathe::BAD;
      return breathe::BAD;
    }

    // Toggle fullscreen
    pRender->ToggleFullscreen();


    // Create the new render
    if (breathe::BAD == InitRender()) {
      bReturnCode=breathe::BAD;
      return breathe::BAD;
    }

    pRender->ReloadTextures();

    return breathe::GOOD;
  }

  bool cApplication::ResizeWindow(unsigned int w, unsigned int h)
  {
    DestroyRender();

    pRender->uiWidth = w;
    pRender->uiHeight = h;

    InitRender();
    pRender->ReloadTextures();

    return breathe::GOOD;
  }

#ifdef BUILD_DEBUG
  void cApplication::CreateGameUnitTest()
  {
    LOG<<"cApplication::CreateGameUnitTest"<<std::endl;

    // If we have already created our game unittest then we can reuse it
    if (pGameUnitTest != nullptr) return;

    pGameUnitTest = new cGameUnitTest(*this);

    pGameUnitTest->Init();
  }
#endif

  void cApplication::_Update(sampletime_t currentTime)
  {
#ifdef BUILD_DEBUG
    if (pGameUnitTest != nullptr) pGameUnitTest->Update(currentTime);

    pFPSRenderGraph->AddPoint(tRender.GetMPF());
    pFPSUpdateGraph->AddPoint(tUpdate.GetMPF());
    pFPSPhysicsGraph->AddPoint(tPhysics.GetMPF());
#endif

    // Update our current state
    GetCurrentState().Update(currentTime);

    // NOTE: The current state needs to have set this camera correctly by now
    pRender->SetFrustum(camera.CreateFrustumFromCamera());

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
    render::material::cMaterialRef pMaterial = pRender->AddMaterial(sFilename);
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


  void cApplication::_RenderScreenSpaceScene(cApplication::cAppState& state, sampletime_t currentTime)
  {
#ifdef BUILD_DEBUG
    if (pGameUnitTest != nullptr) pGameUnitTest->RenderScreenSpace(currentTime);
#endif


    scenegraph2D.Render(currentTime);

    state.RenderScreenSpace(currentTime);

    if (pRender->bIsRenderGui) window_manager.Render();

#ifdef BUILD_DEBUG
    if (IsDebug() && !CONSOLE.IsVisible()) {
      pRender->SetColour(0.0f, 0.0f, 1.0f);

      const float dy = 0.03f;
      float fPosition = 0.1f;

      pRender->BeginRenderingText();

        pFont->printf(0.05f, fPosition += dy, "Log: %s", spitfire::logging::IsLogging() ? "on" : "off");

        const math::cVec3& position = camera.GetEyePosition();
        pFont->printf(0.05f, fPosition += dy, "Position: %.03f, %.03f, %.03f", position.x, position.y, position.z);

#ifdef BUILD_PHYSICS_3D
        pFont->printf(0.05f, fPosition += dy, "Physics Objects: %d", pWorld->GetNumberOfBodies());
#endif

        fPosition += dy;
        pFont->printf(0.05f, fPosition += dy, "uiTriangles: %d", pRender->uiTriangles);
        pFont->printf(0.05f, fPosition += dy, "uiTextureChanges: %d", pRender->uiTextureChanges);
        pFont->printf(0.05f, fPosition += dy, "uiTextureModeChanges: %d", pRender->uiTextureModeChanges);

        fPosition += dy;
        pFont->printf(0.05f, fPosition += dy, "currentTime: %d", currentTime);

        fPosition += dy;
        pRender->SetColour(1.0f, 0.0f, 0.0f);
        pFont->printf(0.05f, fPosition += dy, "fRenderFPS: %.03f", tRender.GetFPS());
        pRender->SetColour(0.0f, 1.0f, 0.0f);
        pFont->printf(0.05f, fPosition += dy, "fUpdateFPS: %.03f", tUpdate.GetFPS());
#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
        pRender->SetColour(0.0f, 0.0f, 1.0f);
        pFont->printf(0.05f, fPosition += dy, "fPhysicsFPS: %.03f", tPhysics.GetFPS());
#endif

        fPosition += dy;
        pRender->SetColour(1.0f, 0.0f, 0.0f);
        pFont->printf(0.05f, fPosition += dy, "fRenderMPF: %.03f", tRender.GetMPF());
        pRender->SetColour(0.0f, 1.0f, 0.0f);
        pFont->printf(0.05f, fPosition += dy, "fUpdateMPF: %.03f", tUpdate.GetMPF());
#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
        pRender->SetColour(0.0f, 0.0f, 1.0f);
        pFont->printf(0.05f, fPosition += dy, "fPhysicsMPF: %.03f", tPhysics.GetMPF());
#endif
      pRender->EndRenderingText();
    }
#endif
  }

  void cApplication::_Render(cApplication::cAppState& state, sampletime_t currentTime)
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

    pRender->SetShaderConstants(shaderConstants);

    //constants.SetValue(TEXT("scene_brightness"), fSceneBrightness);
    //constants.SetValue(TEXT("eye_perceived_brightness"), exposure.GetPerceivedBrightness0To1());
    //constants.SetValue(TEXT("eye_velocity"), fEyeVelocity);cHumanEyeExposureControl exposure;

    //pRender->SetMaterial(*iter, constants);


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

        scenegraph.Render(currentTime, pRender->GetFrustum());

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
            pRender->RenderScreenSpaceRectangleTopLeftIsAt(0.0f, 0.0f, 1.0f, 1.0f);
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
            pRender->SelectTextureUnit1();

            glBindTexture(GL_TEXTURE_2D, pHDRBloomExposureFrameBuffer->uiTexture);

            // Select the lowest mipmap level of detail, this seems to correctly select 1x1
            pHDRBloomExposureFrameBuffer->SelectMipMapLevelOfDetail(10.0f); // Also try -1.0f, +1.0f etc.

            pRender->SelectTextureUnit0();


            pRender->RenderScreenSpaceRectangleTopLeftIsAt(0.0f, 0.0f, 1.0f, 1.0f);
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

            pRender->SetShaderConstant("texture_width", 1024.0f);
            pRender->SetShaderConstant("texture_height", 1024.0f);
            pRender->SetShaderConstant("direction", 0.0f); // 0.0 for horizontal pass, 1.0 for vertical
            pRender->SetShaderConstant("kernel_size", fKernelSize);

            glBindTexture(GL_TEXTURE_2D, pFrameBuffer1->uiTexture);
            pRender->RenderScreenSpaceRectangleTopLeftIsAt(0.0f, 0.0f, 1.0f, 1.0f);
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

            pRender->SetShaderConstant("texture_width", 1024.0f);
            pRender->SetShaderConstant("texture_height", 1024.0f);
            pRender->SetShaderConstant("direction", 1.0f); // 0.0 for horizontal pass, 1.0 for vertical
            pRender->SetShaderConstant("kernel_size", fKernelSize);

            glBindTexture(GL_TEXTURE_2D, pFrameBuffer1->uiTexture);
            pRender->RenderScreenSpaceRectangleTopLeftIsAt(0.0f, 0.0f, 1.0f, 1.0f);
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
          pRender->RenderScreenSpaceRectangleTopLeftIsAt(0.0f, 0.0f, 1.0f, 1.0f);
        }
      }


      render::cRenderToScreen screen;

      {
        // Finally draw our texture to the screen, we don't end rendering to the screen in this function,
        // from now on in our rendering process we use exactly the same method as non-FBO rendering
        render::cRenderScreenSpace screenspace;

        // If we have one more post render effect to go then we do that now
        if (iter != lPostRenderEffects.end()) pRender->ApplyMaterial(*iter);

        glBindTexture(GL_TEXTURE_2D, pFrameBuffer0->uiTexture);
        pRender->RenderScreenSpaceRectangleTopLeftIsAt(0.0f, 0.0f, 1.0f, 1.0f);

        // Clear up our material
        if (iter != lPostRenderEffects.end()) pRender->UnApplyMaterial(*iter);

        // Now we can render any text, gui, etc. that we want to see over the top of any scene post render effects
        _RenderScreenSpaceScene(state, currentTime);
      }
    } else {
      // Normal rendering, straight to the screen
      render::cRenderToScreen screen;

      scenegraph.Render(currentTime, pRender->GetFrustum());

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

  void cApplication::_UpdateEvents(sampletime_t currentTime)
  {
    // Handle the events in the queue
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_ACTIVEEVENT: {
            bActive=event.active.gain != 0;
            if (bActive) LOG.Success("Active", "Active");
            else LOG.Error("Active", "Inactive");
          break;
        }

        case SDL_VIDEORESIZE: {
          if (breathe::BAD == ResizeWindow(event.resize.w, event.resize.h))
            bDone=true;
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
          const float x = event.button.x / float(pRender->uiWidth);
          const float y = event.button.y / float(pRender->uiHeight);
          if (!window_manager.OnMouseEvent(button, state, x, y)) GetCurrentState().OnMouseEvent(button, state, x, y);
          break;
        }

        case SDL_MOUSEBUTTONDOWN: {
          mouse.x = event.button.x;
          mouse.y = event.button.y;
          if (SDL_PRESSED == event.button.state) mouse.down[event.button.button] = true;
          const int button = event.button.button;
          const int state = event.button.state;
          const float x = event.button.x / float(pRender->uiWidth);
          const float y = event.button.y / float(pRender->uiHeight);
          if (!window_manager.OnMouseEvent(button, state, x, y)) GetCurrentState().OnMouseEvent(button, state, x, y);
          break;
        }

        case SDL_MOUSEMOTION: {
          mouse.x = event.button.x;
          mouse.y = event.button.y;
          const int button = event.button.button;
          const int state = event.button.state;
          const float x = event.button.x / float(pRender->uiWidth);
          const float y = event.button.y / float(pRender->uiHeight);
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

  void cApplication::_UpdateKeys(sampletime_t currentTime)
  {
    Uint8 *key = SDL_GetKeyState( NULL );
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


  void cApplication::_UpdateInput(sampletime_t currentTime)
  {
#ifdef BUILD_DEBUG
    if (IsKeyDown(SDLK_F1)) ToggleDebug();
    if (IsKeyDown(SDLK_F2)) {
      if (spitfire::logging::IsLogging()) spitfire::logging::TurnOffLogging();
      else spitfire::logging::TurnOnLogging();
    }

    if (IsKeyDown(SDLK_F3)) pRender->bIsRenderGui = !pRender->bIsRenderGui;
    if (IsKeyDown(SDLK_F4)) pRender->bIsRenderWireframe = !pRender->bIsRenderWireframe;
    if (IsKeyDown(SDLK_F5)) pRender->bIsRenderWithShadersEnabled = pRender->bIsShaderSupported && !pRender->bIsRenderWithShadersEnabled;
    if (IsKeyDown(SDLK_F6)) pRender->bIsCubemappingEnabled = !pRender->bIsCubemappingEnabled;
    if (IsKeyDown(SDLK_F7)) pRender->bIsLightingEnabled = !pRender->bIsLightingEnabled;

    if (IsKeyDown(SDLK_F8)) CreateGameUnitTest();

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
    if (IsKeyDown(SDLK_F9)) bUpdatePhysics = !bUpdatePhysics;
    if (IsKeyDown(SDLK_F10)) {
      bUpdatePhysics = false;
      bStepPhysics = true;
    }
#endif

    if (IsKeyDown(SDLK_F11)) pRender->ReloadTextures();
    if (IsKeyDown(SDLK_F12)) spitfire::util::RunUnitTests();
#endif

    if ((event.key.keysym.mod & (KMOD_ALT)) && IsKeyDown(SDLK_RETURN)) ToggleFullscreen();

    if (!CONSOLE.IsVisible() && IsKeyDown(SDLK_BACKQUOTE)) {
      PushStateSoon(new cApplication::cAppStateConsole(*this));
      return;
    }


    GetCurrentState().UpdateInput(currentTime);
  }

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
  void cApplication::_UpdatePhysics(cApplication::cAppState& state, sampletime_t currentTime)
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
    SDL_WarpMouse(pRender->uiWidth / 2, pRender->uiHeight / 2);
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
      else if (GOOD==Execute(full))
      {

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
    if (bReturnCode == BAD) return bReturnCode;

    LOG.Newline("MainLoop");

    sampletime_t currentTime = spitfire::util::GetTimeMS();

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
    size_t uiPhysicsHz = pWorld->GetFrequencyHz();
#endif
    unsigned int uiUpdateHz = 30;

    float fEventsDelta = 1000.0f / 30.0f; // Should be once every single loop?
    float fInputDelta = 1000.0f / 30.0f;
#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
    float fPhysicsDelta = 1000.0f / uiPhysicsHz;
#endif
    float fUpdateDelta = 1000.0f / uiUpdateHz;

    float fEventsNext = 0.0f;
    float fInputNext = 0.0f;
#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
    float fPhysicsNext = 0.0f;
#endif
    float fUpdateNext = 0.0f;

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
    tPhysics.InitWithLockedFPS(uiPhysicsHz);
#endif
    tUpdate.InitWithLockedFPS(uiUpdateHz);
    tRender.InitWithUnspecifiedFPS();

    //TODO: Activate window so that it is on top as soon as we start
    while (!bDone) {
      // If this fails we have a problem.  At all times we should either have
      // bDone == true or states has one or more states
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
        tRender.Begin();
          _Render(state, currentTime);
        tRender.End();
      }

      breathe::util::YieldThisThread();
    } while (!bDone);

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

    math::cColour green(0.0f, 1.0f, 0.0f);

    pPrevious = new gui::cWidget_StaticText(breathe::gui::GenerateID(), 0.05f, 0.05f, 0.9f, 0.9f);
    pPrevious->SetColour(green);
    pInput = new gui::cWidget_Input(breathe::gui::GenerateID(), 0.05f, 0.05f, 0.9f, 0.9f);
    pInput->SetColour(green);

    AddChild(pPrevious);
    AddChild(pInput);
  }

  void cApplication::cConsoleWindow::_OnEvent(const gui::cEvent& event)
  {
    std::cout<<"cApplication::cConsoleWindow::_OnEvent"<<std::endl;
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

  void breathe::cApplication::cAppStateConsole::_Update(breathe::sampletime_t currentTime)
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

  void breathe::cApplication::cAppStateConsole::_UpdateInput(breathe::sampletime_t currentTime)
  {
    if (app.IsKeyDown(SDLK_BACKQUOTE) || app.IsKeyDown(SDLK_ESCAPE)) {
      app.PopStateSoon();
      return;
    }
  }
}
