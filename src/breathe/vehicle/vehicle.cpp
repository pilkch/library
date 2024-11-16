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

// Battery
//
// https://haynes.com/en-au/tips-tutorials/how-test-car-battery
// https://mechanicbase.com/electric/car-battery-ideal-voltage-range/
//
// Measured voltage should be at least 12.6 volts for a healthy battery
// At 12.2 volts it is only 50% charged
// Below 12 volts it is classed as discharged
// With the engine running the voltage should read between 13.5 and 14.7 volts for a healthy battery and alternator
//
// With the engine turned off, the voltage on the meter should be 12.4 to 12.6 volts on a healthy battery.
// The measurement should rise after you start the car engine and the alternator starts to charge the electrical system. A fully charged car battery voltage falls between 13.7 and 14.7 volts with the engine running.
// Once it goes under 12 volts, the engine will struggle to start. It will be nearly impossible to start a car engine with a voltage under 11 volts. 
const float BATTERY_HEALTHY_VOLTAGE = 12.8f;

// Cranking
//
// Cranking process and factors https://rusefi.com/forum/viewtopic.php?t=2021
// Minimum crank RPM https://www.quora.com/What-would-a-normal-RPM-be-when-cranking-a-car-that-wont-start
// The minimum crank RPM is roughly 200–300 RPM for a good starter motor and a good battery
// The engine RPM at which the starter motor gets the engine to to start, note that this is the resulting figure after the starter motor is fighting against friction and compression
// HACK: This is just a typical crank RPM, when the engine RPM gets to this minimum it will fire, the engine RPM will rise to approximately the idle RPM, and the power to the starter motor will be cut and it will be disengaged
const float fMinimumCrankRPM = 230.0f;

namespace part {

ElectricMotor::ElectricMotor() :
  fFreeRunSpeedRPM(0.0f),
  fFreeRunCurrentAmps(1.0f),
  fStallTorqueNm(1.0f),
  fInertiaKgMeterSquared(1.0f),
  uiOutputGearTeeth(10),
  fInputVoltage(0.0f),
  fAngularVelocityRadiansPerSecond(0.0f),
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
  fRadiusm(0.12f),
  fInertiaKgMeterSquared(1.0f),
  fAngularVelocityRadiansPerSecond(0.0f),
  uiTeeth(1)
{
}

TorqueConverter::TorqueConverter() :
  fMassKg(30.0f),
  fStallSpeedRPM(300.0f),
  fTemperatureDegreesCelcius(spitfire::math::GENERIC_AMBIENT_AIR_AT_SEA_LEVEL_TEMPERATURE_DEGREES_CELCIUS)
{
}

GearBox::GearBox() :
  fInputShaftInertia(1.0f),
  fCounterShaftInertia(1.0f),
  fIdlerGearInertia(1.0f),
  fOutputShaftInertia(1.0f),
  currentGear(0)
{
}

Differential::Differential() :
  type(TYPE::OPEN),
  fInputShaftInertia(1.0f),
  fOutputShaft1Inertia(1.0f),
  fOutputShaft2Inertia(1.0f),
  fRatio(1.0f)
{
}


Engine::Engine() :
  cylinderArrangement(CYLINDER_ARRANGEMENT::V),
  cylinders(6),
  fBoreMillimetres(1.0f),
  fStrokeMillimetres(1.0f),
  fPistonRodLengthMillimetres(1.0f),
  fCylinderDisplacementVolumeLitres(1.0f),
  fTotalEngineMassKg(10.0f),
  fBlockMassKg(1.0f),
  fIndividualPistonMassKg(1.0f),
  fIndividualConrodMassKg(1.0f),
  fCrankShaftMassKg(1.0f),
  fIndividualPistonInertiaKgMeterSquared(1.0f),
  fIndividualConrodInertiaKgMeterSquared(1.0f),
  fCrankshaftInertiaKgMeterSquared(1.0f),
  fCrankRPM(0.0f),
  fStallRPM(550.0f),

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

