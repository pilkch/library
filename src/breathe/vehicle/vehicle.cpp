// Standard headers
#include <iostream>
#include <fstream>

#include <spitfire/math/units.h>
#include <spitfire/math/cCurve.h>

// Breathe headers
#include <breathe/vehicle/vehicle.h>

namespace breathe {

Environment::Environment() :
  fAirTemperatureDegreesCelcius(spitfire::math::GENERIC_AMBIENT_AIR_AT_SEA_LEVEL_TEMPERATURE_DEGREES_CELCIUS),
  fAirPressureKPa(spitfire::math::GENERIC_AMBIENT_AIR_AT_SEA_LEVEL_PRESSURE_KPA),
  fAirDensityKgPerM3(spitfire::math::GENERIC_AMBIENT_AIR_AT_SEA_LEVEL_DENSITY_KG_PER_CUBIC_METER)
{
}



namespace vehicle {

namespace part {

ElectricMotor::ElectricMotor() :
  fFreeRunSpeedRPM(0.0f),
  fFreeRunCurrentAmps(1.0f),
  fStallTorqueNm(1.0f),
  uiOutputGearTeeth(1),
  fInputVoltage(0.0f),
  fRPM(0.0f)
{
}

Clutch::Clutch() :
  fMassKg(1.0f),
  fSurfaceMeanEffectiveRadiusm((0.21f + 0.24f) / 2.0f), // Average of inner and outer disc radius
  fSurfacem2(0.2f),
  fFrictionCoefficient(0.8f),
  fTemperatureDegreesCelcius(spitfire::math::GENERIC_AMBIENT_AIR_AT_SEA_LEVEL_TEMPERATURE_DEGREES_CELCIUS)
{
}

Engine::FlyWheel::FlyWheel() :
  fMassKg(1.0f),
  uiTeeth(1)
{
}

TorqueConverter::TorqueConverter() :
  fMassKg(30.0f),
  fStallSpeedRPM(300.0f),
  fTemperatureDegreesCelcius(spitfire::math::GENERIC_AMBIENT_AIR_AT_SEA_LEVEL_TEMPERATURE_DEGREES_CELCIUS)
{
}

Engine::Engine() :
  cylinderArrangement(CYLINDER_ARRANGEMENT::V),
  cylinders(6),
  fBoreMillimetres(1.0f),
  fStrokeMillimetres(1.0f),
  fPistonRodLengthMillimetres(1.0f),
  fDisplacementLitres(1.0f),
  fTotalEngineMassKg(10.0f),
  fBlockMassKg(1.0f),
  fPistonMassKg(1.0f),
  fConrodMassKg(1.0f),
  fCrankShaftMassKg(1.0f),
  fCrankRPM(150.0f),
  fStallRPM(550.0f),

  fCrankshaftRotationRadians(0.0f),
  fCrankshaftAngularVelocityRadiansPerSecond(0.0f),
  fOilTemperatureCelcius(spitfire::math::GENERIC_AMBIENT_AIR_AT_SEA_LEVEL_TEMPERATURE_DEGREES_CELCIUS)
{
}

bool Engine::IsRunning() const
{
  // We count the engine as running if:
  // 1. The starter motor is *not* running, and
  // 2. The engine speed is currently above the stall speed

  // TODO: Count as running if the engine has not gone below the stall speed since the last time the ignition was turned on?

  return ((starterMotor.fInputVoltage < 0.01f) && (GetRPM() >= fStallRPM));
}


ECU::ECU() :
  powerState(POWER_STATE::OFF),
  fIdleDesiredColdRPM(1200.0f),
  fIdleDesiredOperatingRPM(800.0f),

  fIdleSmallDifferenceRPM(10.0f),
  fIdleLargeDifferenceRPM(100.0f),
  fIdleSmallThrottleCorrection0To1(0.1f),
  fIdleLargeThrottleCorrection0To1(0.2f),

