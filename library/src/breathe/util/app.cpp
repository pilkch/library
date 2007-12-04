/*

antiportal: A polygon that is totally opaque, not rendered, but hiding models behind it, like in Farcry.  
Can be walls that hide players etc. behind them. 

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
#define CROSS(dest,v1,v2) \
	        dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
          dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
          dest[2]=v1[0]*v2[1]-v1[1]*v2[0];

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2) \
          dest[0]=v1[0]-v2[0]; \
          dest[1]=v1[1]-v2[1]; \
          dest[2]=v1[2]-v2[2];

int intersect_triangle(
			double orig[3], double dir[3],
			double vert0[3], double vert1[3], double vert2[3],
			double *t, double *u, double *v)
{
   double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
   double det,inv_det;

   // find vectors for two edges sharing vert0 

   SUB(edge1, vert1, vert0);
   SUB(edge2, vert2, vert0);

   // begin calculating determinant - also used to calculate U parameter 
   CROSS(pvec, dir, edge2);

   // if determinant is near zero, ray lies in plane of triangle 
   det = DOT(edge1, pvec);

#ifdef TEST_CULL           // define TEST_CULL if culling is desired 
   if (det < EPSILON)
      return 0;

   // calculate distance from vert0 to ray origin 
   SUB(tvec, orig, vert0);

   // calculate U parameter and test bounds 
   *u = DOT(tvec, pvec);
   if (*u < 0.0 || *u > det)
      return 0;

   // prepare to test V parameter 
   CROSS(qvec, tvec, edge1);

    // calculate V parameter and test bounds 
   *v = DOT(dir, qvec);
   if (*v < 0.0 || *u + *v > det)
      return 0;

   // calculate t, scale parameters, ray intersects triangle 
   *t = DOT(edge2, qvec);
   inv_det = 1.0 / det;
   *t *= inv_det;
   *u *= inv_det;
   *v *= inv_det;

#else                    // the non-culling branch 
   if (det > -EPSILON && det < EPSILON)
     return 0;

   inv_det = 1.0 / det;

   // calculate distance from vert0 to ray origin 
   SUB(tvec, orig, vert0);

   // calculate U parameter and test bounds 
   *u = DOT(tvec, pvec) * inv_det;
   if (*u < 0.0 || *u > 1.0)
     return 0;

   // prepare to test V parameter 
   CROSS(qvec, tvec, edge1);

   // calculate V parameter and test bounds 
   *v = DOT(dir, qvec) * inv_det;

   if (*v < 0.0 || *u + *v > 1.0)
     return 0;

   // calculate t, ray intersects triangle 
   *t = DOT(edge2, qvec) * inv_det;
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

//FreeType Headers
#include <freetype/ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_TTF.h>
#include <SDL/SDL_net.h>

#ifdef BUILD_PHYSICS_3D
#include <ODE/ode.h>
#endif

#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>
#include <breathe/util/cVar.h>
#include <breathe/util/cTimer.h>

#include <breathe/storage/filesystem.h>
#include <breathe/storage/xml.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cPlane.h>
#include <breathe/math/cQuaternion.h>
#include <breathe/math/cColour.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cOctree.h>

#include <breathe/util/base.h>

#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>
#include <breathe/render/cFont.h>

#ifdef BUILD_PHYSICS_3D
#include <breathe/physics/physics.h>
#endif

#ifdef BUILD_LEVEL
#include <breathe/game/cLevel.h>
#endif

#include <breathe/gui/cWidget.h>
#include <breathe/gui/cWindow.h>
#include <breathe/gui/cWindowManager.h>

#include <breathe/util/unittest.h>
#include <breathe/util/app.h>
#include <breathe/util/thread.h>
#include <breathe/util/lang.h>

#include <breathe/communication/network.h>

#include <breathe/audio/audio.h>

namespace breathe
{
	cApp::cApp(int argc, char **argv) :
#ifdef BUILD_DEBUG
		bDebug(true),
#endif
		bActive(true),
		bDone(false),
		bUpdatePhysics(true),
		bStepPhysics(false),
		bReturnCode(breathe::GOOD),

		pConsoleWindow(nullptr),
		pFont(nullptr)
	{
		CONSOLE.SetApp(this);
		filesystem::SetThisExecutable(breathe::string::ToString_t(argv[0]));
		
		_LoadSearchDirectories();
		
		util::LoadLanguageFile();

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

		SDL_ShowCursor(SDL_DISABLE);

		pRender=new render::cRender();		

		breathe::network::Init();

#ifdef BUILD_LEVEL
		pLevel=new cLevel();
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


	cApp::~cApp()
	{
		SAFE_DELETE(pFont);

		size_t nJoysticks = vJoystick.size();
		for (size_t i = 0; i < nJoysticks; i++)
      SDL_JoystickClose(vJoystick[i]);
		
		TTF_Quit();

		LOG.Success("Destroy", "Audio");
		breathe::audio::Destroy();

#ifdef BUILD_LEVEL
		LOG.Success("Delete", "Level");
		SAFE_DELETE(pLevel);
#endif
		
#ifdef BUILD_PHYSICS_3D
		LOG.Success("Destroy", "Physics");
		breathe::physics::Destroy();
#endif

		LOG.Success("Destroy", "Network");
		breathe::network::Destroy();

		LOG.Success("Delete", "Render");
		SAFE_DELETE(pRender);
		
		LOG.Success("Delete", "Log");		

		LOG.Success("Main", "Successfully exited");
		LOG.Newline("Main", "return " + bReturnCode ? "true" : "false");

		SDL_Quit();
	}

	void cApp::_InitArguments(int argc, char **argv)
	{
		int i = 1;
		std::string s;
		if (i<argc)
		{
			vArgs.push_back(argv[i]);
			s=argv[i];
		}

		for (i=2;i<argc;i++)
		{
			vArgs.push_back(argv[i]);
			s+=" " + std::string(argv[i]);
		}

		LOG.Success("Arguments", s);
	}

	void cApp::_LoadSearchDirectories()
	{
		// Add a default directory
		breathe::filesystem::AddDirectory(TEXT(""));

		// Now load all the rest from the config file
		breathe::xml::cNode root("config.xml");
		breathe::xml::cNode::iterator iter(root);

		if (!iter) return;

		iter.FindChild("config");
		if (!iter) return;

		iter.FindChild("directory");
		if (!iter) return;

		while(iter)
		{
			std::string sDirectory;
			if (iter.GetAttribute("path", sDirectory)) breathe::filesystem::AddDirectory(breathe::string::ToString_t(sDirectory));
			iter.Next("directory");
		};
	}

	bool cApp::InitApp()
	{
		{
			LOG.Success("Init", "Loading config.xml");

			breathe::xml::cNode root("config.xml");
			breathe::xml::cNode::iterator iter(root);

			if (!iter)
			{
				bReturnCode=breathe::BAD;
				return breathe::BAD;
			}

			iter.FindChild("config");
			if (!iter) return breathe::GOOD;

			iter.FindChild("render");
			while(iter)
			{
				std::ostringstream t;
				unsigned int uiValue;

				if (iter.GetAttribute("width", uiValue))
				{
					t.str("");
					t<<"width = ";
					t<<uiValue;
					LOG.Success("Config", t.str());
					pRender->uiWidth = uiValue;
				}

				if (iter.GetAttribute("height", uiValue))
				{
					t.str("");
					t<<"height = ";
					t<<uiValue;
					LOG.Success("Config", t.str());
					pRender->uiHeight = uiValue;
				}

				if (iter.GetAttribute("depth", uiValue))
				{
					t.str("");
					t<<"depth = ";
					t<<uiValue;
					LOG.Success("Config", t.str());
					pRender->uiDepth = uiValue;
				}

				bool bFullscreen;

				if (iter.GetAttribute("fullscreen", bFullscreen))
				{
					LOG.Success("Config", std::string("fullscreen = ") + (bFullscreen ? "true" : "false"));
					pRender->bFullscreen = bFullscreen;
				}

				iter.Next("render");
			};
		}

		// Init SDL 
		if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE) < 0 )
		{
			LOG.Error("SDL", std::string("SDL initialisation failed: ") + SDL_GetError());
			bReturnCode=breathe::BAD;
			return breathe::BAD;
		}
		
		SDL_WM_SetCaption(breathe::string::ToUTF8(LANG("L_Application")).c_str(), "app.ico");

		{

			// Joysticks

			int nJoysticks = SDL_NumJoysticks();

			std::ostringstream t;
			t<<"Joysticks found: "<<nJoysticks;

			if (nJoysticks)
			{
				LOG.Success("SDL", t.str());
				SDL_JoystickEventState(SDL_ENABLE);

				for (int i=0; i < nJoysticks; i++ ) 
				{
					t.str("");
					t << "Joystick(";
					t << i;
					t << ") ";
					t << SDL_JoystickName(i);
					LOG.Success("SDL", t.str());

					//TODO: Create a list of joysticks, close them at the end of the program
					SDL_Joystick *pJoystick = SDL_JoystickOpen(i);

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

			}
			else LOG.Error("SDL", t.str());

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
							/* handle keyboard stuff here 				
							break;

						case SDL_QUIT:

						/* Set whatever flags are necessary to 
						/* end the main game loop here 

							break;


						case SDL_JOYAXISMOTION:  /* Handle Joystick Motion 
							if (( event.jaxis.value < -3200 ) || (event.jaxis.value > 3200 ) ) 
							{
								/* code goes here
							}
							break;

						case SDL_JOYAXISMOTION:  /* Handle Joystick Motion
							if (( event.jaxis.value < -3200 ) || (event.jaxis.value > 3200 ) ) 
							{
								if (event.jaxis.axis == 0) 
								{
									/* Left-right movement code goes here
								}

								if (event.jaxis.axis == 1) 
								{
									/* Up-Down movement code goes here
								}
							}
							break;

						case SDL_JOYBUTTONDOWN:  /* Handle Joystick Button Presses
							if (event.jbutton.button == 0 ) 
							{
								/* code goes here
							}
							break;

						case SDL_JOYHATMOTION:  /* Handle Hat Motion
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
		
		if (breathe::BAD==InitRender())
			return breathe::BAD;

		TTF_Init();

		breathe::audio::Init();

#ifdef BUILD_PHYSICS_3D
		breathe::physics::Init();
#endif

		if (breathe::BAD==LoadScene())
			return breathe::BAD;

	
		window_manager.LoadTheme();
		
		unsigned int n = breathe::gui::GenerateID();

		// Testing Window
#ifdef BUILD_DEBUG
		/*breathe::gui::cWindow* pWindow0 = new breathe::gui::cWindow(breathe::gui::GenerateID(), 0.7f, 0.75f, 0.2f, 0.2f);
		pWindow0->AddChild(new breathe::gui::cWidget_StaticText(n, 0.05f, 0.05f, 0.1f, 0.1f));		
		pWindow0->AddChild(new breathe::gui::cWidget_StaticText(breathe::gui::GenerateID(), 0.05f, 0.5f, 0.1f, 0.1f));		
		pWindow0->AddChild(new breathe::gui::cWidget_Button(breathe::gui::GenerateID(), 0.5f, 0.05f, 0.1f, 0.1f));
		window_manager.AddChild(pWindow0);*/
