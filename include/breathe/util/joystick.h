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
    };
    enum class GAMECONTROLLER_AXIS {
    };

    class cJoystickEventHandler
    {
    public:
      virtual ~cJoystickEventHandler();

      virtual void OnGameControllerConnected(int index) = 0;
      virtual void OnGameControllerDisconnected(int index) = 0;
      virtual void OnGameControllerButtonDown(int index, GAMECONTROLLER_BUTTON button) = 0;
      virtual void OnGameControllerButtonUp(int index, GAMECONTROLLER_BUTTON button) = 0;
    };

    class cJoystickManager
    {
    public:
      cJoystickManager();
      ~cJoystickManager();

      void HandleSDLEvent(const SDL_Event& event);

      void OpenGameControllers();
      void CloseGameControllers();

      float GetAxisValueMinusOneToPlusOne(int index, GAMECONTROLLER_AXIS axis) const;

    private:
      void OpenGameController(int index);
      void CloseGameController(int index);

      bool IsAttached(SDL_GameController* pController) const;

      std::map<int, SDL_GameController*> controllers;
    };
  }
}

#endif // BREATHE_UTIL_JOYSTICK_H
