// Standard library includes
#include <cmath>

#include <vector>
#include <list>
#include <map>

#include <iostream>
#include <sstream>

#include <GL/GLee.h>


#include <spitfire/util/log.h>

#include <spitfire/math/units.h>
#include <spitfire/storage/filesystem.h>


#include <breathe/game/component.h>

#include <breathe/vehicle/cCar.h>

#include <breathe/render/cResourceManager.h>
#include <breathe/render/model/cStaticModelLoader.h>

// Racing Line
// Turn in
// Apex
// Track out
//
// Geometric line (Entry 55 KPH, exit 55 KPH)
// Late apex line (Entry 50 KPH, exit 57 KPH) - This can still be faster due to changes in tire temperature and grip
// Avoiding potholes, oil, bumps, hills, etc. line - This can be a slower line on paper but faster because we stay on good track the whole corner


// Jumps
//
// Accelerating before the jump can bring the nose up
// Braking and letting off the throttle before the jump can bring the nose down
// If the car is nose up in the air we can apply the clutch or tap the brakes slightly to bring the nose down
// If the car is nose down in the air we can floor the accelerator to bring the nose up
// Accelerator, clutching and braking enhance or dampen gyroscopic effects on the chassis


// Things to print to the screen in debug mode
// Accelerator percentage
// Brake percentage
// Hand brake (yes/no)
// Clutch percentage
// Clutch "holding" Nm of torque
// Clutch "allowing" Nm of torque
// Gear
// RPM
// Speed
// Power
// Torque
// Gearbox RPM
// Gear ratio
// Tire temperatures
//   - Perhaps represented by 10cm "bands" going across the tire from left to right?  This would allow the left side of the tire to heat up say if we are going around a clockwise circuit
//   - Show the overall temperature for each tire too (Average of all the bands)
//   - "When you’re going through a corner, especially on a high speed oval, the tire surface can heat up at 50 degrees per second"
// Tire wear
// Brake temperatures
// Brake fade
// Weight on each wheel
// Down force amount front and back
// Grip (Percentage?)
// Bump stop?
// Travel cm
// Ride height cm


// Tires
//
// Nascar - Temperature on the surface of the tire: up to 350-370 degrees on the corner, 200 on the straights
//
//
// Interview with Dave Kaemmer
//
// We’re very excited to get the new tire model into your hands and out into public view finally! We think you’ll find that the new tires are a lot of fun—at the same time easier to drive, but more challenging, too. For sure they are more realistic than the old tires—especially on the high speed ovals, which is why we’re letting you get a first look at them on the new Nationwide car. The car itself represents a big step forward in simulating a Nascar race car—the aerodynamics and suspension have been reworked in addition to the tires. But before you jump into the new Nationwide car and drop the clutch I thought I’d give you a few bits of information to help get you up to speed on these new tires.
//
// First, you’re probably going to be best off if you start with the baseline setups provided with the car (click on the iRacing Setups tab in the garage). You may need to alter your driving style a bit to get the most out of these tires, and you can easily get into a hole by playing with the setup instead of playing with your line, and your technique.
//
// Second, be patient—you’ll need to learn how you can change the balance of the car simply by altering your line, and by changing how and when you lift off and get back on the throttle. These tires, just like real tires, are very sensitive to heat buildup. When you’re going through a corner, especially on a high speed oval, the tire surface can heat up at 50 degrees per second! If the rear tires are heating up faster than the fronts, it’s sayonara—you’re going to spin. If the fronts are heating up faster, the car will be tight, and more steering will make it worse. When both ends are working nearly equally, you’ll know it—and you’ll find out why the drivers have a huge grin when their car is “hooked up.”
//
// By changing how you drive a corner, you can control the heat buildup to a degree. If you drive in really hot, and crank in a lot of steering wheel angle (a very typical simracing technique), it will come as no surprise that you will get understeer, or push, and lots of it. If you do this lap after lap, your right front will likely not make a full fuel run without popping when you wear through the cords. Also you will be slow—maybe not on the first lap, but over a run you will lose a lot of time. That’s because the right front will lose a lot of grip as it nearly catches fire, and melts away.
//
// An alternative is to back off a bit earlier, and turn the car down into the corner without a lot of steering wheel angle. Be patient, and apply the throttle only once the car is heading where you want it, and has rotated enough to get the rears working a bit. Now pick up the throttle, and you should feel the car bite. The easier you drive it into the corner, the earlier you can get back to the gas, but you might find that you start to heat the right rear more than the right front as you do this, and the car will get loose. You need to find the right amount to push the car in order to keep it balanced. A very slight brush on the brakes can do wonders on the way into a corner to get the front planted, and enough speed pulled off to get back on the gas. No need to mash the brakes—that’s another great way to overheat the fronts and move yourself to the back!
//
// Once you have gotten comfortable with altering your driving style while lapping, and seeing how you can move the car from tight to loose and back again, then you might start to play with the car setup in order to fine tune things to be just how you like them. Pay close attention to how the tires are wearing, as much as to the temperatures. Remember that you can dramatically change the temperature and wear simply by changing how you drive, in addition to changing the setup. The keys to setting up the Nationwide car are to keep the front splitter as close to the ground as possible, and to get the four tires to share the workload as evenly as possible. The baseline setups do a good job at both, so if what you really want to do is race, just load the right setup for the track you’re driving, and forget it.
//
// One small disclaimer: remember this is a preview of the new model; there are some features that are not done, such as smoke pouring from your locked up tires. We're still hard at work on completing it, but most of the important stuff is in there. I think it's fair to say that all of us here at iRacing and our dedicated crew of testers can't wait for you to try it--it is a ton of fun.
//
// We hope you like this upgrade to our simulation, and rest assured we’ll continue to improve it for a long time to come!
//


// How does a dynamometer/rolling road work?
//
// The car is put on the rollers. It's driven up to about 40mph, then put in 4th gear. The computer is set to a
// trigger point, usually about 50mph. The operator nails the throttle to the floor and takes the car to the
// red line - as it does so the computer is triggered which started to record data from the rollers.
// Once it's at the red line they ease off and the rollers can work out how powerful the engine is from the
// resistance through the wheels to the rollers.
//


// To measure the power at the fly wheel
//
// A) For engine in 3rd gear (Or closest to 1:1 ratio) we can do a really rough estimate:
// kw_at_flywheel = kw_at_rollers * magic number to take into account gearbox/clutch/diff losses
//
// OR
//
// B) At the flywheel is now quite simple, ONCE the Car has peaked, revved its arse off, the driver puts his foot on the
// clutch so the engine is disconnected.  Now the ROLLING ROAD puts its own power through the rollers and works
// out HOW MUCH HORSE POWER IT HAS TO USE, to make the gearbox and wheels turn at the same speed.
// It then works out the loss that the gearbox and drive created.



// Total Drive Train Loss
// FWD: 10-15% loss
// RWD: 10-18% loss
// AWD: 17-25% loss
//
// Automatic transmission: 2-5% more loss



// Handling Upgrades
//
// Commodore:
// Light weight wheels/semi slick tyres with -3 degrees camber
// Custom Adjustable coil overs
// Sway bars/strut bars/nolathene bushes
// Supercar front bar/air splitter
// Chassis brace kit
// Liner steering rack
//
// WRX/EVO:
// Light weight wheels/semi slick tyres with -2.5 degrees camber
// Heavy duty springs/shocks or coil overs.
//
// Also WRX/EVO:
// Heavy duty sway bars and links
// Strut bar
// Camber setup
// Good set of springs
//
// Also WRX/EVO ultimate:
// Anti lift kit
// Coilovers
// Decent quality rotors and pads like DBA500s and QFCs



// Turbo Chargers
//
// High power engines with small capacities have less exhaust gas available to initially spin the turbo up (or more accurately, proportionally much less than they have at full power), so these engines are even harder to provide with a responsive turbo.
// Ceramic turbines (less mass so faster acceleration)
// Ball-bearing turbos (less frictional drag from the bearing)
// Electric-assist turbos are relatively new and can start the turbo spinning
// To reduce lag place the turbo closer to the exhaust port


// Super Chargers
//
// Example:
// 0.59L Super Charger
//
// Displacement
// 0.59L per revolution
//
// Maximum air flow:
// 150L per second
//
// Compression:
// Built in pressure ratio 1.4 bar
//
// Achievable Engine Output:
// 150Kw / 200Hp
//
// Drive speed max. RPM:
// 16000RPM (continuous rating)
//
// Compression:
// Built in pressure ratio 1.4bar
//
// Pressure Range:
// Up to 2.0bar
//
// Typical Engine Capacity:
// 1.0 – 2.0L
//
// Lubrication:
// Self contained sealed unit or externally pressure fed
//
// Weight:
// 4.5kg (including front cover and standard pulley)



// The Physics of Racing Series
// http://www.miata.net/sport/Physics/

// The Physics of Racing Series: Part 22 The Magic Formula Lateral Version
// http://www.miata.net/sport/Physics/Part22.html


// Backfiring (Actually called afterfire, backfiring is something else)

// BOV = Blow Off Valve
// Here are the basic reasons for afterfire

// 1. Atmo BOV. Makes you rich for a split second when you shift, all that fuel, not 100% burnt, goes into exh, hits the hot gasses in the exh tract, and explodes.
// 2. Catless or free-er flowing exh on deceleration. I dont understand the mechanics of it, but free flowing exh's make backfire pop noises when decelerating in gear. (all cars will do this with an exh like that)

// SPT exh is freeer flowing, I would imagine you are hearing this on deceleration? Or you have an atmo bov and its just poping in the exh, and you oculdnt hear it with the quiet stock muffler.
// Mine does it all the time. TurboXS turboback exhaust, catless uppipe, stock ECU, stock boost, and stock turbo. On rev-match downshifts I get it; usually don't get too many otherwise.

