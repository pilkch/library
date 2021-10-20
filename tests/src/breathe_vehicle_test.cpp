// Standard headers
#include <array>
#include <iostream>
#include <fstream>

#include <spitfire/math/units.h>
#include <spitfire/math/cCurve.h>

// Breathe headers
#include <breathe/vehicle/vehicle.h>

// gtest headers
#include <gtest/gtest.h>

namespace {

const size_t timeStepsPerSecond = 60;
const float fTimeStepMS = 1000.0f / float(timeStepsPerSecond);
const float fTimeStepFractionOfSecond = 1.0f / float(timeStepsPerSecond);

void EnvironmentSetSettings(breathe::Environment& environment)
{
  environment.fAirTemperatureDegreesCelcius = spitfire::math::GENERIC_AMBIENT_AIR_AT_SEA_LEVEL_TEMPERATURE_DEGREES_CELCIUS;
  environment.fAirPressureKPa = spitfire::math::GENERIC_AMBIENT_AIR_AT_SEA_LEVEL_PRESSURE_KPA;
  environment.fAirDensityKgPerM3 = spitfire::math::GENERIC_AMBIENT_AIR_AT_SEA_LEVEL_DENSITY_KG_PER_CUBIC_METER;
}

// Golf GTI Mk6 2010-2014
//
// GTI 1,984 cc (121 cu in) I4
// 2.0 R4 16v TSI/TFSI (EA888)
// Engine Code: CCZB
// Power: 150 kW (210 PS) @ 5,300-6,200
// Torque: 280 N⋅m (207 lb⋅ft) @ 1,700-5,200
// 0 to 100 KPH: 6.9 s
// Top Speed: 240 km/h (6 speed manual), 238 km/h (DSG)
//
// CCTA or CBFA: In the MK6 (2010-2014) GTI, the CBFA engine comes with Secondary Air Injection.
// The engine code is printed on the cylinder block behind the oil filter and on the sticker on the upper timing chain cover (CCT = CCTA, CBF = CBFA)
//
// https://australiancar.reviews/GolfGTi-Mk6_CCZB_Engine.php
//
// Cast iron block
// Weight: 33 kg
// Aluminium alloy cylinder head
// Double overhead camshafts driven by gear chains
// Four valves per cylinder actuated by roller finger cam followers
// Variable intake valve timing
// BorgWarner K03 turbocharger providing peak boost pressure of 1.0 bar (14.4 psi)
// Intercooler
// Variable intake manifold
// Firing order: 1-3-4-2
// Compression ratio of 9.6:1
// Direct injection (Volkswagen’s ‘Fuel Stratified Injection’ or ‘FSI’)
// Bosch Motronic MED 17.5 engine management system
//
// Total weight? of 144 kg
//
// The CCZB engine had hydro-formed, double overhead camshafts that were driven 
// by gear chains (as opposed to roller chains). For the intake camshaft, valve 
// timing could be varied via Volkswagen’s INA camshaft adjustment system which 
// used a hydraulic vane cell adjuster and oil pressure supplied by the engine 
// oil pump. The rotor of the vane cell adjuster was welded to the intake 
// camshaft and enabled an intake camshaft adjustment range of 60 degrees 
// relative to the crankshaft. Timing was map-controlled by the engine control 
// unit (ECU). 
//
// Intake valve lift: 10.7 mm
// Exhaust valve lift: 8.0 mm
//
// The intake and exhaust valves were actuated by roller cam followers with 
// needle bearings and hydraulic valve clearance adjusters.
//
//
//
// Fuel Stratified Injection (FSI)
//
// Directed fuel directly into the combustion chamber via six-hole injectors. 
// The Bosch high-pressure fuel pump was driven by a four lobed cam on the end 
// of the intake camshaft. Furthermore, the pump piston was driven by the 
// camshaft and a cam follower to reduce friction and chain forces. The 
// high-pressure fuel pump produced a maximum pressure of 150 bar and had a 
// pressure limiting valve that would open at around 200 bar and admit pressure 
// into the pump chamber. At engine speeds below 3000 rpm, the CCZB 
// engine closed its intake manifold flaps to increase the tumbling effect of 
// the intake air flow.
//
//
// Ignition
//
// The CCZB engine had four single spark ignition coils and cylinder-selective 
// anti-knock control that was controlled by the Bosch Motronic MED 17.5 engine 
// management system (EMS). The Bosch MED 17.5 EMS had four operating modes:
//   Start: high-pressure, start of fuel-air mixture
//   For several seconds after Start: homogeneous split injection (HOSP)
//   After warm-up phase: engine map-controlled dual injection cycle
//   At coolant temperatures exceeding 80 degrees Celsius: synchronous fuel injection with intake cycle only
//
//  
// https://www.golfmkv.com/forums/index.php?threads/golf-r-internals-swap-opinions-welcome.195829/
// Conrod Weight with Bolts: 547.7g
// 
// https://www.golfmk6.com/forums/index.php?threads/lets-discuss-clutch-options-disc-pressure-plate-flywheel-summary-of-discussion.245648/
//
// Dual Mass Flywheel Weight: 12.92 Kg
// Pressure Pate Weight: 5.21 Kg
// Clutch Disc Weight: 0.68 Kg
// Max Torque Capacity: 440 Nm? (Limited by the pressure plate apparently?)
//
//
// BorgWarner K03 turbocharger
// Integrated with the exhaust manifold into a single unit. For the CCZB engine, the 
// BorgWarner K03 turbocharger provided peak boost pressure of 1.0 bar (14.4 psi).
// It is understood that the OEM part number for the turbocharger is 06J 145 701T/K.
//
// Turbine
// Number of blades: 11
// Blade diameter
//   Inducer: 41.0 mm
//   Exducer: 45.0 mm
//
//
// Compressor
// Number of blades: 6 + 6
// Blade diameter
//   Inducer: 40.2 mm
//   Exducer: 52.5 mm
//
//
//
// Drag (Mk6 Golf GTI)
//
// Vehicle frontal area (A): 2.230 m²
// Drag coefficient (cw): 0.31
// Air resistance index (cw × A): 0.69
// Front axle: 0 kg lift / downforce (front zero lift), rear axle: 31 kg lift (rear lift)
// The front axle, which is statically loaded with 882 kilograms, remains unaffected at 200 km / h, i.e. the lift is zero.
// The drag coefficient has decreased significantly (from 0.33 to 0.31) and the lift is improved: at 200 km / h it approaches zero.
//
//


void EngineSetSettings(breathe::vehicle::part::Engine& engine)
{
  // Golf GTI
  engine.fTotalEngineMassKg = 144.0f;
  engine.fBlockMassKg = 33.0f;
  engine.fPistonMassKg = 0.4989f;
  engine.fConrodMassKg = 0.5477f;
  engine.fBoreMillimetres = 82.5f;
  engine.fStrokeMillimetres = 92.8f;

  engine.flyWheel.fMassKg = 12.92f;
  engine.flyWheel.uiTeeth = 134; // Not sure if this is correct
}

void StarterMotorSetSettings(breathe::vehicle::part::ElectricMotor& starterMotor)
{
#if 0
Rated Power 2.00kW
Rated Voltage 12.00 V
Max Of Power 2.23 kW
Max Of Current 817 Amp
Ripple 18 Amp
Direction CW
Hold Current 12 Amp
Pull Current 30 Amp
Voltage Drop 0.6468 V

Efficiency 85%?
#endif

  starterMotor.fFreeRunSpeedRPM = 10803.0f;
  starterMotor.fFreeRunCurrentAmps = 87.0f;
  starterMotor.fStallTorqueNm = 42.7f;
  //starterMotor.uiOutputGearTeeth = ;

  // NOTE: This is a curve, not a straight line
  starterMotor.curveRPMToTorqueNm.AddPoint(0.0f, 22.5f); // 22.5 Nm at 0 RPM
  starterMotor.curveRPMToTorqueNm.AddPoint(1000.0f, 21.0f); // 21.0 Nm at 1000 RPM
  starterMotor.curveRPMToTorqueNm.AddPoint(3000.0f, 9.0f); // 21.0 Nm at 1000 RPM
  starterMotor.curveRPMToTorqueNm.AddPoint(3600.0f, 0.0f); // 0 Nm for 3600 RPM and greater
}

void ClutchSetSettings(breathe::vehicle::part::Clutch& clutch)
{
  // TODO: These figures don't give us 440 Nm maximum torque capacity, so something is off

  // Golf GTI
  clutch.fMassKg = 0.68f;

  const float fSurfaceOuterRadiuscm = 12.0f;
  const float fSurfaceInnerRadiuscm = fSurfaceOuterRadiuscm - 3.0f; // 3cm wide contact surface
  const float fSurfaceOuterRadiusm = fSurfaceOuterRadiuscm / 100.0f;
  const float fSurfaceInnerRadiusm = fSurfaceInnerRadiuscm / 100.0f;

  // Calculate the area of the "donut" contact area of the clutch plate
  clutch.fSurfacem2 = (spitfire::math::cPI * fSurfaceOuterRadiusm * fSurfaceOuterRadiusm) - (spitfire::math::cPI * fSurfaceInnerRadiusm * fSurfaceInnerRadiusm);

  // Average of inner and outer disc radius
  clutch.fSurfaceMeanEffectiveRadiusm = (fSurfaceOuterRadiusm + fSurfaceInnerRadiusm) / 2.0f;

  clutch.fFrictionCoefficient = 0.8f;

  // TODO: This is rubbish, set real values
  clutch.curveTravel0To1_To_Engagement0To1.AddPoint(0.0f, 0.0f); // 0Nm at 0 A
  clutch.curveTravel0To1_To_Engagement0To1.AddPoint(100.0f, 0.0f); // 0Nm at 100 A
  clutch.curveTravel0To1_To_Engagement0To1.AddPoint(800.0f, 21.23f); // 21.23 Nm at 800 A
  clutch.curveTravel0To1_To_Engagement0To1.AddPoint(2000.0f, 21.23f); // 21.23 Nm for values greater than 800 A

  // TODO: This is rubbish, set real values
  clutch.curveEngagement0To1_To_MaxNm.AddPoint(0.0f, 0.0f); // 0Nm at 0 A
  clutch.curveEngagement0To1_To_MaxNm.AddPoint(100.0f, 0.0f); // 0Nm at 100 A
  clutch.curveEngagement0To1_To_MaxNm.AddPoint(800.0f, 21.23f); // 21.23 Nm at 800 A
  clutch.curveEngagement0To1_To_MaxNm.AddPoint(2000.0f, 21.23f); // 21.23 Nm for values greater than 800 A

  // 380lbs of clamping force for a stock Golf GTI clutch diaphram?
  // 380lbs = 172kg = 1686 N
  clutch.fMaxEngagedForceN = 1686.0f;
}

void GearBoxSetSettings(breathe::vehicle::part::GearBox& gearBox)
{
  // Mk5 Golf GTI
  // 1st: 3.36
  // 2nd: 2.09
  // 3rd: 1.47
  // 4th: 1.10
  // 5th: 1.11
  // 6th: 0.93
  // Reverse: 1.64
  // Final Drive 1: 3.94 (Gears 1 to 4 (And reverse?))
  // Final Drive 2: 3.09 (Gears 5 to 6)

  const float fFinalDrive1 = 3.94f;
  const float fFinalDrive2 = 3.09f;

  gearBox.gears = {
    1.0f / (1.64f * fFinalDrive1), // Reverse
    0.0f, // Neutral
    1.0f / (3.36f * fFinalDrive1),
    1.0f / (2.09f * fFinalDrive1),
    1.0f / (1.47f * fFinalDrive1),
    1.0f / (1.10f * fFinalDrive1),
    1.0f / (1.11f * fFinalDrive2),
    1.0f / (0.93f * fFinalDrive2),
  };
}

void BodySetSettings(breathe::vehicle::part::Body& body)
{
  body.fFrontalAreaMetersSquared = 2.230f;
  body.fDragCoefficient = 0.31f;
}

void RunUpdateIterations(size_t nIterations, breathe::vehicle::VehicleInputs& inputs, breathe::vehicle::Vehicle& vehicle)
{
  for (size_t i = 0; i < nIterations; i++) {
    breathe::vehicle::Update(fTimeStepMS, inputs, vehicle);
  }
}

}


