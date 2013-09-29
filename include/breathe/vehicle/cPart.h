#ifndef CVEHICLEPART_H
#define CVEHICLEPART_H

#include <spitfire/math/cCurve.h>

namespace breathe
{
  class c3ds;

  namespace vehicle
  {
    class cVehicle2;

    // Types for all of the parts that can be used in a car
    // These are for internal use only and should not be loaded from a file etc.
    // Only a name like "<wheel/>", "<engine/>" should be used in the file as these could change at any time

    // Parts directly related to the drive train
    const unsigned int PART_ENGINE = 0;
    const unsigned int PART_CAMSHAFT = 1;
    const unsigned int PART_CRANKSHAFT = 2;
    const unsigned int PART_SUPER_CHARGER = 3;
    const unsigned int PART_TURBO_CHARGER = 4;
    const unsigned int PART_FLYWHEEL = 5;
    const unsigned int PART_CLUTCH = 6;
    const unsigned int PART_GEARBOX = 7;
    const unsigned int PART_DRIVESHAFT = 8;
    const unsigned int PART_DIFFERENTIAL = 9;
    const unsigned int PART_DIFFERENTIAL_AWD_2ND_DIFFERENTIAL = 10;
    const unsigned int PART_WHEELS = 11;
    const unsigned int PART_TIRES = 12;

    // Parts that relate more to the ride and dynamics of the vehicle
    const unsigned int PART_SUPENSION = 13;
    const unsigned int PART_BRAKES = 14;
    const unsigned int PART_PETROL_TANK = 15;

    // The control module is a fictional part that basically controls the state of the car and acts as
    // the controller/brains of the previously mentioned 'dumb' parts
    const unsigned int PART_CONTROL_MODULE = 16;

    class cPart
    {
    public:
      virtual ~cPart() {}

      void Install(cVehicle2* p);
      void Update(durationms_t currentTime) { _Update(currentTime); }

      bool IsValid() const { return _IsValid(); }

      id_t uniqueID;

      unsigned int uiType;
      float fHealth; //Starts at 100.0f and gradually goes down

      cVehicle2* pVehicle;

      c3ds* pModelNew;
      c3ds* pModelBroken;

    protected:
      explicit cPart(unsigned int uiType);

    private:
      virtual bool _IsValid() const { return true; }
      virtual void _Update(durationms_t currentTime)=0;
    };

    enum class ENGINE_TYPE {
      FOUR_CYLINDER_INLINE,
      FOUR_CYLINDER_V,
      FOUR_CYLINDER_BOXER,
      SIX_CYLINDER_INLINE,
      SIX_CYLINDER_V,
      SIX_CYLINDER_BOXER,
      EIGHT_CYLINDER_V,
      TEN_CYLINDER_V,
      TWELVE_CYLINDER_V,
      SIXTEEN_CYLINDER_W
    };

    // engine
    // engine rpm
    // engine rpm : torque nm curve

    // engine_rpm -> superchargerrpm -> boostpsi
    class cPartEngine : public cPart
    {
    public:
      cPartEngine();

      void SetRPM(float_t _fRPM) { fRPM = _fRPM; }
      float_t GetTorqueNm() const { return fTorqueNm; }

    private:
      void _Update(durationms_t currentTime);

      inline float_t GetTorqueNmAtEngineRPM(float_t fEngineRPM) const { return curveEngineRPMToTorqueNm.GetYAtPointX(fEngineRPM); }

      ENGINE_TYPE type;
      float_t fRPM;
      float_t fTorqueNm;
      spitfire::math::cCurve curveEngineRPMToTorqueNm;
      float_t fCompressionRatio; // This might be for example 9.0:1 which is fCompressionRatio = 9.0f / 1.0f;
    };

    // turbo charger inputs:
    // engine exhaust pressure?
    // engine exhaust pressure : turbocharger rpm curve
    // turbocharger rpm : boost psi curve

