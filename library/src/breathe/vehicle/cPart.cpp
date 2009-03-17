#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <list>
#include <set>

// Boost headers
#include <boost/smart_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <ode/ode.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>
#include <spitfire/util/unittest.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/geometry.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>


#include <breathe/game/cLevel.h>

#include <breathe/physics/physics.h>

#include <breathe/game/cPlayer.h>
#include <breathe/game/cPetrolBowser.h>
#include <breathe/vehicle/cPart.h>
#include <breathe/vehicle/cVehicle.h>

namespace breathe
{
  namespace vehicle
  {
    cCurve::cCurve()
    {
      AddPoint(0.0f, 0.0f);
    }

    // x has to be >= 0.0f, it can however go past the last point in the list, it will become the new last point
    // y can be any value it likes
    void cCurve::AddPoint(float_t fX, float_t fY)
    {
      ASSERT(fX >= 0.0f);

      points[fX] = fY;
    }

    float_t cCurve::GetYAtPointX(float_t fMu) const
    {
      if (fMu < 0.0f) return 0.0f;

      float_t fX0 = 0.0f;
      float_t fX1 = 0.0f;
      float_t fY0 = 0.0f;
      float_t fY1 = 0.0f;

      bool bFound = false;

      std::map<float_t, float_t>::const_iterator iter(points.begin());
      const std::map<float_t, float_t>::const_iterator iterEnd(points.end());
      while (iter != iterEnd) {
        const float_t fCurrentX = iter->first;
        const float_t fCurrentY = iter->second;

        // Shuffle the previous values
        fX0 = fX1;
        fX1 = fCurrentX;

        fY0 = fY1;
        fY1 = fCurrentY;

        if (fCurrentX > fMu) {
          // We have found the pair that we are between
          bFound = true;
          break;
        }

        iter++;
      };

      if (!bFound) return 0.0f;

      // Finally return the y value at our x value
      return math::interpolate_linear(fX0, fY0, fX1, fY1, fMu);
    }


    // Out is the state of the clutch when you are not touching the pedal at all.
    // In is the state of the clutch when you have the clutch fully pushed in down to the floor.

    class cClutch
    {
    public:
      float_t GetClutchPushedInAmount0To1() const { ASSERT(fPushedInAmount0To1 >= 0.0f); ASSERT(fPushedInAmount0To1 <= 1.0f); return fPushedInAmount0To1; }
      void SetPushedInAmount0To1(float_t _fPushedInAmount0To1) { ASSERT(_fPushedInAmount0To1 >= 0.0f); ASSERT(_fPushedInAmount0To1 <= 1.0f); fPushedInAmount0To1 = _fPushedInAmount0To1; }

      float_t CalculateOutputTorqueNm(float_t fInputTorqueNm) const;

    private:
      void _Update(sampletime_t currentTime);

      float_t fMaxTorqueNm; // Maximum torque the clutch can take before it slips.

      float_t fPushedInAmount0To1; // How much is the clutch grabbing at the moment?  0.0f is not grabbing at all, 1.0f is fully grabbing.
    };

    float_t cClutch::CalculateOutputTorqueNm(float_t fInputTorqueNm) const
    {
      const float_t fCanHoldTorqueNm = fMaxTorqueNm * fPushedInAmount0To1;
      fInputTorqueNm -= fCanHoldTorqueNm;

      return max<float_t>(fInputTorqueNm, 0.0f);
    }

    class cGear
    {
    public:
      explicit cGear(bool bIsNeutral);

    // A valid gear is any gear that is neutral or reverse or neither, not both
      bool IsValid() const { return !(IsNeutral() && IsReverse()); }

      bool IsNeutral() const { return bIsNeutral; }
      bool IsReverse() const { return fRatio < breathe::math::cEPSILON; }

      float_t GetRatio() const { return fRatio; }
      float_t GetInertia() const { return fInertia; }

      void SetRatio(float_t fRatio);
      void SetInertia(float_t fInertia);

    private:
      bool bIsNeutral;

      float_t fRatio;
      float_t fInertia;
    };

    cGear::cGear(bool _bIsNeutral) :
      fRatio(1.0f),
      fInertia(1.0f)
    {
      bIsNeutral = _bIsNeutral;

      SetRatio(1.0f);
    }

    void cGear::SetRatio(float_t _fRatio)
    {
      // If we are not neutral then we can set our own ratio
      fRatio = (bIsNeutral) ? 0.0f : _fRatio;
    }

    void cGear::SetInertia(float_t _fInertia)
    {
      fInertia = _fInertia;
    }






    // Each gear has a different inertia. As lower gears have more teeth and are larger, the lower gears also have bigger inertia values.
    // They are given in Kg*m^2. Note that the inertia has a squared influence on the total drive train.

    // float fAutomaticTotalTimeInMS =  (1.5f * timeClutchDelayMS) + (2.0f * timeShiftDelayMS) + (1.5f * timeClutchDelayMS);
    // float fSemiAutomaticTotalTimeInMS =  timeClutchDelayMS + (1.5f * timeShiftDelayMS) + timeClutchDelayMS;
    // float fManualTotalTimeInMS = timeShiftDelayMS; // Plus we have already pressed the clutch in and out ourselves (It is not included in this calculation)

    // Each gearbox has:
    // 0 or 1 reverse gears,
    // 0 or 1 neutral gears,
    // 1 to n forwards gears

    // Gear order
    // [r] [n] 1 [2] [3] [4] [5] [6] [..] [n]

    class cGearbox
    {
    public:
      friend class cGearboxController;

      enum STATE
      {
        STATE_IN_GEAR = 0, // When we are in a gear already, the clutch may or may not be in at the moment
        STATE_SHIFTING_GEAR // Currently the clutch is (probably) in and we are actually changing gear ie. between 1st and 2nd
      };

      cGearbox();

      void AddGear(float ratio, float inertia);

      float_t GetInputRPM() const { return fInputRPM; }

      bool HasReverse() const { return bHasReverseGear; }
      bool HasNeutral() const { return bHasNeutralGear; }

      bool IsInReverse() const;
      bool IsInNeutral() const;
      bool IsInForwardsGear() const { return !(IsInReverse() || IsInNeutral()); }

      STATE GetState() const { return state; }