TEST(Breathe, TestVehicleStarterMotorSettings)
{
  breathe::vehicle::part::ElectricMotor starterMotor;
  StarterMotorSetSettings(starterMotor);

  // Check the RPM in to torque out curve
  EXPECT_NEAR(22.5f, starterMotor.curveRPMToTorqueNm.GetYAtPointX(0.0f), 0.001f);
  EXPECT_NEAR(22.35f, starterMotor.curveRPMToTorqueNm.GetYAtPointX(100.0f), 0.001f);
  EXPECT_NEAR(21.75f, starterMotor.curveRPMToTorqueNm.GetYAtPointX(500.0f), 0.001f);
  EXPECT_NEAR(21.0f, starterMotor.curveRPMToTorqueNm.GetYAtPointX(1000.0f), 0.001f);
  EXPECT_NEAR(19.5f, starterMotor.curveRPMToTorqueNm.GetYAtPointX(1250.0f), 0.001f);
  EXPECT_NEAR(18.0f, starterMotor.curveRPMToTorqueNm.GetYAtPointX(1500.0f), 0.001f);
  EXPECT_NEAR(16.5f, starterMotor.curveRPMToTorqueNm.GetYAtPointX(1750.0f), 0.001f);
  EXPECT_NEAR(15.0f, starterMotor.curveRPMToTorqueNm.GetYAtPointX(2000.0f), 0.001f);
  EXPECT_NEAR(9.0f, starterMotor.curveRPMToTorqueNm.GetYAtPointX(3000.0f), 0.001f);
  EXPECT_NEAR(7.5f, starterMotor.curveRPMToTorqueNm.GetYAtPointX(3100.0f), 0.001f);
  EXPECT_NEAR(6.0f, starterMotor.curveRPMToTorqueNm.GetYAtPointX(3200.0f), 0.001f);
  EXPECT_NEAR(4.5f, starterMotor.curveRPMToTorqueNm.GetYAtPointX(3300.0f), 0.001f);
  EXPECT_NEAR(3.0f, starterMotor.curveRPMToTorqueNm.GetYAtPointX(3400.0f), 0.001f);
  EXPECT_NEAR(1.5f, starterMotor.curveRPMToTorqueNm.GetYAtPointX(3500.0f), 0.001f);
  EXPECT_NEAR(0.0f, starterMotor.curveRPMToTorqueNm.GetYAtPointX(3600.0f), 0.001f);
  EXPECT_NEAR(0.0f, starterMotor.curveRPMToTorqueNm.GetYAtPointX(3700.0f), 0.001f);
  EXPECT_NEAR(0.0f, starterMotor.curveRPMToTorqueNm.GetYAtPointX(5000.0f), 0.001f);
}