    // engine_exhaustpsi -> turbochargerrpm -> boostpsi
    class cPartTurboCharger : public cPart
    {
    public:
      cPartTurboCharger();

    private:
      void _Update(durationms_t currentTime);

      inline float_t GetTurboChargerRPMAtEngineExhaustpsi(float_t fEngineExhaustpsi) const { return curveTurboChargerRPMAtEngineExhaustpsi.GetYAtPointX(fEngineExhaustpsi); }
      inline float_t GetBoostpsiAtTurboChargerRPM(float_t fTurboChargerRPM) const { return curveBoostpsiAtTurboChargerRPM.GetYAtPointX(fTurboChargerRPM); }

      float_t fRPM;
      spitfire::math::cCurve curveTurboChargerRPMAtEngineExhaustpsi;
      spitfire::math::cCurve curveBoostpsiAtTurboChargerRPM;
    };



    // Typically 8-12psi, can go to say, 15-25psi in a top dragster
    // OutputBoostpsi:  0.0f  2.0f 4.0f   3.0     2.5f,    2.0f,    1.0f,    0.0f
    // SuperChargerRPM: 0.0f  1.0f 800.0f 1500.0f 2000.0f, 2500.0f, 5000.0f, 10000.0f

    // supercharger
    // engine rpm
    // supercharger rpm
    // supercharger rpm : boost psi curve

    // engine_rpm -> superchargerrpm -> boostpsi
    class cPartSuperCharger : public cPart
    {
    public:
      cPartSuperCharger();

    private:
      void _Update(durationms_t currentTime);

      inline float_t GetBoostpsiAtSuperChargerRPM(float_t fSuperChargerRPM) const { return curveSuperChargerRPMToBoostpsi.GetYAtPointX(fSuperChargerRPM); }

      float_t fRPM;
      float_t fSuperChargerRPMToEngineRPMRatio; // ie. 6:1 = 6.0f
      spitfire::math::cCurve curveSuperChargerRPMToBoostpsi;
    };


    class cBrakes
    {
    public:
      enum class BRAKE_TYPE {
        IRON,
        IRON_CROSS_DRILLED,
        CARBON_CERAMIC,
        CARBON_CERAMIC_CROSS_DRILLED
      };

      BRAKE_TYPE GetBrakeType() const { return brakeType; }
      float_t GetDiametermm() const { return fDiametermm; }
      float_t GetMaximumBrakeForceNm() const;

    private:
      bool _IsValid() const;

      BRAKE_TYPE brakeType;
      float_t fDiametermm;
    };

    inline bool cBrakes::_IsValid() const
    {
      // More than 10cm and less than 100cm
      const bool bIsValidDiameter = (fDiametermm > 100.0f) && (fDiametermm < 1000.0f);

      return bIsValidDiameter;
    }

    inline float_t cBrakes::GetMaximumBrakeForceNm() const
    {
      // Here we basically just generate a number that feels good
      const float n = fDiametermm * fDiametermm;
      switch (brakeType) {
        case BRAKE_TYPE::CARBON_CERAMIC: return 2.5f * n;
        case BRAKE_TYPE::CARBON_CERAMIC_CROSS_DRILLED: return 2.3f * n;
        case BRAKE_TYPE::IRON: return 1.6f * n;
        case BRAKE_TYPE::IRON_CROSS_DRILLED: return 1.4f * n;
      };

      return 1.0f * n;
    }








    // ** cVehicleControlModule

    // Fictional class to contain all drivers aids in one easy to find place

    // Driver aids - automatic shifting
    // Automatic shifting is possible by setting engine.shifting.automatic to 1. See Gearbox shifting for more details.

