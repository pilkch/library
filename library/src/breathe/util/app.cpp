#include <cmath>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cassert>

// writing on a text file
#include <iostream>
#include <fstream>

#include <list>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>

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

#include <ODE/ode.h>

#include <breathe/breathe.h>
#include <breathe/util/cString.h>
#include <breathe/util/log.h>
#include <breathe/util/filesystem.h>
#include <breathe/util/cVar.h>
#include <breathe/util/xml.h>

#include <breathe/util/cTimer.h>

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

#include <breathe/physics/physics.h>
#include <breathe/physics/cContact.h>
#include <breathe/physics/cRayCast.h>
#include <breathe/physics/cPhysicsObject.h>

#include <breathe/game/cLevel.h>
#include <breathe/util/app.h>

#include <breathe/audio/audio.h>

#include <breathe/util/thread.h>
#include <breathe/util/network.h>

#ifdef BUILD_DEBUG
void TestVariable(breathe::cApp* app, std::string variable)
{
	breathe::cVar<std::string>* p = app->VarFind(variable);
	assert(p);
	std::string s = p->GetString();
	bool b = p->GetBool();
	unsigned int ui = p->GetUnsignedInt();
	int i = p->GetInt();
	float f = p->GetFloat();
	CONSOLE<<variable<<"=(\""<<s<<"\","<<b<<","<<ui<<","<<i<<","<<f<<")"<<std::endl;
}
#endif

namespace breathe
{
	cApp::cApp(int argc, char **argv) :
#ifdef BUILD_DEBUG
		bDebug(true),
#endif

    bConsole(false),
		bActive(true),
		bDone(false),
		bUpdatePhysics(true),
		bStepPhysics(false),

		bReturnCode(breathe::GOOD),

		g_info(NULL)
	{
		_InitArguments(argc, argv);

    srand(time(NULL));
		
		LOG<<"This is printed to the log"<<std::endl;
		CONSOLE<<"This is printed to the console"<<std::endl;
		SCREEN<<"This is printed to the screen"<<std::endl;
		
		CONSOLE<<"1"<<std::endl;
		CONSOLE<<"2"<<std::endl;
		CONSOLE<<"1"<<"2"<<"3"<<"4"<<std::endl;

		VarSet("bool", true);
		VarSet("int", 180);
		VarSet("float", 123.45f);
		VarSet("string", "this is a string");

		TestVariable(this, "bool");
		TestVariable(this, "int");
		TestVariable(this, "float");
		TestVariable(this, "string");

		// get all files ending in .dll
		CONSOLE<<"FileSystem Test"<<std::endl;
		breathe::filesystem::path dir_path(".");
		/*directory_iterator end_it;
		for(directory_iterator it(dir_path); it != end_it; ++it) {
			if( !is_directory(it->status()) && extension(it->path()) == ".dll" ) {
				it->path().string();
			}
		}*/


		SDL_ShowCursor(SDL_DISABLE);


		pRender=new render::cRender();
		
		breathe::network::Init();

		pLevel=new cLevel();
	}

	cApp::~cApp()
	{
		unsigned int nJoysticks = vJoystick.size();
		for(unsigned int i = 0; i < nJoysticks; i++)
      SDL_JoystickClose(vJoystick[i]);

		TTF_Quit();

		LOG.Success("Destroy", "Audio");
		breathe::audio::Destroy();

		LOG.Success("Delete", "Level");
		SAFE_DELETE(pLevel);

		LOG.Success("Destroy", "Physics");
		breathe::physics::Destroy();

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
		filesystem::SetThisExecutable(argv[0]);

		int i=1;
		std::string s;
		if(i<argc)
		{
			vArgs.push_back(argv[i]);
			s=argv[i];
		}

		for(i=2;i<argc;i++)
		{
			vArgs.push_back(argv[i]);
			s+=" " + std::string(argv[i]);
		}

		LOG.Success("Arguments", s);
	}

