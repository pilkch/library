#ifndef CCAR_H
#define CCAR_H

#include <spitfire/math/cCurve.h>

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
        void SetAcceleratorInput0To1(float_t _fAcceleratorInput0To1) { fAcceleratorInput0To1 = _fAcceleratorInput0To1; }
        void SetInputPressureKPA(float_t _fInputPressureKPA) { fInputPressureKPA = _fInputPressureKPA; }
        void SetInputTemperatureDegreesCelcius(float_t _fInputTemperatureDegreesCelcius) { fInputTemperatureDegreesCelcius = _fInputTemperatureDegreesCelcius; }

        float_t GetRPM() const { return fRPM; }
        float_t GetTorqueNm() const { return fTorqueNm; }
        float_t GetExhaustPressureKPA() const { return fExhaustPressureKPA; }
        float_t GetExhaustTemperatureDegreesCelcius() const { return fExhaustTemperatureDegreesCelcius; }


        void Update(sampletime_t currentTime);

      private:
        float_t fAcceleratorInput0To1;
        float_t fInputPressureKPA;
        float_t fInputTemperatureDegreesCelcius;
        math::cCurve rpmToTorqueCurve;

        float_t fRPM;
        float_t fTorqueNm;
        float_t fExhaustPressureKPA;
        float_t fExhaustTemperatureDegreesCelcius;
      };



      class cSuperCharger
      {
      public:
        void SetEngineRPM(float_t _fRPM) { fRPM = _fRPM; }
        void SetAmbientAirPressureKPA(float_t _fAmbientAirPressureKPA) { fAmbientAirPressureKPA = _fAmbientAirPressureKPA; }
        void SetAmbientAirTemperatureDegreesCelcius(float_t _fAmbientAirTemperatureDegreesCelcius) { fAmbientAirTemperatureDegreesCelcius = _fAmbientAirTemperatureDegreesCelcius; }

        float_t GetOutputPressureKPA() const { return fOutputPressureKPA; }
        float_t GetOutputTemperatureDegreesCelcius() const { return fOutputTemperatureDegreesCelcius; }

        void Update(sampletime_t currentTime);

      private:
        float_t fEngineRPM;
        float_t fAmbientAirPressureKPA;
        float_t fAmbientAirTemperatureDegreesCelcius;
        math::cCurve rpmToPressureCurve;

        float_t fRPM;
        float_t fOutputPressureKPA;
        float_t fOutputTemperatureDegreesCelcius;
      };

      class cTurboCharger
      {
      public:
        void SetEngineExhaustPressureKPA(float_t _fEngineExhaustPressureKPA) { fEngineExhaustPressureKPA = _fEngineExhaustPressureKPA; }
        void SetEngineExhaustTemperatureDegreesCelcius(float_t _fEngineExhaustTemperatureDegreesCelcius) { fEngineExhaustTemperatureDegreesCelcius = _fEngineExhaustTemperatureDegreesCelcius; }

        float_t GetOutputPressureKPA() const { return fOutputPressureKPA; }
        float_t GetOutputTemperatureDegreesCelcius() const { return fOutputTemperatureDegreesCelcius; }

        void Update(sampletime_t currentTime);

      private:
        float_t fEngineExhaustPressureKPA;
        float_t fEngineExhaustTemperatureDegreesCelcius;
        math::cCurve rpmToPressureCurve;

        float_t fRPM;
        float_t fOutputPressureKPA;
        float_t fOutputTemperatureDegreesCelcius;



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
        void SetCarVelocityKPH(float_t _fCarVelocityKPH) { fCarVelocityKPH = _fCarVelocityKPH; }
        void SetAmbientAirPressureKPA(float_t _fAmbientAirPressureKPA) { fAmbientAirPressureKPA = _fAmbientAirPressureKPA; }
        void SetAmbientAirTemperatureDegreesCelcius(float_t _fAmbientAirTemperatureDegreesCelcius) { fAmbientAirTemperatureDegreesCelcius = _fAmbientAirTemperatureDegreesCelcius; }
        void SetInputPressureKPA(float_t _fInputPressureKPA) { fInputPressureKPA = _fInputPressureKPA; }
        void SetInputTemperatureDegreesCelcius(float_t _fInputTemperatureDegreesCelcius) { fInputTemperatureDegreesCelcius = _fInputTemperatureDegreesCelcius; }

        float_t GetOutputPressureKPA() const { return fOutputPressureKPA; }
        float_t GetOutputTemperatureDegreesCelcius() const { return fOutputTemperatureDegreesCelcius; }

        void Update(sampletime_t currentTime);

      private:
        float_t fCarVelocityKPH;
        float_t fAmbientAirPressureKPA;
        float_t fAmbientAirTemperatureDegreesCelcius;
        float_t fInputPressureKPA;
        float_t fInputTemperatureDegreesCelcius;

        float_t fOutputPressureKPA;
        float_t fOutputTemperatureDegreesCelcius;
      };

      class cWheel
      {
      public:
        float_t GetRPM() const { return fRPM; }
        float_t GetTorqueNm() const { return fTorqueNm; }
        float_t GetWheelSpeedKPH() const;

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

      void SetAmbientAirPressureKPA(float_t _fAmbientAirPressureKPA) { fAmbientAirPressureKPA = _fAmbientAirPressureKPA; }
      void SetAmbientAirTemperatureDegreesCelcius(float_t _fAmbientAirTemperatureDegreesCelcius) { fAmbientAirTemperatureDegreesCelcius = _fAmbientAirTemperatureDegreesCelcius; }

      void Update(sampletime_t currentTime);

    private:
      virtual void _Update(sampletime_t currentTime);

      DRIVE drive;
      float_t fAmbientAirPressureKPA;
      float_t fAmbientAirTemperatureDegreesCelcius;

      float_t fEngineInputAirPressureKPA;
      float_t fEngineInputAirTemperatureDegreesCelcius;


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



      audio::cSourceRef pSourceEngine;
      audio::cSourceRef pSourceTurbo0;
      audio::cSourceRef pSourceTurbo1;
    };
  }
}

#endif // CCAR_H
