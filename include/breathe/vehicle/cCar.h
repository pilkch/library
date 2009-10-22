#ifndef CCAR_H
#define CCAR_H

#include <spitfire/math/cCurve.h>

#include <breathe/game/component.h>

#include <breathe/vehicle/cVehicle.h>

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


      class cEngine
      {
      public:
        void SetAmbientSettings(const cAmbientSettings& _ambientSettings) { ambientSettings = _ambientSettings; }
        void SetAcceleratorInput0To1(float_t _fAcceleratorInput0To1) { fAcceleratorInput0To1 = _fAcceleratorInput0To1; }
        void SetIntakeAirFlow(const cAirFlow& _intakeAirFlow) { intakeAirFlow = _intakeAirFlow; }

        float_t GetRPM() const { return fRPM; }
        float_t GetTorqueNm() const { return fTorqueNm; }
        const cAirFlow& GetExhaustAirFlow() const { return exhaustAirFlow; }


        void Update(sampletime_t currentTime);

      private:
        cAmbientSettings ambientSettings;
        float_t fAcceleratorInput0To1;
        cAirFlow intakeAirFlow;
        math::cCurve rpmToTorqueCurve;

        float_t fRPM;
        float_t fTorqueNm;
        cAirFlow exhaustAirFlow;
      };



      class cSuperCharger
      {
      public:
        void SetAmbientSettings(const cAmbientSettings& _ambientSettings) { ambientSettings = _ambientSettings; }
        void SetEngineRPM(float_t _fRPM) { fRPM = _fRPM; }

        const cAirFlow& GetOutputAirFlow() const { return outputAirFlow; }

        void Update(sampletime_t currentTime);

      private:
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
        void SetAmbientSettings(const cAmbientSettings& _ambientSettings) { ambientSettings = _ambientSettings; }
        void SetEngineExhaustAirFlow(const cAirFlow& _exhaustAirFlow) { exhaustAirFlow = _exhaustAirFlow; }

        const cAirFlow& GetOutputAirFlow() const { return outputAirFlow; }

        void Update(sampletime_t currentTime);

      private:
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

        const cAirFlow& GetOutputAirFlow() const { return outputAirFlow; }

        void Update(sampletime_t currentTime);

      private:
        cAmbientSettings ambientSettings;
        float_t fCarVelocityKPH;
        cAirFlow inputAirFlow;

        cAirFlow outputAirFlow;
      };

      class cWheel
      {
      public:
        float_t GetRPM() const { return fRPM; }
        float_t GetTorqueNm() const { return fTorqueNm; }
        float_t GetSpeedKPH() const;

        void SetRPM(float_t _fRPM) { fRPM = _fRPM; }
        void SetTorqueNm(float_t _fTorqueNm) { fTorqueNm = _fTorqueNm; }

        void Update(sampletime_t currentTime);

      private:
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
      explicit cVehicleCar(cGameObject& object);

      enum class DRIVE {
        AWD,
        RWD,
        FWD
      };

      bool IsAWD() const { return (drive == DRIVE::AWD); }
      bool IsRWD() const { return (drive == DRIVE::RWD); }
      bool IsFWD() const { return (drive == DRIVE::FWD); }

      float GetRPM() const { return engine.GetRPM(); }

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


      car::cEngine engine;

      // These are all optional
      std::vector<car::cSuperCharger> superChargers;
      std::vector<car::cTurboCharger> turboChargers;
      std::vector<car::cInterCooler> interCoolers;

      //car::cGearbox gearbox;
      //car::cClutch clutch;
      //vcDifferential differential;

      // TODO: suspension, tires

      std::vector<car::cWheel> wheels;



      physics::cCarRef pCar;

      audio::cSourceRef pSourceEngine;
      audio::cSourceRef pSourceTurbo0;
      audio::cSourceRef pSourceTurbo1;
    };
  }
}

#endif // CCAR_H
