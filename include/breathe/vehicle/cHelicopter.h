#ifndef CHELICOPTER_H
#define CHELICOPTER_H

#include <breathe/vehicle/cVehicle.h>

namespace breathe
{
  namespace game
  {
    // http://en.wikipedia.org/wiki/Helicopter
    // http://science.howstuffworks.com/helicopter.htm
    // http://science.howstuffworks.com/helicopter5.htm
    // Cyclic: Forward, back, left, right (For horizontal movement)
    // Collective: Accelerator and angle of attack of main rotor (For vertical movement)
    // Pedals: Angle of attack of rear rotor (For movement about the z axis)

    // We support
    // Increase/descrease speed of main rotor
    // Increase/descrease angle of attack of main rotor for left, right, forwards and backwards movement
    // Increase/descrease angle of attack of rear rotor

    // We do not support
    // Increase/descrease angle of attack of main rotor for increase/decrease thrust to move vertically without increasing engine speed
    // Probably a whole lot of other less obvious stuff

    class cVehicleHelicopter : public cVehicleBase
    {
    public:
      explicit cVehicleHelicopter(cGameObject& object);

    private:
      virtual void _Init();
      virtual void _Update(durationms_t currentTime);

      audio::cSourceRef pSourceEngine;
    };
  }
}

#endif // CHELICOPTER_H
