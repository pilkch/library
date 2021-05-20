#pragma once

// Standard headers
#include <array>

#include <spitfire/math/units.h>
#include <spitfire/math/cCurve.h>

namespace breathe {

// TODO: Move this somewhere more generic
class Environment {
public:
  Environment();

  float fAirTemperatureDegreesCelcius;
  float fAirPressureKPa;
  float fAirDensityKgPerM3;
};


namespace vehicle {

namespace part {


// From vdrift

struct DriveShaft
{
  float inertia;
  float inv_inertia;
  float ang_velocity;
  float angle;

  DriveShaft() : inertia(1), inv_inertia(1), ang_velocity(0), angle(0) {}

  void applyImpulse(float impulse)
  {
    ang_velocity += inv_inertia * impulse;
  }

  void integrate(float dt)
  {
    angle += ang_velocity * dt;
  }
};


class ElectricMotor {
public:
  ElectricMotor();

  float fFreeRunSpeedRPM;
  float fFreeRunCurrentAmps;
  float fStallTorqueNm; // If there is this much or more torque to overcome then the electric motor is "stalling" and will draw its maximum current, get warm and start burning out

  spitfire::math::cCurve curveAmpInToTorqueOutNm;

  uint8_t uiOutputGearTeeth;

  // Dynamic
  float fInputVoltage;
  float fRPM;
};

class Clutch {
public:
  Clutch();

  float fMassKg;
  float fSurfaceMeanEffectiveRadiusm;
  float fSurfacem2;
  float fFrictionCoefficient;

  // 1. Calculate the level of engagement
  spitfire::math::cCurve curveTravel0To1_To_Engagement0To1; // Look up table of how far the clutch moves (0 is "foot off the pedal", 1 is "pedal to the floor") to how much engagement the clutch has (1 is 100% biting)
  // 2. Then we look up what that engagement means in terms of torque
  spitfire::math::cCurve curveEngagement0To1_To_MaxNm; // Look up table of how far the clutch moves (0 is "foot off the pedal", 1 is "pedal to the floor") to how much engagement the clutch has (1 is 100% biting)

  float fMaxEngagedForceN;

  // Dynamic
  float fTemperatureDegreesCelcius;
};

// TODO: Refactor to a a Turbine class then use it for turbos and torque converters?
class TorqueConverter {
  TorqueConverter();

  float fMassKg;
  float fStallSpeedRPM; // The speed at which torque is passed through to the gear box

  // Dynamic
  float fTemperatureDegreesCelcius;
};

class GearBox {
public:
  std::vector<float> gears; // Negative for a reverse gear, 0 for neutral and positive for forwards gears

  // Dynamic
  size_t currentGear;
};

class Engine {
public:
  Engine();

  bool IsRunning() const;

  float GetRPM() const { return spitfire::math::RadiansPerSecondToRPM(fCrankshaftAngularVelocityRadiansPerSecond); }

  // TODO: Do we care about the angle for V and W engines?
  enum class CYLINDER_ARRANGEMENT {
    INLINE,
    FLAT,
    V,
    W,
    ROTARY,
  };

  enum class STROKE {
    TWO,
    FOUR,
  };

  enum class FUEL {
    PETROL,
    DIESEL,
    ETHANOL,
    E85,
  };

  CYLINDER_ARRANGEMENT cylinderArrangement;
  size_t cylinders;
  float fBoreMillimetres;
  float fStrokeMillimetres;
  float fPistonRodLengthMillimetres;
  float fDisplacementLitres;

  float fTotalEngineMassKg;
  float fBlockMassKg;
  float fPistonMassKg;
  float fConrodMassKg;
  float fCrankShaftMassKg;

  // TODO: This should really be a function of how much power the engine is making and how much load there is from the drive train
  float fStallRPM;

  struct FlyWheel {
    FlyWheel();

    float fMassKg;
    uint8_t uiTeeth;
  };

  FlyWheel flyWheel;
  ElectricMotor starterMotor;

  // Dynamic
  float fCrankshaftRotationRadians;
  float fCrankshaftAngularVelocityRadiansPerSecond;
  float fOilTemperatureCelcius;
};


// TODO: Typically we would idle will typically be higher for say 30 seconds after starting until the engine has reached the operating temperature range
// TODO: Support turning on the radiator fans when the water temperature goes too high
// TODO: Identify slipping wheels and support traction control (Cut ignition? Apply the brake to just that wheel?)
// TODO: Identify locked up wheels and support ABS (Release brake)
// TODO: Implement an automatic transmission with the ECU picking which gear to set the transmission to and when
class ECU {
public:
  ECU();

  float fIdleDesiredColdRPM; // Our target RPM when the engine is cold
  float fIdleDesiredOperatingRPM; // Our target RPM when the engine is at operating temperature

  float fIdleSmallDifferenceRPM; // If the difference in RPM is this much apply the small throttle correction
  float fIdleLargeDifferenceRPM; // If the difference in RPM is this much apply the large throttle correction
  float fIdleSmallThrottleCorrection0To1; // The amount of throttle to apply for a small correction
  float fIdleLargeThrottleCorrection0To1; // The amount of throttle to apply for a larger correction

  float fRevLimiterThrottleCutRPM; // The RPM at which throttle is cut to protect the engine
};




// TODO: Add wings to generate negative lift in Newtons
// TODO: Road vehicles can actually generate positive lift at 200KPH. Simulate general body lift as a small wing?
class Body {
public:
  Body();

  float fFrontalAreaMetersSquared;
  float fDragCoefficient;
};

}


class Vehicle {
public:
  Vehicle();

  float GetRPMAtFlywheel() const { return engine.GetRPM(); }
  float GetRPMAfterClutch() const { return 0.0f; }
  float GetRPMAfterGearBox() const { return 0.0f; }

  part::Engine engine;
  part::ECU ecu;
  part::Clutch clutch;
  part::GearBox gearBox;
  part::Body body;

  // Dynamic
  bool starterMotorEngaged;
  float fPedalTravelClutch0To1;
  float fPedalTravelAccelerator0To1; // 0 means the foot is off the pedal, 1 means the pedal is to the floor
  float fPedalTravelBrake0To1;
};


// States
// open, zero torque is transmitted between the input and the output shafts
// slipping, some amount of torque is transmitted between input and output shafts; the speed difference between input and output shaft is significant (e.g. 500 rpm)
// micro-slipping, almost all of the input torque is transmitted through the clutch; the speed difference between input and output shaft is very small, around 5-10 rpm
// locked (closed, clamped), there is no slip between input and output shaft, all the input torque is transmitted through the clutch
enum class CLUTCH_STATE {
  OPEN,
  SLIPPING,
  MICRO_SPLIPPING,
  LOCKED,
};

float GetClutchOutputTorqueNm(float fPedalTravelClutch0To1, float fInputTorqueNm, const breathe::vehicle::part::Clutch& clutch, CLUTCH_STATE& outputClutchState);

float GetTorqueConverterTorqueOutNm(float fInputTorqueNm);

void UpdateECU(float fTimeStepFractionOfSecond, breathe::vehicle::Vehicle& vehicle, float& fOutputThrottle0To1);

void UpdateEngineDrivetrainWheels(float fTimeStepFractionOfSecond, breathe::vehicle::Vehicle& vehicle);

void Update(float fTimeStepFractionOfSecond, breathe::vehicle::Vehicle& vehicle);

}

}