      // Total number of gears including reverse and neutral
      size_t GetNumberOfGears() const { return gears.size(); }

      // Return 0..n of all the gears, ie. if we have a reverse and currentGear == 0 then we are in reverse
      size_t GetCurrentGearIndex() const { return currentGear; }

      // Returns 1..n+1 of the forward gear we are currently in (For use in displaying to the user, ie. at the start line 1 will be displayed on the screen)
      size_t GetCurrentGearNumber() const;

      const cGear& GetCurrentGear() const;

      float GetGearRatio(size_t gear) const;
      float GetGearInertia(size_t gear) const;

      void ShiftUp();
      void ShiftDown();

      void Update(sampletime_t currentTime);

      cClutch clutch;

    private:
      void GetRatioAndInertia(size_t gear, float& ratio, float& inertia) const;

      void _SetTargetGear(size_t gear);

      //std::vector<cGear*> gears;

      // First is ratio
      // Second is inertia
      std::map<float, float> gears;
      bool bHasReverseGear;
      bool bHasNeutralGear;

      float timeShiftDelayMS; // How long each actual shift takes

      size_t currentGear;
      size_t targetGear;

      STATE state;
      sampletime_t stateEnteredTime;

      float_t fInputRPM;
    };

    cGearbox::cGearbox() :
      bHasReverseGear(false),
      bHasNeutralGear(false),
      timeShiftDelayMS(200), // How long each shift takes
      state(STATE_IN_GEAR),
      fInputRPM(1000.0f)
    {
    }

    // Returns true if we have a reverse gear and we are in gear 0
    bool cGearbox::IsInReverse() const
    {
      return bHasReverseGear && (currentGear == 0);
    }

    // Returns true if we:
    // Have a neutral gear and no reverse and are in gear 0 or
    // Have a neutral gear and a reverse gear and are in gear 1
    bool cGearbox::IsInNeutral() const
    {
      return bHasNeutralGear && ((!bHasReverseGear && (currentGear == 0)) || (bHasReverseGear && (currentGear == 1)));
    }

    size_t cGearbox::GetCurrentGearNumber() const
    {
      ASSERT(IsInForwardsGear());
      size_t gear = currentGear + 1; // + 1 because we are using this number for displaying remember so it will be 1..n+1 based not 0..n
      if (bHasReverseGear) gear--;
      if (bHasNeutralGear) gear--;

      // This cannot equal 0 at the moment, it should be in the range of 1..n+1
      ASSERT(gear != 0);
      return gear;
    }


    void cGearbox::ShiftUp()
    {
      ASSERT(!gears.empty());
      _SetTargetGear(targetGear + 1);
    }

    void cGearbox::ShiftDown()
    {
      ASSERT(!gears.empty());
      if (targetGear != 0) _SetTargetGear(targetGear - 1);
    }

    void cGearbox::_SetTargetGear(size_t gear)
    {
      ASSERT(!gears.empty());
      const size_t n = gears.size();
      if (gear < n) targetGear = gear;
      else if(n != 0) targetGear = n - 1;
      else targetGear = 0;
    }

    void cGearbox::GetRatioAndInertia(size_t gear, float& ratio, float& inertia) const
    {
      ratio = 0.0f;
      inertia = 0.0f;

      size_t i = 0;
      std::map<float, float>::const_iterator iter(gears.begin());
      const std::map<float, float>::const_iterator iterEnd(gears.end());
      while (iter != iterEnd) {
        if (i == gear) {
          ratio = iter->first;
          inertia = iter->second;
        }

        i++;
        iter++;
      }
    }

    float cGearbox::GetGearRatio(size_t gear) const
    {
      float ratio;
      float inertia;
      GetRatioAndInertia(gear, ratio, inertia);

      return ratio;
    }

    float cGearbox::GetGearInertia(size_t gear) const
    {
      float ratio;
      float inertia;
      GetRatioAndInertia(gear, ratio, inertia);

      return inertia;
    }

    void cGearbox::Update(sampletime_t currentTime)
    {
      ASSERT(!gears.empty());

      ASSERT(targetGear <= gears.size());
      ASSERT(currentGear <= gears.size());

      if (state == STATE_IN_GEAR) {
        // If we are already in the gear that we want then return
        if (targetGear == currentGear) return;

        // We are in a gear other than the target gear, better start shifting

      }
    }




    class cGearboxController
    {
    public:
      explicit cGearboxController(cGearbox& gearbox);

      void InjectInputClutch(float_t fValue0To1) { _InjectInputClutch(fValue0To1); }
      void InjectInputChangeUp() { _InjectInputChangeUp(); }
      void InjectInputChangeDown() { _InjectInputChangeDown(); }

      void Update(sampletime_t currentTime) { _Update(currentTime); }

    protected:
      cGearbox& gearbox;

    private:
      virtual void _InjectInputClutch(float_t fValue0To1) {}
      virtual void _InjectInputChangeUp() {}
      virtual void _InjectInputChangeDown() {}

      virtual void _Update(sampletime_t currentTime) = 0;
    };

    cGearboxController::cGearboxController(cGearbox& _gearbox) :
      gearbox(_gearbox)
    {
    }



    class cGearboxManual : public cGearboxController
    {
    public:
      explicit cGearboxManual(cGearbox& gearbox);

    private:
      virtual void _InjectInputClutch(float_t fValue0To1);
      virtual void _InjectInputChangeUp();
      virtual void _InjectInputChangeDown();
    };

    cGearboxManual::cGearboxManual(cGearbox& _gearbox) :
      cGearboxController(_gearbox)
    {
    }

    void cGearboxManual::_InjectInputClutch(float_t fValue0To1)
    {
      gearbox.clutch.SetPushedInAmount0To1(fValue0To1);
    }

    void cGearboxManual::_InjectInputChangeUp()
    {
      gearbox.ShiftUp();
    }

    void cGearboxManual::_InjectInputChangeDown()
    {
      gearbox.ShiftDown();
    }


    class cGearboxAutomatic : public cGearboxController
    {
    public:
      explicit cGearboxAutomatic(cGearbox& gearbox);

      void SetChangeUpAtRPM(float_t fRPM) { fChangeUpAtRPM = fRPM; }
      void SetChangeDownAtRPM(float_t fRPM) { fChangeDownAtRPM = fRPM; }