// Stock engine management uses tons of extra fuel, so it tends to do that a lot. Get a dyno-tune and it will increase power and decrease afterfires. :)

// Get off the throttle and let engine braking take over. i.e. let the engine slow the car down. At first the engine will start to decelerate the car but it gets to a certain speed where it is no longer slowing down the car but sort of evens out to a neutral state. its not slowing or going faster. At that point if you apply enough throttle (not too much to make it accelerate) the car will start to afterfire. I do it all the time. On a Boost gauge its usually at 0. no vaccum or boost when afterfire occurs.



// As it was explained to me, is that what happens is while you are accelerating, small amounts of unburnt and half burnt gas coat the cats, pipes, fiberglass and the walls of the muffler. When you let off the gas the car goes into DFCO mode (decelleration fuel cut off) which cuts all spark and all fuel going into the engine. So now the engine is pumping (oxygen filled) normal air through it. When you have O2 + fuel + heat you get small explosions in the tail pipe. Same reasons apply to the SRT.

// A car backfires because unburned fuel is getting to the muffler. The heat of the exhaust system causes the fuel to combust, thus a backfire.


// Gear boxes
// H box
// Sequential shift box
// Paddle shift (Sequential?)
//
// BMW E36 Race Car:
// "The whine is from the straight cut gears in the transmission versus helical gears used in normal transmissions."




// http://opende.sourceforge.net/wiki/index.php/HOWTO_4_wheel_vehicle


// Camshaft turns at half the speed of the crankshaft, one input and one output valve per cylinder
// Dual over head cams means there are two camshafts, two input and two output valves per cylinder


// 1 Nm = 1 Joule
// 1 Wh = 3600 Joules
// 1 kWh = 3,412 BTU

// 1 kWh = 1 kW used up in an hour
// Using 10 kW this energy would last just 6 minutes


// 1 U.S. liquid gallon = 3.785411784 litres
// 1 Imperial (UK) gallon = 4.54609 litres
// 1 pound = 0.45359237 kg

// Air in Kg : Fuel in Kg ratio
// Normal 14:1
// Best power 12.6:1
// Cruising for fuel economy 15:1

// Fuel
// 1 gallon of petrol (UK gallon) weighs about 7.5 pounds
// Air weighs just over 2 pounds per cubic yard
// Every gallon of fuel needs about 100 pounds of air.
// That translates into about 50 cubic yards of air per gallon of fuel.
// To put it into perspective, that's about the same volume of air as in a room 12 feet by 14 feet.

// 1 gallon of petrol is 7.5 pounds
// 7.5 pounds = 3.40194278 Kg

// 3.40194278 Kg of petrol : 45.359237 Kg of air




// There are three variables that affect torque:
//
// * The size of the piston face
// * The amount of pressure that the ignited fuel applies to the face of the piston
// * The distance the piston travels on each stroke (therefore the diameter of the crankshaft). The bigger the diameter of the crankshaft, the bigger the lever arm and therefore the more torque.



// Forumla 1
// KERS - Brake battery regeneration, can release up to 80 hp for up to 6 seconds, full charge at start of race
// DRS - Dynamic rear wing - only within certain zones for a maximum distance (Per lap?)




// TODO: This is made up, whatever feels right?
const float_t fNormalAirFlowCubicMetresPerSecond = 1.0f;

namespace breathe
{
  namespace game
  {
    namespace car
    {
      // Calculate the output of an rpm and torque going into a gear with this ratio
      // NOTE: This gear is very generic and assumes 100% efficiency
      void CalculateGearing(float_t fGearRatio, float_t fInputRPM, float_t fInputTorqueNm, float_t& fOutputRPM, float_t& fOutputTorqueNm)
      {
        // Avoid a divide by zero
        if (fGearRatio != 0.0f) {
          fOutputRPM = fInputRPM * (1.0f / fGearRatio);
          fOutputTorqueNm = fInputTorqueNm * fGearRatio;
        } else {
          fOutputRPM = 0.0f;
          fOutputTorqueNm = 0.0f;
        }
      }


      cAirFlow::cAirFlow() :
        fDensityKgPerCubicMetre(0.0f),
        fFlowCubicMetresPerSecond(0.0f),
        fTemperatureDegreesCelcius(0.0f)
      {
      }

      void cAirFlow::Assign(const cAirFlow& rhs)
      {
        fDensityKgPerCubicMetre = rhs.fDensityKgPerCubicMetre;
        fFlowCubicMetresPerSecond = rhs.fFlowCubicMetresPerSecond;
        fTemperatureDegreesCelcius = rhs.fTemperatureDegreesCelcius;
      }

      // Basically *this += rhs
      void cAirFlow::Combine(const cAirFlow& rhs)
      {
        // TODO: Change properties more realistically
        fDensityKgPerCubicMetre = (fDensityKgPerCubicMetre + rhs.fDensityKgPerCubicMetre) * 0.5f;
        fFlowCubicMetresPerSecond = (fFlowCubicMetresPerSecond + rhs.fFlowCubicMetresPerSecond) * 0.5f;
        fTemperatureDegreesCelcius = (fTemperatureDegreesCelcius + rhs.fTemperatureDegreesCelcius) * 0.5f;
      }

      // Basically return *this / nParts
      cAirFlow cAirFlow::SplitIntoParts(size_t nParts) const
      {
        cAirFlow split = *this;

        ASSERT(nParts != 0);

        // TODO: Change properties more realistically
        //split.fDensityKgPerCubicMetre = split.fDensityKgPerCubicMetre;
        split.fFlowCubicMetresPerSecond /= float_t(nParts);
        //split.fTemperatureDegreesCelcius = split.fTemperatureDegreesCelcius;

        return split;
      }

      void cAirFlow::SetDensityKgPerCubicMetersAndFlowCubicMetresPerSecondAndTemperatureDegreesCelcius(float_t _fDensityKgPerCubicMetre, float_t _fFlowCubicMetresPerSecond, float_t _fTemperatureDegreesCelcius)
      {
        fDensityKgPerCubicMetre = _fDensityKgPerCubicMetre;
        fFlowCubicMetresPerSecond = _fFlowCubicMetresPerSecond;
        fTemperatureDegreesCelcius = _fTemperatureDegreesCelcius;
      }

      void cAirFlow::ApplyCompressionOrDecompression(float_t fRatio)
      {
        ASSERT(fRatio != 0.0f);

        // Apply the change to the other parameters
        fTemperatureDegreesCelcius *= 1.0f / fRatio;

        // Apply the change to the actual parameter
        fDensityKgPerCubicMetre *= fRatio;
      }

      void cAirFlow::ApplyFlowRateChange(float_t fRatio)
      {
        ASSERT(fRatio != 0.0f);

        // Apply the change to the actual parameter
        fFlowCubicMetresPerSecond *= fRatio;
      }

      void cAirFlow::ApplyTemperatureChange(float_t fRatio)
      {
        ASSERT(fRatio != 0.0f);

        // Apply the change to the other parameters
        fDensityKgPerCubicMetre *= 1.0f / fRatio;

        // Apply the change to the actual parameter
        fTemperatureDegreesCelcius *= fRatio;
      }




      cAmbientSettings::cAmbientSettings() :
        fDensityKgPerCubicMetre(0.0f),
        fPressureKPA(0.0f),
        fTemperatureDegreesCelcius(0.0f)
      {
      }

      cAmbientSettings::cAmbientSettings(float_t _fDensityKgPerCubicMetre, float_t _fPressureKPA, float_t _fTemperatureDegreesCelcius) :
        fDensityKgPerCubicMetre(_fDensityKgPerCubicMetre),
        fPressureKPA(_fPressureKPA),
        fTemperatureDegreesCelcius(_fTemperatureDegreesCelcius)
      {
      }

      void cAmbientSettings::Assign(const cAmbientSettings& rhs)
      {
        fDensityKgPerCubicMetre = rhs.fDensityKgPerCubicMetre;
        fPressureKPA = rhs.fPressureKPA;
        fTemperatureDegreesCelcius = rhs.fTemperatureDegreesCelcius;
      }



      cEngine::cEngine() :
        fMassKg(300.0f),
        nCylinders(0),
        fBoreRadiusCentiMeters(0.0f),
        fStrokeLengthCentiMeters(0.0f),
        fCompressionRatioMassAirToPetrol(0.0f),
        fInjectorSizeMassFuelKgPerSecond(0.0f),
        fRedLineRPM(6500.0f),
        fAcceleratorInput0To1(0.0f),
        fRPM(0.0f),
        fTorqueNm(0.0f)
      {
        // Very rough estimate of a torque curve for a 2001 Corvette (That's the best I could find!)
        // http://www.corvettemuseum.com/specs/2001/performance.htm
        // http://www.corvettemuseum.com/specs/2001/images/graph.jpg
        rpmToTorqueCurve.AddPoint(0.0f, 0.0f);
        rpmToTorqueCurve.AddPoint(800.0f, 379.6f);
        rpmToTorqueCurve.AddPoint(1600.0f, 447.4f);
        rpmToTorqueCurve.AddPoint(2400.0f, 467.7f);
        rpmToTorqueCurve.AddPoint(3200.0f, 481.3f);
        rpmToTorqueCurve.AddPoint(4000.0f, 508.4);
        rpmToTorqueCurve.AddPoint(4800.0f, 542.0f);
        rpmToTorqueCurve.AddPoint(5000.0f, 500.0f);
        rpmToTorqueCurve.AddPoint(5600.0f, 440.6f);
        rpmToTorqueCurve.AddPoint(6000.0f, 372.8f);
        rpmToTorqueCurve.AddPoint(6500.0f, 300.0f);
        rpmToTorqueCurve.AddPoint(7000.0f, 200.0f);
        rpmToTorqueCurve.AddPoint(7500.0f, 80.0f);
        rpmToTorqueCurve.AddPoint(8000.0f, 0.0f);
      }

