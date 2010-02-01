// Standard library includes
#include <cmath>

#include <vector>
#include <list>
#include <map>

#include <iostream>
#include <sstream>

// Boost includes
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <GL/GLee.h>


#include <spitfire/util/log.h>

#include <spitfire/math/units.h>
#include <spitfire/storage/filesystem.h>


#include <breathe/game/component.h>

#include <breathe/vehicle/cCar.h>


#include <breathe/render/model/cStaticModelLoader.h>


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

// A car backfires because unburned fuel is getting to the muffler. The heat of the exhause system causes the fuel to combust, thus a backfire.



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

      void cEngine::Update(sampletime_t currentTime)
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

        const sampletime_t timeBetweenEachFiring = ...;

        if (currentTime >= nextFiring) {
          // Control the air fuel mix
          1 petrol : 16 air or something?

          ... use up some petrol


          nextFiring = currentTime + timeBetweenEachFiring;
        }
#endif
      }


      cSuperCharger::cSuperCharger() :
        fMassKg(20.0f),
        fEngineRPM(0.0f),
        fRatio(5.0f / 1.0f),
        fRPM(0.0f)
      {
      }

      void cSuperCharger::Update(sampletime_t currentTime)
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

      void cTurboCharger::Update(sampletime_t currentTime)
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



      void cInterCooler::Update(sampletime_t currentTime)
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

      void cClutch::Update(sampletime_t currentTime)
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

      void cGearBox::Update(sampletime_t currentTime)
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

      void cDifferential::Update(sampletime_t currentTime)
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

      void cWheel::Update(sampletime_t currentTime)
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

  void Update(spitfire::sampletime_t currentTime);

private:
  float_t fNaturalRPM;
  float_t fHighRPMLoudnessFactor0To1;
  breathe::audio::cSourceRef pSource;

  float_t fRPM;
  float_t fAccelerator0To1;
};

void cEngineAudioManager::Update(spitfire::sampletime_t currentTime)
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

  void Update(spitfire::sampletime_t currentTime);

private:
  float_t fHighPressureLoudnessFactor0To1;
  float_t fMinimumBlowOffPressureKPA;
  breathe::audio::cSourceRef pSource;

  float_t fTurboPressureKPA;
};

void cBlowOffValveAudioManager::Update(spitfire::sampletime_t currentTime)
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

void cCarComponent::Update(sampletime_t currentTime)
{
  turboAudio.SetAccelerator0To1(fAccelerator0To1);
  turboAudio.SetRPM(fRPM);
  turboAudio.Update(currentTime);

  blowOffValveAudio.SetTurboPressureKPA(turbo.GetPressureKPA());
  blowOffValveAudio.Update(currentTime);

}
#endif



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
         item.pMaterial = pRender->AddMaterialAsAlias(sCarPaintMaterialFilename, sAlias);
         const size_t n = item.pMaterial->vLayer.size();
         for (size_t i = 0; i < n; i++) {
            if (i == 0) {
               item.pMaterial->vLayer[i]->pTexture = pRender->AddTexture(sDiffuseTextureFilename);
            } else if (i == 1) {
               item.pMaterial->vLayer[i]->pTexture = pRender->AddTexture(sGlossAndMetallicTextureFilename);
            } else if (item.pMaterial->vLayer[i]->uiTextureMode == breathe::render::TEXTURE_MODE::TEXTURE_CUBE_MAP) {
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
         item.pMaterial = pRender->AddMaterial(model.mesh[iMesh]->sMaterial);
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
      pVehicle->AddComponent(breathe::game::COMPONENT_PHYSICS, pPhysicsComponent);

      //breathe::game::cRenderComponent* pRenderComponent = new breathe::game::cRenderComponent(*pVehicle);
      //pRenderComponent->SetSceneNode(pNode);
      //pVehicle->AddComponent(breathe::game::COMPONENT_RENDERABLE, pRenderComponent);





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
      pVehicle->AddComponent(breathe::game::COMPONENT_VEHICLE, pVehicleComponent);







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
   pVehicle->AddComponent(breathe::game::COMPONENT_PHYSICS, pPhysicsComponent);

   breathe::game::cRenderComponent* pRenderComponent = new breathe::game::cRenderComponent(*pVehicle);
   pRenderComponent->SetSceneNode(pNode);
   pVehicle->AddComponent(breathe::game::COMPONENT_RENDERABLE, pRenderComponent);

   breathe::game::cVehicleComponent* pVehicleComponent = new breathe::game::cVehicleComponent(*pVehicle);
   pVehicleComponent->SetCar();
   pVehicle->AddComponent(breathe::game::COMPONENT_VEHICLE, pVehicleComponent);

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

      if (object.IsComponentPresentAndEnabledOrDisabled(breathe::game::COMPONENT_AUDIOSOURCE)) {
        pAudioSourceComponent = object.GetComponentIfEnabledOrDisabled<breathe::game::cAudioSourceComponent>(breathe::game::COMPONENT_AUDIOSOURCE);
      } else {
        pAudioSourceComponent = new breathe::game::cAudioSourceComponent(object);

        // Add it to the game object
        object.AddComponent(breathe::game::COMPONENT_AUDIOSOURCE, pAudioSourceComponent);
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

    void cVehicleCar::_Update(sampletime_t currentTime)
    {
      ASSERT(wheels.size() == 4);


      const car::cAmbientSettings ambientSettings(math::cAMBIENT_AIR_AT_SEA_LEVEL_DENSITY_KG_PER_CUBIC_METER, math::cAMBIENT_AIR_AT_SEA_LEVEL_PRESSURE_KPA, math::cAMBIENT_AIR_AT_SEA_LEVEL_TEMPERATURE_DEGREES_CELCIUS);

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


      /*Fnet = Facceleration + Fbraking + Fdragresistance + Frollingresistance

      Friction coefficients:
      Concrete (wet)  Rubber  0.30
      Concrete (dry)  Rubber  1.0
      Metal   Wood  0.2-0.6*/


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
  }
}