  return ((starterMotor.fInputVoltage < 0.01f) && (fCrankRPM >= fStallRPM));
}

bool Engine::IsAboveStallSpeed() const
{
  return (fCrankRPM >= fStallRPM);
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

Wheel::Wheel() :
  fInertiaKgMeterSquared(1.0f)
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


Vehicle::Vehicle()
{
  wheels.push_back(vehicle::part::Wheel());
  wheels.push_back(vehicle::part::Wheel());
  wheels.push_back(vehicle::part::Wheel());
  wheels.push_back(vehicle::part::Wheel());
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
    if (inputs.ignitionKeyTurned && !vehicle.engine.IsAboveStallSpeed()) {
      std::cout<<"UpdateECU changing state to ACCESSORIES_OFF_STARTER_MOTOR_FIRING"<<std::endl;
      ecu.powerState = part::ECU::POWER_STATE::ACCESSORIES_OFF_STARTER_MOTOR_FIRING;
    }
  } else if (ecu.powerState == part::ECU::POWER_STATE::ACCESSORIES_OFF_STARTER_MOTOR_FIRING) {
    if (vehicle.engine.IsAboveStallSpeed()) {
      std::cout<<"UpdateECU changing state to ACCESSORIES_ON_ENGINE_RUNNING"<<std::endl;
      ecu.powerState = part::ECU::POWER_STATE::ACCESSORIES_ON_ENGINE_RUNNING;
    }
  } else if (ecu.powerState == part::ECU::POWER_STATE::ACCESSORIES_ON_ENGINE_RUNNING) {
    if (!vehicle.engine.IsAboveStallSpeed()) {
      // The engine has stalled, return to accessories on
      std::cout<<"UpdateECU changing state to ACCESSORIES_ON"<<std::endl;
      ecu.powerState = part::ECU::POWER_STATE::ACCESSORIES_ON;
    }
  }



  if (ecu.powerState == part::ECU::POWER_STATE::ACCESSORIES_OFF_STARTER_MOTOR_FIRING) {
    vehicle.ecuActions.headlights = false; // Turn off the lights when we are starting the engine
    vehicle.engine.starterMotor.fInputVoltage = BATTERY_HEALTHY_VOLTAGE;
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


class EffectiveInertiaCalculator {
public:
  EffectiveInertiaCalculator() :
    fInertiaFirstBodyKgMeterSquared(1.0f)
  {
  }

  void InitWithInertiaOfFirstBody(float fInertiaFirstBodyKgMeterSquared)
  {
    fInertiaFirstBodyKgMeterSquared = fInertiaFirstBodyKgMeterSquared;
  }

  void AddBodyWithEffectiveInertia(float fInertiaKgMeterSquared)
  {
    // NOTE: This is a bit of a hack, for bodies that are added to the last inertia with no gear change we add it as if it were a gear change of 1x, this generates more calculations, but makes the code simple
    AddGearRatioAndBodyWithInertia(1.0f, fInertiaKgMeterSquared);
  }

  void AddGearRatioAndBodyWithInertia(float fGearRatio, float fInertiaKgMeterSquared)
  {
    chainOfGearRatioAndInertias.push_back(std::make_pair(fGearRatio, fInertiaKgMeterSquared));
  }

  float GetEffectiveInertia() const
  {
    // http://www.racer.nl/tech/effinertia.htm
    // Start with the first body effective inertia, then add every subsequent gear ratio and inertia
    // I_effective = I_1 + (gearRatio1^2)*I_2 + ((gearRatio2*gearRatio1)^2)*I_3 + ((gearRatio3*gearRatio2*gearRatio1)^2)*I_4

    float fEffectiveInertia = fInertiaFirstBodyKgMeterSquared;
    float fEffectiveGearRatio = 1.0f;

    for (auto&& p : chainOfGearRatioAndInertias) {
      //std::cout<<"Effective inertia adding "<<p.first<<", "<<p.second<<std::endl;
      fEffectiveGearRatio *= p.first;
      fEffectiveInertia += (fEffectiveGearRatio * fEffectiveGearRatio) * p.second;
    }

    return fEffectiveInertia;
  }

private:
  float fInertiaFirstBodyKgMeterSquared;
  std::vector<std::pair<float, float>> chainOfGearRatioAndInertias;
};


// t = I.α
// t is the torque on axis of rotation required to change its motion with an angular acceleration 'a'
// I is the moment of inertia

float GetTorqueNmRequiredForAngularAcceleration(float fInertia, float fAngularAcceleration)
{
  return (fInertia * fAngularAcceleration);
}

float GetAngularAccelerationForTorqueNm(float fInertia, float fTorqueNm)
{
  return (fTorqueNm / fInertia);
}




void UpdateEngineDrivetrainWheels(float fTimeStepFractionOfSecond, const Environment& environment, breathe::vehicle::Vehicle& vehicle)
{
  spitfire::math::RPMTorquePair outputStarterMotorRPMAndTorqueNm;

  const float fStarterToFlyWheelGearRatio = float(vehicle.engine.flyWheel.uiTeeth) / float(vehicle.engine.starterMotor.uiOutputGearTeeth);

  if (vehicle.engine.starterMotor.fInputVoltage >= 12.0f) {
    // NOTE: When voltage is applied to the starter motor a solenoid pushes the starter motor drive shaft out to engage the starter motor cog with the flywheel
    // We just assume a perfect connection is occuring when a voltage is applied and there is zero connection when no voltage is applied

    // Apply the voltage to the starter motor to start it turning
    // HACK: We just asssume if the input voltage is >= 12V then we are driving the starter motor and we apply torque based on the RPM of the motor

    spitfire::math::RPMTorquePair starterMotorRPMAndTorqueNm;
    starterMotorRPMAndTorqueNm.fRPM = vehicle.engine.starterMotor.fRPM;
    starterMotorRPMAndTorqueNm.fTorqueNm = vehicle.engine.starterMotor.curveRPMToTorqueNm.GetYAtPointX(vehicle.engine.starterMotor.fRPM);

    spitfire::math::ApplyGear(starterMotorRPMAndTorqueNm, fStarterToFlyWheelGearRatio, outputStarterMotorRPMAndTorqueNm);

    vehicle.engine.starterMotor.fRPM = outputStarterMotorRPMAndTorqueNm.fRPM;

    // Just for debugging keep track of the highest RPM we have acheived
    static float fDebugMaxRPM = 0.0f;
    if (outputStarterMotorRPMAndTorqueNm.fRPM > fDebugMaxRPM) fDebugMaxRPM = outputStarterMotorRPMAndTorqueNm.fRPM;

    std::cout<<"Update starter motor to fly wheel gear ratio "<<fStarterToFlyWheelGearRatio<<", in "<<starterMotorRPMAndTorqueNm.fRPM<<" rpm, "<<starterMotorRPMAndTorqueNm.fTorqueNm<<" Nm, out "<<outputStarterMotorRPMAndTorqueNm.fRPM<<" rpm, "<<outputStarterMotorRPMAndTorqueNm.fTorqueNm<<" Nm, debug max rpm was "<<fDebugMaxRPM<<" rpm"<<std::endl;



    //if (fTorqueRequiredToTurnCrankshaftNm > vehicle.engine.starterMotor.fStallTorqueNm) {
      // Burning out starter motor, trying to turn starter motor but something is stopping it
    //} else {
      // Starter motor applying torque
    //}
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
*/

  // Get the effective inertia of engine components, flywheel, then if clutch in inertia of gearbox, driveshaft, diff, etc.

  breathe::vehicle::EffectiveInertiaCalculator effectiveInertiaCalculator;

  effectiveInertiaCalculator.InitWithInertiaOfFirstBody(vehicle.engine.cylinders * (vehicle.engine.fIndividualPistonInertiaKgMeterSquared + vehicle.engine.fIndividualConrodInertiaKgMeterSquared));

  effectiveInertiaCalculator.AddBodyWithEffectiveInertia(vehicle.engine.fCrankshaftInertiaKgMeterSquared);
  effectiveInertiaCalculator.AddBodyWithEffectiveInertia(vehicle.engine.flyWheel.fInertiaKgMeterSquared);

/*
  const float fEngineConnectedInertia = ... 

  vehicle.engine.fCrankshaftAngularVelocityRadiansPerSecond += fTimeStepFractionOfSecond * fEngineConnectedInertia * fEngineTorqueNm;
  vehicle.engine.fCrankshaftRotationRadians += fTimeStepFractionOfSecond * vehicle.engine.fCrankshaftAngularVelocityRadiansPerSecond;
*/

  effectiveInertiaCalculator.AddBodyWithEffectiveInertia(vehicle.clutch.fInertiaInputKgMeterSquared);



  float fTotalTorqueNm = 0.0f;

  // Add our starter motor torque
  fTotalTorqueNm += outputStarterMotorRPMAndTorqueNm.fTorqueNm; 

  // If the engine is running then add the engine torque
  if (vehicle.engine.fCrankRPM >= fMinimumCrankRPM) {
    fTotalTorqueNm += vehicle.engine.curveRPMToTorqueNm.GetYAtPointX(vehicle.engine.fCrankRPM);
  }

  // HACK: Magic numbers
  if (vehicle.engine.fCrankRPM > 0.000f) {
    // Friction and compression losses
    // This is proportional to the RPM as it the number of revolutions and compressions strokes increase with the RPM
    fTotalTorqueNm -= 0.1f * vehicle.engine.fCrankRPM;
  }


  if (fTotalTorqueNm >= 0.001f) {
    // TODO: Switch to verlet integration https://gamedev.stackexchange.com/questions/15708/how-can-i-implement-gravity

    // Simple Euler integration
    // acceleration = force(time, position) / mass;
    // time += timestep;
    // position += timestep * velocity;
    // velocity += timestep * acceleration;

    // Acceleration
    // Now calculate the acceleration by dividing the resulting force by your vehicle's mass (in Kg). 15000 N / 1500 Kg = 10 m/s2 (We are just doing the engine internals here, up to the clutch, compression is not taken into account)
    const float fTotalAccelerationMetersPerSecondSquared = fTotalTorqueNm / effectiveInertiaCalculator.GetEffectiveInertia();

    // TODO: Add the equivalent of the "position += timestep * velocity;" step above too

    // Apply this acceleration to the current speed, accounting for how much time the engine power is being applied for (ie how many seconds each run loop iteration is supposed to simulate). If your current speed is about 9 m/s (~20 mph) and you want each loop iteration to simulate 0.1 seconds, you'd do 9 m/s + (10 m/s2 * 0.1) = 10 m/s (new speed)
    //std::cout<<"rpm: "<<vehicle.engine.fCrankRPM<<", torque "<<fTotalTorqueNm<<" Nm, "<<effectiveInertiaCalculator.GetEffectiveInertia()<<" inertia, adding "<<fTimeStepFractionOfSecond<<" * "<<fTotalAccelerationMetersPerSecondSquared<<std::endl;
    vehicle.engine.fCrankRPM += (fTimeStepFractionOfSecond * fTotalAccelerationMetersPerSecondSquared);

    // TODO: Work backwards to find the starter motor RPM?
    //vehicle.engine.fCrankRPM = spitfire::math::GetGearOutputRPM(vehicle.engine.starterMotor.fRPM, fStarterToFlyWheelGearRatio);
  }



  // TODO: Only do the rest of this if the clutch is engaged
  effectiveInertiaCalculator.AddBodyWithEffectiveInertia(vehicle.clutch.fInertiaOutputKgMeterSquared);


  const breathe::vehicle::part::GearBox& gearBox = vehicle.gearBox;
  assert(gearBox.currentGear < gearBox.gearRatios.size());
  const float fGearRatio = gearBox.gearRatios[gearBox.currentGear];

  // NOTE: This isn't exactly accurate, we simulate the gear ratio as the effective ratio across the gearbox, but we don't know the individual ratios between input shaft, counter shaft, idler gear and output shaft
  // We nominate one engagement to be use fGearRatio and the others just use 1.0 so that they don't modify the effective gear ratio across the gearbox

  effectiveInertiaCalculator.AddBodyWithEffectiveInertia(gearBox.fInputShaftInertia);

  if (fGearRatio < -0.1f) {
    // Reverse, use input shaft, idler gear, counter shaft, and output shaft
    effectiveInertiaCalculator.AddBodyWithEffectiveInertia(gearBox.fCounterShaftInertia);
    effectiveInertiaCalculator.AddGearRatioAndBodyWithInertia(fGearRatio, gearBox.fIdlerGearInertia);
    effectiveInertiaCalculator.AddBodyWithEffectiveInertia(gearBox.fOutputShaftInertia);
  } else if (spitfire::math::IsApproximatelyEqual(fGearRatio, 0.0f)) {
    // Neutral, input shaft only, no gear is engaged
  } else if (spitfire::math::IsApproximatelyEqual(fGearRatio, 1.0f, 0.1f)) {
    // 1:1 ratio is a special case, don't use counter shaft
    effectiveInertiaCalculator.AddBodyWithEffectiveInertia(gearBox.fOutputShaftInertia);
  } else {
    // All other gears use the input shaft, counter shaft and output shaft
    effectiveInertiaCalculator.AddGearRatioAndBodyWithInertia(fGearRatio, gearBox.fCounterShaftInertia);
    effectiveInertiaCalculator.AddBodyWithEffectiveInertia(gearBox.fOutputShaftInertia);
  }



  const breathe::vehicle::part::Differential& differential = vehicle.differential;

  effectiveInertiaCalculator.AddBodyWithEffectiveInertia(differential.fInputShaftInertia);


  breathe::vehicle::EffectiveInertiaCalculator effectiveInertiaCalculator1 = effectiveInertiaCalculator;
  breathe::vehicle::EffectiveInertiaCalculator effectiveInertiaCalculator2 = effectiveInertiaCalculator;

  effectiveInertiaCalculator1.AddGearRatioAndBodyWithInertia(differential.fRatio, differential.fOutputShaft1Inertia);
  effectiveInertiaCalculator2.AddGearRatioAndBodyWithInertia(differential.fRatio, differential.fOutputShaft2Inertia);

  assert(vehicle.wheels.size() >= 2);
  effectiveInertiaCalculator1.AddBodyWithEffectiveInertia(vehicle.wheels[0].fInertiaKgMeterSquared);
  effectiveInertiaCalculator2.AddBodyWithEffectiveInertia(vehicle.wheels[1].fInertiaKgMeterSquared);


  //const float fEffectiveInertia1 = effectiveInertiaCalculator1.GetEffectiveInertia();
  //const float fEffectiveInertia2 = effectiveInertiaCalculator2.GetEffectiveInertia();
  //std::cout<<"Driveline effective inertia at wheels 1 is "<<fEffectiveInertia1<<", 2 is "<<fEffectiveInertia2<<std::endl;



  // Final force acting on the road from the tyres
  // https://x-engineer.org/calculate-wheel-torque-engine/
  // Torque is a measure of force and distance, not force alone, so we need to divide it by the drive wheel's total radius to get just force alone. 5544 N-m / 0.34105m = 16255.681 Newtons (that's 3654 lbs) 
  //fForceFromWheelsToRoadNewtons = outputWheelRPMAndTorque.fTorqueNm / vehicle.wheels[0].fRadiusm;
}

void Update(float fTimeStepFractionOfSecond, const Environment& environment, const VehicleInputs& inputs, breathe::vehicle::Vehicle& vehicle)
{
  UpdateECU(fTimeStepFractionOfSecond, inputs, vehicle);

  UpdateEngineDrivetrainWheels(fTimeStepFractionOfSecond, environment, vehicle);
}

}

}
