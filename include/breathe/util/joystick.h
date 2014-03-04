#ifndef BREATHE_UTIL_JOYSTICK_H
#define BREATHE_UTIL_JOYSTICK_H

// Standard headers
#include <map>

// SDL headers
#include <SDL2/SDL_gamecontroller.h>

// Breathe headers
#include <breathe/breathe.h>

namespace breathe
{
  namespace util
  {
    enum class GAMECONTROLLER_BUTTON {
      A,
      B,
      X,
      Y,
      BACK,
      GUIDE,
      START,
      LEFTSTICK,
      RIGHTSTICK,
      LEFTSHOULDER,
      RIGHTSHOULDER,
      DPAD_UP,
      DPAD_DOWN,
      DPAD_LEFT,
      DPAD_RIGHT
    };

    enum class GAMECONTROLLER_AXIS {
      LEFTX,
      LEFTY,
      RIGHTX,
      RIGHTY,
      TRIGGERLEFT,
      TRIGGERRIGHT,
    };

    class cJoystickEventListener
    {
    public:
      virtual ~cJoystickEventListener() {}

      virtual void OnGameControllerConnected(int index) = 0;
      virtual void OnGameControllerDisconnected(int index) = 0;
      virtual void OnGameControllerButtonDown(int index, GAMECONTROLLER_BUTTON button) = 0;
      virtual void OnGameControllerButtonUp(int index, GAMECONTROLLER_BUTTON button) = 0;
      virtual void OnGameControllerAxisMotion(int index, GAMECONTROLLER_AXIS axis) = 0;
    };

    class cJoystickManager
    {
    public:
      cJoystickManager();
      ~cJoystickManager();

      void SetEventListener(cJoystickEventListener& listener);

      void HandleSDLEvent(const SDL_Event& event);

      void OpenGameControllers();
      void CloseGameControllers();

      float GetAxisValueMinusOneToPlusOne(int index, GAMECONTROLLER_AXIS axis) const;

    private:
      void OpenGameController(int index);
      void CloseGameController(int index);

      bool IsAttached(SDL_GameController* pController) const;

      cJoystickEventListener* pEventListener;

      std::map<int, SDL_GameController*> controllers;
    };
  }
}

#endif // BREATHE_UTIL_JOYSTICK_H