#endif
		
		// Console Window
		
		pConsoleWindow = new cConsoleWindow();
		window_manager.AddChild(pConsoleWindow);
		pConsoleWindow->InitConsoleWindow();


		if (breathe::BAD==InitScene())
			return breathe::BAD;


		breathe::audio::StartAll();

		// Setup mouse
		SDL_WarpMouse(pRender->uiWidth/2, pRender->uiHeight/2);

		//SDL_ShowCursor(SDL_DISABLE);

		return breathe::GOOD;
	}

	bool cApp::DestroyApp()
	{
		SDL_ShowCursor(SDL_ENABLE);

		breathe::audio::StopAll();

		return breathe::GOOD;
	}

	bool cApp::InitRender()
	{
		if (breathe::BAD == pRender->PreInit())
		{
			bReturnCode = breathe::BAD;
			return breathe::BAD;
		}
		
		pRender->SetPerspective();

		if (breathe::BAD == pRender->Init())
			return breathe::BAD;

		return breathe::GOOD;
	}

	bool cApp::DestroyRender()
	{
		pRender->Destroy();

		return breathe::GOOD;
	}

	bool cApp::ToggleFullscreen()
	{
		// Destroy the old render
		if (breathe::BAD==DestroyRender())
		{
			bReturnCode=breathe::BAD;
			return breathe::BAD;
		}

		// Toggle fullscreen
		pRender->ToggleFullscreen();


		// Create the new render
		if (breathe::BAD==InitRender())
		{
			bReturnCode=breathe::BAD;
			return breathe::BAD;
		}

		pRender->ReloadTextures();

		return breathe::GOOD;		
	}

	bool cApp::ResizeWindow(unsigned int w, unsigned int h)
	{
		DestroyRender();

		pRender->uiWidth = w;
		pRender->uiHeight = h;

		InitRender();
		pRender->ReloadTextures();

		return breathe::GOOD;
	}
	
	void cApp::_Update(sampletime_t currentTime)
	{
		std::string s;
		breathe::constant_stack<std::string>::iterator iter = CONSOLE.begin();
		breathe::constant_stack<std::string>::iterator iterEnd = CONSOLE.end();

		while (iter != iterEnd) {
			s.append((*iter) + "\n");
			iter++;
		}
		
		pConsoleWindow->GetPrevious().SetText(breathe::string::ToString_t(s));

		// Finally, call our derived update function
		Update(currentTime);

		// Now update our other sub systems
		breathe::audio::Update(currentTime);
	}

	void cApp::_UpdateEvents(sampletime_t currentTime)
	{
		// handle the events in the queue 
		while ( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
				case SDL_ACTIVEEVENT:
					{
						bActive=event.active.gain != 0;
						if (bActive) LOG.Success("Active", "Active");
						else LOG.Error("Active", "Inactive");
					}
					break;

				case SDL_VIDEORESIZE:
					if (breathe::BAD == ResizeWindow(event.resize.w, event.resize.h))
						bDone=true;
					break;

				case SDL_KEYUP:
					_OnKeyUp(&event.key.keysym);
					break;

				case SDL_KEYDOWN:		
					_OnKeyDown(&event.key.keysym);
					break;

				case SDL_MOUSEBUTTONUP:
					mouse.x = event.button.x;
					mouse.y = event.button.y;
					mouse.down[event.button.button] = true;
					break;

				case SDL_MOUSEBUTTONDOWN:
					mouse.x = event.button.x;
					mouse.y = event.button.y;
					if (SDL_PRESSED == event.button.state) mouse.down[event.button.button] = true;
					break;

				case SDL_MOUSEMOTION:
					mouse.x = event.button.x;
					mouse.y = event.button.y;
					break;

				case SDL_QUIT:
					LOG.Success("SDL", "SDL_Quit: Quiting");
					bDone=true;
					break;

				default:
					break;
			}
		}
	}

	void cApp::AddKeyRepeat(unsigned int code)
	{
		mKey[code] = new cKey(code, true, true, false);
	}

	void cApp::AddKeyNoRepeat(unsigned int code)
	{
		mKey[code] = new cKey(code, false, false, false);
	}

	void cApp::AddKeyToggle(unsigned int code)
	{
		mKey[code] = new cKey(code, false, false, true);
	}

	bool cApp::IsKeyDown(unsigned int code)
	{
		std::map<unsigned int, cKey* >::iterator iter=mKey.find(code);

		if (iter!=mKey.end())
			return iter->second->IsDown();

		return false;
	}

	void cApp::_UpdateKeys(sampletime_t currentTime)
	{
		Uint8 *key = SDL_GetKeyState( NULL );
		cKey* p;

		std::map<unsigned int, cKey* >::iterator iter=mKey.begin();
		while(iter != mKey.end())
		{
			p = (iter->second);

			//This key is pressed
			if (key[p->uiCode])
			{
				//This key can be held down
				if (p->bRepeat)
				{
					p->bDown=true;
					p->bCollected=false;
				}
				//This key can only be pressed once
				else
				{
					p->bDown=false;
					p->bCollected=false;
				}
			}
			else
				p->bDown = false;

			iter++;
		}

		
		SDL_GetMouseState(&mouse.x, &mouse.y);
	}

	void cApp::_OnKeyDown(SDL_keysym *keysym)
	{
		unsigned int code = keysym->sym;

		std::map<unsigned int, cKey* >::iterator iter = mKey.find(code);
		if (iter != mKey.end()) iter->second->SetDown(CONSOLE.IsVisible());
		
		if (CONSOLE.IsVisible()) {
			// Remove key from list
			IsKeyDown(code);
			
			if (!CONSOLE.AddKey(code)) ConsoleHide();
		}
	}

	void cApp::_OnKeyUp(SDL_keysym *keysym)
	{
		unsigned int code = keysym->sym;

		std::map<unsigned int, cKey* >::iterator iter = mKey.find(code);
		if (iter != mKey.end()) iter->second->SetUp(CONSOLE.IsVisible());

		if (CONSOLE.IsVisible()) CONSOLE.AddKey(code);
	}
	
	void cApp::_OnMouseUp(int button, int x, int y)
	{
		
	}
	
	void cApp::_OnMouseDown(int button, int x, int y)
	{
		
	}
	
	void cApp::_OnMouseMove(int button, int x, int y)
	{
		
	}


	void cApp::_UpdateInput(sampletime_t currentTime)
	{
#ifdef BUILD_DEBUG
		if (IsKeyDown(SDLK_F1)) ToggleDebug();

		if (IsKeyDown(SDLK_F2)) pRender->ReloadTextures();
		if (IsKeyDown(SDLK_F3)) pRender->bShader = pRender->bCanShader && !pRender->bShader;
		if (IsKeyDown(SDLK_F4)) pRender->bCubemap = !pRender->bCubemap;
		if (IsKeyDown(SDLK_F5)) pRender->bLight = !pRender->bLight;
		if (IsKeyDown(SDLK_F6)) pRender->bRenderWireframe = !pRender->bRenderWireframe;
		
		if (IsKeyDown(SDLK_F7)) bUpdatePhysics = !bUpdatePhysics;
		if (IsKeyDown(SDLK_F8)) {
			bUpdatePhysics = false;
			bStepPhysics = true;
		}

		if (IsKeyDown(SDLK_F9)) util::RunUnitTests();		
#endif

		UpdateInput(currentTime);
	}

	void cApp::CursorShow()
	{
		SDL_ShowCursor(SDL_ENABLE);
	}

	void cApp::CursorHide()
	{
		SDL_ShowCursor(SDL_DISABLE);
	}
	
	void cApp::ConsoleShow()
	{
		assert(pConsoleWindow != nullptr);
		pConsoleWindow->Show();
		CONSOLE.Show();
		CursorShow();
	}

	void cApp::ConsoleHide()
	{
		assert(pConsoleWindow != nullptr);
		pConsoleWindow->Hide();
		CONSOLE.Hide();
		CursorHide();
	}

	void cApp::ConsoleExecute(const std::string& command)
	{
		Execute(command);
	}

	//This is for executing one single line, cannot have ";"
	void cApp::_ConsoleExecuteSingleCommand(const std::string& command)
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
			else if (bDebug)
			{
				std::string c="Line: [";

				unsigned int n = args.size();
				for (unsigned int a = 0;a < n; a++)
					c+="(" + args[a] + ")";

				c+="]";

				CONSOLE<<c<<std::endl;
			}