TEST(Breathe, TestVehicleClutch)
{
  breathe::vehicle::part::Clutch clutch;

  ClutchSetSettings(clutch);

  const float fTorqueCapacityNm = breathe::vehicle::GetClutchTorqueCapacityNm(clutch);
  ASSERT_NEAR(283.248f, fTorqueCapacityNm, 0.1f);

  // Clutch fully engaged

  breathe::vehicle::CLUTCH_STATE clutchState = breathe::vehicle::CLUTCH_STATE::OPEN;
  float fOutputTorqueNm = 0.0f;
  fOutputTorqueNm = breathe::vehicle::GetClutchOutputTorqueNm(0.0f, 0.0f, clutch, clutchState);
  EXPECT_EQ(breathe::vehicle::CLUTCH_STATE::LOCKED, clutchState);
  EXPECT_NEAR(0.0f, fOutputTorqueNm, 0.1f);

  fOutputTorqueNm = breathe::vehicle::GetClutchOutputTorqueNm(0.0f, 10.0f, clutch, clutchState);
  EXPECT_EQ(breathe::vehicle::CLUTCH_STATE::LOCKED, clutchState);
  EXPECT_NEAR(10.0f, fOutputTorqueNm, 0.1f);

  fOutputTorqueNm = breathe::vehicle::GetClutchOutputTorqueNm(0.0f, 100.0f, clutch, clutchState);
  EXPECT_EQ(breathe::vehicle::CLUTCH_STATE::LOCKED, clutchState);
  EXPECT_NEAR(100.0f, fOutputTorqueNm, 0.1f);

  fOutputTorqueNm = breathe::vehicle::GetClutchOutputTorqueNm(0.0f, 200.0f, clutch, clutchState);
  EXPECT_EQ(breathe::vehicle::CLUTCH_STATE::LOCKED, clutchState);
  EXPECT_NEAR(200.0f, fOutputTorqueNm, 0.1f);

  // Slipping
  // The clutch's maximum torque capacity is calculated at 283 Nm so everything after this should slip

  fOutputTorqueNm = breathe::vehicle::GetClutchOutputTorqueNm(0.0f, 300.0f, clutch, clutchState);
  EXPECT_EQ(breathe::vehicle::CLUTCH_STATE::SLIPPING, clutchState);
  EXPECT_NEAR(283.248f, fOutputTorqueNm, 0.1f);

  fOutputTorqueNm = breathe::vehicle::GetClutchOutputTorqueNm(0.0f, 500.0f, clutch, clutchState);
  EXPECT_EQ(breathe::vehicle::CLUTCH_STATE::SLIPPING, clutchState);
  EXPECT_NEAR(283.248f, fOutputTorqueNm, 0.1f);

  fOutputTorqueNm = breathe::vehicle::GetClutchOutputTorqueNm(0.0f, 1000.0f, clutch, clutchState);
  EXPECT_EQ(breathe::vehicle::CLUTCH_STATE::SLIPPING, clutchState);
  EXPECT_NEAR(283.248f, fOutputTorqueNm, 0.1f);


  // Clutch fully disengaged, no contact, no slipping

  fOutputTorqueNm = breathe::vehicle::GetClutchOutputTorqueNm(1.0f, 0.0f, clutch, clutchState);
  EXPECT_EQ(breathe::vehicle::CLUTCH_STATE::OPEN, clutchState);
  EXPECT_NEAR(0.0f, fOutputTorqueNm, 0.1f);

  fOutputTorqueNm = breathe::vehicle::GetClutchOutputTorqueNm(1.0f, 10.0f, clutch, clutchState);
  EXPECT_EQ(breathe::vehicle::CLUTCH_STATE::OPEN, clutchState);
  EXPECT_NEAR(0.0f, fOutputTorqueNm, 0.1f);

  fOutputTorqueNm = breathe::vehicle::GetClutchOutputTorqueNm(1.0f, 100.0f, clutch, clutchState);
  EXPECT_EQ(breathe::vehicle::CLUTCH_STATE::OPEN, clutchState);
  EXPECT_NEAR(0.0f, fOutputTorqueNm, 0.1f);

  fOutputTorqueNm = breathe::vehicle::GetClutchOutputTorqueNm(1.0f, 200.0f, clutch, clutchState);
  EXPECT_EQ(breathe::vehicle::CLUTCH_STATE::OPEN, clutchState);
  EXPECT_NEAR(0.0f, fOutputTorqueNm, 0.1f);

  fOutputTorqueNm = breathe::vehicle::GetClutchOutputTorqueNm(1.0f, 300.0f, clutch, clutchState);
  EXPECT_EQ(breathe::vehicle::CLUTCH_STATE::OPEN, clutchState);
  EXPECT_NEAR(0.0f, fOutputTorqueNm, 0.1f);

  fOutputTorqueNm = breathe::vehicle::GetClutchOutputTorqueNm(1.0f, 500.0f, clutch, clutchState);
  EXPECT_EQ(breathe::vehicle::CLUTCH_STATE::OPEN, clutchState);
  EXPECT_NEAR(0.0f, fOutputTorqueNm, 0.1f);

  fOutputTorqueNm = breathe::vehicle::GetClutchOutputTorqueNm(1.0f, 1000.0f, clutch, clutchState);
  EXPECT_EQ(breathe::vehicle::CLUTCH_STATE::OPEN, clutchState);
  EXPECT_NEAR(0.0f, fOutputTorqueNm, 0.1f);
}

