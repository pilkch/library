#ifndef CCAR_H
#define CCAR_H

#include <spitfire/math/cCurve.h>
#include <spitfire/math/units.h>

#include <breathe/game/component.h>

#include <breathe/vehicle/cVehicle.h>

// For testing 2004 Corvette:
// http://www.corvetteactioncenter.com/specs/c5/2004/specs.html0-60 mph   3.9 sec[4]
// http://en.wikipedia.org/wiki/Chevrolet_Corvette_C5_Z06
// http://en.wikipedia.org/wiki/Chevrolet_Corvette_C5_Z06#Performance
// 0-100 mph   9.2 sec
// 0-100-0 mph   13.56 sec
// 1/4 Mile  11.9 sec
// Skid Pad  1.03 G
// Top Speed   171 mph (275 km/h)
// Nürburgring Nordschleife Lap Time 7:56

namespace breathe
{
  namespace game
  {
    namespace car
    {
      // This is a pretty dodgy class.
      // It is not very scientific, perhaps density and flow could be combined?
      // The Apply functions are not actually based on any physics laws.
      class cAirFlow
      {
      public:
        cAirFlow();

        cAirFlow(const cAirFlow& rhs) { Assign(rhs); }
        cAirFlow& operator=(const cAirFlow& rhs) { Assign(rhs); return *this; }

        void SetDensityKgPerCubicMetersAndFlowCubicMetresPerSecondAndTemperatureDegreesCelcius(float_t fDensityKgPerCubicMetre, float_t fFlowCubicMetresPerSecond, float_t fTemperatureDegreesCelcius);

        float_t GetDensityKgPerCubicMetre() const { return fDensityKgPerCubicMetre; }
        float_t GetFlowCubicMetresPerSecond() const { return fFlowCubicMetresPerSecond; }
        float_t GetTemperatureDegreesCelcius() const { return fTemperatureDegreesCelcius; }

        void ApplyCompressionOrDecompression(float_t fRatio); // A value from 0.0f to n, basically the resulting density will be fDensityKgPerCubicMetre * fRatio
        void ApplyFlowRateChange(float_t fRatio); // A value from 0.0f to n, basically the resulting flow will be fFlowCubicMetresPerSecond * fRatio
        void ApplyTemperatureChange(float_t fRatio); // A value from 0.0f to n, basically the resulting temperature will be fTemperatureDegreesCelcius * fRatio

        void Combine(const cAirFlow& rhs); // Basically *this += rhs
        cAirFlow SplitIntoParts(size_t nParts) const; // Basically return *this / nParts

      private:
        void Assign(const cAirFlow& rhs);

        float_t fDensityKgPerCubicMetre;
        float_t fFlowCubicMetresPerSecond;
        float_t fTemperatureDegreesCelcius;
      };


      class cAmbientSettings
      {
      public:
        cAmbientSettings();
        cAmbientSettings(float_t fDensityKgPerCubicMetre, float_t fPressureKPA, float_t fTemperatureDegreesCelcius);

        cAmbientSettings(const cAmbientSettings& rhs) { Assign(rhs); }
        cAmbientSettings& operator=(const cAmbientSettings& rhs) { Assign(rhs); return *this; }

        float_t GetDensityKgPerCubicMetre() const { return fDensityKgPerCubicMetre; }
        float_t GetPressureKPA() const { return fPressureKPA; }
        float_t GetTemperatureDegreesCelcius() const { return fTemperatureDegreesCelcius; }

      private:
        void Assign(const cAmbientSettings& rhs);

        float_t fDensityKgPerCubicMetre;
        float_t fPressureKPA;
        float_t fTemperatureDegreesCelcius;
      };


      class cChassis
      {
      public:
        // TODO: We need other stuff such as down force, spoilers? etc. centre of gravity?
        float_t GetMassKg() const { return fMassKg; }
        float_t GetWidthMetres() const { return fWidthMetres; }
        float_t GetLengthMetres() const { return fLengthMetres; }
        float_t GetHeightMetres() const { return fHeightMetres; }

        void Update(sampletime_t currentTime) {}

      private:
        float_t fMassKg;
        float_t fWidthMetres;
        float_t fLengthMetres;
        float_t fHeightMetres;
      };


      class cEngine
      {
      public:
        cEngine();

        size_t GetNumberOfCylinders() const;
        void SetNumberOfCylinders(size_t nCylinders);

        float GetBoreRadiusCentiMeters() const;
        void SetBoreRadiusCentiMeters(float fBoreRadiusCentiMeters);

