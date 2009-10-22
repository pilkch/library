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


// TODO: This is made up, whatever feels right?
const float_t fNormalAirFlowCubicMetresPerSecond = 1.0f;

namespace breathe
{
  namespace game
  {
    namespace car
    {
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



        fRPM += 50.0f * fAcceleratorInput0To1;

        fRPM *= 0.99f;

        fRPM = spitfire::math::clamp(fRPM, 1000.0f, 16000.0f);


/*
        // I know, in a real engine each of the four stages of an internal combustion engine are constantly taking place at a different place on each cylinder
        // We greatly simplify this by saying that the four stages happen instantaneously and "firing" happens to each cylinder in turn, so we should get similar
        // fuel economy etc. without doing the correct calculations.

        // Basically, milliseconds between each firing = (RPM -> revs per second) / cylinders

        const sampletime_t timeBetweenEachFiring = ...;

        if (currentTime >= nextFiring) {
          // Control the air fuel mix
          1 petrol : 16 air or something?

          ... use up some petrol


          nextFiring = currentTime + timeBetweenEachFiring;
        }*/
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









      // *** cGearBox

      cGearBox::cGearBox() :
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

        // Avoid a divide by zero for the neutral gear
        if (gears[currentGear] != 0.0f) {
          fRPMAfterGearBox = fRPMBeforeGearBox * (1.0f / gears[currentGear]);
          fTorqueNmAfterGearBox = fTorqueNmBeforeGearBox * (1.0f / gears[currentGear]);
        } else {
          fRPMAfterGearBox = 0.0f;
          fTorqueNmAfterGearBox = 0.0f;
        }
      }



      // *** cDifferential

      cDifferential::cDifferential() :
        fRatio(3.42f)
      {
      }

      void cDifferential::Update(sampletime_t currentTime)
      {
        ASSERT(fRatio != 0.0f);
        fRPMAfterDifferential = fRPMBeforeDifferential * (1.0f / fRatio);
        fTorqueNmAfterDifferential = fTorqueNmBeforeDifferential * (1.0f / fRatio);
      }








      // *** cWheel

      float_t cWheel::GetSpeedKPH() const
      {
        // 17 inch rim, 3 cm tire
        const float fRadiusOfTireCentimeters = math::InchesToCentimeters(17.0f) + 3.0f;

        const float fSpeedCentimetersPerMinute = ((2.0f * math::cPI * fRadiusOfTireCentimeters) * fRPM);
        const float fSpeedKPH = fSpeedCentimetersPerMinute * (60.0 / 100000.0f); // 60 minutes in an hour, 100000 cm in a kilometer

        return fSpeedKPH;
      }