      size_t cEngine::GetNumberOfCylinders() const
      {
        return nCylinders;
      }

      void cEngine::SetNumberOfCylinders(size_t _nCylinders)
      {
        nCylinders = _nCylinders;
      }

      float cEngine::GetBoreRadiusCentiMeters() const
      {
        return fBoreRadiusCentiMeters;
      }

      void cEngine::SetBoreRadiusCentiMeters(float _fBoreRadiusCentiMeters)
      {
        fBoreRadiusCentiMeters = _fBoreRadiusCentiMeters;
      }

      float cEngine::GetStrokeLengthCentiMeters() const
      {
        return fStrokeLengthCentiMeters;
      }

      void cEngine::SetStrokeLengthCentiMeters(float _fStrokeLengthCentiMeters)
      {
        fStrokeLengthCentiMeters = _fStrokeLengthCentiMeters;
      }

      float cEngine::GetCompressionRatioMassAirToPetrol() const
      {
        return fCompressionRatioMassAirToPetrol;
      }

      void cEngine::SetCompressionRatioMassAirToPetrol(float _fCompressionRatioMassAirToPetrol)
      {
        fCompressionRatioMassAirToPetrol = _fCompressionRatioMassAirToPetrol;
      }

      float cEngine::GetMassAirKgPerSecond() const
      {
        return fMassAirKgPerSecond;
      }

      void cEngine::SetMassAirKgPerSecond(float _fMassAirKgPerSecond)
      {
        fMassAirKgPerSecond = _fMassAirKgPerSecond;
      }

      float cEngine::GetInjectorSizeMassFuelKgPerSecond() const
      {
        return fInjectorSizeMassFuelKgPerSecond;
      }

      void cEngine::SetInjectorSizeMassFuelKgPerSecond(float _fInjectorSizeMassFuelKgPerSecond)
      {
        fInjectorSizeMassFuelKgPerSecond = _fInjectorSizeMassFuelKgPerSecond;
      }

      float cEngine::GetRPM() const
      {
        return fRPM;
      }

      void cEngine::SetRPM(float _fRPM)
      {
        fRPM = _fRPM;
      }

      float cEngine::GetCylinderDisplacementCubicCentiMeters() const
      {
        // The bore (diameter of the cylinder) is 4", and the stroke (twice the crankshaft throw radius) is 3.5".
        // The volume of that cylindrical volume is then (PI) * R2 * H or 3.1416 * 2 * 2 * 3.5 or around 44 cubic inches.
        // Since that engine has eight cylinders that are each that volume, its total 'displacement' is 44 * 8 or around 350 cubic inches. This engine is generally called the Chevy 350 V-8.

        return spitfire::math::cPI * fBoreRadiusCentiMeters * fBoreRadiusCentiMeters * fStrokeLengthCentiMeters;
      }

      float cEngine::GetEngineDisplacementCubicCentiMeters() const
      {
        return float(nCylinders) * GetCylinderDisplacementCubicCentiMeters();
      }

      void cEngine::Update(durationms_t currentTime)
      {
        // TODO: Should this be multiplied by the size of the engine?
        const float_t fEngineAirFlowCubicMetresPerSecond = fNormalAirFlowCubicMetresPerSecond;
        intakeAirFlow.SetDensityKgPerCubicMetersAndFlowCubicMetresPerSecondAndTemperatureDegreesCelcius(ambientSettings.GetDensityKgPerCubicMetre(), fEngineAirFlowCubicMetresPerSecond, ambientSettings.GetTemperatureDegreesCelcius());

        if (fRPM != 0.0f) {
          // Take in more air depending on rpm, we should definitely be capping this or using a curve of some sort.
          // Perhaps intakeAirFlow should have a maximum that we can possibly increase our intake to?
          const float_t fRatio = fRPM / 1000.0f;
          intakeAirFlow.ApplyFlowRateChange(fRatio);
        }

        const float_t fExhaustDensityKgPerCubicMeter = intakeAirFlow.GetDensityKgPerCubicMetre();
        const float_t fExhaustFlowCubicMetresPerSecond = intakeAirFlow.GetFlowCubicMetresPerSecond() * 3.0f;
        const float_t fExhaustTemperatureDegreesCelcius = intakeAirFlow.GetTemperatureDegreesCelcius() * 5.0f;
        exhaustAirFlow.SetDensityKgPerCubicMetersAndFlowCubicMetresPerSecondAndTemperatureDegreesCelcius(fExhaustDensityKgPerCubicMeter, fExhaustFlowCubicMetresPerSecond, fExhaustTemperatureDegreesCelcius);


        float_t fActualAcceleratorInput0To1 = fAcceleratorInput0To1;

        // Cut the accelerator if we are above the red line
        if (fRPM > fRedLineRPM) fActualAcceleratorInput0To1 = 0.0f;

        fRPM += 100.0f * fActualAcceleratorInput0To1;

        fRPM *= 0.99f;

        fRPM = spitfire::math::clamp(fRPM, 1000.0f, 16000.0f);


        fTorqueNm = rpmToTorqueCurve.GetYAtPointX(fRPM);

#if 0
        // I know, I know, in a real engine each of the four stages of an internal combustion engine are constantly taking place at a different place on each cylinder
        // We greatly simplify this by saying that the four stages happen instantaneously and "firing" happens to each cylinder in turn, so we should get similar
        // fuel economy etc. without doing the correct calculations.

        // Basically, milliseconds between each firing = (RPM -> revs per second) / cylinders

        const durationms_t timeBetweenEachFiring = ...;

        if (currentTime >= nextFiring) {
          // Control the air fuel mix
          1 petrol : 16 air or something?

          ... use up some petrol


          nextFiring = currentTime + timeBetweenEachFiring;
        }
#endif
      }



      cEFI::cEFI(cEngine& _engine) :
        engine(_engine),
        fMassFuelAirRatio(0.0f)
      {
      }

      float cEFI::GetMassAirFuelRatio() const
      {
        // Convert to an air to fuel ratio
        return 1.0f / fMassFuelAirRatio;
      }

      void cEFI::SetMassAirFuelRatio(float fMassAirFuelRatio)
      {
        // Convert to a fuel to air ratio
        fMassFuelAirRatio = 1.0f / fMassAirFuelRatio;
      }

      float cEFI::GetMassFuelAirRatio() const
      {
        return fMassFuelAirRatio;
      }

      void cEFI::SetMassFuelAirRatio(float _fMassFuelAirRatio)
      {
        fMassFuelAirRatio = _fMassFuelAirRatio;
      }

      float cEFI::GetAirMassFlowRateKgPerStroke() const
      {
        const float fMassAirKgPerMinute = 60.0f * engine.GetMassAirKgPerSecond();
        const float fMinutesPerRevolution = 1.0f / engine.GetRPM();
        const float fRevolutionsPerStroke = 1.0f / 2.0f; // Revolutions / Stroke = 1 / 2, whether it's a four stroke or a two-stroke engine

        // MassAirKg / Stroke = MassAirKg/Minute * Minutes/Revolution * Revolutions/Stroke
        return fMassAirKgPerMinute * fMinutesPerRevolution * fRevolutionsPerStroke;
      }

      /*float cEFI::GetAirMassFlowRateKgPerSecond() const
      {
        const float fMassAirKgPerMinute = ;
        const float fMinutesPerRevolution = 1.0f / fRPM;
        const float fRevolutionsPerStroke = 1.0f / 2.0f; // Revolutions / Stroke = 1 / 2, whether it's a four stroke or a two-stroke engine

        // MassAirKg / Stroke = MassAirKg/Minute * Minutes/Revolution * Revolutions/Stroke
        const float fMassAirKgPerStroke = fMassAirKgPerMinute * fMinutesPerRevolution * fRevolutionsPerStroke;
        ...
      }*/

      float cEFI::GetPulseWidthMS() const
      {
        // https://secure.wikimedia.org/wikipedia/en/wiki/Fuel_injection#Sample_pulsewidth_calculations
        // https://secure.wikimedia.org/wikipedia/en/wiki/Fuel_injection#Calculate_injector_pulsewidth_from_airflow
        // TODO: This equation is wrong, it relies on the weight of the air and fuel which is ok, except that it only works for pounds, so we convert back at this point

        const float fMassAirLbPerMinute = 60.0f *  spitfire::math::KiloGramsToPounds(engine.GetMassAirKgPerSecond());
        const float fMassFuelLbPerMinute = 60.0f * spitfire::math::KiloGramsToPounds(engine.GetInjectorSizeMassFuelKgPerSecond());
        const float fMinutesPerRevolution = 1.0f / engine.GetRPM();
        const float fRevolutionsPerStroke = 1.0f / 2.0f; // Revolutions / Stroke = 1 / 2, whether it's a four stroke or a two-stroke engine

        // MassAirLbPerMinute * Minutes/Revolution * Revolutions/Stroke * MassFuelLb/MassAirLb * 1/MassFuelLbPerMinute = PulseWidth
        // 0.55 lb per min * 1 min/700 rev * 1 revolution/2 strokes * 1/14.64 * 2.5/min per lb = 6.7 * 10^-5 min = 4ms

        // PulseWidth = MassAirLbPerMinute * Minutes/Revolution * Revolutions/Stroke * MassFuelLb/MassAirLb * 1/MassFuelLbPerMinute
        return fMassAirLbPerMinute * fMinutesPerRevolution * fRevolutionsPerStroke * fMassFuelAirRatio * fMassFuelLbPerMinute * 60000.0f;
      }