    // Driver aids - traction control
    // Traction control can be used to avoid spinning the (often rear) wheels while accelerating. It is allowed in
    // certain classes to implement this. When the wheel speeds differ too much, TC kicks in and cuts the throttle,
    // until the wheel speed ratio returns to acceptable levels. The following variables determine the behavior:
    // * traction_control.max_velocity_ratio; determines the ratio of the front vs. rear wheel speed that is
    // allowed without TC intervening. A typical value is 1.1, meaning to allow 10% of speed difference (the rear
    // wheels spinning 10% faster than the fronts).
    // * traction_control.min_velocity_ratio; when TC kicks in, this is the ratio that is needed to let the
    // throttle be accepted again. A typical value according to an F1 site explanation is 1.0, although this seems not
    // to work on frontdrive cars; there actually while braking the ratio may not go below 1.0, so a value of 1.05 is
    // a better starting value. A symptom of this problem is if you have a FWD car which now & then (especially in AI)
    // seems to slow down far too long, as if the throttle is cut. Check the controller debug screen (^4) and see if
    // TC is active for prolonged periods of time.
    // * traction_control.enable; if 1, TC is enabled for this car.
    // * tcs.max_velocity_ratio; determines the ratio of the front vs. rear wheel speed that is allowed without TCS intervening. A typical value is 1.1, meaning to allow 10% of speed difference (the rear wheels spinning 10% faster than the fronts).
    // * tcs.min_velocity_ratio; when TCS kicks in, this is the ratio that is needed to let the throttle be accepted again. A typical value according to an F1 site explanation is 1.0, although a value slightly between 1.0 and max_velocity_ratio may be more appropriate.
    // * tcs.enable; if 1, TCS is enabled for this car.

    // Driver aids - ABS
    // A typical ABS is composed of a central electronic control unit (ECU), four wheel speed sensors (one for each wheel), and two or more hydraulic
    // valves within the vehicle brake circuit. The ECU constantly monitors the rotational speed of each wheel. When it senses that any number of
    // wheels are rotating considerably slower than the others (a condition that is likely to bring it to lock - see note below), it actuates the valves
    // to decrease the pressure on the specific braking circuit for the individual wheel, effectively reducing the braking force on that wheel. The
    // wheel(s) then turn faster; when they turn too fast, the force is reapplied. This process is repeated continuously, and this causes the characteristic
    // pulsing feel through the brake pedal. A typical anti-lock system can apply and release braking pressure up to 20 times a second.

    class cVehicleControlModule : public cPart
    {
    public:
      cVehicleControlModule();

      bool IsAutomaticShifting() const { return bAutomaticShifting; }
      bool IsAutomaticClutching() const { return bAutomaticClutching; }
      bool IsTractionControl() const { return bTractionControl; }
      bool IsAntiLockBrakingSystem() const { return bAntiLockBrakingSystem; }

      size_t GetWheelCount() const { return 4; }

      void Update(durationms_t currentTime);

    private:
      bool _IsValid() const;

      void GetSlowestAndFastestWheelSpeedsRPM(float_t& fSlowest, float_t& fFastest);

      const float fMaximumTCSSlipRatio;

      // The vehicle artificially stops the engine wrecking itself at this RPM
      // float_t fAccelerator = GetInput0To1();
      // if (fRPM > fRevLimitRPM) fAccelerator = 0.0f;
      bool bRevLimiter;
      float_t fRevLimitRPM; // 0.0f-20000.0f

      // The vehicle automatically shifts up at this RPM
      // if (fRPM > fAutomaticShiftingRPM) SetStateShiftUp(bAutomaticClutching);
      bool bAutomaticShifting;
      float_t fAutomaticShiftingRPM; // 0.0f-20000.0f

      // Without this if you don't use the clutch when changing gears nothing will happen, the gear will not be changed
      bool bAutomaticClutching;

      // The vehicle will allow slip up to a maximum of (1.0f - fTractionControl) * fMaximumTCSSlipRatio;
      // float_t fAccelerator = GetInput0To1();
      // float_t fSlowestWheel = math::infinity;
      // float_t fFastestWheel = -math::infinity;
      // const size_t n = GetWheelCount();
      // for (size_t i = 0; i < n; i++) {
      //   const float fWheelRPM = wheel[i].GetRPM();
      //   if (fWheelRPM < fSlowestWheel) fSlowestWheel = fWheelRPM;
      //   if (fWheelRPM > fFastestWheel) fFastestWheel = fWheelRPM;
      // }
            //
      // if (fSlowestWheel > 0.0f) {
      //   const float_t fRatio = fFastestWheel / fSlowestWheel;
      //   if (fRatio > 1.2) fAccelerator = 0.0f;
      // }
      bool bTractionControl;
      float_t fTractionControl; // 0.0f-1.0f

