#ifndef BREATHE_UTIL_JOYSTICK_H
#define BREATHE_UTIL_JOYSTICK_H

// Standard headers
#include <map>

// SDL headers
#include <SDL3/SDL_gamecontroller.h>

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

    class cJoystickManager;

    class cJoystickEvent
    {
    public:
      friend class cJoystickManager;

      cJoystickEvent();

      bool IsConnected() const { return (type == TYPE::CONNECTED); }
      bool IsDisconnected() const { return (type == TYPE::DISCONNECTED); }
      bool IsButtonDown() const { return (type == TYPE::BUTTON_DOWN); }
      bool IsButtonUp() const { return (type == TYPE::BUTTON_UP); }
      bool IsAxisMotion() const { return (type == TYPE::AXIS_MOTION); }

      int GetIndex() const { return index; }
      GAMECONTROLLER_BUTTON GetButton() const { return button; }
      GAMECONTROLLER_AXIS GetAxis() const { return axis; }

    protected:
      enum class TYPE {
        CONNECTED,
        DISCONNECTED,
        BUTTON_DOWN,
        BUTTON_UP,
        AXIS_MOTION
      };
      TYPE type;
      int index;
      GAMECONTROLLER_BUTTON button;
      GAMECONTROLLER_AXIS axis;
    };

    class cJoystickEventListener
    {
    public:
      virtual ~cJoystickEventListener() {}

      virtual void OnJoystickEvent(const cJoystickEvent& event) = 0;
    };

    class cJoystickManager
    {
    public:
      cJoystickManager();
      ~cJoystickManager();

      void SetEventListener(cJoystickEventListener& listener);
      void InvalidateEventListener();

      void HandleSDLEvent(const SDL_Event& event);

      void OpenGameControllers();
      void CloseGameControllers();

      float GetAxisValueMinusOneToPlusOne(int index, GAMECONTROLLER_AXIS axis) const;

    private:
      void OpenGameController(int index);
      void CloseGameController(int index);

      bool IsAttached(SDL_GameController* pController) const;

      cJoystickEventListener* pEventListener;
      
      class cGameController
      {
      public:
        cGameController();

        SDL_GameController* pController;
        SDL_Haptic* pHaptic;
      };

      std::map<int, cGameController> controllers;
    };
  }
}

#endif // BREATHE_UTIL_JOYSTICK_H
