#ifndef CVEHICLEPART_H
#define CVEHICLEPART_H

namespace breathe
{
  class c3ds;

  namespace vehicle
  {
    class cVehicle;

    const int VEHICLEPART_NONE = 0;
    const int VEHICLEPART_TURBOCHARGER = 1;
    const int VEHICLEPART_SUPERCHARGER = 2;

    class cPart
    {
    public:
      typedef uint32_t id_t;

      cPart();

      void Install(cVehicle* p);
      void Update(sampletime_t currentTime) { _Update(currentTime); }

      unsigned int uiType;
      float fHealth; //Starts at 100.0f and gradually goes down

      cVehicle* pVehicle;

      c3ds* pModelNew;
      c3ds* pModelBroken;

    private:
      virtual void _Update(sampletime_t currentTime)=0;
    };

    class cPartTurboCharger : public cPart
    {
    public:
      cPartTurboCharger();

      void Install(cVehicle *p);

    private:
      void _Update(sampletime_t currentTime);
    };

    class cPartSuperCharger : public cPart
    {
    public:
      cPartSuperCharger();

      void Install(cVehicle *p);

    private:
      void _Update(sampletime_t currentTime);
    };
  }
}

#endif // CVEHICLEPART_H