        float GetStrokeLengthCentiMeters() const;
        void SetStrokeLengthCentiMeters(float fStrokeLengthCentiMeters);

        float GetCompressionRatioMassAirToPetrol() const;
        void SetCompressionRatioMassAirToPetrol(float fCompressionRatioMassAirToPetrol);

        void SetAmbientSettings(const cAmbientSettings& _ambientSettings) { ambientSettings = _ambientSettings; }
        void SetAcceleratorInput0To1(float_t _fAcceleratorInput0To1) { fAcceleratorInput0To1 = _fAcceleratorInput0To1; }
        void SetIntakeAirFlow(const cAirFlow& _intakeAirFlow) { intakeAirFlow = _intakeAirFlow; }

        // This will be proportional to the accelerator and affected by how fast the air is flowing over the vehicle and into the engine
        float GetMassAirKgPerSecond() const;
        void SetMassAirKgPerSecond(float fMassAirKgPerSecond);

        float GetInjectorSizeMassFuelKgPerSecond() const;
        void SetInjectorSizeMassFuelKgPerSecond(float fInjectorSizeMassFuelKgPerSecond);

        float GetRPM() const;
        void SetRPM(float fRPM);

        float GetCylinderDisplacementCubicCentiMeters() const;
        float GetEngineDisplacementCubicCentiMeters() const;

        float_t GetMassKg() const { return fMassKg; }
        float_t GetTorqueNm() const { return fTorqueNm; }
        float_t GetPowerKw() const { return spitfire::math::NmToKw(fTorqueNm, fRPM); }
        const cAirFlow& GetExhaustAirFlow() const { return exhaustAirFlow; }

        void Update(sampletime_t currentTime);

      private:
        float_t fMassKg;
        size_t nCylinders;
        float fBoreRadiusCentiMeters;
        float fStrokeLengthCentiMeters;
        float_t fRedLineRPM;
        float fCompressionRatioMassAirToPetrol;
        float fMassAirKgPerSecond;
        float fInjectorSizeMassFuelKgPerSecond;
        cAmbientSettings ambientSettings;
        float_t fAcceleratorInput0To1;
        cAirFlow intakeAirFlow;
        math::cCurve rpmToTorqueCurve;

        float fRPM;
        float_t fTorqueNm;
        cAirFlow exhaustAirFlow;
      };
      
      
      // Electronic Fuel Injection
      //
      // https://secure.wikimedia.org/wikipedia/en/wiki/Fuel_injection#Sample_pulsewidth_calculations
      // https://secure.wikimedia.org/wikipedia/en/wiki/Fuel_injection#Calculate_injector_pulsewidth_from_airflow
      // https://secure.wikimedia.org/wikipedia/en/wiki/Fuel_injection#Calculate_fuel-flow_rate_from_pulsewidth
      // http://mb-soft.com/public2/engine.html
      //
      // The total amount of gas-air mixture in the COMPLETE cylinder (initially at approximately atmospheric pressure of 15 PSIA) is now already squeezed into just the volume of the cylinder above the piston.
      // If you think about it, the initial gas-air mixture is here already squeezed into HALF its original volume, and so it is already at about TWICE the initial pressure (or now 30 PSIA).
      //
      // Pressures can be described in two different ways, Absolute and Gauge.
      // In this case, we know that the air started out at the natural pressure of 15 PSI, which is an Absolute pressure, so it is sometimes written PSIA.
      // If you measured that pressure with an air pressure gauge, it would read 0 PSI, because there is no difference in pressure from natural.
      // This is called gauge pressure, and would be written 0 PSIG. They mean the same thing, and absolute pressure is always 15 higher than gauge pressure. 
      //
      // Virtually all cars and trucks use the spark-ignition or Otto cycle engine.
      // There are a couple common alternatives: The compression-ignition or Diesel cycle and the Brayton or Joule cycle.
      //
      // An Otto cycle has an isentropic compression, followed by a constant volume combustion explosion, followed by an isentropic expansion.
      // A Diesel cycle has an isentropic compression followed by a NON-explosive combustion at (relatively) constant pressure, followed by the isentropic expansion. 
      //
      // The area shown at the top of the drawing is an additional volume that remains even when the piston is at the very highest point, a location called TDC for Top Dead Center, which will mean more in our second drawing.
      // The space above the piston at TDC is carefully designed. In this specific case, it has a volume of around 6.3 cubic inches.
      //
      // When the piston began its upward movement (at BDC, bottom dead center), there was then a volume of gas-air mixture above it of (44 + 6.3) or 50.3 cubic inches. When the piston has gotten to TDC,
      // as in this drawing, all that gas-air mixture has now been compressed into the remaining 6.3 cubic inches. The ratio of these numbers, 50.3 / 6.3 is called the Compression Ratio of the engine. In this case, it is about 8.0.
      //
      // This drawing shows the moment when that gas-air mixture is most compressed. The 8.0 compression ratio means that the 15 PSIA beginning mixture, is now
      // at about 8.0 times that pressure, or around 120 PSIA. (Technically not precisely, because of some really technical characteristics of what happens when gases are compressed isentropically.)
      // The cylinder compression is measured and is essentially this number. Except that that device is a gauge, so the reading would be 105 PSIG.
      //
      // All actual internal combustion engines rely on KEEPING that explosion pressure for as long as possible!  The total effect regarding rotating the crankshaft is the Integral of the net
      // force actually applied to the crankshaft by that connecting rod for as long as there is explosive pressure inside the cylinder. In an engine that is operating
      // properly, contributions to this Integral begin at the instant of ignition and end when the exhaust valve begins to open.  The instantaneous force applied as torque in rotating
      // the crankshaft continuously changes during this "power stroke". It actually begins with a slight negative contribution since ignition is timed to occur before TDC, but
      // not much pressure yet develops since the flame is still spreading inside the cylinder.
      //
      // The contribution becomes exactly zero at TDC, and then quickly rises as the internal burning and pressure continues and the leverage angle at the crankshaft improves.
      // Eventually, the piston going down reduces the pressure, and engine cooling also does, and good design times the exhaust valve to begin opening about when productive torque is no longer available.