#endif
		}
	}

	/*//This is for executing multiple lines of commands, seperated by ";"
	void cApp::ConsoleExecute(const std::string& command)
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

	void cApp::_Render(sampletime_t currentTime)
	{
		BeginRender(currentTime);

			pRender->Begin();
				pRender->BeginRenderScene();
					RenderScene(currentTime);
				pRender->EndRenderScene();
				pRender->BeginScreenSpaceRendering();
					RenderScreenSpace(currentTime);
					
					#ifdef _DEBUG
					if (!CONSOLE.IsVisible() && IsDebug())
					{
						pRender->SetColour(0.0f, 0.0f, 1.0f);
						
						float fPosition = 0.1f;
#ifdef BUILD_PHYSICS_3D
						pFont->printf(0.05f, fPosition+=0.05f, "Physics Objects: %d", breathe::physics::size());
#endif

						fPosition+=0.05f;
						pFont->printf(0.05f, fPosition+=0.05f, "uiTriangles: %d", pRender->uiTriangles);
						pFont->printf(0.05f, fPosition+=0.05f, "uiTextureChanges: %d", pRender->uiTextureChanges);
						pFont->printf(0.05f, fPosition+=0.05f, "uiTextureModeChanges: %d", pRender->uiTextureModeChanges);
						pFont->printf(0.05f, fPosition+=0.05f, "fRenderFPS: %.03f", tRender.GetFPS());
						pFont->printf(0.05f, fPosition+=0.05f, "fUpdateFPS: %.03f", tUpdate.GetFPS());
						pFont->printf(0.05f, fPosition+=0.05f, "fPhysicsFPS: %.03f", tPhysics.GetFPS());
						pFont->printf(0.05f, fPosition+=0.05f, "currentTime: %d", currentTime);
						
						fPosition+=0.05f;
						pFont->printf(0.05f, fPosition+=0.05f, "fRenderMPF: %.03f", tRender.GetMPF());
						pFont->printf(0.05f, fPosition+=0.05f, "fUpdateMPF: %.03f", tUpdate.GetMPF());
						pFont->printf(0.05f, fPosition+=0.05f, "fPhysicsMPF: %.03f", tPhysics.GetMPF());
					}
					#endif //_DEBUG


					window_manager.Render();
				pRender->EndScreenSpaceRendering();
			pRender->End();

		EndRender(currentTime);
	}

	bool cApp::Run()
	{
		LOG.Newline("Run");

		bReturnCode = InitApp();
		if (bReturnCode == BAD) return bReturnCode;

    LOG.Newline("MainLoop");

		sampletime_t currentTime = breathe::util::GetTime();

    unsigned int uiPhysicsHz = physics::GetFrequencyHz();
		unsigned int uiUpdateHz = 30;
		unsigned int uiTargetFramesPerSecond = 60;

		float fEventsDelta=1000.0f/30.0f; // Should be once every single loop?
		float fInputDelta=1000.0f/30.0f;
		float fPhysicsDelta=1000.0f/uiPhysicsHz;
		float fUpdateDelta=1000.0f/uiUpdateHz;
		float fRenderDelta=1000.0f/uiTargetFramesPerSecond;

		float fEventsNext=0.0f;
		float fInputNext=0.0f;
		float fPhysicsNext=0.0f;
		float fUpdateNext=0.0f;
		float fRenderNext=0.0f;

		tPhysics.Init(uiPhysicsHz);
		tUpdate.Init(uiUpdateHz);
		tRender.Init(uiTargetFramesPerSecond);

		//TODO: Activate window so that it is on top as soon as we start

		do
		{
			currentTime = util::GetTime();

      if (currentTime > fEventsNext)
			{
				_UpdateEvents(currentTime);
				fEventsNext = currentTime + fEventsDelta;
			}

			if (currentTime > fInputNext)
			{
				// These have to be in this order or nothing gets collected
				_UpdateInput(currentTime);
				_UpdateKeys(currentTime);
				fInputNext = currentTime + fInputDelta;
			}

			if (bStepPhysics || (bUpdatePhysics && currentTime > fPhysicsNext))
			{
				UpdatePhysics(currentTime);
				tPhysics.Update(currentTime);
				fPhysicsNext = currentTime + fPhysicsDelta;
			}

			if (currentTime > fUpdateNext)
			{
				_Update(currentTime);
				tUpdate.Update(currentTime);
				fUpdateNext = currentTime + fUpdateDelta;
			}

			if (bActive && !bDone)// && currentTime > fRenderNext)
			{
				_Render(currentTime);
				tRender.Update(currentTime);
				fRenderNext = currentTime + fRenderDelta;
			}
			breathe::util::YieldThisThread();
		}while (!bDone);

		LOG.Newline("DestroyScene");
		DestroyScene();

		LOG.Newline("DestroyApp");
		DestroyApp();

		return bReturnCode;
	}



	// *** cConsoleWindow
	cApp::cConsoleWindow::cConsoleWindow() :
		gui::cWindow(breathe::gui::GenerateID(), 0.05f, 0.05f, 0.4f, 0.4f, LANG("L__Console")),
		pPrevious(nullptr),
		pInput(nullptr)
	{
	}

	cApp::cConsoleWindow::~cConsoleWindow()
	{
	}

	void cApp::cConsoleWindow::InitConsoleWindow()
	{
		Hide();
		SetResizable();

		pPrevious = new gui::cWidget_StaticText(breathe::gui::GenerateID(), 0.05f, 0.05f, 0.9f, 0.9f);
		pInput = new gui::cWidget_Input(breathe::gui::GenerateID(), 0.05f, 0.05f, 0.9f, 0.9f);

		AddChild(pPrevious);
		AddChild(pInput);
	}

	void cApp::cConsoleWindow::_OnEvent(gui::id_t idControl)
	{
		int x = 0;
		x++;
	}
	
	
	// *** cKey
	cApp::cKey::cKey(unsigned int code, bool variable, bool repeat, bool toggle) :
		uiCode(code),

		bVariable(variable),
		bRepeat(repeat),
		bToggle(toggle),
		
		bDown(false),
		bCollected(false)
	{
	}

	bool cApp::cKey::IsDown()
	{
		if (bDown)
		{
			if (bRepeat)
			{
				bCollected=true;
				return true;
			}
			else if (!bCollected)
			{
				bCollected=true;
				return true;
			}
      bDown=false;
			bCollected=true;
		}

		return false;
	}

	void cApp::cKey::SetDown(bool bConsole)
	{
		bDown = true;
		bCollected = false;
	}

	void cApp::cKey::SetUp(bool bConsole)
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

	

	bool cApp::IsMouseLeftButtonDown() const
	{
		return mouse.down[0];
	}

	bool cApp::IsMouseRightButtonDown() const
	{
		return mouse.down[2];
	}
	
	bool cApp::IsMouseScrollDown() const
	{
		return mouse.down[4];
	}

	bool cApp::IsMouseScrollUp() const
	{
		return mouse.down[5];
	}

	int cApp::GetMouseX() const
	{
		return mouse.GetY();
	}

	int cApp::GetMouseY() const
	{
		return mouse.GetX();
	}
}