    private:
      virtual void _InjectInputChangeUp();
      virtual void _InjectInputChangeDown();

      void _Update(sampletime_t currentTime);

      void InternalChangeUp();
      void InternalChangeDown();
      void HandlePotentialChanging();

      size_t targetGearIndex;

      float_t fChangeUpAtRPM; // At what RPM does the automatic transmission change up?
      float_t fChangeDownAtRPM; // At what RPM does the automatic transmission change down?
      sampletime_t timeClutchPushInDelayMS; // How long it takes to push the clutch in from fully let out
      sampletime_t timeClutchLetOutDelayMS; // How long it takes to let the clutch out
    };

    cGearboxAutomatic::cGearboxAutomatic(cGearbox& _gearbox) :
      cGearboxController(_gearbox),
      targetGearIndex(0)
    {
    }

    void cGearboxAutomatic::_InjectInputChangeUp()
    {
      if (!gearbox.IsInForwardsGear()) InternalChangeUp();
    }

    void cGearboxAutomatic::_InjectInputChangeDown()
    {
      if (gearbox.HasReverse() && !gearbox.IsInForwardsGear()) InternalChangeDown();
    }

    void cGearboxAutomatic::InternalChangeUp()
    {
      size_t i = gearbox.GetCurrentGearIndex();
      const size_t n = gearbox.GetNumberOfGears();
      i++;
      if (i < n) targetGearIndex = i;
    }

    void cGearboxAutomatic::InternalChangeDown()
    {
      const size_t i = gearbox.GetCurrentGearIndex();
      if (i != 0) targetGearIndex = i - 1;
    }

    void cGearboxAutomatic::HandlePotentialChanging()
    {
      const float_t fRPM = gearbox.GetInputRPM();

      if (fRPM > fChangeUpAtRPM) InternalChangeUp();
      else if (fRPM < fChangeDownAtRPM) InternalChangeDown();
    }

    void cGearboxAutomatic::_Update(sampletime_t currentTime)
    {
      cGearbox::STATE state = gearbox.GetState();

      if (state == cGearbox::STATE_IN_GEAR) {
        // If we are in reverse or neutral and we are not changing up or down then we have nothing to do, so return
        if (gearbox.IsInReverse() || gearbox.IsInNeutral()) return;

        HandlePotentialChanging();

        // We are already in the target gear, there is nothing to do, so return
        if (targetGearIndex == gearbox.GetCurrentGearIndex()) {
          // TODO: We may also wish to rev the engine slightly if we are below stalling speed
          gearbox.clutch.SetPushedInAmount0To1(0.0f);
          return;
        }
      }

      // Push the clutch in
      gearbox.clutch.SetPushedInAmount0To1(1.0f);

      // If we don't have the required amount of clutching then we are not ready to change, so return
      if (gearbox.clutch.GetClutchPushedInAmount0To1() < 0.9f) return;

      // We are ready to shift down
      if (targetGearIndex > gearbox.GetCurrentGearIndex()) gearbox.ShiftUp();
      else gearbox.ShiftDown();
    }


    cPart::cPart(unsigned int _uiType) :
      uiType(_uiType),
      fHealth(100.0f)
    {
    }

    void cPart::Install(cVehicle *p)
    {
      pVehicle = p;
    }


    // engine
    // engine rpm
    // engine rpm : torque nm curve

    // engine_rpm -> superchargerrpm -> boostpsi
    cPartEngine::cPartEngine() :
      cPart(PART_ENGINE)
    {
      // Temporary torque curve for a Honda S2000
      curveEngineRPMToTorqueNm.AddPoint(1000.0f, 140.0f);
      curveEngineRPMToTorqueNm.AddPoint(2000.0f, 149.14f);
      curveEngineRPMToTorqueNm.AddPoint(2200.0f, 145.07f);
      curveEngineRPMToTorqueNm.AddPoint(2500.0f, 147.78f);
      curveEngineRPMToTorqueNm.AddPoint(3000.0f, 169.50f);
      curveEngineRPMToTorqueNm.AddPoint(3300.0f, 172.19f);
      curveEngineRPMToTorqueNm.AddPoint(4000.0f, 169.50f);
      curveEngineRPMToTorqueNm.AddPoint(4500.0f, 166.77f);
      curveEngineRPMToTorqueNm.AddPoint(5600.0f, 172.19f);
      curveEngineRPMToTorqueNm.AddPoint(5800.0f, 170.83f);
      curveEngineRPMToTorqueNm.AddPoint(6000.0f, 168.12f);
      curveEngineRPMToTorqueNm.AddPoint(6100.0f, 177.61f);
      curveEngineRPMToTorqueNm.AddPoint(6200.0f, 186.42f);
      curveEngineRPMToTorqueNm.AddPoint(6300.0f, 192.53f);
      curveEngineRPMToTorqueNm.AddPoint(6500.0f, 195.92f);
      curveEngineRPMToTorqueNm.AddPoint(6700.0f, 195.92f);
      curveEngineRPMToTorqueNm.AddPoint(7000.0f, 195.24f);
      curveEngineRPMToTorqueNm.AddPoint(7600.0f, 190.49f);
      curveEngineRPMToTorqueNm.AddPoint(8000.0f, 184.39f);
      curveEngineRPMToTorqueNm.AddPoint(8200.0f, 183.04f);
      curveEngineRPMToTorqueNm.AddPoint(8300.0f, 146.43f);
      curveEngineRPMToTorqueNm.AddPoint(9500.0f, 146.43f);
    }

    void cPartEngine::_Update(sampletime_t currentTime)
    {
      fTorqueNm = GetTorqueNmAtEngineRPM(fRPM);
    }







    cPartTurboCharger::cPartTurboCharger() :
      cPart(PART_TURBO_CHARGER)
    {
    }

    void cPartTurboCharger::_Update(sampletime_t currentTime)
    {
      // http://www.turbochargedpower.com/Turbo%20vs%20Blowers.htm
      // http://www.hi-flow.com/HP4FAQSuper.htm
      // http://en.wikipedia.org/wiki/Turbocharger

      float_t fEngineExhaustpsi = 3.0f;
      fRPM = GetTurboChargerRPMAtEngineExhaustpsi(fEngineExhaustpsi);
      //float_t outputboostpsi = GetBoostpsiAtTurboChargerRPM(fRPM);
    }