      class cEFI
      {
      public:
        explicit cEFI(cEngine& engine);

        float GetMassAirFuelRatio() const;
        void SetMassAirFuelRatio(float fMassAirFuelRatio);

        float GetMassFuelAirRatio() const;
        void SetMassFuelAirRatio(float fMassFuelAirRatio);

        float GetAirMassFlowRateKgPerStroke() const;
        float GetAirMassFlowRateKgPerSecond() const;
        float GetPulseWidthMS() const;
        float GetEngineFuelFlowRateKgPerSecond() const;

      private:
        cEngine& engine;

        float fMassFuelAirRatio;
      };



      class cSuperCharger
      {
      public:
        cSuperCharger();

        void SetAmbientSettings(const cAmbientSettings& _ambientSettings) { ambientSettings = _ambientSettings; }
        void SetEngineRPM(float_t _fRPM) { fRPM = _fRPM; }

        float_t GetMassKg() const { return fMassKg; }
        const cAirFlow& GetOutputAirFlow() const { return outputAirFlow; }

        void Update(sampletime_t currentTime);

      private:
        float_t fMassKg;
        cAmbientSettings ambientSettings;
        float_t fEngineRPM;
        math::cCurve rpmToPressureCurve;
        float_t fRatio;

        float_t fRPM;
        cAirFlow outputAirFlow;
      };


      class cTurboCharger
      {
      public:
        cTurboCharger();

        void SetAmbientSettings(const cAmbientSettings& _ambientSettings) { ambientSettings = _ambientSettings; }
        void SetEngineExhaustAirFlow(const cAirFlow& _exhaustAirFlow) { exhaustAirFlow = _exhaustAirFlow; }

        float_t GetMassKg() const { return fMassKg; }
        const cAirFlow& GetOutputAirFlow() const { return outputAirFlow; }

        void Update(sampletime_t currentTime);

      private:
        float_t fMassKg;
        cAmbientSettings ambientSettings;
        cAirFlow exhaustAirFlow;
        math::cCurve rpmToPressureCurve;

        float_t fRPM;
        cAirFlow outputAirFlow;



      //public:
        //void SetMinimumBlowOffPressureKPA(float_t _fMinimumBlowOffPressureKPA) { fMinimumBlowOffPressureKPA = _fMinimumBlowOffPressureKPA; }
        //void SetSource(breathe::audio::cSourceRef _pSource) { pSource = _pSource; }

      //private:
        //float_t fMinimumBlowOffPressureKPA;
        //breathe::audio::cSourceRef pSource;
      };

      class cInterCooler
      {
      public:
        void SetAmbientSettings(const cAmbientSettings& _ambientSettings) { ambientSettings = _ambientSettings; }
        void SetCarVelocityKPH(float_t _fCarVelocityKPH) { fCarVelocityKPH = _fCarVelocityKPH; }

        void SetInputAirFlow(const cAirFlow& _inputAirFlow) { inputAirFlow = _inputAirFlow; }