      float cEFI::GetEngineFuelFlowRateKgPerSecond() const
      {
        // https://secure.wikimedia.org/wikipedia/en/wiki/Fuel_injection#Calculate_fuel-flow_rate_from_pulsewidth
        const float fPulseWidthMSPerIntakeStroke = GetPulseWidthMS() / 2.0f;
        const float fMinutesPerMS = 1.0f / 60000.0f;
        const float fMassFuelKgPerHour = spitfire::math::PoundsToKiloGrams(24.0f);
        const float fHoursPerSecond = 1.0f / 3600.0f;
        const float fIntakeStrokesPerRevolution = 4.0f;
        const float fRPM = engine.GetRPM();

        // (Fuel flow rate) = (2.0 ms/intake-stroke) × (hour/3,600,000 ms) × (24 lb-fuel/hour) × (4-intake-stroke/rev) × (700 rev/min) × (60 min/h) = (2.24 lb/h)
        // FuelFlowRateKgPerSecond = PulseWidthMSPerIntakeStroke * HoursPerMS * MassFuelKgPerHour * IntakeStrokesPerRevolution * RPM
        return (fPulseWidthMSPerIntakeStroke * fMinutesPerMS) * (fMassFuelKgPerHour * fHoursPerSecond) * fIntakeStrokesPerRevolution * fRPM;
      }



      cSuperCharger::cSuperCharger() :
        fMassKg(20.0f),
        fEngineRPM(0.0f),
        fRatio(5.0f / 1.0f),
        fRPM(0.0f)
      {
      }

      void cSuperCharger::Update(durationms_t currentTime)
      {
        fRPM = fEngineRPM * fRatio;

        const float_t fSuperChargerAirFlowCubicMetresPerSecond = fNormalAirFlowCubicMetresPerSecond;
        outputAirFlow.SetDensityKgPerCubicMetersAndFlowCubicMetresPerSecondAndTemperatureDegreesCelcius(ambientSettings.GetDensityKgPerCubicMetre(), fSuperChargerAirFlowCubicMetresPerSecond, ambientSettings.GetTemperatureDegreesCelcius());

        // Compress the air depending on rpm, we should definitely be capping this or using a curve of some sort.
        // Perhaps intakeAirFlow should have a maximum that we can possibly increase our intake to?
        const float_t fCompressionRatio = fRPM / 1000.0f;
        outputAirFlow.ApplyCompressionOrDecompression(fCompressionRatio);

        // Take in more air depending on rpm, we should definitely be capping this or using a curve of some sort.
        // Perhaps intakeAirFlow should have a maximum that we can possibly increase our intake to?
        const float_t fFlowRateRatio = fRPM / 1000.0f;
        outputAirFlow.ApplyFlowRateChange(fFlowRateRatio);
      }


      cTurboCharger::cTurboCharger() :
        fMassKg(5.0f),
        fRPM(0.0f)
      {
      }

      void cTurboCharger::Update(durationms_t currentTime)
      {
        fRPM = exhaustAirFlow.GetFlowCubicMetresPerSecond();


        const float_t fTurboAirFlowCubicMetresPerSecond = fNormalAirFlowCubicMetresPerSecond;
        outputAirFlow.SetDensityKgPerCubicMetersAndFlowCubicMetresPerSecondAndTemperatureDegreesCelcius(ambientSettings.GetDensityKgPerCubicMetre(), fTurboAirFlowCubicMetresPerSecond, ambientSettings.GetTemperatureDegreesCelcius());

        // Compress the air depending on rpm, we should definitely be capping this or using a curve of some sort.
        // Perhaps intakeAirFlow should have a maximum that we can possibly increase our intake to?
        const float_t fCompressionRatio = fRPM / 1000.0f;
        outputAirFlow.ApplyCompressionOrDecompression(fCompressionRatio);

        // Take in more air depending on rpm, we should definitely be capping this or using a curve of some sort.
        // Perhaps intakeAirFlow should have a maximum that we can possibly increase our intake to?
        const float_t fFlowRateRatio = fRPM / 1000.0f;
        outputAirFlow.ApplyFlowRateChange(fFlowRateRatio);



        /*if ((fRPM < fBlowOffPoint) || (fAccelerator0To1 > 0.3f)) {
          // If there is not enough rpm or the accelerator is being applied then we do not blow off
          pSource->SetVolume(0.0f);
        } else {
          // Ok, the rpm is high and accelerator is not being applied, let's simulate a blow off
          pSource->SetVolume(1.0f);

          // As the rpm increases the volume goes up exponentially
          if (fLoudnessFactor0To1 > math::cEPSILON) {
            pSource->SetVolume(1.0f + (fHighRPMLoudnessFactor0To1 * math::squared(fRPM)));
          }
        }*/
      }



      void cInterCooler::Update(durationms_t currentTime)
      {
         outputAirFlow = inputAirFlow;

         // Cool the air depending on vehicle velocity, we should definitely be capping this or using a curve of some sort.
         // Perhaps we should have a maximum that we can possibly decrease our temperature by?
         const float_t fMinimumCarVelocityKPH = 10.0f;
         if (fCarVelocityKPH < fMinimumCarVelocityKPH) {
            // For really slow speeds and reversing we apply a constant quite low ratio
            const float_t fRatio = 0.95f;
            outputAirFlow.ApplyTemperatureChange(fRatio);
         } else  {
            // For all faster speeds we apply an actual ratio
            const float_t fNormalisedCarVelocityKPH = fCarVelocityKPH - fMinimumCarVelocityKPH;
            const float_t fRatio = 0.95f - (fNormalisedCarVelocityKPH / 300.0f);
            outputAirFlow.ApplyTemperatureChange(fRatio);
         }



        // TODO: Use fAmbientAirPressureKPA and fAmbientAirTemperatureDegreesCelcius

        //fOutputPressureKPA = fInputPressureKPA;
        //fOutputTemperatureDegreesCelcius = fInputTemperatureDegreesCelcius - cooling from fCarVelocityKPH;
      }







      // *** cClutch

      cClutch::cClutch() :
        fMassKg(10.0f),

        fInputClutch0To1(1.0f),
        fRPMBeforeClutch(0.0f),
        fTorqueNmBeforeClutch(0.0f),

        fRPMAfterClutch(0.0f),
        fTorqueNmAfterClutch(0.0f)
      {
      }

      void cClutch::Update(durationms_t currentTime)
      {
        const float_t fOneMinusInputClutch0To1 = 1.0f - fInputClutch0To1;
        fRPMAfterClutch = fOneMinusInputClutch0To1 * fRPMBeforeClutch;
        fTorqueNmAfterClutch = fOneMinusInputClutch0To1 * fTorqueNmBeforeClutch;
      }



      // *** cGearBox

      cGearBox::cGearBox() :
        fMassKg(20.0f),
        fEfficiency0To1(0.9f),
        currentGear(1), // Default to neutral even though we don't actually have a neutral yet
        fRPMBeforeGearBox(0.0f),
        fTorqueNmBeforeGearBox(0.0f),

        fRPMAfterGearBox(0.0f),
        fTorqueNmAfterGearBox(0.0f)
      {
      }

      void cGearBox::AddGear(float_t fGearRatio)
      {
        gears.push_back(fGearRatio);
      }

      void cGearBox::ChangeGearUp()
      {
        if ((currentGear + 1) < gears.size()) currentGear++;
      }

      void cGearBox::ChangeGearDown()
      {
        if (currentGear != 0) currentGear--;
      }

      float_t cGearBox::GetGearRatio() const
      {
        ASSERT(currentGear < gears.size());
        return gears[currentGear];
      }

      void cGearBox::Update(durationms_t currentTime)
      {
        ASSERT(currentGear < gears.size());

        currentGear = 5;

        CalculateGearing(gears[currentGear], fRPMBeforeGearBox, fTorqueNmBeforeGearBox, fRPMAfterGearBox, fTorqueNmAfterGearBox);

        // This gearbox may not be 100% efficient
        fTorqueNmAfterGearBox *= fEfficiency0To1;
      }



      // *** cDifferential

      cDifferential::cDifferential() :
        fRatio(3.42f / 1.0f)
      {
      }

      void cDifferential::Update(durationms_t currentTime)
      {
        CalculateGearing(fRatio, fRPMBeforeDifferential, fTorqueNmBeforeDifferential, fRPMAfterDifferential, fTorqueNmAfterDifferential);
      }








      // *** cWheel

      cWheel::cWheel() :
        fMassOfRimKg(20.0f),
        fMassOfTireKg(10.0f),
        fRPM(0.0f),
        fTorqueNm(0.0f)
      {
        fDiametreOfRimCentimetres = math::InchesToCentimeters(17.0f);
        fProfileOfTireCentimetres = 3.0f;
        fWidthCentimetres = math::InchesToCentimeters(8.0f);
      }

      float_t cWheel::GetSpeedKPH() const
      {
        // 17 inch rim, 3 cm tire on each side of the rim
        const float fRadiusOfTireCentimeters = (0.5f * fDiametreOfRimCentimetres) + fProfileOfTireCentimetres;

        const float fSpeedCentimetersPerMinute = ((2.0f * math::cPI * fRadiusOfTireCentimeters) * fRPM);
        const float fSpeedKPH = fSpeedCentimetersPerMinute * (60.0 / 100000.0f); // 60 minutes in an hour, 100000 cm in a kilometer

        return fSpeedKPH;
      }

      void cWheel::Update(durationms_t currentTime)
      {
        // http://drpetter.proboards.com/index.cgi?board=rigid&action=display&thread=6

/*        ...
        const float fWeightOfCarOnThisWheel = fLiftingForceProvidedByThisSpring;

        const float fTireGrip0To1 = 0.9f;
        const float fDrivenOnSurfaceGrip0To1 = 1.0f; // Less for wet tarmac and less for dirt/sand/gravel etc.

        const float fForceAppliedByTireKw = math::NmToKw(fTorqueNm, fRPM) * fTireGrip0To1 * fDrivenOnSurfaceGrip0To1;

        fVelocity = fVelocity + fForceAppliedByTireKw / fWeightOfCarOnThisWheel;*/
      }
    }






