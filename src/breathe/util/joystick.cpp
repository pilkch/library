// Standard headers
#include <iostream>

// SDL headers
#include <SDL2/SDL.h>

// Spitfire headers
#include <spitfire/util/log.h>

// Breathe headers
#include <breathe/util/joystick.h>

namespace breathe
{
  namespace util
  {
    cJoystickManager::cJoystickManager()
    {
      SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);

      OpenGameControllers();
    }

    cJoystickManager::~cJoystickManager()
    {
      CloseGameControllers();

      SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
    }

    void cJoystickManager::OpenGameControllers()
    {
      size_t nControllers = 0;

      int nJoysticks = SDL_NumJoysticks();
      for (int i = 0; i < nJoysticks; i++) {
        if (SDL_IsGameController(i)) {
          OpenGameController(nControllers);
          nControllers++;
        }
      }
    }

    void cJoystickManager::CloseGameControllers()
    {
      std::map<int, SDL_GameController*>::iterator iter(controllers.begin());
      const std::map<int, SDL_GameController*>::iterator iterEnd(controllers.end());
      while (iter != iterEnd) {
        SDL_GameControllerClose(iter->second);

        iter++;
      }

      controllers.clear();
    }

    void cJoystickManager::OpenGameController(int index)
    {
      std::map<int, SDL_GameController*>::iterator iter = controllers.find(index);
      if (iter == controllers.end()) {
        SDL_GameController* pController = SDL_GameControllerOpen(index);
        controllers[index] = pController;

        const char* szName = SDL_GameControllerName(pController);
        LOG<<"cJoystickManager::OpenGameController Using controller \""<<((szName != nullptr) ? szName : "Unknown controller")<<"\""<<std::endl;
      }
    }

    void cJoystickManager::CloseGameController(int index)
    {
      std::map<int, SDL_GameController*>::iterator iter = controllers.find(index);
      if (iter != controllers.end()) {
        SDL_GameControllerClose(iter->second);
        controllers.erase(iter);
      }
    }

    bool cJoystickManager::IsAttached(SDL_GameController* pController) const
    {
      return (SDL_GameControllerGetAttached(pController) == 1);
    }

    /*
    SDL_CONTROLLER_BUTTON_A,
    SDL_CONTROLLER_BUTTON_B,
    SDL_CONTROLLER_BUTTON_X,
    SDL_CONTROLLER_BUTTON_Y,
    SDL_CONTROLLER_BUTTON_BACK,
    SDL_CONTROLLER_BUTTON_GUIDE,
    SDL_CONTROLLER_BUTTON_START,
    SDL_CONTROLLER_BUTTON_LEFTSTICK,
    SDL_CONTROLLER_BUTTON_RIGHTSTICK,
    SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
    SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
    SDL_CONTROLLER_BUTTON_DPAD_UP,
    SDL_CONTROLLER_BUTTON_DPAD_DOWN,
    SDL_CONTROLLER_BUTTON_DPAD_LEFT,
    SDL_CONTROLLER_BUTTON_DPAD_RIGHT
    Uint8 uiValue = SDL_GameControllerGetButton(pController, SDL_CONTROLLER_BUTTON button);

    // Get the current state of an axis control on a game controller.
    // The state is a value ranging from -32768 to 32767.
    // The axis indices start at index 0.
    SDL_CONTROLLER_AXIS_LEFTX,
    SDL_CONTROLLER_AXIS_LEFTY,
    SDL_CONTROLLER_AXIS_RIGHTX,
    SDL_CONTROLLER_AXIS_RIGHTY,
    SDL_CONTROLLER_AXIS_TRIGGERLEFT,
    SDL_CONTROLLER_AXIS_TRIGGERRIGHT,

    Sint16 iValue = SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS axis);*/

    const char* ControllerAxisName(const SDL_GameControllerAxis axis)
    {
        switch (axis)
        {
          #define AXIS_CASE(ax) case SDL_CONTROLLER_AXIS_##ax: return #ax
            AXIS_CASE(INVALID);
            AXIS_CASE(LEFTX);
            AXIS_CASE(LEFTY);
            AXIS_CASE(RIGHTX);
            AXIS_CASE(RIGHTY);
            AXIS_CASE(TRIGGERLEFT);
            AXIS_CASE(TRIGGERRIGHT);
    #undef AXIS_CASE
            default: return "???";
        }
    }