  fRevLimiterThrottleCutRPM(5500.0f)
{
}

Body::Body() :
  fFrontalAreaMetersSquared(0.0f),
  fDragCoefficient(0.0f)
{
}

}


VehicleInputs::VehicleInputs() :
  headlights(false),
  ignitionKeyTurned(false),
  fHandBrake0To1(0.0f),
  fPedalTravelClutch0To1(0.0f),
  fPedalTravelAccelerator0To1(0.0f),
  fPedalTravelBrake0To1(0.0f)
{
}

void VehicleInputs::Clear()
{
  headlights = false;
  ignitionKeyTurned = false;
  fHandBrake0To1 = 0.0f;
  fPedalTravelClutch0To1 = 0.0f;
  fPedalTravelAccelerator0To1 = 0.0f;
  fPedalTravelBrake0To1 = 0.0f;
}


ECUActions::ECUActions() :
  headlights(false),
  fHandBrake0To1(0.0f),
  fClutch0To1(0.0f),
  fThrottle0To1(0.0f),
  fBrake0To1(0.0f)
{
}

void ECUActions::Clear()
{
  headlights = false;
  fHandBrake0To1 = 0.0f;
  fClutch0To1 = 0.0f;
  fThrottle0To1 = 0.0f;
  fBrake0To1 = 0.0f;
}

float GetClutchTorqueCapacityNm(const breathe::vehicle::part::Clutch& clutch)
{
#if 1
  // Maximum torque this clutch can hold without slipping
  // https://86.43.94.97/moodlecp9a/mod/resource/view.php?id=235
  // T=SPμR
  //
  // T is the maximum torque that could be transmitted (measured in SI units, Nm)
  // S is the number of friction couples in contact. For example, in a single plate dry clutch, there are 2 friction couples: one between the front of the clutch disc and the flywheel, and one between the back of the clutch disk and the pressure plate.
  // P Pressure plate clamping force. [This is a force and is measured in newtons]
  // μ is the coefficient of friction between the clutch lining and the flywheel and pressure plate. This is just a ratio, with no units of measurement.
  // R is the mean (average) effective radius (M.E.R.) of the clutch disc. This is a distance and is measured in metres.

  // 2 friction couples for a single plate dry clutch
  const size_t frictionCouples = 2;

  const float fTorqueCapacityNm = float(frictionCouples) * clutch.fMaxEngagedForceN * clutch.fFrictionCoefficient * clutch.fSurfaceMeanEffectiveRadiusm;
#else
  const float fNormalForceN = fEngagement0To1 * clutch.fMaxEngagedForceN;

  const float fPressurePa = fNormalForceN / clutch.fSurfacem2;

  const float fFrictionForceN = clutch.fFrictionCoefficient * fPressurePa;

  // Maximum torque this clutch can hold without slipping
  const float fTorqueCapacityNm = ...;
#endif

  return fTorqueCapacityNm;
}

float GetClutchOutputTorqueNm(float fPedalTravelClutch0To1, float fInputTorqueNm, const breathe::vehicle::part::Clutch& clutch, CLUTCH_STATE& outputClutchState)
{
  float fOutputTorqueNm = 0.0f;

  outputClutchState = CLUTCH_STATE::OPEN;

  if (fPedalTravelClutch0To1 <= 0.9f) {
    const float fEngagement0To1 = 1.0f - fPedalTravelClutch0To1;

    const float fMaxTorqueCapacityNm = GetClutchTorqueCapacityNm(clutch);
    const float fTorqueCapacityNm = fEngagement0To1 * fMaxTorqueCapacityNm;

    std::cout<<"clutch.fFrictionCoefficient: "<<clutch.fFrictionCoefficient<<std::endl;
    std::cout<<"clutch.fSurfaceMeanEffectiveRadiusm: "<<clutch.fSurfaceMeanEffectiveRadiusm<<std::endl;
    std::cout<<"clutch.fSurfacem2: "<<clutch.fSurfacem2<<std::endl;
    std::cout<<"fInputTorqueNm: "<<fInputTorqueNm<<std::endl;
    std::cout<<"fEngagement0To1: "<<fEngagement0To1<<std::endl;
    std::cout<<"fTorqueCapacityNm: "<<fTorqueCapacityNm<<std::endl;

    if (fInputTorqueNm > fTorqueCapacityNm) {
      // Slipping
      fOutputTorqueNm = fTorqueCapacityNm;
      outputClutchState = CLUTCH_STATE::SLIPPING;
    } else {
      // Locking
      fOutputTorqueNm = fInputTorqueNm;
      outputClutchState = CLUTCH_STATE::LOCKED;
    }

    // TODO: SLIPPING, // not enough clamping force and difference in speed > 100 rpm?
    // TODO: MICRO_SPLIPPING, // not enough clamping force and difference in speed > 10 rpm?
  }

  return fOutputTorqueNm;
}

// https://simplemotor.com/calculations/

float GetTorqueConverterTorqueOutNm(float fInputTorqueNm)
{
  /*const float fInputPowerW = fInputTorqueNm * RPMToRadiansPerSecond(fRotationalSpeedInputShaftRPM);

  const float fPowerLossW = f(friction, viscous effects, other effects….)

  const float fOutputPowerW = fOutputTorqueNm * fRotationalSpeedOutputShaftRadiansPerSecond;
  const float fOutputPowerW = fInputPowerW – fPowerLossW = fInputTorqueNm * RPMToRadiansPerSecond(fRotationalSpeedInputShaftRPM) – fPowerLossW;

  const float fOutputPowerW = fMechanicalEfficiency * fInputPowerW;

  const float fMechanicalEfficiency = fOutputPowerW / fInputPowerW;*/

  return 0.0f;
}


void UpdateECU(float fTimeStepFractionOfSecond, const VehicleInputs& inputs, breathe::vehicle::Vehicle& vehicle)
{
  // Update the vehicle from the player inputs
  vehicle.ecuActions.headlights = inputs.headlights;
  vehicle.ecuActions.fHandBrake0To1 = spitfire::math::clamp(inputs.fHandBrake0To1, 0.0f, 1.0f);
  vehicle.ecuActions.fClutch0To1 = spitfire::math::clamp(inputs.fPedalTravelClutch0To1, 0.0f, 1.0f);
  vehicle.ecuActions.fThrottle0To1 = spitfire::math::clamp(inputs.fPedalTravelAccelerator0To1, 0.0f, 1.0f);
  vehicle.ecuActions.fBrake0To1 = spitfire::math::clamp(inputs.fPedalTravelBrake0To1, 0.0f, 1.0f);

  part::ECU& ecu = vehicle.ecu;

  // Check if we need to transition to a new state
  if ((ecu.powerState == part::ECU::POWER_STATE::OFF) || (ecu.powerState == part::ECU::POWER_STATE::ACCESSORIES_ON)) {
    if (inputs.ignitionKeyTurned && !vehicle.engine.IsRunning()) {
      ecu.powerState = part::ECU::POWER_STATE::ACCESSORIES_OFF_STARTER_MOTOR_FIRING;
    }
  }

  if (ecu.powerState == part::ECU::POWER_STATE::ACCESSORIES_OFF_STARTER_MOTOR_FIRING) {
    if (vehicle.engine.IsRunning()) {
      ecu.powerState = part::ECU::POWER_STATE::ACCESSORIES_ON_ENGINE_RUNNING;
    }
  }

  if (ecu.powerState == part::ECU::POWER_STATE::ACCESSORIES_ON_ENGINE_RUNNING) {
    if (!vehicle.engine.IsRunning()) {
      // The engine has stalled, return to accessories on
      ecu.powerState = part::ECU::POWER_STATE::ACCESSORIES_ON;
    }
  }



  if (ecu.powerState == part::ECU::POWER_STATE::ACCESSORIES_OFF_STARTER_MOTOR_FIRING) {
    vehicle.ecuActions.headlights = false; // Turn off the lights when we are starting the engine
    vehicle.engine.starterMotor.fInputVoltage = 12.0f;
  } else {
    vehicle.engine.starterMotor.fInputVoltage = 0.0f;
  }

  if ((ecu.powerState == part::ECU::POWER_STATE::ACCESSORIES_OFF_STARTER_MOTOR_FIRING) || (ecu.powerState == part::ECU::POWER_STATE::ACCESSORIES_ON_ENGINE_RUNNING)) {
    const float fRPM = vehicle.GetRPMAtFlywheel();

    // Adjust idle RPM speed to keep the engine from stalling
    float fRPMDiff = 0.0f;

    if (vehicle.engine.fOilTemperatureCelcius < 90.0f) {
      // Engine is cold
      if (fRPM < ecu.fIdleDesiredColdRPM) {
        fRPMDiff = ecu.fIdleDesiredColdRPM - fRPM;
      }
    } else {
      // Engine is warm
      if (fRPM < ecu.fIdleDesiredOperatingRPM) {
        fRPMDiff = ecu.fIdleDesiredOperatingRPM - fRPM;
      }
    }

    // Check if we need to adjust the idle RPM speed
    if (fRPMDiff > ecu.fIdleLargeDifferenceRPM) {
      vehicle.ecuActions.fThrottle0To1 = max(vehicle.ecuActions.fThrottle0To1, ecu.fIdleLargeThrottleCorrection0To1);
    } else if (fRPMDiff > ecu.fIdleSmallDifferenceRPM) {
      vehicle.ecuActions.fThrottle0To1 = max(vehicle.ecuActions.fThrottle0To1, ecu.fIdleSmallThrottleCorrection0To1);
    }


    // Rev limiter
    if (vehicle.GetRPMAtFlywheel() >= ecu.fRevLimiterThrottleCutRPM) {
      vehicle.ecuActions.fThrottle0To1 = 0.0f;
    }
  }


  // Clamp vehicle inputs
  vehicle.ecuActions.fHandBrake0To1 = spitfire::math::clamp(vehicle.ecuActions.fHandBrake0To1, 0.0f, 1.0f);
  vehicle.ecuActions.fClutch0To1 = spitfire::math::clamp(vehicle.ecuActions.fClutch0To1, 0.0f, 1.0f);
  vehicle.ecuActions.fThrottle0To1 = spitfire::math::clamp(vehicle.ecuActions.fThrottle0To1, 0.0f, 1.0f);
  vehicle.ecuActions.fBrake0To1 = spitfire::math::clamp(vehicle.ecuActions.fBrake0To1, 0.0f, 1.0f);
}

void UpdateEngineDrivetrainWheels(float fTimeStepFractionOfSecond, breathe::vehicle::Vehicle& vehicle)
{
  if (vehicle.engine.starterMotor.fInputVoltage >= 12.0f) {
    // NOTE: When voltage is applied to the starter motor a solenoid pushes the starter motor drive shaft out to engage the starter motor cog with the flywheel
    // We just assume a perfect connection is occuring when a voltage is applied and there is zero connection when no voltage is applied

    // Apply the voltage to the starter motor to start it turning
    // HACK: We just asssume if the input voltage is >= 12V then we are driving the starter motor and we apply torque based on the RPM of the motor

    // *********** Uncomment this
    /*const float fStarterMotorTorqueNm = vehicle.engine.curveRPMToTorqueNm.GetYAtPointX(vehicle.engine.fRPM);

    ... calculate torque required to turn 
    const float fRequiredTorqueToTurnCrankshaftNm = ...;

    if (fRequiredTorqueToTurnCrankshaftNm > fStallTorqueNm) {
      ... burning out starter motor
    } else {
      ... apply torque
    }*/
  }

  /*const float fEngineTorqueNm = vehicle.engine.fOutputRPMAtFlywheel ...;

  CLUTCH_STATE clutchState = CLUTCH_STATE::OPEN;

  const float fOutputTorqueNm = GetClutchOutputTorqueNm(vehicle.engine.fPedalTravelClutch0To1, fEngineTorqueNm, vehicle.clutch, clutchState);
  ...

  if (clutchState == CLUTCH_STATE::OPEN) {
    ... no connection between the engine and gearbox
  } else {
    ... at least some torque is being applied to the gearbox
  }

  const float fEngineConnectedInertia = ... get inertia of engine components, flywheel, then if clutch in inertia of gearbox, driveshaft, diff, etc.;

  vehicle.engine.fCrankshaftAngularVelocityRadiansPerSecond += fTimeStepFractionOfSecond * fEngineConnectedInertia * fEngineTorqueNm;
  vehicle.engine.fCrankshaftRotationRadians += fTimeStepFractionOfSecond * vehicle.engine.fCrankshaftAngularVelocityRadiansPerSecond;
*/
}

void Update(float fTimeStepFractionOfSecond, const VehicleInputs& inputs, breathe::vehicle::Vehicle& vehicle)
{
  UpdateECU(fTimeStepFractionOfSecond, inputs, vehicle);

  UpdateEngineDrivetrainWheels(fTimeStepFractionOfSecond, vehicle);
}

}

}