    cPartSuperCharger::cPartSuperCharger() :
      cPart(PART_SUPER_CHARGER),
      fRPM(1.0f),
      fSuperChargerRPMToEngineRPMRatio(2.0f) // ie. 6:1 = 6.0f
    {
    }

    void cPartSuperCharger::_Update(sampletime_t currentTime)
    {
      // http://www.turbochargedpower.com/Turbo%20vs%20Blowers.htm
      // http://www.hi-flow.com/HP4FAQSuper.htm
      // http://en.wikipedia.org/wiki/Supercharger
      float_t fEngineRPM = 1000.0f;
      fRPM = fSuperChargerRPMToEngineRPMRatio * fEngineRPM;
      //float_t outputboostpsi = GetBoostpsiAtSuperChargerRPM(fRPM);
    }




    cVehicleControlModule::cVehicleControlModule() :
      cPart(PART_CONTROL_MODULE),
      fMaximumTCSSlipRatio(1.2f)
    {
    }

    bool cVehicleControlModule::_IsValid() const
    {
      // Check that we have valid values
      if (bAutomaticShifting && fAutomaticShiftingRPM < 0.0f) return false;
      if (bRevLimiter && fRevLimitRPM < 0.0f) return false;

      if (bAutomaticShifting && bRevLimiter) {
        // Check that the transmission will shift before the rev limiter kicks in
        if (fAutomaticShiftingRPM > fRevLimitRPM) return false;
      }

      if (bTractionControl) {
        // Check that traction control is between 0.0f and 1.0f
        if ((fTractionControl < 0.0f) || (fTractionControl > 1.0f)) return false;
      }

      if (bAntiLockBrakingSystem) {
        // Check that ABS is between 0.0f and 1.0f
        if ((fAntiLockBrakingSystem < 0.0f) || (fAntiLockBrakingSystem > 1.0f)) return false;
      }

      return true;
    }

    void cVehicleControlModule::GetSlowestAndFastestWheelSpeedsRPM(float_t& fSlowestWheel, float_t& fFastestWheel)
    {
      fSlowestWheel = math::cINFINITY;
      fFastestWheel = -math::cINFINITY;
      /*const size_t n = GetWheelCount();
      for (size_t i = 0; i < n; i++) {
        const float fWheelRPM = wheel[i].GetRPM();
        if (fWheelRPM < fSlowestWheel) fSlowestWheel = fWheelRPM;
        if (fWheelRPM > fFastestWheel) fFastestWheel = fWheelRPM;
      }*/
    }

    void cVehicleControlModule::Update(sampletime_t currentTime)
    {
      /*const float_t fRPM = vehicle.GetRPM();
      float_t fAccelerator0To1 = vehicle.GetAccelerator0To1();

      // Rev Limiter
      if (bRevLimiter) {
        if (fRPM > fRevLimitRPM) fAccelerator0To1 = 0.0f;
      }

      // Automatic Shifting
      if (bAutomaticShifting) {
        if (fRPM > fAutomaticShiftingRPM) SetStateShiftUp(bAutomaticClutching);
      }

      // Traction Control
      // The vehicle will allow slip up to a maximum of (1.0f - fTractionControl) * fMaximumTCSSlipRatio;
      if (bTractionControl) {
        float_t fAccelerator = GetInput0To1();
        float_t fSlowestWheel = 0.0f;
        float_t fFastestWheel = 0.0f;
        GetSlowestAndFastestWheelSpeedsRPM(fSlowestWheel, fFastestWheel);

        if (fSlowestWheel > 0.0f) {
          const float_t fRatio = fFastestWheel / fSlowestWheel;
          const float_t fMaximumAllowed = (1.0f - fTractionControl) * fMaximumTCSSlipRatio;
          if (fRatio > fMaximumAllowed) fAccelerator = 0.0f;
        }
      }

      // This will detect if any of the wheels are spinning subtantially less than the rest and actually let go of
      // the brake on that wheel if it is applied
      // float_t fLetGo = fAntiLockBrakingSystem;
      if (bAntiLockBrakingSystem) {
        float_t fAccelerator = GetInput0To1();
        float_t fSlowestWheel = 0.0f;
        float_t fFastestWheel = 0.0f;
        GetSlowestAndFastestWheelSpeedsRPM(fSlowestWheel, fFastestWheel);

        if (fSlowestWheel > 0.0f) {
          const float_t fRatio = fFastestWheel / fSlowestWheel;
          const float_t fMaximumAllowed = (1.0f - fAntiLockBrakingSystem) * fMaximumABSSlipRatio;
          if (fRatio > fMaximumAllowed) {
            const size_t n = GetWheelCount();
            for (size_t i = 0; i < n; i++) {
              const float_t fCurrentWheelRPM = GetWheelRPM(i);
              if ((fCurrentWheelRPM / fFastestWheel) > fMaximumAllowed) {
                fBrake[i] *= (1.0f - fAntiLockBrakingSystem);
              }
            }
          }
        }
      }*/
    }













    // Requires GearRatio* gear ratios, i* inertia values
    float cDriveTrain::GetEffectiveInertiaFor2Bodies1Gears() const
    {
      ASSERT(bodyInertia.size() == 2);
      ASSERT(gearRatio.size() == 1);

      const float GearRatio1 = gearRatio[0];
      const float i1 = bodyInertia[0];
      const float i2 = bodyInertia[1];

      float Ei2 = (1.0f / (GearRatio1 * GearRatio1)) * i2;
      float Ei1 = i1;

      return Ei1 + Ei2;
    }

    float cDriveTrain::GetEffectiveInertiaFor3Bodies2Gears() const
    {
      ASSERT(bodyInertia.size() == 3);
      ASSERT(gearRatio.size() == 2);

      const float GearRatio1 = gearRatio[0];
      const float GearRatio2 = gearRatio[1];
      const float i1 = bodyInertia[0];
      const float i2 = bodyInertia[1];
      const float i3 = bodyInertia[2];

      float Ei3 = GearRatio1 * (1.0f / (GearRatio2 * GearRatio2)) * i3;
      float Ei2 = (1.0f / (GearRatio1 * GearRatio1)) * i2;
      float Ei1 = i1;

      return Ei1 + Ei2 + Ei3;
    }