    class cEngineAudioManager
    {
    public:
      void SetNaturalRPM(float_t _fNaturalRPM) { fNaturalRPM = _fNaturalRPM; }
      // This is used for formula 1 engines for example where the loudest point is at really high rpm when they scream down the straights
      void SetHighRPMLoudnessFactor0To1(float_t _fHighRPMLoudnessFactor0To1) { fHighRPMLoudnessFactor0To1 = _fHighRPMLoudnessFactor0To1; }
      void SetSource(breathe::audio::cSourceRef _pSource) { pSource = _pSource; }

      void SetRPM(float_t _fRPM) { fRPM = _fRPM; }
      void SetAccelerator0To1(float_t _fAccelerator0To1) { fAccelerator0To1 = _fAccelerator0To1; }

      void Update(spitfire::durationms_t currentTime);

    private:
      float_t fNaturalRPM;
      float_t fHighRPMLoudnessFactor0To1;
      breathe::audio::cSourceRef pSource;

      float_t fRPM;
      float_t fAccelerator0To1;
    };

    void cEngineAudioManager::Update(spitfire::durationms_t currentTime)
    {
      // As the rpm increases the volume goes up exponentially
      if (fHighRPMLoudnessFactor0To1 > spitfire::math::cEPSILON) {
        pSource->SetVolume(1.0f + (fHighRPMLoudnessFactor0To1 * spitfire::math::squared(fRPM)));
      }
    }

    typedef cEngineAudioManager cSuperChargerAudioManager;
    typedef cEngineAudioManager cTurboAudioManager;



    // NOTE: The blow off valve source is constantly playing at volume 0.0f, if the blow off valve actually blows off then volume is set to 1.0f
    class cBlowOffValveAudioManager
    {
    public:
      void SetHighPressureLoudnessFactor0To1(float_t _fHighPressureLoudnessFactor0To1) { fHighPressureLoudnessFactor0To1 = _fHighPressureLoudnessFactor0To1; }
      void SetMinimumBlowOffPressureKPA(float_t _fMinimumBlowOffPressureKPA) { fMinimumBlowOffPressureKPA = _fMinimumBlowOffPressureKPA; }
      void SetSource(breathe::audio::cSourceRef _pSource) { pSource = _pSource; }

      void SetTurboPressureKPA(float_t _fTurboPressureKPA) { fTurboPressureKPA = _fTurboPressureKPA; }

      void Update(spitfire::durationms_t currentTime);

    private:
      float_t fHighPressureLoudnessFactor0To1;
      float_t fMinimumBlowOffPressureKPA;
      breathe::audio::cSourceRef pSource;

      float_t fTurboPressureKPA;
    };

    void cBlowOffValveAudioManager::Update(spitfire::durationms_t currentTime)
    {
      if (fTurboPressureKPA < fMinimumBlowOffPressureKPA) {
        // Not enough pressure to blow off
        pSource->SetVolume(0.0f);
      } else {
        // Ok this blow off valve is active
        pSource->SetVolume(1.0f);

        // As the pressure increases the volume goes up exponentially
        if (fHighPressureLoudnessFactor0To1 > spitfire::math::cEPSILON) {
          pSource->SetVolume(1.0f + (fHighPressureLoudnessFactor0To1 * spitfire::math::squared(fTurboPressureKPA)));
        }

        pSource->SetPitch(spitfire::math::squared(fTurboPressureKPA) / fMinimumBlowOffPressureKPA);
      }
    }








    #if 0
    cTurboAudioManager turboAudio;
    cBlowOffValveAudioManager blowOffValveAudio;

    void cCarComponent::Init()
    {
      blowOffValveAudio.SetHighPressureLoudnessFactor0To1(0.5f);
      blowOffValveAudio.SetMinimumBlowOffPressureKPA(...?);
    }

    void cCarComponent::Update(durationms_t currentTime)
    {
      turboAudio.SetAccelerator0To1(fAccelerator0To1);
      turboAudio.SetRPM(fRPM);
      turboAudio.Update(currentTime);

      blowOffValveAudio.SetTurboPressureKPA(turbo.GetPressureKPA());
      blowOffValveAudio.Update(currentTime);

    }
    #endif
  }
}

class cCarFactory
{
public:
  cCarFactory();

  void CreateCar(breathe::scenegraph3d::cGroupNodeRef, breathe::render::cTextureFrameBufferObjectRef& pCubeMapTexture);

private:
  size_t iCar;
};

cCarFactory::cCarFactory() :
  iCar(0)
{
}

void cCarFactory::CreateCar(breathe::scenegraph3d::cGroupNodeRef pGroupNode, breathe::render::cTextureFrameBufferObjectRef& pCubeMapTexture)
{
   pCubeMapTexture.reset();


   std::vector<breathe::render::model::cStaticModelSceneNodeFactoryItem> meshes;

#if 1
   spitfire::string_t sFilename;
   breathe::filesystem::FindResourceFile(TEXT("models/chinook/body.obj"), sFilename);

   spitfire::string_t sBodyMaterialFilename = breathe::filesystem::GetPath(sFilename) + TEXT("body.mat");

   spitfire::string_t sDiffuseTextureFilename;
   breathe::filesystem::FindResourceFile(TEXT("models/chinook/body.png"), sDiffuseTextureFilename);
#else
   spitfire::string_t sFilename;
   breathe::filesystem::FindResourceFile(TEXT("models/sphere/sphere.obj"), sFilename);

   spitfire::string_t sBodyMaterialFilename = breathe::filesystem::GetPath(sFilename) + TEXT("sphere.mat");

   spitfire::string_t sDiffuseTextureFilename;
   breathe::filesystem::FindResourceFile(TEXT("models/sphere/sphere.png"), sDiffuseTextureFilename);
#endif

   spitfire::string_t sGlossAndMetallicTextureFilename;
   breathe::filesystem::FindResourceFile(TEXT("models/sphere/gloss_and_metallic.png"), sGlossAndMetallicTextureFilename);

   spitfire::string_t sCarPaintMaterialFilename;
   breathe::filesystem::FindResourceFile(TEXT("materials/car_paint.mat"), sCarPaintMaterialFilename);

   breathe::render::model::cStaticModelSceneNodeFactory factory;
   factory.LoadFromFile(sFilename, meshes);

   breathe::render::model::cStaticModel model;

   breathe::render::model::cStaticModelLoader loader;
   loader.Load(sFilename, model);

   const size_t nMeshes = model.mesh.size();
   for (size_t iMesh = 0; iMesh < nMeshes; iMesh++) {
      breathe::render::model::cStaticModelSceneNodeFactoryItem item;

      item.pVBO.reset(new breathe::render::cVertexBufferObject);

      item.pVBO->SetVertices(model.mesh[iMesh]->vertices);
      item.pVBO->SetTextureCoordinates(model.mesh[iMesh]->textureCoordinates);

      item.pVBO->Compile();

      LOG<<"sMaterial=\""<<model.mesh[iMesh]->sMaterial<<"\""<<std::endl;
      LOG<<"sCarPaintMaterialFilename=\""<<sCarPaintMaterialFilename<<"\""<<std::endl;
      LOG<<"sBodyMaterialFilename=\""<<sBodyMaterialFilename<<"\""<<std::endl;
      if (model.mesh[iMesh]->sMaterial == sBodyMaterialFilename) {
         const spitfire::string_t sAlias = TEXT("chinook") + spitfire::string::ToString(iCar);
         item.pMaterial = pResourceManager->AddMaterialAsAlias(sCarPaintMaterialFilename, sAlias);
         const size_t n = item.pMaterial->vLayer.size();
         for (size_t i = 0; i < n; i++) {
            if (i == 0) {
               item.pMaterial->vLayer[i]->pTexture = pResourceManager->AddTexture(sDiffuseTextureFilename);
            } else if (i == 1) {
               item.pMaterial->vLayer[i]->pTexture = pResourceManager->AddTexture(sGlossAndMetallicTextureFilename);
            } else if (item.pMaterial->vLayer[i]->uiTextureMode == breathe::render::TEXTURE_MODE::CUBE_MAP) {
               // Cast shared_ptr from a normal cTextureRef to a cTextureFrameBufferObjectRef
               pCubeMapTexture.reset(new breathe::render::cTextureFrameBufferObject);

               // Create our cubemap texture
               pCubeMapTexture->SetModeCubeMap();
               pCubeMapTexture->SetWidth(256);
               pCubeMapTexture->SetHeight(256);

               pCubeMapTexture->Create();

               // Set the texture on the material to this new cubemap texture
               item.pMaterial->vLayer[i]->pTexture = pCubeMapTexture;
            }
         }
      } else {
         item.pMaterial = pResourceManager->AddMaterial(model.mesh[iMesh]->sMaterial);
      }

      meshes.push_back(item);
   }


   // We should have a valid cubemap texture by now
   ASSERT(pCubeMapTexture != nullptr);

   factory.CreateSceneNodeAttachedTo(meshes, pGroupNode);
}