      // This will detect if any of the wheels are spinning subtantially less than the rest and actually let go of
      // the brake on that wheel if it is applied
      // float_t fLetGo = fAntiLockBrakingSystem;
      bool bAntiLockBrakingSystem;
      float_t fAntiLockBrakingSystem; // 0.0f-1.0f
    };





    // ** cDriveTrain

    // In a drivetrain there are n gears and n + 1 bodies between them.  The gears each have a ratio, the bodies each have an inertia

    // A drivetrain is setup like so:
    // body_inertia -> gear_ratio -> body_intertia -> gear_ratio -> body_intertia

    // Usage:
    // cDriveTrain d;
    // d.SetFirstBodyInertia(fInertia0);
    // d.AddGear(fRatio0, fInertia1);
    // d.AddGear(fRatio1, fInertia2);
    // float fEffectiveInertia = d.GetEffectiveIntertia();
    // float fRotationalAcceleration = d.GetRotationalAccelerationFromTorque(fTorque);

    class cDriveTrain
    {
    public:
      void SetFirstBodyInertia(float fInertia) { gearRatio.clear(); bodyInertia.clear(); bodyInertia.push_back(fInertia); }

      // fRatio is a ratio ie. 0.6
      // fInertia is in kg*m^2
      void AddGear(float fGearRatio, float fInertiaOfBodyAfterGear)
      {
        gearRatio.push_back(fGearRatio);
        bodyInertia.push_back(fInertiaOfBodyAfterGear);
      }

      float GetEffectiveIntertia() const
      {
        ASSERT(!gearRatio.empty());
        ASSERT(!bodyInertia.empty());

        // Make sure that nInertias = nGears + 1
        ASSERT((gearRatio.size() + 1) == bodyInertia.size());

        const size_t n = gearRatio.size();
        if (n == 1) return GetEffectiveInertiaFor2Bodies1Gears();
        else if (n == 2) return GetEffectiveInertiaFor3Bodies2Gears();

        return GetEffectiveInertiaFor4Bodies3Gears();
      }

      float GetRotationalAccelerationFromTorque(float fTorque) const
      {
        ASSERT(!gearRatio.empty());
        ASSERT(!bodyInertia.empty());

       // Make sure that nInertias = nGears + 1
        ASSERT((gearRatio.size() + 1) == bodyInertia.size());

        const size_t n = gearRatio.size();
        if (n == 1) return GetRotationalAccelerationFromTorqueFor2Bodies1Gears(fTorque);
        else if (n == 2) return GetRotationalAccelerationFromTorqueFor3Bodies2Gears(fTorque);

        return GetRotationalAccelerationFromTorqueFor4Bodies3Gears(fTorque);
      }

    private:
      // Requires GearRatio* gear ratios, i* inertia values
      float GetEffectiveInertiaFor2Bodies1Gears() const;
      float GetEffectiveInertiaFor3Bodies2Gears() const;
      float GetEffectiveInertiaFor4Bodies3Gears() const;

      float GetRotationalAccelerationFromTorqueFor2Bodies1Gears(float fTorque) const;
      float GetRotationalAccelerationFromTorqueFor3Bodies2Gears(float fTorque) const;
      float GetRotationalAccelerationFromTorqueFor4Bodies3Gears(float fTorque) const;
      //float GetRotationalAccelerationFromTorqueFor6Bodies5Gears(float fTorque) const;

      std::vector<float> gearRatio;
      std::vector<float> bodyInertia;
    };
  }
}

#endif // CVEHICLEPART_H
