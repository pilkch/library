#ifndef CVEHICLE_PLANE_H
#define CVEHICLE_PLANE_H

#include <breathe/game/component.h>

namespace breathe
{
  namespace game
  {
    // http://en.wikipedia.org/wiki/Fixed-wing_aircraft
    // http://science.howstuffworks.com/airplane1.htm

    class cVehiclePlane : public cVehicleBase
    {
    public:
      explicit cVehiclePlane(cGameObject& object);

    private:
      virtual void _Update(sampletime_t currentTime);
    };
  }
}

#endif // CVEHICLE_PLANE_H