namespace breathe
{
  namespace vehicle
  {
    void cVehicleFactory::CreateCar(breathe::physics::cWorld* pWorld, breathe::game::cGameObjectCollection& gameobjects, breathe::scenegraph3d::cGroupNodeRef pNode, const spitfire::math::cVec3& position, const spitfire::math::cQuaternion& rotation, cVehicle& vehicle) const
    {
      LOG<<"cVehicleFactory::CreateCar"<<std::endl;

      cCarFactory factory;



      breathe::render::cTextureFrameBufferObjectRef pCubeMapTexture;

      factory.CreateCar(pNode, pCubeMapTexture);
      ASSERT(pCubeMapTexture != nullptr);




      pNode->SetRelativePosition(position);
      pNode->SetRelativeRotation(rotation);




      vehicle.Create(gameobjects, pNode, pCubeMapTexture);

      breathe::game::cGameObjectRef pVehicle = vehicle.GetGameObject();

      pVehicle->SetPositionRelative(position);




      //breathe::string_t sFilename;
      //breathe::filesystem::FindResourceFile(TEXT("vehicle/toyota_gt_one/mesh.3ds"), sFilename);
      //pVehicle->pModel = pRender->GetModel(sFilename);
      //breathe::filesystem::FindResourceFile(TEXT("vehicle/wheel00/mesh.3ds"), sFilename);
      //pVehicle->vWheel[0]->pModel = pVehicle->vWheel[1]->pModel = pVehicle->vWheel[2]->pModel = pVehicle->vWheel[3]->pModel = pRender->GetModel(sFilename);

      // Create the physics object
      breathe::physics::cCarProperties properties;
      properties.SetMassKg(1000.0f);
      properties.SetPositionAbsolute(position);

      properties.fWidthMetres = 1.0f;
      properties.fDepthMetres = 2.0f;
      properties.fHeightMetres = 0.5f;

      // Suspension
      properties.fSuspensionStiffness = 20.0f;
      properties.fSuspensionCompression = 4.4f;
      properties.fSuspensionDamping = 2.3f;
      properties.fSuspensionRestLengthCentimetres = 30.0f;
      properties.fSuspensionMaxTravelCentimetres = 50.0f;

      // Wheels
      properties.nWheels = 4;
      properties.fWheelMassKg = 20.0f;
      properties.fWheelWidthCentimetres = 40.0f;
      properties.fWheelRadiusCentimetres = 50.0f;
      properties.fTireFrictionSlip = 0.9f;

      breathe::physics::cCarRef pCar = physics::GetWorld()->CreateCar(properties);


      //breathe::scenegraph3d::cModelNodeRef pNode(new breathe::scenegraph3d::cModelNode);
      //pNode->SetRelativePosition(position);
      //pNode->SetRelativeRotation(spitfire::math::cQuaternion());

      //breathe::scenegraph3d::cStateSet& stateset = pNode->GetStateSet();
      //stateset.SetStateFromMaterial(pVehicleVBOMaterial);

      //breathe::scenegraph_common::cStateVertexBufferObject& vertexBufferObject = stateset.GetVertexBufferObject();
      //vertexBufferObject.pVertexBufferObject = pVehicleVBO;
      //vertexBufferObject.SetEnabled(true);
      //vertexBufferObject.bHasValidValue = true;

      // Attach to the root node
      //breathe::scenegraph3d::cSceneNodeRef pRoot = scenegraph.GetRoot();
      //breathe::scenegraph3d::cGroupNode* pRootAsGroupNode = static_cast<breathe::scenegraph3d::cGroupNode*>(pRoot.get());
      //pRootAsGroupNode->AttachChild(pNode);




      breathe::game::cPhysicsComponent* pPhysicsComponent = new breathe::game::cPhysicsComponent(*pVehicle);
      pPhysicsComponent->SetBody(pCar->GetChassis());
      pVehicle->AddComponent(breathe::game::COMPONENT::PHYSICS, pPhysicsComponent);

      //breathe::game::cRenderComponent* pRenderComponent = new breathe::game::cRenderComponent(*pVehicle);
      //pRenderComponent->SetSceneNode(pNode);
      //pVehicle->AddComponent(breathe::game::COMPONENT::RENDERABLE, pRenderComponent);





      std::vector<breathe::scenegraph3d::cGroupNodeRef> wheels;

      for (size_t i = 0; i < 4; i++) {
        // Create the wheel
        spitfire::string_t sFilename;
        breathe::filesystem::FindResourceFile(TEXT("models/chinook/rotor.obj"), sFilename);

        breathe::render::model::cStaticModelSceneNodeFactory factory;

        std::vector<breathe::render::model::cStaticModelSceneNodeFactoryItem> meshes;
        factory.LoadFromFile(sFilename, meshes);

        const float x = 2.0f;
        const float y = 10.4f;
        const float z = 8.5f;

        breathe::scenegraph3d::cGroupNodeRef pNodeWheel(new breathe::scenegraph3d::cGroupNode);

        const spitfire::string_t sName = TEXT("wheel") + spitfire::string::ToString(i);
        pNodeWheel->SetUniqueName(sName);

        pNodeWheel->SetRelativePosition(spitfire::math::cVec3(x, y, z));

        //spitfire::math::cQuaternion rotation;
        //rotation.SetFromAxisAngle(spitfire::math::v3Up, spitfire::math::DegreesToRadians(45.0f));
        //pNodeWheel->SetRelativeRotation(rotation);

        factory.CreateSceneNodeAttachedTo(meshes, pNodeWheel);

        pNode->AttachChild(pNodeWheel);

        wheels.push_back(pNodeWheel);
      }




      breathe::game::cVehicleComponent* pVehicleComponent = new breathe::game::cVehicleComponent(*pVehicle);
      pVehicleComponent->SetCar(pCar, wheels);
      pVehicle->AddComponent(breathe::game::COMPONENT::VEHICLE, pVehicleComponent);







      SCREEN<<"cVehicleFactory::CreateCar returning"<<std::endl;
    }
  }
}




/*breathe::game::cGameObjectRef CreateVehicle(breathe::physics::cWorld* pWorld, breathe::scenegraph3d::cSceneGraph& scenegraph, breathe::game::cGameObjectCollection& gameobjects, const breathe::math::cVec3& position)
{
   LOG<<"CreateVehicle"<<std::endl;

   breathe::game::cGameObjectRef pVehicle(new breathe::game::cGameObject);

   //breathe::string_t sFilename;
   //breathe::filesystem::FindResourceFile(TEXT("vehicle/toyota_gt_one/mesh.3ds"), sFilename);
   //pVehicle->pModel = pRender->GetModel(sFilename);
   //breathe::filesystem::FindResourceFile(TEXT("vehicle/wheel00/mesh.3ds"), sFilename);
   //pVehicle->vWheel[0]->pModel = pVehicle->vWheel[1]->pModel = pVehicle->vWheel[2]->pModel = pVehicle->vWheel[3]->pModel = pRender->GetModel(sFilename);

   // Test crate
   breathe::physics::cPhysicsObjectRef pPhysicsObject(new breathe::physics::cPhysicsObject);
   pPhysicsObject->fMassKg = 10.0f;

   pPhysicsObject->CreateBox(pWorld, position);


   breathe::scenegraph3d::cModelNodeRef pNode(new breathe::scenegraph3d::cModelNode);
   pNode->SetRelativePosition(position);
   pNode->SetRelativeRotation(spitfire::math::cQuaternion());

   breathe::scenegraph3d::cStateSet& stateset = pNode->GetStateSet();
   stateset.SetStateFromMaterial(pVehicleVBOMaterial);

   breathe::scenegraph_common::cStateVertexBufferObject& vertexBufferObject = stateset.GetVertexBufferObject();
   vertexBufferObject.pVertexBufferObject = pVehicleVBO;
   vertexBufferObject.SetEnabled(true);
   vertexBufferObject.bHasValidValue = true;

   // Attach to the root node
   breathe::scenegraph3d::cSceneNodeRef pRoot = scenegraph.GetRoot();
   breathe::scenegraph3d::cGroupNode* pRootAsGroupNode = static_cast<breathe::scenegraph3d::cGroupNode*>(pRoot.get());
   pRootAsGroupNode->AttachChild(pNode);




   breathe::game::cPhysicsComponent* pPhysicsComponent = new breathe::game::cPhysicsComponent(*pVehicle);
   pPhysicsComponent->SetPhysicsObject(pPhysicsObject);
   pVehicle->AddComponent(breathe::game::COMPONENT::PHYSICS, pPhysicsComponent);

   breathe::game::cRenderComponent* pRenderComponent = new breathe::game::cRenderComponent(*pVehicle);
   pRenderComponent->SetSceneNode(pNode);
   pVehicle->AddComponent(breathe::game::COMPONENT::RENDERABLE, pRenderComponent);

   breathe::game::cVehicleComponent* pVehicleComponent = new breathe::game::cVehicleComponent(*pVehicle);
   pVehicleComponent->SetCar();
   pVehicle->AddComponent(breathe::game::COMPONENT::VEHICLE, pVehicleComponent);

   gameobjects.Add(pVehicle);






   //LOG.Success("Game", "Set Vehicle Model");
   //breathe::string_t sFilename;
   //breathe::filesystem::FindResourceFile(TEXT("vehicle/toyota_gt_one/mesh.3ds"), sFilename);
   //pVehicle->pModel = pRender->GetModel(sFilename);
   //breathe::filesystem::FindResourceFile(TEXT("vehicle/wheel00/mesh.3ds"), sFilename);
   //pVehicle->vWheel[0]->pModel = pVehicle->vWheel[1]->pModel = pVehicle->vWheel[2]->pModel = pVehicle->vWheel[3]->pModel= //pRender->GetModel(sFilename);




   //breathe::vehicle::cVehicleRef pVehicle(breathe::vehicle::cVehicle::Create());

   //pVehicle->Init(s, 1);
   //pVehicle->PhysicsInit(s);

   //pLevel->AddVehicle(pVehicle);

   //breathe::filesystem::FindResourceFile(TEXT("vehicle/toyota_gt_one/mesh.3ds"), sFilename);
   //pVehicle->pModel=pRender->GetModel(sFilename);

   //breathe::filesystem::FindResourceFile(TEXT("vehicle/wheel00/mesh.3ds"), sFilename);
   //pVehicle->vWheel[0]->pModel=pRender->GetModel(sFilename);
   //pVehicle->vWheel[1]->pModel=pRender->GetModel(sFilename);
   //pVehicle->vWheel[2]->pModel=pRender->GetModel(sFilename);
   //pVehicle->vWheel[3]->pModel=pRender->GetModel(sFilename);

   return pVehicle;
}*/