	#define EPSILON 0.000001
#define CROSS(dest,v1,v2) \
          dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
          dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
          dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#define SUB(dest,v1,v2)
          dest[0]=v1[0]-v2[0]; \
          dest[1]=v1[1]-v2[1]; \
          dest[2]=v1[2]-v2[2]; 

int
intersect_triangle(double orig[3], double dir[3],
                   double vert0[3], double vert1[3], double vert2[3],
                   double *t, double *u, double *v)
{
   double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
   double det,inv_det;

   /* find vectors for two edges sharing vert0 */
   SUB(edge1, vert1, vert0);
   SUB(edge2, vert2, vert0);

   /* begin calculating determinant - also used to calculate U parameter */
   CROSS(pvec, dir, edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   det = DOT(edge1, pvec);

#ifdef TEST_CULL           /* define TEST_CULL if culling is desired */
   if (det < EPSILON)
      return 0;

   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = DOT(tvec, pvec);
   if (*u < 0.0 || *u > det)
      return 0;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

    /* calculate V parameter and test bounds */
   *v = DOT(dir, qvec);
   if (*v < 0.0 || *u + *v > det)
      return 0;

   /* calculate t, scale parameters, ray intersects triangle */
   *t = DOT(edge2, qvec);
   inv_det = 1.0 / det;
   *t *= inv_det;
   *u *= inv_det;
   *v *= inv_det;
#else                    /* the non-culling branch */
   if (det > -EPSILON && det < EPSILON)
     return 0;
   inv_det = 1.0 / det;

   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = DOT(tvec, pvec) * inv_det;
   if (*u < 0.0 || *u > 1.0)
     return 0;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

   /* calculate V parameter and test bounds */
   *v = DOT(dir, qvec) * inv_det;
   if (*v < 0.0 || *u + *v > 1.0)
     return 0;

   /* calculate t, ray intersects triangle */
   *t = DOT(edge2, qvec) * inv_det;
#endif
   return 1;
}

	bool cApp::InitApp()
	{
antiportal: A polygon that is totally opaque, not rendered, but hiding models behind it, like in Farcry.  Can be walls that hide players etc. behind them. 

Identify and replace these in XML:
&amp;	&
&lt;	<
&gt;	>
&quot;	"
&apos;	'
change comments <!-- --> to comment object (bComment = true). 

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

			iter.FindChild("directory");
			if (!iter) return breathe::GOOD;

			while(iter)
			{
				std::string sDirectory;
				if(iter.GetAttribute("path", &sDirectory)) breathe::filesystem::AddDirectory(sDirectory);

				iter.Next("directory");
			};

			iter = root;
			if (!iter) return breathe::GOOD;

			iter.FindChild("config");
			if (!iter) return breathe::GOOD;

			iter.FindChild("render");
			while(iter)
			{
				std::ostringstream t;
				unsigned int uiValue;
				if(iter.GetAttribute("width", &uiValue))
				{
					t.str("");
					t<<"width = ";
					t<<uiValue;
					LOG.Success("Config", t.str());
					pRender->uiWidth = uiValue;
				}
				if(iter.GetAttribute("height", &uiValue))
				{
					t.str("");
					t<<"height = ";
					t<<uiValue;
					LOG.Success("Config", t.str());
					pRender->uiHeight = uiValue;
				}
				if(iter.GetAttribute("depth", &uiValue))
				{
					t.str("");
					t<<"depth = ";
					t<<uiValue;
					LOG.Success("Config", t.str());
					pRender->uiDepth = uiValue;
				}

				bool bFullscreen;
				if(iter.GetAttribute("fullscreen", &bFullscreen))
				{
					LOG.Success("Config", std::string("fullscreen = ") + (bFullscreen ? "true" : "false"));
					pRender->bFullscreen = bFullscreen;
				}

				iter.Next("render");
			};
		}



		// Init SDL 
		if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE) < 0 )
		{
			LOG.Error("SDL", std::string("SDL initialisation failed: ") + SDL_GetError());
			bReturnCode=breathe::BAD;
			return breathe::BAD;
		}

		{
			// Joysticks
			int nJoysticks = SDL_NumJoysticks();
			std::ostringstream t;
			t << "Joysticks found: ";
			t << nJoysticks;

			if(nJoysticks)
			{
				LOG.Success("SDL", t.str());
				SDL_JoystickEventState(SDL_ENABLE);

				for(int i=0; i < nJoysticks; i++ ) 
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
			else
        LOG.Error("SDL", t.str());

			/*if(nJoysticks)
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
								if ( ( event.jaxis.value < -3200 ) || (event.jaxis.value > 3200 ) ) 
								{
									/* code goes here
								}
								break;


								case SDL_JOYAXISMOTION:  /* Handle Joystick Motion
								if ( ( event.jaxis.value < -3200 ) || (event.jaxis.value > 3200 ) ) 
								{
										if( event.jaxis.axis == 0) 
										{
												/* Left-right movement code goes here
										}

										if( event.jaxis.axis == 1) 
										{
												/* Up-Down movement code goes here
										}
								}
								break;


								case SDL_JOYBUTTONDOWN:  /* Handle Joystick Button Presses
								if ( event.jbutton.button == 0 ) 
								{
										/* code goes here
								}
								break;


								case SDL_JOYHATMOTION:  /* Handle Hat Motion
								if ( event.jhat.value & SDL_HAT_UP )
								{
										// Do up stuff here
								}

								if ( event.jhat.value & SDL_HAT_LEFT )
								{
										// Do left stuff here
								}

								if ( event.jhat.value & SDL_HAT_RIGHTDOWN )
								{
										// Do right and down together stuff here
								}
								break;
						}
				}

				// End loop here
			}*/
		}

		std::string s = ("app.ico");
		LOG.Success("SDL", "Setting caption to " + s);
		SDL_WM_SetCaption(title.c_str(), s.c_str());

		if(breathe::BAD==InitRender())
			return breathe::BAD;

		TTF_Init();

		breathe::audio::Init();
		
		breathe::physics::Init();

		if(breathe::BAD==LoadScene())
			return breathe::BAD;

		if(breathe::BAD==InitScene())
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
		// Fetch the video info 
		videoInfo = SDL_GetVideoInfo( );

		if ( !videoInfo )
		{
			LOG.Error("SDL", std::string("Video query failed: ") + SDL_GetError());
			bReturnCode=breathe::BAD;
			return breathe::BAD;
		}
		
		if(pRender->bFullscreen)
		{
			LOG.Success("App", "Going to fullscreen");
			pRender->uiFlags |= SDL_FULLSCREEN;
		}
		else
		{
			LOG.Success("App", "Going to windowed");
			pRender->uiFlags &= ~SDL_FULLSCREEN;
		}

		// This checks to see if surfaces can be stored in memory 
		if ( videoInfo->hw_available )
		{
			pRender->uiFlags |= SDL_HWSURFACE;
			pRender->uiFlags &= ~SDL_SWSURFACE;
		}
		else
		{
			pRender->uiFlags |= SDL_SWSURFACE;
			pRender->uiFlags &= ~SDL_HWSURFACE;
			LOG.Error("SDL", "SOFTWARE SURFACE");
		}

		// This checks if hardware blits can be done 
		if ( videoInfo->blit_hw )
			pRender->uiFlags |= SDL_HWACCEL;
		else
		{
			pRender->uiFlags &= ~SDL_HWACCEL;
			LOG.Error("SDL", "SOFTWARE BLIT");
		}

		// Sets up OpenGL double buffering 
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

		// get a SDL surface 
		pRender->pSurface = SDL_SetVideoMode(pRender->uiWidth, pRender->uiHeight, pRender->uiDepth, pRender->uiFlags);

		// Verify there is a surface 
		if(!pRender->pSurface)
		{
			LOG.Error("SDL", std::string("Video mode set failed: ") + SDL_GetError());
			bReturnCode=breathe::BAD;
			return breathe::BAD;
		}

		if(breathe::BAD==SetPerspective())
			return breathe::BAD;

		if(breathe::BAD==pRender->Init())
			return breathe::BAD;

		return breathe::GOOD;
	}

	bool cApp::DestroyRender()
	{
		SDL_FreeSurface(pRender->pSurface);

		pRender->pSurface = NULL;

		return breathe::GOOD;
	}
	
	bool cApp::ToggleFullscreen()
	{	
		// Destroy the old render
		if(breathe::BAD==DestroyRender())
		{
			bReturnCode=breathe::BAD;
			return breathe::BAD;
		}

		// Toggle fullscreen
		pRender->ToggleFullscreen();

		// Create the new render
		if(breathe::BAD==InitRender())
		{
			bReturnCode=breathe::BAD;
			return breathe::BAD;
		}

		pRender->ReloadTextures();
		
		return breathe::GOOD;		
	}

	cVar<std::string>* cApp::VarFind(std::string name)
	{
		std::map<std::string, cVar<std::string>*>::iterator iter=mVar.begin();

		std::string s;
		while(iter!=mVar.end())
		{
			if(name==iter->first)
				return iter->second;

			iter++;
		};

		return NULL;
	}

	bool cApp::SetPerspective()
	{
		pRender->SetPerspective();

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

	void cApp::UpdateEvents(float fCurrentTime)
	{
		// handle the events in the queue 
		while ( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
				case SDL_ACTIVEEVENT:				
					{
						bActive=event.active.gain != 0;
						if(bActive)
							LOG.Success("Active", "Active");
						else
							LOG.Error("Active", "Inactive");
					}
					break;
				case SDL_VIDEORESIZE:

					if(breathe::BAD == ResizeWindow(event.resize.w, event.resize.h))
						bDone=true;

					break;
				case SDL_KEYUP:
					OnKeyUp(&event.key.keysym);

					break;
				case SDL_MOUSEMOTION:
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					OnMouse(event.button.button, event.button.state, event.button.x, event.button.y);

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
		
		if(iter!=mKey.end())
			return iter->second->IsKeyDown();

		return false;
	}

	void cApp::UpdateKeys(float fCurrentTime)
	{
		{
			Uint8 *key = SDL_GetKeyState( NULL );

			cKey* p;
			std::map<unsigned int, cKey* >::iterator iter=mKey.begin();
			
			while(iter != mKey.end())
			{
				p = (iter->second);

				//This key is pressed
				if(key[p->uiCode])
				{
					//This key can be held down
					if(p->bRepeat)
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
					p->bDown=false;

				iter++;
			}
		}
	}

	void cApp::OnKeyUp(SDL_keysym *keysym)
	{
		unsigned int code=keysym->sym;

		std::map<unsigned int, cKey* >::iterator iter=mKey.find(code);
		
		if(iter!=mKey.end())
			iter->second->SetKeyUp(bConsole);

		if(bConsole)
			ConsoleAddKey(code);
	}



	void cApp::ConsoleAddKey(unsigned int uiCode)
	{
		//Uint8 *key = SDL_GetKeyState( NULL );

		//key for shift, control, etc. modifiers
		//if(key[SDLK_SHIFT])
		/*SDLK_NUMLOCK		= 300,
	SDLK_CAPSLOCK		= 301,
	SDLK_SCROLLOCK		= 302,
	SDLK_RSHIFT		= 303,
	SDLK_LSHIFT		= 304,
	SDLK_RCTRL		= 305,
	SDLK_LCTRL		= 306,
	SDLK_RALT		= 307,
	SDLK_LALT		= 308,
	SDLK_RMETA		= 309,
	SDLK_LMETA		= 310,
	SDLK_LSUPER		= 311,		//Left "Windows" key
	SDLK_RSUPER		= 312,		//Right "Windows" key
	SDLK_MODE		= 313,		//"Alt Gr" key
	SDLK_COMPOSE		= 314,		//Multi-key compose key*/

		if(SDLK_RETURN==uiCode || SDLK_KP_ENTER==uiCode)
		{
			ConsoleExecute(CONSOLE.current);
			CONSOLE.current="";
			CONSOLE.uiCursorPosition=0;
		}
		else if(SDLK_ESCAPE==uiCode || SDLK_BACKQUOTE==uiCode)
		{
			bConsole=false;
			CONSOLE.current="";
			CONSOLE.uiCursorPosition=0;
		}
		else if(SDLK_DELETE==uiCode)
		{
			if(CONSOLE.uiCursorPosition<CONSOLE.current.size())
				CONSOLE.current.erase(CONSOLE.uiCursorPosition, 1);
		}
		else if(SDLK_BACKSPACE==uiCode)
		{
			if(CONSOLE.uiCursorPosition>0)
			{
				CONSOLE.current.erase(CONSOLE.uiCursorPosition-1, 1);
				CONSOLE.uiCursorPosition--;
			}
		}
		else if(SDLK_LEFT==uiCode)
		{
			if(CONSOLE.uiCursorPosition>0)
        CONSOLE.uiCursorPosition--;
		}
		else if(SDLK_RIGHT==uiCode)
		{
			if(CONSOLE.uiCursorPosition<CONSOLE.current.size())
				CONSOLE.uiCursorPosition++;
		}
		else if(SDLK_HOME==uiCode)
      CONSOLE.uiCursorPosition=0;
		else if(SDLK_END==uiCode)
      CONSOLE.uiCursorPosition=CONSOLE.current.size();

		else if(SDLK_TAB==uiCode)
			; //TODO: Autocomplete
		else if(SDLK_CLEAR==uiCode)
			CONSOLE.current="";
		else if(SDLK_UP==uiCode)
			; //TODO: History of typed in items
		else if(SDLK_DOWN==uiCode)
			; //TODO: History of typed in items
		else if(SDLK_PAGEUP==uiCode)
			; //TODO: History of all console items
		else if(SDLK_PAGEDOWN==uiCode)
			; //TODO: History of all console items

		else if(uiCode<300)
		{
			/*#include <SDL/SDL_keysym.h>
					
			if((uiCode<SDLK_a || uiCode>SDLK_z) && (uiCode<SDLK_0 || uiCode>SDLK_9))
			{
				std::ostringstream t;

				t<<"unmapped key: "<<uiCode<<" ("<<static_cast<unsigned char>(uiCode)<<")";
				
				ConsoleAddLine(t.str());
			}*/

			std::string s;
			s += static_cast<breathe::unicode_char>(uiCode);
			CONSOLE.current.insert(CONSOLE.uiCursorPosition, s);
			CONSOLE.uiCursorPosition++;
		}

		//When we press a key we want to see where we are up to
		CONSOLE.uiCursorBlink=0;
	}
	
	//This is for executing one single line, cannot have ";"
	void cApp::_ConsoleExecuteSingleCommand(std::string s)
	{
		CONSOLE<<s;

		std::string full(s);
		std::vector<std::string> args;

		{
			int whitespace;
			std::string parameter;

			//We have a line, find the individual parts
			while(s.length())
			{
				whitespace=s.find(" ");

				if(std::string::npos==whitespace)
				{
					parameter=s;
					s="";
				}
				else
				{
					parameter=s.substr(0, whitespace);
					s=s.substr(whitespace+1);
				}

				if(parameter.length())
					args.push_back(parameter);
			};
		}

		
		if(args.size())
		{
			if("quit"==full || "exit"==full)
				bDone=true;
			else if("var"==args[0])
			{
				std::map<std::string, cVar<std::string>* >::iterator iter=mVar.begin();

				while(iter!=mVar.end())
				{
					CONSOLE<<iter->first + " \"" + (iter->second)->GetString() + "\"";

					iter++;
				};
			}
			else if(GOOD==Execute(full))
			{
				
			}
#ifdef BUILD_DEBUG
			else if(bDebug)
			{
				unsigned int a=0;
				std::string c;

				c="Line: [";
				for(a=0;a<args.size();a++)
					c+="(" + args[a] + ")";
				c+="]";

				CONSOLE<<c;
			}
#endif
		}
	}

	//This is for executing multiple lines of commands, seperated by ";"
	void cApp::ConsoleExecute(std::string s)
	{
		if(""!=s)
		{
			//Take out all \n, each line should be finished with a ";" or it is not a valid line
			s=string::Replace(s, "\n", " ");

			//Split into std::strings that are all ended with ";"
			int endofline;
			std::string line;
			std::string parameter;

			while(s.length())
			{
				endofline=s.find(";");

				if(std::string::npos==endofline)
				{
					line=s;
					s="";
				}
				else
				{
					line=s.substr(0, endofline);
					s=s.substr(endofline+1);
				}

				if(line.length())
					_ConsoleExecuteSingleCommand(line);	
			};
		}
	}

	void cApp::Render(float fCurrentTime)
	{
		pRender->Begin();
			pRender->BeginRenderScene();
				RenderScene(fCurrentTime);
			pRender->EndRenderScene();
			pRender->BeginScreenSpaceRendering();
				RenderScreenSpace(fCurrentTime);
			pRender->EndScreenSpaceRendering();
		pRender->End();
	}


	bool cApp::Run()
	{
		LOG.Newline("Run");
		bReturnCode = InitApp();
		if (bReturnCode == BAD) return bReturnCode;

    LOG.Newline("MainLoop");
		float fCurrentTime = 0.0f;

		unsigned int uiPhysicsHz = (unsigned int)(1000.0f * 1000.0f * physics::fInterval);
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

		do
		{
			fCurrentTime = util::GetTime();


      if(fCurrentTime > fEventsNext)
			{
				UpdateEvents(fCurrentTime);

				fEventsNext = fCurrentTime+fEventsDelta;
			}
			
			if(fCurrentTime > fInputNext)
			{
				// These have to be in this order or nothing gets collected
				UpdateInput(fCurrentTime);
				UpdateKeys(fCurrentTime);

				fInputNext=fCurrentTime+fInputDelta;
			}

			if(bStepPhysics || (bUpdatePhysics && fCurrentTime > fPhysicsNext))
			{
				UpdatePhysics(fCurrentTime);

				tPhysics.Update(fCurrentTime);

				fPhysicsNext=fCurrentTime+fPhysicsDelta;
			}

			if(fCurrentTime > fUpdateNext)
			{
				Update(fCurrentTime);

				tUpdate.Update(fCurrentTime);

				fUpdateNext=fCurrentTime+fUpdateDelta;
			}
		
			
			if(!bDone && bActive)// && fCurrentTime > fRenderNext)
			{
				Render(fCurrentTime);

				tRender.Update(fCurrentTime);

				fRenderNext = fCurrentTime + fRenderDelta;
			}
		}while (!bDone);

		LOG.Newline("DestroyScene");
		DestroyScene();

		LOG.Newline("DestroyApp");
		DestroyApp();

		return bReturnCode;
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

	bool cApp::cKey::IsKeyDown()
	{
		if(bDown)
		{
			if(bRepeat)
			{
				bCollected=true;
				return true;
			}
			else if(!bCollected)
			{
				bCollected=true;
				return true;
			}

      bDown=false;
			bCollected=true;
		}

		return false;
	}
	
	void cApp::cKey::SetKeyUp(bool bConsole)
	{
		if(bRepeat)
		{
			bDown=false;
			bCollected=false;
		}
		else if(!bConsole)
		{
			bDown=true;
			bCollected=false;
		}
	}
}