        float_t GetMassKg() const { return fMassKg; }
        const cAirFlow& GetOutputAirFlow() const { return outputAirFlow; }

        void Update(sampletime_t currentTime);

      private:
        float_t fMassKg;
        cAmbientSettings ambientSettings;
        float_t fCarVelocityKPH;
        cAirFlow inputAirFlow;

        cAirFlow outputAirFlow;
      };



      class cClutch
      {
      public:
        cClutch();

        float_t GetMassKg() const { return fMassKg; }

        void SetInputClutch0To1(float_t _fInputClutch0To1) { fInputClutch0To1 = _fInputClutch0To1; }
        void SetRPMBeforeClutch(float_t _fRPMBeforeClutch) { fRPMBeforeClutch = _fRPMBeforeClutch; }
        void SetTorqueNmBeforeClutch(float_t _fTorqueNmBeforeClutch) { fTorqueNmBeforeClutch = _fTorqueNmBeforeClutch; }

        float_t GetRPMAfterClutch() const { return fRPMAfterClutch; }
        float_t GetTorqueNmAfterClutch() const { return fTorqueNmAfterClutch; }

        void Update(sampletime_t currentTime);

      private:
        float_t fMassKg;
        float_t fFrictionCoefficient;
        float_t fMaxTorqueNm; // How much torque this clutch can hold, Nm after this will make the clutch slip

        float_t fInputClutch0To1;
        float_t fRPMBeforeClutch;
        float_t fTorqueNmBeforeClutch;

        float_t fRPMAfterClutch;
        float_t fTorqueNmAfterClutch;
      };


      // Gear ratios for a 2004 Corvette
      // 0 = reverse   -3.28:1
      // 1 = neutral   0.0:1
      // 2 = 1st gear  2.97:1
      // 3 = 2nd gear  2.07:1
      // 4 = 3rd gear  1.43:1
      // 5 = 4th gear  1.00:1
      // 6 = 5th gear  0.84:1
      // 7 = 6th gear  0.56:1

      class cGearBox
      {
      public:
        cGearBox();

        void AddGear(float_t fGearRatio);

        // NOTE: This includes reverse (0) and neutral (1)
        size_t GetNumberOfGears() const { return gears.size(); }
        size_t GetCurrentGear() const { return currentGear; }
        float_t GetGearRatio() const;

        void ChangeGearUp();
        void ChangeGearDown();

        void SetMassKg(float_t _fMassKg) { fMassKg = _fMassKg; }
        void SetEfficiency0To1(float_t _fEfficiency0To1) { fEfficiency0To1 = _fEfficiency0To1; }
        void SetRPMBeforeGearBox(float_t _fRPMBeforeGearBox) { fRPMBeforeGearBox = _fRPMBeforeGearBox; }
        void SetTorqueNmBeforeGearBox(float_t _fTorqueNmBeforeGearBox) { fTorqueNmBeforeGearBox = _fTorqueNmBeforeGearBox; }

        float_t GetMassKg() const { return fMassKg; }
        float_t GetRPMAfterGearBox() const { return fRPMAfterGearBox; }
        float_t GetTorqueNmAfterGearBox() const { return fTorqueNmAfterGearBox; }

        void Update(sampletime_t currentTime);

      private:
        float_t fMassKg;
        float_t fEfficiency0To1;
        size_t currentGear;
        std::vector<float_t> gears;

        float_t fRPMBeforeGearBox;
        float_t fTorqueNmBeforeGearBox;

        float_t fRPMAfterGearBox;
        float_t fTorqueNmAfterGearBox;
      };


      // Diff ratio for a 2004 Corvette
      // 3.42:1

      class cDifferential
      {
      public:
        cDifferential();

        void SetMassKg(float_t _fMassKg) { fMassKg = _fMassKg; }
        void SetEfficiency0To1(float_t _fEfficiency0To1) { fEfficiency0To1 = _fEfficiency0To1; }
        void SetRatio(float_t _fRatio) { fRatio = _fRatio; }

        float_t GetMassKg() const { return fMassKg; }
        float_t GetRatio() const { return fRatio; }

        void SetRPMBeforeDifferential(float_t _fRPMBeforeDifferential) { fRPMBeforeDifferential = _fRPMBeforeDifferential; }
        void SetTorqueNmBeforeDifferential(float_t _fTorqueNmBeforeDifferential) { fTorqueNmBeforeDifferential = _fTorqueNmBeforeDifferential; }

        float_t GetRPMAfterDifferential() const { return fRPMAfterDifferential; }
        float_t GetTorqueNmAfterDifferential() const { return fTorqueNmAfterDifferential; }

