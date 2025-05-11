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
    const char* ControllerAxisName(const SDL_GameControllerAxis axis)
    {
      switch (axis) {
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
      switch (button) {
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

    GAMECONTROLLER_AXIS SDLGameControllerAxisToBreatheGameControllerAxis(SDL_GameControllerAxis axis)
    {
      switch (axis)
      {
        #define AXIS_CASE(ax) case SDL_CONTROLLER_AXIS_##ax: return GAMECONTROLLER_AXIS::ax
        AXIS_CASE(LEFTX);
        AXIS_CASE(LEFTY);
        AXIS_CASE(RIGHTX);
        AXIS_CASE(RIGHTY);
        AXIS_CASE(TRIGGERLEFT);
        #undef AXIS_CASE
        default: return GAMECONTROLLER_AXIS::TRIGGERRIGHT;
      }
    }

    GAMECONTROLLER_BUTTON SDLGameControllerButtonToBreatheGameControllerButton(SDL_GameControllerButton button)
    {
      switch (button)
      {
        #define BUTTON_CASE(ax) case SDL_CONTROLLER_BUTTON_##ax: return GAMECONTROLLER_BUTTON::ax
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
        #undef BUTTON_CASE
        default: return GAMECONTROLLER_BUTTON::DPAD_RIGHT;
      }
    }


    // ** cJoystickEvent

    cJoystickEvent::cJoystickEvent() :
      type(TYPE::CONNECTED),
      index(0),
      button(GAMECONTROLLER_BUTTON::A),
      axis(GAMECONTROLLER_AXIS::LEFTX)
    {
    }

    // ** cGameController

    cJoystickManager::cGameController::cGameController() :
      pController(nullptr),
      pHaptic(nullptr)
    {
    }


    // ** cJoystickManager

    cJoystickManager::cJoystickManager() :
      pEventListener(nullptr)
    {
      SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);

      OpenGameControllers();
    }

    cJoystickManager::~cJoystickManager()
    {
      CloseGameControllers();

      SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
    }

    void cJoystickManager::SetEventListener(cJoystickEventListener& listener)
    {
      pEventListener = &listener;
    }

    void cJoystickManager::InvalidateEventListener()
    {
      pEventListener = nullptr;
    }

    void cJoystickManager::OpenGameControllers()
    {
      size_t nControllers = 0;

      const int nJoysticks = SDL_NumJoysticks();
      for (int i = 0; i < nJoysticks; i++) {
        if (SDL_IsGameController(i)) {
          OpenGameController(int(nControllers));
          nControllers++;
        }
      }
    }

    void cJoystickManager::CloseGameControllers()
    {
      std::map<int, cGameController>::iterator iter(controllers.begin());
      const std::map<int, cGameController>::iterator iterEnd(controllers.end());
      while (iter != iterEnd) {
        if (pEventListener != nullptr) {
          cJoystickEvent event;
          event.type = cJoystickEvent::TYPE::DISCONNECTED;
          event.index = iter->first;
          pEventListener->OnJoystickEvent(event);
        }

        if (iter->second.pHaptic != nullptr) SDL_HapticClose((iter->second).pHaptic);

        SDL_GameControllerClose(iter->second.pController);

        iter++;
      }

      controllers.clear();
    }

    void cJoystickManager::OpenGameController(int index)
    {
      std::map<int, cGameController>::iterator iter = controllers.find(index);
      if (iter == controllers.end()) {
        cGameController controller;
        controller.pController = SDL_GameControllerOpen(index);

        const char* szName = SDL_GameControllerName(controller.pController);
        LOG("Using controller \"", ((szName != nullptr) ? szName : "Unknown controller"), "\"");

        SDL_Joystick* pJoystick = SDL_GameControllerGetJoystick(controller.pController);
        if (SDL_JoystickIsHaptic(pJoystick)) {
          SDL_Haptic* pHaptic = SDL_HapticOpenFromJoystick(pJoystick);
          printf("Haptic Effects: %d\n", SDL_HapticNumEffects(pHaptic));
          printf("Haptic Query: %x\n", SDL_HapticQuery(pHaptic));
          if (!SDL_HapticRumbleSupported(pHaptic)) {
            SDL_HapticClose(pHaptic);
            pHaptic = nullptr;
          } else if (SDL_HapticRumbleInit(pHaptic) != 0) {
            printf("Haptic Rumble Init: %s\n", SDL_GetError());
            SDL_HapticClose(pHaptic);
            pHaptic = nullptr;
          } else controller.pHaptic = pHaptic;
        }

        controllers[index] = controller;
        
        if (pEventListener != nullptr) {
          cJoystickEvent event;
          event.type = cJoystickEvent::TYPE::CONNECTED;
          event.index = index;
          pEventListener->OnJoystickEvent(event);
        }
      }
    }

    void cJoystickManager::CloseGameController(int index)
    {
      std::map<int, cGameController>::iterator iter = controllers.find(index);
      if (iter != controllers.end()) {
        if (pEventListener != nullptr) {
          cJoystickEvent event;
          event.type = cJoystickEvent::TYPE::DISCONNECTED;
          event.index = index;
          pEventListener->OnJoystickEvent(event);
        }

        if (iter->second.pHaptic != nullptr) SDL_HapticClose((iter->second).pHaptic);

        SDL_GameControllerClose(iter->second.pController);

        controllers.erase(iter);
      }
    }

    bool cJoystickManager::IsAttached(SDL_GameController* pController) const
    {
      return SDL_GamepadConnected(pController);
    }

    /*
    Uint8 uiValue = SDL_GameControllerGetButton(pController, SDL_CONTROLLER_BUTTON button);

    // Get the current state of an axis control on a game controller.
    // The state is a value ranging from -32768 to 32767.
    // The axis indices start at index 0.
    Sint16 iValue = SDL_GameControllerGetAxis(pController, SDL_CONTROLLER_AXIS axis);*/


    void UpdateInput()
    {
      /*if (SDL_GameControllerGetAttached(gamecontroller)) return;

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

    void cJoystickManager::HandleSDLEvent(const SDL_Event& sdlEvent)
    {
      LOG("");

      switch (sdlEvent.type) {
        case SDL_CONTROLLERDEVICEADDED: {
          LOG("SDL_CONTROLLERDEVICEADDED");
          const int index = sdlEvent.cdevice.which;
          OpenGameController(index);
          break;
        }
        case SDL_CONTROLLERDEVICEREMOVED: {
          LOG("SDL_CONTROLLERDEVICEREMOVED");
          const int index = sdlEvent.cdevice.which;
          CloseGameController(index);
          break;
        }
        case SDL_CONTROLLERBUTTONDOWN: {
          LOG("SDL_CONTROLLERBUTTONDOWN device ", sdlEvent.cdevice.which, ", button ", sdlEvent.cbutton.which, ", ", sdlEvent.cbutton.button, " ('", ControllerButtonName(static_cast<SDL_GameControllerButton>(sdlEvent.cbutton.button)), "')");
          if (pEventListener != nullptr) {
            cJoystickEvent event;
            event.type = cJoystickEvent::TYPE::BUTTON_DOWN;
            event.index = sdlEvent.cdevice.which;
            event.button = SDLGameControllerButtonToBreatheGameControllerButton(static_cast<SDL_GameControllerButton>(sdlEvent.cbutton.button));
            pEventListener->OnJoystickEvent(event);
          }
          break;
        }
        case SDL_CONTROLLERBUTTONUP: {
          LOG("SDL_CONTROLLERBUTTONUP device ", sdlEvent.cdevice.which, ", button ", sdlEvent.cbutton.which, ", ", sdlEvent.cbutton.button, " ('", ControllerButtonName(static_cast<SDL_GameControllerButton>(sdlEvent.cbutton.button)), "')");
          if (pEventListener != nullptr) {
            cJoystickEvent event;
            event.type = cJoystickEvent::TYPE::BUTTON_UP;
            event.index = sdlEvent.cdevice.which;
            event.button = SDLGameControllerButtonToBreatheGameControllerButton(static_cast<SDL_GameControllerButton>(sdlEvent.cbutton.button));
            pEventListener->OnJoystickEvent(event);
          }
          break;
        }
        case SDL_CONTROLLERAXISMOTION: {
          LOG("SDL_CONTROLLERAXISMOTION device " , sdlEvent.cdevice.which , ", axis " , sdlEvent.caxis.which , ", " , sdlEvent.caxis.axis , " ('" , ControllerAxisName(static_cast<SDL_GameControllerAxis>(sdlEvent.caxis.axis)) , "') value: " , sdlEvent.caxis.value);
          if (pEventListener != nullptr) {
            cJoystickEvent event;
            event.type = cJoystickEvent::TYPE::AXIS_MOTION;
            event.index = sdlEvent.cdevice.which;
            event.axis = SDLGameControllerAxisToBreatheGameControllerAxis(static_cast<SDL_GameControllerAxis>(sdlEvent.caxis.axis));
            pEventListener->OnJoystickEvent(event);
          }
          break;
        }
        default:
          break;
      }
    }
  }
}
