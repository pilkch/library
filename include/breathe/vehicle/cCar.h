#ifndef CCAR_H
#define CCAR_H

#include <breathe/game/component.h>

namespace breathe
{
  namespace game
  {
     namespace car
     {
        class cEngine
        {
        public:
           float_t GetRPM() const { return fRPM; }

           void SetAcceleratorInput0To1(float_t _fAcceleratorInput0To1) { fAcceleratorInput0To1 = _fAcceleratorInput0To1; }

           void Update(sampletime_t currentTime);

        private:
           float_t fAcceleratorInput0To1;
           float_t fRPM;
        };
     }

    class cVehicleCar : public cVehicleBase
    {
    public:
      explicit cVehicleCar(cGameObject& object);

    private:
      virtual void _Update(sampletime_t currentTime);

       car::cEngine engine;

       audio::cSourceRef pSourceEngine;
       audio::cSourceRef pSourceTurbo0;
       audio::cSourceRef pSourceTurbo1;
    };
  }
}

#endif // CCAR_H