    float cDriveTrain::GetEffectiveInertiaFor4Bodies3Gears() const
    {
      ASSERT(bodyInertia.size() == 4);
      ASSERT(gearRatio.size() == 3);

      const float GearRatio1 = gearRatio[0];
      const float GearRatio2 = gearRatio[1];
      const float GearRatio3 = gearRatio[2];
      const float i1 = bodyInertia[0];
      const float i2 = bodyInertia[1];
      const float i3 = bodyInertia[2];
      const float i4 = bodyInertia[3];

      float Ei4 = GearRatio1 * GearRatio2 * (1.0f / (GearRatio3 * GearRatio3)) * i4;
      float Ei3 = GearRatio1 * (1.0f / (GearRatio2 * GearRatio2)) * i3;
      float Ei2 = (1.0f / (GearRatio1 * GearRatio1)) * i2;
      float Ei1 = i1;

      return Ei1 + Ei2 + Ei3 + Ei4;
    }


    float cDriveTrain::GetRotationalAccelerationFromTorqueFor2Bodies1Gears(float fTorque) const
    {
      ASSERT(bodyInertia.size() == 2);
      ASSERT(gearRatio.size() == 1);

      const float GearRatio1 = gearRatio[0];
      const float i1 = bodyInertia[0];
      const float i2 = bodyInertia[1];

      return fTorque /
          (i1 + (1.0f / (GearRatio1 * GearRatio1)) * i2);
    }

    float cDriveTrain::GetRotationalAccelerationFromTorqueFor3Bodies2Gears(float fTorque) const
    {
      ASSERT(bodyInertia.size() == 3);
      ASSERT(gearRatio.size() == 2);

      const float GearRatio1 = gearRatio[0];
      const float GearRatio2 = gearRatio[1];
      const float i1 = bodyInertia[0];
      const float i2 = bodyInertia[1];
      const float i3 = bodyInertia[2];

      return fTorque /
          (i1 + (1.0f / (GearRatio1 * GearRatio1)) * i2) +
          (GearRatio1 * (1.0f / (GearRatio2 * GearRatio2)) * i3);
    }

    float cDriveTrain::GetRotationalAccelerationFromTorqueFor4Bodies3Gears(float fTorque) const
    {
      ASSERT(bodyInertia.size() == 4);
      ASSERT(gearRatio.size() == 3);

      const float GearRatio1 = gearRatio[0];
      const float GearRatio2 = gearRatio[1];
      const float GearRatio3 = gearRatio[2];
      const float i1 = bodyInertia[0];
      const float i2 = bodyInertia[1];
      const float i3 = bodyInertia[2];
      const float i4 = bodyInertia[3];

      return fTorque /
          (i1 + (1.0f / (GearRatio1 * GearRatio1)) * i2) +
          (GearRatio1 * (1.0f / (GearRatio2 * GearRatio2)) * i3) +
          (GearRatio1 * GearRatio2 * (1.0f / (GearRatio3 * GearRatio3)) * i4);
    }

    /*float cDriveTrain::GetRotationalAccelerationFromTorqueFor6Bodies5Gears(float fTorque) const
    {
      ASSERT(bodyInertia.size() == 6);
      ASSERT(gearRatio.size() == 5);

      const float GearRatio1 = gearRatio[0];
      const float GearRatio2 = gearRatio[1];
      const float GearRatio3 = gearRatio[2];
      const float GearRatio4 = gearRatio[3];
      const float GearRatio5 = gearRatio[4];
      const float i1 = bodyInertia[0];
      const float i2 = bodyInertia[1];
      const float i3 = bodyInertia[2];
      const float i4 = bodyInertia[3];
      const float i5 = bodyInertia[4];
      const float i6 = bodyInertia[5];

      return fTorque /
      (i1 + (1.0f / (GearRatio1 * GearRatio1)) * i2) +
      (GearRatio1 * (1.0f / (GearRatio2 * GearRatio2)) * i3) +
      (GearRatio1 * GearRatio2 * (1.0f / (GearRatio3 * GearRatio3)) * i4) +
      (GearRatio1 * GearRatio2 * GearRatio3 * (1.0f / (GearRatio4 * GearRatio4)) * i5) +
      (GearRatio1 * GearRatio2 * GearRatio3 * GearRatio4 * (1.0f / (GearRatio5 * GearRatio5)) * i6);
    }*/
  }

#ifdef BUILD_DEBUG
  class cCurveUnitTest : protected util::cUnitTestBase
  {
  public:
    cCurveUnitTest() :
      cUnitTestBase(TEXT("cCurveUnitTest"))
      {
      }

      void Test()
      {
        breathe::vehicle::cPartEngine engine;

        for (breathe::sampletime_t currentTime = 0; currentTime < 1000; currentTime += 100) {
          const float_t fRPM = float_t(currentTime);
          engine.SetRPM(fRPM);

          engine.Update(currentTime);

          const float_t fTorqueNm = engine.GetTorqueNm();
          LOG<<""<<currentTime<<"RPM = "<<fTorqueNm<<"Nm = "<<breathe::math::NmToKw(fTorqueNm, fRPM)<<"Kw"<<std::endl;
        }

        for (breathe::sampletime_t currentTime = 1000; currentTime < 10001; currentTime += 1000) {
          const float_t fRPM = float_t(currentTime);
          engine.SetRPM(fRPM);

          engine.Update(currentTime);

          const float_t fTorqueNm = engine.GetTorqueNm();
          LOG<<""<<currentTime<<"RPM = "<<fTorqueNm<<"Nm = "<<breathe::math::NmToKw(fTorqueNm, fRPM)<<"Kw"<<std::endl;
        }

        sampletime_t currentTime = 1000;
        engine.SetRPM(30000.0f);
        engine.Update(currentTime);
        ASSERT(engine.GetTorqueNm() < 0.1f);
        engine.SetRPM(100000.0f);
        engine.Update(currentTime);
        ASSERT(engine.GetTorqueNm() < 0.1f);
      }
  };

  cCurveUnitTest gCurveUnitTest;
#endif
}