/*
  800.0f,   // mass
  -0.2f,    // Y shift
  0.0f,     // Z shift

  40.0f,    // suspension stiffness
  2.3f,     // suspension damping
  2.4f,     // suspension compression
  0.10f,  //0.15f,  // suspension rest_length
  10.0f,  //0.30f,  // max suspension travel (cm)
  0.2f,     // roll influence
  2.0f,     // wheel friction
  1600.0f,  // max engine force
  200.0f,   // max breaking force
  0.5f,     // steering clamp

  0.0f,   //0.2f,    // fwheel X shift
      0.0f    //0.2f     // rwheel X shift
*/

namespace breathe
{
  namespace game
  {
    cVehicleCar::cVehicleCar(cGameObject& _object, physics::cCarRef _pCar, const std::vector<breathe::scenegraph3d::cGroupNodeRef>& _wheelNodes) :
      cVehicleBase(_object, TYPE::CAR),
      drive(DRIVE::RWD),
      fActualVelocityKPH(0.0f),
      pCar(_pCar),
      wheelNodes(_wheelNodes)
    {
    }

    void cVehicleCar::InitParts()
    {
      for (size_t i = 0; i < 4; i++) wheels.push_back(car::cWheel());


      // Gear ratios for a 2004 Corvette
      gearbox.AddGear(-3.28f / 1.0f); // Reverse
      gearbox.AddGear(0.0f / 1.0f); // Neutral
      gearbox.AddGear(2.97f / 1.0f);
      gearbox.AddGear(2.07f / 1.0f);
      gearbox.AddGear(1.43f / 1.0f);
      gearbox.AddGear(1.00f / 1.0f);
      gearbox.AddGear(0.84f / 1.0f);
      gearbox.AddGear(0.56f / 1.0f);


      // Diff ratio for a 2004 Corvette
      differential.SetRatio(3.42f / 1.0f);
    }

    void cVehicleCar::_Init()
    {
      LOG<<"cVehicleCar::_Init"<<std::endl;

      InitParts();


      spitfire::string_t sFilename;
      spitfire::filesystem::FindResourceFile(TEXT("audio/engine.wav"), sFilename);


      breathe::audio::cBufferRef pBuffer = breathe::audio::GetManager()->CreateBuffer(sFilename);
      if (pBuffer == nullptr) {
        SCREEN<<"cVehicleCar::_Init pBuffer=NULL, returning"<<std::endl;
        return;
      }

      pSourceEngine = breathe::audio::GetManager()->CreateSourceAttachedToObject(pBuffer);
      if (pSourceEngine == nullptr) {
        SCREEN<<"cVehicleCar::_Init pSourceEngine=NULL, returning"<<std::endl;
        return;
      }


      breathe::game::cAudioSourceComponent* pAudioSourceComponent = nullptr;

      if (object.IsComponentPresentAndEnabledOrDisabled(breathe::game::COMPONENT::AUDIOSOURCE)) {
        pAudioSourceComponent = object.GetComponentIfEnabledOrDisabled<breathe::game::cAudioSourceComponent>(breathe::game::COMPONENT::AUDIOSOURCE);
      } else {
        pAudioSourceComponent = new breathe::game::cAudioSourceComponent(object);

        // Add it to the game object
        object.AddComponent(breathe::game::COMPONENT::AUDIOSOURCE, pAudioSourceComponent);
      }

      ASSERT(pAudioSourceComponent != nullptr);
      pAudioSourceComponent->AddSource(pSourceEngine);



      pSourceEngine->SetLooping();

      LOG<<"cVehicleCar::_Init Playing source"<<std::endl;
      pSourceEngine->Play();
    }


    float_t cVehicleCar::GetTotalMassKg() const
    {
      float_t fMassKg = chassis.GetMassKg() + engine.GetMassKg();

      {
        const size_t n = superChargers.size();
        for (size_t i = 0; i < n; i++) fMassKg += superChargers[i].GetMassKg();
      }
      {
        const size_t n = turboChargers.size();
        for (size_t i = 0; i < n; i++) fMassKg += turboChargers[i].GetMassKg();
      }
      {
        const size_t n = interCoolers.size();
        for (size_t i = 0; i < n; i++) fMassKg += interCoolers[i].GetMassKg();
      }

      fMassKg += clutch.GetMassKg() + gearbox.GetMassKg() + differential.GetMassKg();

      {
        const size_t n = wheels.size();
        for (size_t i = 0; i < n; i++) {
          fMassKg += wheels[i].GetMassOfRimKg();
          fMassKg += wheels[i].GetMassOfTireKg();
        }
      }

      return fMassKg;
    }

    void cVehicleCar::_Update(durationms_t currentTime)
    {
      ASSERT(wheels.size() == 4);


      const car::cAmbientSettings ambientSettings(math::GENERIC_AMBIENT_AIR_AT_SEA_LEVEL_DENSITY_KG_PER_CUBIC_METER, math::GENERIC_AMBIENT_AIR_AT_SEA_LEVEL_PRESSURE_KPA, math::GENERIC_AMBIENT_AIR_AT_SEA_LEVEL_TEMPERATURE_DEGREES_CELCIUS);

      // Update the engine first
      engine.SetAmbientSettings(ambientSettings);
      engine.SetIntakeAirFlow(engineIntakeAirFlow);
      engine.SetAcceleratorInput0To1(fInputAccelerator0To1);

      engine.Update(currentTime);


      if (!superChargers.empty()) {
        // Update each super charger
        const size_t n = superChargers.size();
        for (size_t i = 0; i < n; i++) {
          superChargers[i].SetEngineRPM(engine.GetRPM());
          superChargers[i].SetAmbientSettings(ambientSettings);

          superChargers[i].Update(currentTime);

          // Add to the combined output air pressure
          engineIntakeAirFlow.Combine(superChargers[i].GetOutputAirFlow());
        }
      }

      if (!turboChargers.empty()) {
        // Update each turbo charger
        const size_t n = turboChargers.size();

        // Split the pressure between each turbo equally, I don't know how realistic this is for two differently sized turbos, you might want more exhaust going to the larger one?
        ASSERT(n != 0);
        const car::cAirFlow splitExhaustAirFlow = engine.GetExhaustAirFlow().SplitIntoParts(n);

        for (size_t i = 0; i < n; i++) {
          turboChargers[i].SetAmbientSettings(ambientSettings);
          turboChargers[i].SetEngineExhaustAirFlow(splitExhaustAirFlow);

          turboChargers[i].Update(currentTime);

          // Add to the combined output air pressure
          engineIntakeAirFlow.Combine(turboChargers[i].GetOutputAirFlow());
        }
      }

      if (!interCoolers.empty()) {
        // Pass the engine input air through each intercooler and perform any cooling
        const size_t n = interCoolers.size();
        for (size_t i = 0; i < n; i++) {
          interCoolers[i].SetCarVelocityKPH(fActualVelocityKPH);
          interCoolers[i].SetAmbientSettings(ambientSettings);
          interCoolers[i].SetInputAirFlow(engineIntakeAirFlow);

          interCoolers[i].Update(currentTime);

          // Set our input air pressure and temperature to whatever the intercooler is putting out
          engineIntakeAirFlow = interCoolers[i].GetOutputAirFlow();
        }
      }

      // TODO: gearbox, clutch, differential, suspension, tires

      {
        // Update wheels
        float_t fRPM = engine.GetRPM();
        float_t fTorqueNm = engine.GetTorqueNm();


        // Clutch
        clutch.SetRPMBeforeClutch(fRPM);
        clutch.SetTorqueNmBeforeClutch(fTorqueNm);

        clutch.Update(currentTime);

        fRPM = clutch.GetRPMAfterClutch();
        fTorqueNm = clutch.GetTorqueNmAfterClutch();


        // GearBox
        gearbox.SetRPMBeforeGearBox(fRPM);
        gearbox.SetTorqueNmBeforeGearBox(fTorqueNm);

        gearbox.Update(currentTime);

        fRPM = gearbox.GetRPMAfterGearBox();
        fTorqueNm = gearbox.GetTorqueNmAfterGearBox();



        // Differential
        differential.SetRPMBeforeDifferential(fRPM);
        differential.SetTorqueNmBeforeDifferential(fTorqueNm);

        differential.Update(currentTime);

        fRPM = differential.GetRPMAfterDifferential();
        fTorqueNm = differential.GetTorqueNmAfterDifferential();


        // Wheels
        // Divide the torque evenly amount the wheels
        /*if (IsAWD()) {
          fRPM /= 4.0f;
          fTorqueNm /= 4.0f;
        } else {
          fRPM /= 2.0f;
          fTorqueNm /= 2.0f;
        }*/

        if (IsAWD() || IsRWD()) {
          wheels[WHEEL_REAR_LEFT].SetRPM(fRPM);
          wheels[WHEEL_REAR_LEFT].SetTorqueNm(fTorqueNm);
          wheels[WHEEL_REAR_RIGHT].SetRPM(fRPM);
          wheels[WHEEL_REAR_RIGHT].SetTorqueNm(fTorqueNm);
        }

        if (IsAWD() || IsFWD()) {
          wheels[WHEEL_FRONT_LEFT].SetRPM(fRPM);
          wheels[WHEEL_FRONT_LEFT].SetTorqueNm(fTorqueNm);
          wheels[WHEEL_FRONT_RIGHT].SetRPM(fRPM);
          wheels[WHEEL_FRONT_RIGHT].SetTorqueNm(fTorqueNm);
        }

        const size_t n = wheels.size();
        for (size_t i = 0; i < n; i++) {
          wheels[i].Update(currentTime);
        }
      }





      const float_t fEngineRPM = engine.GetRPM();
      const float_t fNaturalEngineRPM = 3000.0f;
      if (pSourceEngine != nullptr) {
        pSourceEngine->SetPitch(fEngineRPM / fNaturalEngineRPM);
      }










      /*if (fInputBrake0To1 > 0.01f) {
        // This is more of a brake than an actual go down method
        fInputAccelerator0To1 = 0.0f;
      }

      if (fInputLeft0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputLeft0To1 * pPhysicsObject->GetMassKg() * 2.0f * breathe::math::v3Up);
        pPhysicsObject->AddTorqueRelativeToWorldNm(torqueNm);
      }
      if (fInputRight0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputRight0To1 * pPhysicsObject->GetMassKg() * -2.0f * breathe::math::v3Up);
        pPhysicsObject->AddTorqueRelativeToWorldNm(torqueNm);
      }

      if (fInputHandBrake0To1 > 0.01f) {
      }*/


      clutch.SetInputClutch0To1(fInputClutch0To1);



      if (bIsInputChangeGearUp) {
        gearbox.ChangeGearUp();
      }
      if (bIsInputChangeGearDown) {
        gearbox.ChangeGearDown();
      }


      // Fnet = Facceleration + Fbraking + Fdragresistance + Frollingresistance

      // Friction coefficients:
      // Concrete (wet)  Rubber  0.30
      // Concrete (dry)  Rubber  1.0
      // Metal   Wood  0.2-0.6


      // Force on a single tire
      //
      // F = (MassOfWholeCar / nWheels) + MassOfWheel * GravitationalAcceleration9.8m/s/s


      // http://en.wikipedia.org/wiki/Rolling_resistance#Physical_formula
      // F = Crr * Nf
      //
      // F is the rolling resistance force
      // Crr is the dimensionless rolling resistance coefficient or coefficient of rolling friction (CRF)
      // Nf is the normal force
      //
      // F = Nf * b / r
      //
      // F is the rolling resistance force
      // r is the wheel radius
      // b is the rolling resistance coefficient or coefficient of rolling friction with dimension of length
      // Nf is the normal force
      //
      // b = Crr * r
      //
      //
      // Rolling Resistance Coefficient Examples
      // Crr                b        Description
      // 0.0002 to 0.0010   0.5 mm   Railroad steel wheel on steel rail
      // 0.0002 to 0.0010   0.1 mm   Hardened steel ball bearings on steel
      // 0.0022 to 0.005             Production bicycle tires at 120 psi and 50 km/h, measured on rollers
      // 0.0025                      Special Michelin solar car/eco-marathon tires
      // 0.005                       Tram rails standard dirty with straights and curves
      // 0.0055                      Typical BMX bicycle tires used for solar cars
      // 0.006 - 0.01                Low rolling resistance car tire
      // 0.010 to 0.015              Ordinary car tires on concrete
      // 0.055 to 0.065              Ordinary car tires on grass, mud, and sand
      // 0.3                         Ordinary car tires on sand
      //
      //
      // Examples:
      //
      // How big is the area of the tire contact patch?
      // AreaOfContactPatch = WeightOfCar / 4 / PressureOfTires
      // AreaOfContactPatchSquareInches = WeightOfCarInPounds / 4 / PressureOfTiresPsi
      // AreaOfContactPatchSquareMeters = WeightOfCarInKg / 4 / PressureOfTiresPa   (NOTE: 1 Pa = 1 N / 1 m squared)
      // AreaOfContactPatchSquareMeters = 1000 Kg × / 4 / 32 Pa
      //
      // A car of 1000 kg on asphalt will need a force of around 100 newtons for rolling
      // N = WeightOfCar * Gravity * Crr
      // 98.1 N = 1000 kg × 9.81 m/s2 × 0.01

      const float fSteeringAngleMinusOneToPlusOne = -fInputLeft0To1 + fInputRight0To1;

      for (size_t i = 0; i < 4; i++) {
        pCar->SetWheelAccelerationForceNewtons(i, fInputAccelerator0To1 * 10000.0f);
        pCar->SetWheelBrakingForceNewtons(i, fInputBrake0To1 * 10000.0f);
        pCar->SetWheelSteeringAngleMinusOneToPlusOne(i, fSteeringAngleMinusOneToPlusOne * 30.0f);
      }

      pCar->Update(currentTime);



      for (size_t i = 0; i < 4; i++) {
        wheelNodes[i]->SetRelativePosition(pCar->GetWheelPositionRelative(i));
        wheelNodes[i]->SetRelativeRotation(pCar->GetWheelRotationRelative(i));
      }



      // Set values back to defaults for next time
      fInputAccelerator0To1 = 0.0f;
      fInputBrake0To1 = 0.0f;
      fInputLeft0To1 = 0.0f;
      fInputRight0To1 = 0.0f;
      fInputHandBrake0To1 = 0.0f;
      fInputClutch0To1 = 0.0f;
    }



