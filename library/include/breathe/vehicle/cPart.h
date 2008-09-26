#ifndef CVEHICLEPART_H
#define CVEHICLEPART_H

namespace breathe
{
  class c3ds;

  namespace vehicle
  {
    class cVehicle;

    const int VEHICLEPART_NONE=0;
    const int VEHICLEPART_TURBOCHARGER=1;
    const int VEHICLEPART_SUPERCHARGER=2;

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

    /*// Out is the state of the clutch when you are not touching the pedal at all.
    // In is the state of the clutch when you have the clutch fully pushed in down to the floor.

    class cClutch
    {
    public:
      float_t GetClutchPushedInAmount0To1() const { ASSERT(fPushedInAmount0To1 >= 0.0f); ASSERT(fPushedInAmount0To1 <= 1.0f); return fPushedInAmount0To1; }
      void SetPushedInAmount0To1(float_t _fPushedInAmount0To1) { ASSERT(_fPushedInAmount0To1 >= 0.0f); ASSERT(_fPushedInAmount0To1 <= 1.0f); fPushedInAmount0To1 = _fPushedInAmount0To1; }

      float_t CalculateOutputTorqueNm(float_t fInputTorqueNm) const;

    private:
      float_t fMaxTorqueNm; // Maximum torque the clutch can take before it slips.

      float_t fPushedInAmount0To1; // How much is the clutch grabbing at the moment?  0.0f is not grabbing at all, 1.0f is fully grabbing.
    };

    inline float_t cClutch::CalculateOutputTorqueNm(float_t fInputTorqueNm) const
    {
      const float_t fCanHoldTorqueNm = fMaxTorqueNm * fPushedInAmount0To1;
      fInputTorqueNm -= fCanHoldTorqueNm;

      return max(fInputTorqueNm, 0.0f);
    }*/
  }
}

#endif // CVEHICLEPART_H