    const char* ControllerButtonName(const SDL_GameControllerButton button)
    {
        switch (button)
        {
    #define BUTTON_CASE(btn) case SDL_CONTROLLER_BUTTON_##btn: return #btn
            BUTTON_CASE(INVALID);
            BUTTON_CASE(A);
            BUTTON_CASE(B);
            BUTTON_CASE(X);
            BUTTON_CASE(Y);
            BUTTON_CASE(BACK);
            BUTTON_CASE(GUIDE);
            BUTTON_CASE(START);
            BUTTON_CASE(LEFTSTICK);
            BUTTON_CASE(RIGHTSTICK);
            BUTTON_CASE(LEFTSHOULDER);
            BUTTON_CASE(RIGHTSHOULDER);
            BUTTON_CASE(DPAD_UP);
            BUTTON_CASE(DPAD_DOWN);
            BUTTON_CASE(DPAD_LEFT);
            BUTTON_CASE(DPAD_RIGHT);
    #undef BUTTON_CASE
            default: return "???";
        }
    }


    void UpdateInput()
    {
      /*if (SDL_GameControllerGetAttached( gamecontroller ) == 0) return;

      for (i = 0; i <SDL_CONTROLLER_BUTTON_MAX; ++i) {
        if (SDL_GameControllerGetButton(gamecontroller, i) == SDL_PRESSED) {
          DrawRect(screen, i * 34, SCREEN_HEIGHT - 34, 32, 32);
        }
      }

      for (i = 0; i < SDL_CONTROLLER_AXIS_MAX / 2; ++i) {
        // Draw the X/Y axis
        int x, y;
        x = (((int) SDL_GameControllerGetAxis(gamecontroller, i * 2 + 0)) + 32768);
        x *= SCREEN_WIDTH;
        x /= 65535;
        if (x < 0) {
          x = 0;
        } else if (x > (SCREEN_WIDTH - 16)) {
          x = SCREEN_WIDTH - 16;
        }
        y = (((int) SDL_GameControllerGetAxis(gamecontroller, i * 2 + 1)) + 32768);
        y *= SCREEN_HEIGHT;
        y /= 65535;
        if (y < 0) {
          y = 0;
        } else if (y > (SCREEN_HEIGHT - 16)) {
          y = SCREEN_HEIGHT - 16;
        }
      }*/
    }

    void cJoystickManager::HandleSDLEvent(const SDL_Event& event)
    {
      LOG<<"cJoystickManager::HandleSDLEvent"<<std::endl;
      switch (event.type) {
        case SDL_CONTROLLERDEVICEADDED: {
          LOG<<"cJoystickManager::HandleSDLEvent SDL_CONTROLLERDEVICEADDED"<<std::endl;
          const int index = event.cdevice.which;
          OpenGameController(index);
          break;
        }
        case SDL_CONTROLLERDEVICEREMOVED: {
          LOG<<"cJoystickManager::HandleSDLEvent SDL_CONTROLLERDEVICEREMOVED"<<std::endl;
          const int index = event.cdevice.which;
          CloseGameController(index);
          break;
        }
        case SDL_CONTROLLERAXISMOTION:
          LOG<<"cJoystickManager::HandleSDLEvent SDL_CONTROLLERAXISMOTION device "<<event.cdevice.which<<", axis "<<event.caxis.which<<", "<<event.caxis.axis<<" ('"<<ControllerAxisName(static_cast<SDL_GameControllerAxis>(event.caxis.axis))<<"') value: "<<event.caxis.value<<std::endl;
          break;
        case SDL_CONTROLLERBUTTONDOWN:
          LOG<<"cJoystickManager::HandleSDLEvent SDL_CONTROLLERBUTTONDOWN device "<<event.cdevice.which<<", button "<<event.cbutton.which<<", "<<event.cbutton.button<<" ('"<<ControllerButtonName(static_cast<SDL_GameControllerButton>(event.cbutton.button))<<"')"<<std::endl;
          break;
        case SDL_CONTROLLERBUTTONUP:
          LOG<<"cJoystickManager::HandleSDLEvent SDL_CONTROLLERBUTTONUP device "<<event.cdevice.which<<", button "<<event.cbutton.which<<", "<<event.cbutton.button<<" ('"<<ControllerButtonName(static_cast<SDL_GameControllerButton>(event.cbutton.button))<<"')"<<std::endl;
          break;
        default:
          break;
      }
    }
  }
}