    void TestEFIWithSpecificParameters(cEngine& engine, cEFI& efi, float fRPM, float fMassAirKgPerSecond, float fMassAirFuelRatio)
    {
      engine.SetRPM(fRPM);
      engine.SetMassAirKgPerSecond(fMassAirKgPerSecond);
      efi.SetMassAirFuelRatio(fMassAirFuelRatio);

      const float fPulseWidthMS = efi.GetPulseWidthMS();
      std::cout<<"PulseWidth="<<fPulseWidthMS<<"ms"<<std::endl;
      const float fEngineFuelFlowRateKgPerSecond = efi.GetEngineFuelFlowRateKgPerSecond();
      std::cout<<"EngineFuelFlowRate="<<fEngineFuelFlowRateKgPerSecond * 60.0f * 60.0f<<"Kg/h, "<<spitfire::math::KiloGramsToPounds(fEngineFuelFlowRateKgPerSecond) * 60.0f * 60.0f<<"lb/h"<<std::endl;
    }

    void TestEFI()
    {
      cEngine engine;
      cEFI efi(engine);

      // Cylinder and Engine Capacity
      // http://mb-soft.com/public2/engine.html
      // The bore (diameter of the cylinder) is 4", and the stroke (twice the crankshaft throw radius) is 3.5".
      // The volume of that cylindrical volume is then (PI) * R2 * H or 3.1416 * 2 * 2 * 3.5 or around 44 cubic inches.
      // Since that engine has eight cylinders that are each that volume, its total 'displacement' is 44 * 8 or around 350 cubic inches. This engine is generally called the Chevy 350 V-8.
      engine.SetNumberOfCylinders(8);
      engine.SetBoreRadiusCentiMeters(spitfire::math::InchesToCentiMeters(2.0f));
      engine.SetStrokeLengthCentiMeters(spitfire::math::InchesToCentiMeters(3.5f));
      engine.SetCompressionRatioMassAirToPetrol(8.0f / 1.0f);

      float fCylinderDisplacementCubicCentiMeters = engine.GetCylinderDisplacementCubicCentiMeters();
      std::cout<<"Cylinder="<<spitfire::math::CubicCentiMetersToCubicInches(fCylinderDisplacementCubicCentiMeters)<<"ci, "<<fCylinderDisplacementCubicCentiMeters<<"cc"<<std::endl;
      float fEngineDisplacementCubicCentiMeters = engine.GetEngineDisplacementCubicCentiMeters();
      std::cout<<"Engine="<<spitfire::math::CubicCentiMetersToCubicInches(fEngineDisplacementCubicCentiMeters)<<"ci, "<<fEngineDisplacementCubicCentiMeters<<"cc"<<std::endl;


      // Engine Petrol Usage Dynamic Range
      // https://secure.wikimedia.org/wikipedia/en/wiki/Fuel_injection#Sample_pulsewidth_calculations
      // https://secure.wikimedia.org/wikipedia/en/wiki/Fuel_injection#Calculate_injector_pulsewidth_from_airflow
      // https://secure.wikimedia.org/wikipedia/en/wiki/Fuel_injection#Calculate_fuel-flow_rate_from_pulsewidth
      // Create a 5L engine and test it at idle (700 RPM) and maximum power (5500 RPM)
      engine.SetNumberOfCylinders(8);
      engine.SetBoreRadiusCentiMeters(spitfire::math::InchesToCentiMeters(2.0f));
      engine.SetStrokeLengthCentiMeters(spitfire::math::InchesToCentiMeters(3.05f));
      engine.SetCompressionRatioMassAirToPetrol(8.0f / 1.0f);
      engine.SetInjectorSizeMassFuelKgPerSecond(spitfire::math::PoundsToKiloGrams(2.5f) / 60.0f);

      // Engine at idle
      // 0.55 lb per min * 1 min/700 rev * 1 revolution/2 strokes * 1/14.64 * 2.5/min per lb = 6.7 * 10^-5 min = 4ms
      // (Fuel flow rate) = (2.0 ms/intake-stroke) × (hour/3,600,000 ms) × (24 lb-fuel/hour) × (4-intake-stroke/rev) × (700 rev/min) × (60 min/h) = (2.24 lb/h)
      TestEFIWithSpecificParameters(engine, efi, 700.0f, spitfire::math::PoundsToKiloGrams(0.55f) / 60.0f, 14.64f / 1.0f);

      // Engine at maximum power
      // 28.0 lb per min * 1 min/5500 rev * 1 revolution/2 strokes * 1/11.0 * 2.5/min per lb = 57.9 * 10^-5 min = 35ms
      // (Fuel flow rate) = (17.3 ms/intake-stroke) × (hour/3,600,000-ms) × (24 lb-fuel/hour) × (4-intake-stroke/rev) × (5500-rev/min) × (60-min/hour) = (152 lb/h)
      TestEFIWithSpecificParameters(engine, efi, 5500.0f, spitfire::math::PoundsToKiloGrams(28.0f) / 60.0f, 11.0f / 1.0f);
    }
  }
}