      void cWheel::Update(sampletime_t currentTime)
      {
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

      // Test crate
      breathe::physics::cBoxProperties properties;
      properties.SetWeightKg(1000.0f);
      properties.SetPositionAbsolute(position);

      breathe::physics::cBodyRef pBody = physics::GetWorld()->CreateBody(properties);


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
      pPhysicsComponent->SetBody(pBody);
      pVehicle->AddComponent(breathe::game::COMPONENT_PHYSICS, pPhysicsComponent);

      //breathe::game::cRenderComponent* pRenderComponent = new breathe::game::cRenderComponent(*pVehicle);
      //pRenderComponent->SetSceneNode(pNode);
      //pVehicle->AddComponent(breathe::game::COMPONENT_RENDERABLE, pRenderComponent);

      breathe::game::cVehicleComponent* pVehicleComponent = new breathe::game::cVehicleComponent(*pVehicle);
      pVehicleComponent->SetCar();
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
   pPhysicsObject->fWeightKg = 10.0f;

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


namespace breathe
{
  namespace game
  {
    cVehicleCar::cVehicleCar(cGameObject& _object) :
      cVehicleBase(_object, TYPE::CAR),
      drive(DRIVE::RWD)
    {
    }

    void cVehicleCar::InitParts()
    {
      for (size_t i = 0; i < 4; i++) wheels.push_back(car::cWheel());


      // Gear ratios for a 2004 Corvette
      gearbox.AddGear(-3.28f); // Reverse
      gearbox.AddGear(0.0f); // Neutral
      gearbox.AddGear(2.97f);
      gearbox.AddGear(2.07f);
      gearbox.AddGear(1.43f);
      gearbox.AddGear(1.00f);
      gearbox.AddGear(0.84f);
      gearbox.AddGear(0.56f);


      // Diff ratio for a 2004 Corvette
      differential.SetRatio(3.42f);
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

    void cVehicleCar::_Update(sampletime_t currentTime)
    {
      ASSERT(wheels.size() == 4);


      const car::cAmbientSettings ambientSettings(math::cAMBIENT_AIR_AT_SEA_LEVEL_DENSITY_KG_PER_CUBIC_METER, math::cAMBIENT_AIR_AT_SEA_LEVEL_PRESSURE_KPA, math::cAMBIENT_AIR_AT_SEA_LEVEL_TEMPERATURE_DEGREES_CELCIUS);

      // Update the engine first
      engine.SetAmbientSettings(ambientSettings);
      engine.SetIntakeAirFlow(engineIntakeAirFlow);
      engine.SetAcceleratorInput0To1(fInputAccelerator0To1);

      engine.Update(currentTime);

/*
      {
        // Update each super charger
        const size_t n = superChargers.size();
        for (size_t i = 0; i < n; i++) {
          superChargers[i].SetEngineRPM(engine.GetRPM());
          superChargers[i].SetAmbientAirPressureKPA(fAmbientAirPressureKPA);
          superChargers[i].SetAmbientAirTemperatureDegreesCelcius(fAmbientAirTemperatureDegreesCelcius);

          superChargers[i].Update(currentTime);

          // Add to the combined output air pressure
          fEngineInputAirPressureKPA += superChargers[i].GetOutputPressureKPA();
          fEngineInputAirTemperatureDegreesCelcius = max(fEngineInputAirTemperatureDegreesCelcius, superChargers[i].GetOutputTemperatureDegreesCelcius());
        }
      }

      if (!turboChargers.empty()) {
        // Update each turbo charger
        const size_t n = turboChargers.size();

        // Split the pressure between each turbo equally, I don't know how realistic this is for two differently sized turbos, you might want more exhaust going to the larger one?
        ASSERT(n != 0);
        const float_t fSplitEngineExhaustPressureKPA = engine.GetExhaustPressureKPA() / n;

        for (size_t i = 0; i < n; i++) {
          turboChargers[i].SetEngineExhaustPressureKPA(fSplitEngineExhaustPressureKPA);
          turboChargers[i].SetEngineExhaustTemperatureDegreesCelcius(engine.GetExhaustTemperatureDegreesCelcius());

          turboChargers[i].Update(currentTime);

          // Add to the combined output air pressure
          fEngineInputAirPressureKPA += turboChargers[i].GetOutputPressureKPA();
          fEngineInputAirTemperatureDegreesCelcius = max(fEngineInputAirTemperatureDegreesCelcius, turboChargers[i].GetOutputTemperatureDegreesCelcius());
        }
      }

      {
        // Pass the engine input air through each intercooler and perform any cooling
        const size_t n = interCoolers.size();
        for (size_t i = 0; i < n; i++) {
          interCoolers[i].SetCarVelocityKPH(fVelocityKPH);
          interCoolers[i].SetAmbientAirPressureKPA(fAmbientAirPressureKPA);
          interCoolers[i].SetAmbientAirTemperatureDegreesCelcius(fAmbientAirTemperatureDegreesCelcius);
          interCoolers[i].SetInputPressureKPA(fEngineInputAirPressureKPA);
          interCoolers[i].SetInputTemperatureDegreesCelcius(fEngineInputAirTemperatureDegreesCelcius);

          interCoolers[i].Update(currentTime);

          // Set our input air pressure and temperature to whatever the intercooler is putting out
          fEngineInputAirPressureKPA = interCoolers[i].GetOutputPressureKPA();
          fEngineInputAirTemperatureDegreesCelcius = interCoolers[i].GetOutputTemperatureDegreesCelcius();
        }
      }*/

      // TODO: gearbox, clutch, differential, suspension, tires

      {
        // Update wheels
        float_t fRPM = engine.GetRPM();
        float_t fTorqueNm = engine.GetTorqueNm();


        // TODO: Go through clutch, first


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
      std::cout<<"fEngineRPM="<<fEngineRPM<<std::endl;
      if (pSourceEngine != nullptr) {
        std::cout<<"Setting pitch"<<std::endl;
        pSourceEngine->SetPitch(fEngineRPM / fNaturalEngineRPM);
      }










      /*if (fInputBrake0To1 > 0.01f) {
        // This is more of a brake than an actual go down method
        fInputAccelerator0To1 = 0.0f;
      }

      if (fInputLeft0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputLeft0To1 * pPhysicsObject->GetWeightKg() * 2.0f * breathe::math::v3Up);
        pPhysicsObject->AddTorqueRelativeToWorldNm(torqueNm);
      }
      if (fInputRight0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputRight0To1 * pPhysicsObject->GetWeightKg() * -2.0f * breathe::math::v3Up);
        pPhysicsObject->AddTorqueRelativeToWorldNm(torqueNm);
      }

      if (fInputHandBrake0To1 > 0.01f) {
      }

      if (fInputClutch0To1 > 0.01f) {
      }*/



      if (bIsInputChangeGearUp) {
        gearbox.ChangeGearUp();
      }
      if (bIsInputChangeGearDown) {
        gearbox.ChangeGearDown();
      }



      cPhysicsComponent* pPhysicsComponent = object.GetComponentIfEnabled<cPhysicsComponent>(COMPONENT_PHYSICS);
      if (pPhysicsComponent == nullptr) return;

      physics::cBodyRef pBody = pPhysicsComponent->GetBody();
      if (pBody == nullptr) return;



      // JUST FOR TESTING
      if (fInputAccelerator0To1 > 0.01f) {
        breathe::math::cVec3 forceKg(fInputAccelerator0To1 * pBody->GetWeightKg() * 100.0f * breathe::math::v3Up);
        pBody->AddForceRelativeToObjectKg(forceKg);
      }
      if (fInputBrake0To1 > 0.01f) {
        // This is more of a brake than an actual go down method
        fInputAccelerator0To1 = 0.0f;
      }

      if (fInputLeft0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputLeft0To1 * pBody->GetWeightKg() * 2.0f * breathe::math::v3Up);
        pBody->AddTorqueRelativeToWorldNm(torqueNm);
      }
      if (fInputRight0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputRight0To1 * pBody->GetWeightKg() * -2.0f * breathe::math::v3Up);
        pBody->AddTorqueRelativeToWorldNm(torqueNm);
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
