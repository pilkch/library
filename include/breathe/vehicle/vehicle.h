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

// Fuel densities
// NOTE: These are at room temperature (20 degrees C)
const float FUEL_DENSITY_PETROL_KG_PER_LITRE = 0.75f;
const float FUEL_DENSITY_DIESEL_KG_PER_LITRE = 0.83f; // European EN 590 Diesel


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

  spitfire::math::cCurve curveRPMToTorqueNm;

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

  // TODO: This is really a function of the torque applied to the crank shaft by the starter motor
  float fCrankRPM;

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


// Calcualte the engine displacement
// https://carbiketech.com/engine-capacity-cc/
// V = π/4 x (D)² x H x N
inline constexpr float GetEngineDisplacementLitres(size_t nCylinders, float fPistonBoremm, float fPistonStrokemm)
{
  const float fCubicMillimeters = ((spitfire::math::cPI * 0.25f * (fPistonBoremm * fPistonBoremm) * fPistonStrokemm) * nCylinders);

  // Convert to cubic litres
  return fCubicMillimeters * 0.000001f;
}

// Number of revolutions per power stroke
// https://en.wikipedia.org/wiki/Mean_effective_pressure
// For a 2-stroke engine: 1
// For a 4-stroke engine: 2
inline constexpr float GetRevolutionsPerPowerStroke(Engine::STROKE stroke)
{
  return (stroke == Engine::STROKE::TWO) ? 1.0f : 2.0f;
}

// The number of firing strokes per second is the number of revolutions per second in the case of two-stroke engines and half the number of revolutions per second in the case of four stroke engines
// https://www.alternatewars.com/BBOW/Engineering/PistonEngine_Power.htm
inline constexpr float GetNumberOfFiringStrokesPerSecond(float fRPM, Engine::STROKE stroke)
{
  const float fRevolutionsPerSecond = fRPM / 60.0f;
  return (stroke == Engine::STROKE::TWO) ? fRevolutionsPerSecond : (fRevolutionsPerSecond * 0.5f);
}

// https://www.alternatewars.com/BBOW/Engineering/PistonEngine_Power.htm
// MPS = ( 2 * Stroke * RPM ) / 60
inline constexpr float GetMeanPistonSpeedMetersPerSecond(float fPistonStrokemm, float fRPM)
{
  const float fPistonStrokeLengthMeters = fPistonStrokemm * 0.001f;
  return (2.0f * fPistonStrokeLengthMeters * fRPM) / 60.0f;
}

// https://en.wikipedia.org/wiki/Mean_effective_pressure
// BMEP = 2pi nc * (T/Vd)
// nc is the number of revolutions per power stroke
// T is the torque in Nm
// Vd is the displacement volume in cubic meters
inline constexpr float GetBrakeMeanEffectivePressurePa(float fDisplacementLitres, float fTorqueNm, Engine::STROKE stroke)
{
  const float fDisplacementCubicMeters = fDisplacementLitres * 0.001f;
  const float fRevolutionsPerPowerStroke = GetRevolutionsPerPowerStroke(stroke);
  return (((2.0f * spitfire::math::cPI * fRevolutionsPerPowerStroke) * fTorqueNm) / fDisplacementCubicMeters);
}


// Basic Equation for Piston Engine Power Output
// https://www.alternatewars.com/BBOW/Engineering/PistonEngine_Power.htm
// P = (n * ( π / 4 ) * D^2 * L) * p * N
inline constexpr float EngineEstimatePowerKiloWatts(size_t nCylinders, float fPistonBoremm, float fPistonStrokemm, float fMeanEffectivePressurePa, float fNumberOfFiringStrokesPerSecond)
{
  const float fPistonBoreMeters = fPistonBoremm * 0.001f;
  const float fPistonStrokeMeters = fPistonStrokemm * 0.001f;
  const float fPowerWatts = (nCylinders * (spitfire::math::cPI / 4.0f) * (fPistonBoreMeters * fPistonBoreMeters) * fPistonStrokeMeters) * fMeanEffectivePressurePa * fNumberOfFiringStrokesPerSecond;

  // Convert to kilowatts
  return fPowerWatts * 0.001f;
}



// TODO: Typically we would idle higher for say 30 seconds after starting until the engine has reached the operating temperature range
// TODO: Support turning on the radiator fans when the water temperature goes too high
// TODO: Identify slipping wheels and support traction control (Cut ignition? Apply the brake to just that wheel?)
// TODO: Identify locked up wheels and support ABS (Release brake)
// TODO: Implement an automatic transmission with the ECU picking which gear to set the transmission to and when
class ECU {
public:
  ECU();

  enum class POWER_STATE {
    OFF,
    ACCESSORIES_ON, // We haven't turned on the engine yet or it has stalled
    ACCESSORIES_OFF_STARTER_MOTOR_FIRING, // We are trying to start the engine
    ACCESSORIES_ON_ENGINE_RUNNING, // We have started the engine and it is currently running normally
  };
  POWER_STATE powerState;

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


struct VehicleInputs {
public:
  VehicleInputs();

  void Clear();

  bool headlights; // NOTE: We could have off, auto, low, high
  bool ignitionKeyTurned;
  float fHandBrake0To1;
  float fPedalTravelClutch0To1;
  float fPedalTravelAccelerator0To1; // 0 means the foot is off the pedal, 1 means the pedal is to the floor
  float fPedalTravelBrake0To1;
};

struct ECUActions {
  ECUActions();

  void Clear();

  bool headlights;
  float fHandBrake0To1;
  float fClutch0To1;
  float fThrottle0To1;
  float fBrake0To1;
};

class Vehicle {
public:
  float GetRPMAtFlywheel() const { return engine.GetRPM(); }
  float GetRPMAfterClutch() const { return 0.0f; }
  float GetRPMAfterGearBox() const { return 0.0f; }

  part::Engine engine;
  part::ECU ecu;
  part::Clutch clutch;
  part::GearBox gearBox;
  part::Body body;

  // Dynamic
  // NOTE: These inputs are the vehicle inputs after being processed by the ECU
  ECUActions ecuActions;
};


// States
// open, zero torque is transmitted between the input and the output shafts
// slipping, some amount of torque is transmitted between input and output shafts; the speed difference between input and output shaft is significant (e.g. 500 rpm)
// micro-slipping, almost all of the input torque is transmitted through the clutch; the speed difference between input and output shaft is very small, around 5-10 rpm
// locked (closed, clamped), there is no slip between input and output shaft, all the input torque is transmitted through the clutch
// TODO: Do we even care about micro slipping? Just check if state == SLIPPING and then check the difference between input and output torque?
enum class CLUTCH_STATE {
  OPEN,
  SLIPPING,
  MICRO_SPLIPPING,
  LOCKED,
};

float GetClutchTorqueCapacityNm(const breathe::vehicle::part::Clutch& clutch);
float GetClutchOutputTorqueNm(float fPedalTravelClutch0To1, float fInputTorqueNm, const breathe::vehicle::part::Clutch& clutch, CLUTCH_STATE& outputClutchState);

float GetTorqueConverterTorqueOutNm(float fInputTorqueNm);

void UpdateECU(float fTimeStepFractionOfSecond, const VehicleInputs& inputs, breathe::vehicle::Vehicle& vehicle);

void UpdateEngineDrivetrainWheels(float fTimeStepFractionOfSecond, breathe::vehicle::Vehicle& vehicle);

void Update(float fTimeStepFractionOfSecond, const VehicleInputs& inputs, breathe::vehicle::Vehicle& vehicle);

}

}
