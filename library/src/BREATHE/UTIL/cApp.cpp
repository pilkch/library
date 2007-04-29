#include <cmath>
#include <ctime>
#include <cstdio>
#include <cstdlib>

// writing on a text file
#include <iostream>
#include <fstream>

#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <list>

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

#include <ODE/ode.h>

#include <BREATHE/cBreathe.h>
#include <BREATHE/UTIL/cString.h>
#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/UTIL/cFileSystem.h>
#include <BREATHE/UTIL/cVar.h>
#include <BREATHE/UTIL/cConsole.h>
#include <BREATHE/UTIL/cXML.h>

#include <BREATHE/UTIL/cTimer.h>

#include <BREATHE/MATH/cMath.h>
#include <BREATHE/MATH/cVec3.h>
#include <BREATHE/MATH/cVec4.h>
#include <BREATHE/MATH/cMat4.h>
#include <BREATHE/MATH/cPlane.h>
#include <BREATHE/MATH/cQuaternion.h>
#include <BREATHE/MATH/cColour.h>
#include <BREATHE/MATH/cFrustum.h>
#include <BREATHE/MATH/cOctree.h>


#include <BREATHE/UTIL/cBase.h>
#include <BREATHE/RENDER/MODEL/cMesh.h>
#include <BREATHE/RENDER/MODEL/cModel.h>
#include <BREATHE/RENDER/MODEL/cStatic.h>


#include <BREATHE/RENDER/cTexture.h>
#include <BREATHE/RENDER/cTextureAtlas.h>
#include <BREATHE/RENDER/cMaterial.h>
#include <BREATHE/RENDER/cRender.h>




#include <BREATHE/PHYSICS/cPhysicsObject.h>
#include <BREATHE/PHYSICS/cPhysics.h>

#include <BREATHE/GAME/cLevel.h>
#include <BREATHE/UTIL/cApp.h>

namespace BREATHE
{
	cApp::cApp(int argc, char **argv)
	{
    srand(time(NULL));

		pLog=new cLog();
		pLog->Init();

		{
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

			pLog->Success("Arguments", s);
		}
		
		pFileSystem=new cFileSystem();

    bConsole=false;
		bDebug=false;
		bActive=true;
		bDone=false;
		bUpdatePhysics=true;
		bStepPhysics=false;

		bReturnCode=BREATHE::GOOD;

		pConsole=new cConsole();

		g_info=NULL;
		
		pRender=new RENDER::cRender();


		SDL_ShowCursor(SDL_DISABLE);

		pPhysics=new PHYSICS::cPhysics();

		pLevel=new cLevel(pRender);
	}

	cApp::~cApp()
	{
		pLog->Success("Delete", "Level");
		SAFE_DELETE(pLevel);

		pLog->Success("Delete", "Physics");
		SAFE_DELETE(pPhysics);

		pLog->Success("Delete", "Render");
		SAFE_DELETE(pRender);
		
		pLog->Success("Delete", "FileSystem");
		SAFE_DELETE(pFileSystem);
		
		pLog->Success("Delete", "Log");
		
		pLog->Success("Main", "Successfully exited");
		pLog->Success("Main", "return " + bReturnCode);

		SAFE_DELETE(pLog);

		TTF_Quit();

		SDL_Quit();
	}

	