TEST(Breathe, TestVehicleStarterMotorStartEngine)
{
  breathe::Environment environment;
  EnvironmentSetSettings(environment);

  breathe::vehicle::Vehicle vehicle;

  EngineSetSettings(vehicle.engine);
  StarterMotorSetSettings(vehicle.engine.starterMotor);
  ClutchSetSettings(vehicle.clutch);
  GearBoxSetSettings(vehicle.gearBox);
  BodySetSettings(vehicle.body);

  // The engine should not be running yet
  EXPECT_NEAR(0.0f, vehicle.engine.starterMotor.fRPM, 1.0f);
  EXPECT_FALSE(vehicle.engine.IsRunning());
  EXPECT_NEAR(0.0f, vehicle.GetRPMAtFlywheel(), 0.1f);
  EXPECT_NEAR(0.0f, vehicle.GetRPMAfterClutch(), 0.1f);
  EXPECT_NEAR(0.0f, vehicle.GetRPMAfterGearBox(), 0.1f);

  breathe::vehicle::VehicleInputs inputs;

  // Test the starter motor starting up
  // Initially not started
  RunUpdateIterations(3, inputs, vehicle);

  EXPECT_NEAR(0.0f, vehicle.engine.starterMotor.fRPM, 1.0f);
  EXPECT_FALSE(vehicle.engine.IsRunning());

  // Crank the starter motor
  inputs.ignitionKeyTurned = true;

  // Wait for it to start the engine
  RunUpdateIterations(200, inputs, vehicle);

  // The starter motor should now be turning
  EXPECT_NEAR(800.0f, vehicle.engine.starterMotor.fRPM, 50.0f);
  EXPECT_TRUE(vehicle.engine.IsRunning());

  // The starter motor can be turned off now
  inputs.ignitionKeyTurned = false;

  // Wait for the starter motor to disengage and the engine to settle
  RunUpdateIterations(200, inputs, vehicle);

  EXPECT_NEAR(800.0f, vehicle.engine.starterMotor.fRPM, 50.0f);
  EXPECT_TRUE(vehicle.engine.IsRunning());

  // The engine should now be started
  EXPECT_NEAR(800.0f, vehicle.GetRPMAtFlywheel(), 50.0f);
  EXPECT_NEAR(800.0f, vehicle.GetRPMAfterClutch(), 50.0f);
  EXPECT_NEAR(800.0f, vehicle.GetRPMAfterGearBox(), 50.0f);
}