        void Update(sampletime_t currentTime);

      private:
        float_t fMassKg;
        float_t fEfficiency0To1;
        float_t fRatio;

        float_t fRPMBeforeDifferential;
        float_t fTorqueNmBeforeDifferential;

        float_t fRPMAfterDifferential;
        float_t fTorqueNmAfterDifferential;
      };



      class cWheel
      {
      public:
        cWheel();

        // Total height of wheel is diameter of rim + (2 * profile of tire)
        float_t GetDiametreOfRimCentimetres() const { return fDiametreOfRimCentimetres; }
        float_t GetProfileOfTireCentimetres() const { return fProfileOfTireCentimetres; }
        float_t GetWidthCentimetres() const { return fWidthCentimetres; }

        float_t GetMassOfRimKg() const { return fMassOfRimKg; }
        float_t GetMassOfTireKg() const { return fMassOfTireKg; }

        float_t GetRPM() const { return fRPM; }
        float_t GetTorqueNm() const { return fTorqueNm; }
        float_t GetPowerKw() const { return spitfire::math::NmToKw(fTorqueNm, fRPM); }
        float_t GetSpeedKPH() const;

        void SetRPM(float_t _fRPM) { fRPM = _fRPM; }
        void SetTorqueNm(float_t _fTorqueNm) { fTorqueNm = _fTorqueNm; }

        void Update(sampletime_t currentTime);

      private:
        float_t fDiametreOfRimCentimetres;
        float_t fProfileOfTireCentimetres;
        float_t fWidthCentimetres;
        float_t fMassOfRimKg;
        float_t fMassOfTireKg;
        float_t fRPM;
        float_t fTorqueNm;
      };
    }


    // Restrictions
    // All superchargers and turbo chargers are setup in parallel
    // All intercoolers are in series, air is piped through 0 then 1, etc. to n - 1

    const size_t WHEEL_REAR_LEFT = 0;
    const size_t WHEEL_REAR_RIGHT = 1;
    const size_t WHEEL_FRONT_LEFT = 2;
    const size_t WHEEL_FRONT_RIGHT = 3;

    class cVehicleCar : public cVehicleBase
    {
    public:
      cVehicleCar(cGameObject& object, physics::cCarRef pCar, const std::vector<breathe::scenegraph3d::cGroupNodeRef>& wheels);

      enum class DRIVE {
        AWD,
        RWD,
        FWD
      };

      bool IsAWD() const { return (drive == DRIVE::AWD); }
      bool IsRWD() const { return (drive == DRIVE::RWD); }
      bool IsFWD() const { return (drive == DRIVE::FWD); }

      float_t GetTotalMassKg() const;

      // NOTE: This includes reverse (0) and neutral (1)
      size_t GetGear() const { return gearbox.GetCurrentGear(); }

      float_t GetEngineRPM() const { return engine.GetRPM(); }
      float_t GetEngineTorqueNm() const { return engine.GetTorqueNm(); }
      float_t GetEnginePowerKw() const { return engine.GetPowerKw(); }

      float_t GetActualVelocityKPH() const { return fActualVelocityKPH; }


      // Mostly for debug purposes etc.
      const car::cEngine& GetEngine() const { return engine; }
      const std::vector<car::cSuperCharger>& GetSuperChargers() const { return superChargers; }
      const std::vector<car::cTurboCharger>& GetTurboChargers() const { return turboChargers; }
      const std::vector<car::cWheel>& GetWheels() const { return wheels; }

      void Update(sampletime_t currentTime);

    private:
      virtual void _Init();
      virtual void _Update(sampletime_t currentTime);

      void InitParts();

      DRIVE drive;

      car::cAirFlow engineIntakeAirFlow;
      float_t fActualVelocityKPH;


      // Parts

      car::cChassis chassis;

      car::cEngine engine;

      // These are all optional
      std::vector<car::cSuperCharger> superChargers;
      std::vector<car::cTurboCharger> turboChargers;
      std::vector<car::cInterCooler> interCoolers;

      car::cClutch clutch;
      car::cGearBox gearbox;
      car::cDifferential differential;

      // TODO: suspension, tires

      std::vector<car::cWheel> wheels;




      physics::cCarRef pCar;

      std::vector<breathe::scenegraph3d::cGroupNodeRef> wheelNodes;

      audio::cSourceRef pSourceEngine;
      audio::cSourceRef pSourceTurbo0;
      audio::cSourceRef pSourceTurbo1;
    };
  }
}

#endif // CCAR_H