	bool cApp::Init()
	{
		{
			pLog->Success("Init", "Loading config.xml");
			BREATHE::XML::cNode pRoot("config.xml");

			pRoot = *(pRoot.FindChild("config"));

			BREATHE::XML::cNode* p = pRoot.FindChild("directory");
			while(p)
			{
				std::string sDirectory;
				if(p->GetAttribute("path", &sDirectory))
					pFileSystem->AddDirectory(sDirectory);

				p=p->Next("directory");
			};

			p = pRoot.FindChild("render");
			while(p)
			{
				std::ostringstream t;
				unsigned int uiValue;
				if(p->GetAttribute("width", &uiValue))
				{
					t.str("");
					t<<"width = ";
					t<<uiValue;
					pLog->Success("Config", t.str());
					pRender->uiWidth = uiValue;
				}
				if(p->GetAttribute("height", &uiValue))
				{
					t.str("");
					t<<"height = ";
					t<<uiValue;
					pLog->Success("Config", t.str());
					pRender->uiHeight = uiValue;
				}
				if(p->GetAttribute("depth", &uiValue))
				{
					t.str("");
					t<<"depth = ";
					t<<uiValue;
					pLog->Success("Config", t.str());
					pRender->uiDepth = uiValue;
				}

				bool bFullscreen;
				if(p->GetAttribute("fullscreen", &bFullscreen))
				{
					pLog->Success("Config", std::string("fullscreen = ") + (bFullscreen ? "true" : "false"));
					pRender->bFullscreen = bFullscreen;
				}

				p=p->Next("render");
			};
		}



		// Init SDL 
		if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE | SDL_INIT_JOYSTICK) < 0 )
		{
			pLog->Error("SDL", std::string("SDL initialisation failed: ") + SDL_GetError());
			bReturnCode=BREATHE::BAD;
			return BREATHE::BAD;
		}

		{
			// Joysticks
			int nJoysticks = SDL_NumJoysticks();
			std::ostringstream t;
			t << "Joysticks found: ";
			t << nJoysticks;

			if(nJoysticks)
			{
				pLog->Success("SDL", t.str());
				SDL_JoystickEventState(SDL_ENABLE);

				for(int i=0; i < nJoysticks; i++ ) 
				{
					t.str("");
					t << "Joystick(";
					t << i;
					t << ") ";
					t << SDL_JoystickName(i);
					pLog->Success("SDL", t.str());

					//TODO: Create a list of joysticks, close them at the end of the program
					SDL_Joystick *joystick = SDL_JoystickOpen(i);

					t.str("");
					t << "Buttons=";
					t << SDL_JoystickNumButtons(joystick);
					t << ", Axes=";
					t << SDL_JoystickNumAxes(joystick);
					t << ", Hats=";
					t << SDL_JoystickNumHats(joystick);
					t << ", Balls=";
					t << SDL_JoystickNumBalls(joystick);
					pLog->Success("SDL", t.str());

					// ...

					// After we are entirely finished with each joystick close them
					SDL_JoystickClose(joystick);
				}
			}
			else
        pLog->Error("SDL", t.str());

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

		std::string s = (STRING::ToLower(sTitle) + ".ico");
		pLog->Success("SDL", "Setting caption to " + s);
		SDL_WM_SetCaption(sTitle.c_str(), s.c_str());

		// Fetch the video info 
		videoInfo = SDL_GetVideoInfo( );

		if ( !videoInfo )
		{
			pLog->Error("SDL", std::string("Video query failed: ") + SDL_GetError());
			bReturnCode=BREATHE::BAD;
			return BREATHE::BAD;
		}

		
		
		if(pRender->bFullscreen)
			pRender->uiFlags |= SDL_FULLSCREEN;

		// This checks to see if surfaces can be stored in memory 
		if ( videoInfo->hw_available )
			pRender->uiFlags |= SDL_HWSURFACE;
		else
		{
			pRender->uiFlags |= SDL_SWSURFACE;
			pLog->Error("SDL", "SOFTWARE SURFACE");
		}

		// This checks if hardware blits can be done 
		if ( videoInfo->blit_hw )
			pRender->uiFlags |= SDL_HWACCEL;
		else
			pLog->Error("SDL", "SOFTWARE BLIT");

		// Sets up OpenGL double buffering 
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

		// get a SDL surface 
		surface = SDL_SetVideoMode(pRender->uiWidth, pRender->uiHeight, pRender->uiDepth, pRender->uiFlags);

		// Verify there is a surface 
		if(!surface)
		{
			pLog->Error("SDL", std::string("Video mode set failed: ") + SDL_GetError());
			bReturnCode=BREATHE::BAD;
			return BREATHE::BAD;
		}

		if(BREATHE::BAD==ResizeWindow(pRender->uiWidth, pRender->uiHeight))
			return BREATHE::BAD;

		if(BREATHE::BAD==pRender->Init())
			return BREATHE::BAD;

		TTF_Init();

		if(BREATHE::BAD==LoadScene())
			return BREATHE::BAD;

		if(BREATHE::BAD==InitScene())
			return BREATHE::BAD;

		return BREATHE::GOOD;
	}

	
	bool cApp::ToggleFullscreen()
	{
		DestroyTextures();

		if(pRender->bFullscreen)
		{
			pRender->uiFlags &= ~(SDL_FULLSCREEN);
			pRender->bFullscreen = false;
		}
		else
		{
			pRender->uiFlags |= SDL_FULLSCREEN;
			pRender->bFullscreen = true;
		}


		surface=SDL_SetVideoMode(pRender->uiWidth, pRender->uiHeight, pRender->uiDepth, pRender->uiFlags);

		if(surface)
		{
			if(pRender->bFullscreen)
			{
				pLog->Newline();
				pLog->Success("SDL", "Changed to fullscreen");
			}
			else
			{
				pLog->Newline();
				pLog->Success("SDL", "Changed to windowed");
			}

			pRender->Init();
			LoadTextures();

			return BREATHE::GOOD;
		}

		pLog->Error("SDL", std::string("Could not get a surface after toggle full screen: ") + SDL_GetError());

		bReturnCode=BREATHE::BAD;
		
		return BREATHE::BAD;		
	}

	cVar *cApp::VarFind(std::string name)
	{
		std::map<std::string, cVar * >::iterator iter=mVar.begin();

		std::string s;
		while(iter!=mVar.end())
		{
			if(name==iter->first)
				return iter->second;

			iter++;
		};

		return NULL;
	}
	
	void cApp::VarSet(std::string name, std::string value)
	{
		std::map<std::string, cVar * >::iterator iter=mVar.begin();

		std::string s;
		while(iter!=mVar.end())
		{
			if(name==iter->first)
			{
				*(iter->second)=value;
				return;
			}

			iter++;
		};

		mVar[name]=new cVar(value);
	}


	cAppKey::cAppKey(unsigned int code, bool repeat)
	{
		uiCode=code;
		bRepeat=repeat;
		bDown=false;
		bCollected=false;
	}

	bool cApp::ResizeWindow(unsigned int w, unsigned int h)
	{
		// handle resize event 
		surface = SDL_SetVideoMode(w, h, pRender->uiDepth, pRender->uiFlags);
		if ( !surface )
		{
			pLog->Error("SDL", std::string("Could not get a surface after resize: ") + SDL_GetError());
			bReturnCode=BREATHE::BAD;
			return BREATHE::BAD;
		}

		




		// Height / width ration
		GLfloat ratio;

		// Protect against a divide by zero 
		if ( h == 0 )
			h = 1;

		ratio = ( GLfloat )w / ( GLfloat )h;

		// Setup our viewport. 
		glViewport( 0, 0, ( GLint )w, ( GLint )h );

		// change to the projection matrix and set our viewing volume. 
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity( );

		// Set our perspective 
		gluPerspective( 45.0f, ratio, 0.1f, 1000.0f );

		// Make sure we're changing the model view and not the projection 
		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();

		return BREATHE::GOOD;
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
							pLog->Success("Active", "Active");
						else
							pLog->Error("Active", "Inactive");
					}
					break;
				case SDL_VIDEORESIZE:
					if(!ResizeWindow(event.resize.w, event.resize.h))
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
					pLog->Success("SDL", "SDL_Quit: Quiting");
					bDone=true;

					break;
				default:
					break;
			}
		}
	}

	void cApp::AddKey(unsigned int code, bool repeat)
	{
		cAppKey *p=new cAppKey(code, repeat);

		mKey[code]=p;
	}

	bool cApp::IsKeyDown(unsigned int code)
	{
		bool bDown=false;
		cAppKey *p;
		std::map<unsigned int, cAppKey * >::iterator iter=mKey.find(code);
		
		if(iter!=mKey.end())
		{
			p=(iter->second);
			if(p->bDown)
			{
				if(p->bRepeat)
				{
					p->bCollected=true;
					return true;
				}
				else if(!p->bCollected)
				{
					p->bCollected=true;
					return true;
				}

        p->bDown=false;
				p->bCollected=true;
			}
		}

		return false;
	}

	void cApp::UpdateKeys(float fCurrentTime)
	{
		Uint8 *key = SDL_GetKeyState( NULL );

		cAppKey *p;
		std::map<unsigned int, cAppKey * >::iterator iter=mKey.begin();
		
		while(iter!=mKey.end())
		{
			p=(iter->second);

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
	void cApp::OnKeyUp(SDL_keysym *keysym)
	{
		unsigned int code=keysym->sym;

		cAppKey *p;
		std::map<unsigned int, cAppKey * >::iterator iter=mKey.find(code);
		
		if(iter!=mKey.end())
		{
			p=(iter->second);
			
			if(p->bRepeat)
			{
				p->bDown=false;
				p->bCollected=false;
			}
			else if(!bConsole)
			{
				p->bDown=true;
				p->bCollected=false;
			}
		}

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
			ConsoleExecute(pConsole->sLine);
			pConsole->sLine="";
			pConsole->uiCursorPosition=0;
		}
		else if(SDLK_ESCAPE==uiCode || SDLK_BACKQUOTE==uiCode)
		{
			bConsole=false;
			pConsole->sLine="";
			pConsole->uiCursorPosition=0;
		}
		else if(SDLK_DELETE==uiCode)
		{
			if(pConsole->uiCursorPosition<pConsole->sLine.size())
				pConsole->sLine.erase(pConsole->uiCursorPosition, 1);
		}
		else if(SDLK_BACKSPACE==uiCode)
		{
			if(pConsole->uiCursorPosition>0)
			{
				pConsole->sLine.erase(pConsole->uiCursorPosition-1, 1);
				pConsole->uiCursorPosition--;
			}
		}
		else if(SDLK_LEFT==uiCode)
		{
			if(pConsole->uiCursorPosition>0)
        pConsole->uiCursorPosition--;
		}
		else if(SDLK_RIGHT==uiCode)
		{
			if(pConsole->uiCursorPosition<pConsole->sLine.size())
				pConsole->uiCursorPosition++;
		}
		else if(SDLK_HOME==uiCode)
      pConsole->uiCursorPosition=0;
		else if(SDLK_END==uiCode)
      pConsole->uiCursorPosition=pConsole->sLine.size();

		else if(SDLK_TAB==uiCode)
			; //TODO: Autocomplete
		else if(SDLK_CLEAR==uiCode)
			pConsole->sLine="";
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
			s+=static_cast<unsigned char>(uiCode);
			pConsole->sLine.insert(pConsole->uiCursorPosition, s);
			pConsole->uiCursorPosition++;
		}

		//When we press a key we want to see where we are up to
		pConsole->uiCursorBlink=0;
	}
	
	//TODO: use a cVar instead of a constant
	void cApp::ConsoleAddLine(std::string s)
	{
		if(pConsole->lLine.size()>=10)
			pConsole->lLine.pop_front();

		pConsole->lLine.push_back(s);
	}

	//This is for executing one single line, cannot have ";"
	void cApp::ConsoleExecuteSingleCommand(std::string s)
	{
		ConsoleAddLine(s);

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
				std::map<std::string, cVar * >::iterator iter=mVar.begin();

				while(iter!=mVar.end())
				{
					ConsoleAddLine(iter->first + " \"" + std::string(*(iter->second)) + "\"");

					iter++;
				};
			}
			else if(GOOD==Execute(full))
				;
			else if(bDebug)
			{
				unsigned int a=0;
				std::string c;

				c="Line: [";
				for(a=0;a<args.size();a++)
					c+="(" + args[a] + ")";
				c+="]";

				ConsoleAddLine(c);
			}
		}
	}

	//This is for executing multiple lines of commands, seperated by ";"
	void cApp::ConsoleExecute(std::string s)
	{
		if(""!=s)
		{
			//Take out all \n, each line should be finished with a ";" or it is not a valid line
			s=STRING::Replace(s, "\n", " ");

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
          ConsoleExecuteSingleCommand(line);	
			};
		}
	}


	void cApp::MainLoop()
	{
		SDL_WarpMouse(pRender->uiWidth/2, pRender->uiHeight/2);
		//SDL_ShowCursor(SDL_DISABLE);	
		
		
		float fCurrentTime=0.0f;

		unsigned int uiPhysicsHz=30;
		unsigned int uiUpdateHz=30;
		unsigned int uiTargetFramesPerSecond=60;

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
			fCurrentTime=UTIL::GetTime();


      if(fCurrentTime > fEventsNext)
			{
				UpdateEvents(fCurrentTime);

				fEventsNext=fCurrentTime+fEventsDelta;
			}
			
			if(fCurrentTime > fInputNext)
			{
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

			if(bActive)// && fCurrentTime > fRenderNext)
			{
				pRender->BeginFrame(fCurrentTime);
					RenderScene(fCurrentTime);
				pRender->BeginHUD(fCurrentTime);
					RenderHUD(fCurrentTime);
				pRender->EndFrame();

				tRender.Update(fCurrentTime);

				fRenderNext = fCurrentTime + fRenderDelta;
			}
		}while (!bDone);

		SDL_ShowCursor(SDL_ENABLE);	
	}
}