/*

Car parameters
From VDrift Documentation Wiki
Jump to: navigation, search

The file's fields are mostly the same as the Vamos car parameters, explained in the Vamos docs. The units are all in MKS (meters, kilograms, seconds). It might also help to read The Physics of Racing by Brian Beckman. For unit conversion you can go to: This Site.

A vector of 3 floats ( 1.0, 3.0, 1.5 ) will be interpreted as distances from the car-body-center: X (positive in drive direction), Y (positive to left) and Z (positive up).

The file contains several sections. Each section will now be described, along with example values from the XS.car file. The XS has performance comparable to the Honda S2000.
Contents
[hide]

    * 1 Top level parameters
    * 2 Steering
    * 3 Engine
    * 4 Clutch
    * 5 Transmission
    * 6 Differential
    * 7 Fuel tank
    * 8 Suspension
    * 9 Tire
    * 10 Brakes
    * 11 Driver
    * 12 Drag
    * 13 Wing
    * 14 Wheel
    * 15 Contact-points
    * 16 Particle

[edit] Top level parameters

drive = RWD

The only top level parameter is the "drive" parameter. It accepts values "RWD", "FWD", "AWD" that correspond to rear wheel drive, front wheel drive, and all wheel drive, respectively.
[edit] Steering

max-angle = 33.19

This defines the maximum angle that the wheels will turn in each direction. For the XS, when the steering wheel is full left, the wheels would be at -33.19 degrees.
[edit] Engine

position = 0.86, 0.0, -0.21
mass = 140.0
max-power = 1.79e5
peak-engine-rpm = 7800.0
rpm-limit = 9000.0
inertia = 0.25
idle = 0.02
start-rpm = 1000
stall-rpm = 350
fuel-consumption = 1e-9
torque-friction = 0.0003
torque-curve-00 = 1000, 140.0
torque-curve-01 = 2000, 149.14
torque-curve-02 = 2200, 145.07
torque-curve-03 = 2500, 147.78
torque-curve-04 = 3000, 169.50
torque-curve-05 = 3300, 172.19
torque-curve-06 = 4000, 169.50
torque-curve-07 = 4500, 166.77
torque-curve-08 = 5600, 172.19
torque-curve-09 = 5800, 170.83
torque-curve-10 = 6000, 168.12
torque-curve-11 = 6100, 177.61
torque-curve-12 = 6200, 186.42
torque-curve-13 = 6300, 192.53
torque-curve-14 = 6500, 195.92
torque-curve-15 = 6700, 195.92
torque-curve-16 = 7000, 195.24
torque-curve-17 = 7600, 190.49
torque-curve-18 = 8000, 184.39
torque-curve-19 = 8200, 183.04
torque-curve-20 = 8300, 146.43
torque-curve-21 = 9500, 146.43

The position and mass parameters affect the weight distribution of the car. The torque curve is calculated from max-power and peak-engine-rpm using a polynomial expression given in Motor Vehicle Dynamics, Genta (1997), where peak-engine-rpm is the engine speed at which the maximum power output (max-power) is achieved. Alternatively, the torque curve can be explicitly defined, as in the example above. A rev limit can be set with rpm-limit. The rotational inertia of the moving parts is inertia. idle is the throttle position at idle. Starting the engine initially sets the engine speed to start-rpm. Letting the engine speed drop below stall-rpm makes the engine stall. The rate of fuel consumption is set with fuel-consumption. The actual fuel consumed each second (in units of liters) is the fuel-consumption parameter times RPM times throttle (throttle is from 0.0 to 1.0, where 1.0 is full throttle).
[edit] Clutch

sliding = 0.27
radius = 0.15
area = 0.75
max-pressure = 11079.26

The torque on the clutch is found by dividing the clutch pressure by the value in the area tag and multiplying by the radius and sliding (friction) parameters.
[edit] Transmission

gears = 6
gear-ratio-r = -2.8
gear-ratio-1 = 3.133
gear-ratio-2 = 2.045
gear-ratio-3 = 1.481
gear-ratio-4 = 1.161
gear-ratio-5 = 0.943
gear-ratio-6 = 0.763
shift-delay = 0.08

The number of forward gears is set with the gears parameter. The gear ration for reverse and all of the forward gears is then defined. The shift-delay tag tells how long it takes to change gears. For a paddle-shifter, like a modern Formula One car, shift-delay can be set to zero.
[edit] Differential

final-drive = 4.100
anti-slip = 600.0

The final drive provides an additional gear reduction. The anti-slip parameter limits the difference in speed between two wheels on the same axle.
[edit] Fuel tank

position = -0.8, -0.1, -0.26
capacity = 0.0492
volume = 0.0492
fuel-density = 730.0

The fuel tank's position, the current volume of fuel and the density of the fuel affect the car's weight distribution. The capacity tag sets the maximum volume of fuel that the tank can hold. The initial volume is set with the volume tag. The density of the fuel is set with fuel-density.
[edit] Suspension

Front/rear parameters are broken into two fields. Per-wheel parameters are broken into four fields. In the example below the front suspension is shown, followed by the front left wheel.

spring-constant = 49131.9
bounce = 2588
rebound = 2612
travel = 0.19
max-compression-velocity = 10.0
camber = -1.33
caster = 6.12
toe = 0.0
anti-roll = 8000.0

position = 1.14, 0.76, -0.03
hinge = 0,0,0

The hinge is the center of the wheel's path as the suspension moves. The location of the hinge is determined by suspension geometry, and may be outside of the car itself. The position is the point at which suspension forces (from the wheels) are applied to the chassis. bounce and rebound are the damping coefficients for compression and expansion of the suspension, respectively. If the speed at which the suspension is compressed, or expanded exceeds the value in max-compression-velocity, the dampers “lock up.” Wheel alignment is set with the camber, caster, and toe tags. All angles are in degrees.
[edit] Tire

Front/rear parameters are broken into two fields. In the example below the front tire section is shown.

radius = 0.29
rolling-resistance = 1.3e-2, 6.5e-6
rotational-inertia = 10.0
tread = 0.0
# Lateral force
a0=1.6
a1=-38
a2=1201
a3=1914
a4=8.7
a5=0.014
a6=-0.24
a7=1.0
a8=-0.03
a9=-0.0013
a10=-0.15
a111=-8.5
a112=-0.29
a12=17.8
a13=-2.4
# Longitudinal force
b0=1.7
b1=-80
b2=1571
b3=23.3
b4=300
b5=0
b6=0.0068
b7=0.055
b8=-0.024
b9=0.014
b10=0.26
b11=-86
b12=350
# Aligning moment
c0=2.3
c1=-3.8
c2=-3.14
c3=-1.16
c4=-7.2
c5=0.0
c6=0.0
c7=0.044
c8=-0.58
c9=0.18
c10=0.043
c11=0.048
c12=-0.0035
c13=-0.18
c14=0.14
c15=-1.029
c16=0.27
c17=-1.1

The two elements of rolling-resistance are the constant and velocity-squared terms, respectively. Radius defines the radius of the tire. The tread parameter ranges over arbitrary values of 0.0 to 1.0, where 0.0 is a road tire and 1.0 is an off-road tire. The longitudinal, transverse, and aligning section each contain a vector of “magic formula” coefficients as presented in Motor Vehicle Dynamics, Genta (1997). A description is shown below:

Shape factor ........................................... A0
Load infl. on lat. friction coeff (*1000)... (1/kN) .... A1
Lateral friction coefficient at load = 0 (*1000) ....... A2
Maximum stiffness ........................ (N/deg) ..... A3
Load at maximum stiffness ................ (kN) ........ A4
Camber infiuence on stiffness ............ (%/deg/100) . A5
Curvature change with load ............................. A6
Curvature at load = 0 .................................. A7
Horizontal shift because of camber ........(deg/deg).... A8
Load influence on horizontal shift ........(deg/kN)..... A9
Horizontal shift at load = 0 ..............(deg)........ A10
Camber influence on vertical shift ........(N/deg/kN)... A111
Camber influence on vertical shift ........(N/deg/kN**2) A112
Load influence on vertical shift ..........(N/kN)....... A12
Vertical shift at load = 0 ................(N).......... A13

Shape factor ........................................... B0
Load infl. on long. friction coeff (*1000)... (1/kN) ... B1
Longitudinal friction coefficient at load = 0 (*1000)... B2
Curvature factor of stiffness ............ (N/%/kN**2) . B3
Change of stiffness with load at load = 0 (N/%/kN) ..... B4
Change of progressivity of stiffness/load (1/kN) ....... B5
Curvature change with load ............................. B6
Curvature change with load ............................. B7
Curvature at load = 0 .................................. B8
Load influence on horizontal shift ....... (%/kN) ...... B9
Horizontal shift at load = 0 ............. (%) ......... B10
Load influence on vertical shift ......... (N/kN) ...... B11
Vertical shift at load = 0 ............... (N) ......... B12

Shape factor ........................................... C0
Load influence of peak value ............ (Nm/kN**2) ... C1
Load influence of peak value ............ (Nm/kN) ...... C2
Curvature factor of stiffness ........... (Nm/deg/kN**2) C3
Change of stiffness with load at load = 0 (Nm/deg/kN) .. C4
Change of progressivity of stiffness/load (1/kN) ....... C5
Camber influence on stiffness ........... (%/deg/100) .. C6
Curvature change with load ............................. C7
Curvature change with load ............................. C8
Curvature at load = 0 .................................. C9
Camber influence of stiffness .......................... C10
Camber influence on horizontal shift......(deg/deg)..... C11
Load influence on horizontal shift........(deg/kN)...... C12
Horizontal shift at load = 0..............(deg)......... C13
Camber influence on vertical shift........(Nm/deg/kN**2) C14
Camber influence on vertical shift........(Nm/deg/kN)... C15
Load influence on vertical shift..........(Nm/kN)....... C16
Vertical shift at load = 0................(Nm).......... C17

[edit] Brakes

Front/rear parameters are broken into two fields. In the example below the front section is shown.

friction = 0.73
max-pressure = 4.0e6
bias = 0.60
radius = 0.14
area = 0.015

The bias parameter is the fraction of braking pressure applied to the front brakes (in the front brake section) or the rear brakes (in the rear brake section). To make sense, the rear value should equal 1.0 minus the front value. The maximum brake torque is calculated as friction * area * bias * max-pressure * radius. Some fraction of this value is applied based on the brake pedal.
[edit] Driver

position = -0.62, -0.35, -0.12
mass = 90.0
view-position = -0.64, 0.35, 0.30
hood-mounted-view-position = 0.55, 0, 0.17

The position and mass affect the weight distribution of the car. The view positions define 3D coordinates for camera placement.
[edit] Drag

position = 0.0, 0.0, 0.2
frontal-area = 2
drag-coefficient = 0.3

The frontal area and coefficient of drag, set with frontal-area and drag-coefficient, are used to calculate the drag force.
[edit] Wing

Front/rear parameters are broken into two fields. In the example below the front section is shown.

position = 1.9, 0.0, 0.60
frontal-area = 0.2
drag-coefficient = 0.0
surface-area = 0.3
lift-coefficient = -0.5
efficiency = 0.95

Downforce can be added with wings. The amount of downforce is determined by the value in the lift-coefficient tag. If the lift coefficient is positive, upforce is generated. This is usually undesirable for cars. The efficiency determines how much drag is added as downforce increases. The surface-area is the surface area of the wing. This value is also used in the drag calculation.
[edit] Wheel

Per-wheel parameters are broken into four fields. In the example below the front left wheel is shown.

position = 1.14, 0.76, -0.47
roll-height = 0.29
mass = 18.14
restitution = 0.1

[edit] Contact-points

mass = 0.05
position-00 = 1.96, 0.37, -0.24
position-01 = 1.96, -0.37, -0.24
position-02 = 1.52, 0.83, 0.16
position-03 = 1.52, -0.83, 0.16
position-04 = -0.10, 0.89, -0.24
position-05 = -0.10, -0.89, -0.24
position-06 = -2.18, -0.83, -0.10
position-07 = -2.18, 0.83, -0.10

These values are used for weight distribution and balance only. They no longer perform any contact-related function.
[edit] Particle

These parameters are broken into a series of values starting at 00 and going to some number less than 100. The particle-00 is shown below.

mass = 30.0
position = -1.28, 0.0, -0.36

These values are used for weight distribution and balance.
Retrieved from "http://wiki.vdrift.net/Car_parameters"









5.4        Gearbox

A gearbox has the task of converting angular velocities and torques in the drivetrain. It has a set of gears characterized by their ratio of conversion. These ratios are used for multiplying the torque and dividing the angular velocity from the engine to the wheels and vice versa from the wheels to the engine, based on the theorem that the product of the two must remain constant during the conversion.

There are two methods for changing gears: manual and automatic. Automatic gear change is based upon efficiency: the gear is switched to the neighboring higher or lower one if the absolute torque is higher in that gear compared to the current one. The efficiency of the torque output is decreased during automatic gear usage (as it is in real life) to give manual gears a benefit. The gearbox has control of the clutch during these operations.
5.5        Differential

The basic idea of a differential is much easier to implement in code than it is to build in real life. A differential has the duty of ensuring that all the wheels receive the same amount of torque while enabling them to rotate at different speeds. This is necessary because during a turn the wheels of the car cover different distances within the same time. In programming this can be accomplished by simply dividing the available torque at a preset ratio among the driven wheels. The angular velocity of the wheels is averaged to find the effective rotation of the drivetrain.

While the basic differential is “open” in the sense that it does permit limitless difference between the angular velocities of the wheels it affects, this scheme is not always useful. In the case of powerful racecars and off-road vehicles it is desired to limit the “slip” of the differential. The solution to this problem up to date is similar to that of the viscous clutch, which should be enhanced in the future.
5.6        Suspension

We have provided a rather simple solution to the suspension system of the car. Wheel suspensions are treated completely independently; also there is no camber variation with the suspension travel.

As only vertical motion is permitted for the wheel in the body frame of the car, the horizontal forces acting on the wheel are simply forwarded to the chassis itself. This gives acceptable results but completely neglects the real geometry of the suspension that is normally taken into account by the introduction of roll centers [11]. With the current solution roll centers of both the front and rear axles are set permanently at ground level, which increases the tendency of the car to flip over in some situations. This can be countered by using the vertical suspension force as the normal force in the tire model calculations; in this case two wrongs do make a right.

The vertical travel of the wheel is governed by two nonlinear sets of spring and damping forces, one of them reserved for the bump stop (a piece of strong rubber that stops the suspension movement at the point of maximum suspension travel). Spring forces depend on the vertical position of the wheel in the car frame and damping on its velocity. These forces are applied to both the car chassis and the respective wheel.
5.7        Tire Model

As mentioned above, the tire model of a driving simulation is the single most important feature of the dynamical model. This should be evident because under normal circumstances only the tires of the car touch the ground and therefore all major forces acting upon the car body ultimately originate from the tire. The tire model is responsible for traction and cornering forces and thus determines the handling characteristics of the car, and how the engine torque is translated to acceleration. The tire model is simplified to the generation of three components: normal, longitudinal and lateral forces. The latter two forces are decoupled to the maximum extent possible and then combined to meet the limits set by a friction ellipse. Aligning moment is currently neglected because no means of force-feedback have been introduced to the simulation.

Modeling tires in real time is widely agreed upon by professionals to be a difficult problem. Tire models are either purely analytical or semi-empirical, representing functions to fit experimental data. Both types of models can be steady-state or dynamical, with a different range of validity concerning the domain of frequencies encountered by the tire. For the macro height variation of the terrain a steady-state approach is sufficient, and the simulation currently utilizes such a model. With the recent introduction of the displacement height-maps the validity of such a solution decreases but fortunately any steady-state model can be extended to a dynamical one with the introduction of two new state variables [11]. This is to be done in the near future.








Steady-state tire models generally calculate traction and cornering forces based upon two properties of a wheel in motion: slip ratio and slip angle [11][12]. To understand these properties it must be made clear that the tire generates forces due to its deformation best represented by complex nonlinear spring forces. A tire producing traction (or braking) forces has different angular velocity as compared to the same tire rolling freely at the same traveling speed. This difference is characterized best by slip ratio, which is defined by:


where ω is the actual angular velocity of the tire and w0 (=vx/Reff, Reff is the effective radius of the wheel) is the angular velocity of a free-rolling tire moving with the same linear velocity as the driven or braked tire. Slip angle (SA) is the angle between the wheel plane and its direction of motion.

The industry standard for tire modeling in real-time applications is the so-called Magic Tire Model of Pacejka [11][12]. It is a semi-empirical model recognized to be exceptionally correct for derivation of tire forces under explicit circumstances. Unfortunately its parameter set is rather complex and cannot be modified intuitively to account for changes in the tire or the type of surface the tire rolls upon (since it would require different, hard-to-obtain experimental data sets for each case). Even though we wish to implement the Magic Tire Model in the future (because of its applicability on tarmac), we were concerned with finding an analytical model due to these issues. Finally we chose to implement a tire model based upon that of James Lacombe [9].

Lacombe’s model is an analytical steady-state model. It focuses on clearly distinguishing two regions of the tire contact patch, a static (sticking) and sliding region. The sliding region is handled based upon a Coulomb-type friction model and the static region based upon linear and nonlinear spring forces concerning the longitudinal and lateral forces respectively (the normal force is also generated from a nonlinear spring very much similarly to most of the models). The strength of the model lies in its approach to the derivation of forces during combined longitudinal and lateral acceleration, while the discreet cases produce results similar to those of other tire models. The ratio of the static and sliding regions varies smoothly during the simulation and the sliding region produces longitudinal and lateral forces in a coupled manner since a Coulomb-type frictional force always acts against the direction of sliding motion, in this case the motion of the contact patch on the ground. (The direction of motion of the contact patch is normally not the same as that of the wheel, only if the wheel is completely blocked and does not roll at all.)




While the model at work in the simulation is not the most sophisticated one possible and wishes to be enhanced in the future, it does respond lively to parameter changes and driving style. Important real-world effects such as turn-in braking and power oversteer can be produced with ease in the simulation, along with other cases of under- and oversteer. Handling differences between front-, rear- and four-wheel driven vehicles are very pronounced and probably even exaggerated to some extent. On the whole the car behavior is quite realistic compared to the simplicity of the model.

It is important to note that determining the validity range of the parameters in the model is in cases almost as hard as deriving the model itself and accounts for a major part in the realism of the final product.




*/

// http://www.miata.net/sport/Physics/
