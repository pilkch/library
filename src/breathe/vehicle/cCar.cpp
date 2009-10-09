// Standard library includes
#include <cmath>

#include <vector>
#include <list>
#include <map>

#include <sstream>

// Boost includes
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <GL/GLee.h>

#include <ode/ode.h>


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

namespace breathe
{
  namespace game
  {
    namespace car
    {
      void cEngine::Update(sampletime_t currentTime)
      {
        fRPM += 20.0f * fAcceleratorInput0To1;

        fRPM *= 0.99f;

        fRPM = spitfire::math::clamp(fRPM, 1000.0f, 5000.0f);


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

      void cInterCooler::Update(sampletime_t currentTime)
      {
        // TODO: Use fAmbientAirPressureKPA and fAmbientAirTemperatureDegreesCelcius

        //fOutputPressureKPA = fInputPressureKPA;
        //fOutputTemperatureDegreesCelcius = fInputTemperatureDegreesCelcius - cooling from fCarVelocityKPH;
      }




      void cTurboCharger::Update(sampletime_t currentTime)
      {
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



      float_t cWheel::GetWheelSpeedKPH() const
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
      /*void cCarFactory::CreateCar(breathe::physics::cWorld* pWorld, breathe::game::cGameObjectCollection& gameobjects, breathe::scenegraph3d::cGroupNodeRef pNode, const spitfire::math::cVec3& position, const spitfire::math::cQuaternion& rotation, cVehicle& vehicle) const
      {
         LOG<<"cCarFactory::CreateCar"<<std::endl;

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
         breathe::physics::cPhysicsObjectRef pPhysicsObject(new breathe::physics::cPhysicsObject);
         pPhysicsObject->fWeightKg = 10000.0f;

         pPhysicsObject->CreateBox(pWorld, position);


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
         pPhysicsComponent->SetPhysicsObject(pPhysicsObject);
         pVehicle->AddComponent(breathe::game::COMPONENT_PHYSICS, pPhysicsComponent);

         //breathe::game::cRenderComponent* pRenderComponent = new breathe::game::cRenderComponent(*pVehicle);
         //pRenderComponent->SetSceneNode(pNode);
         //pVehicle->AddComponent(breathe::game::COMPONENT_RENDERABLE, pRenderComponent);

         breathe::game::cVehicleComponent* pVehicleComponent = new breathe::game::cVehicleComponent(*pVehicle);
         pVehicleComponent->SetCar();
         pVehicle->AddComponent(breathe::game::COMPONENT_VEHICLE, pVehicleComponent);














         {
            // Create this rotor node
            spitfire::string_t sFilename;
            breathe::filesystem::FindResourceFile(TEXT("models/chinook/rotor.obj"), sFilename);

            breathe::render::model::cStaticModelSceneNodeFactory factory;

            std::vector<breathe::render::model::cStaticModelSceneNodeFactoryItem> meshes;
            factory.LoadFromFile(sFilename, meshes);

            const float x = 0.0f;
            const float y = -10.4f;
            const float z = 8.5f;

            breathe::scenegraph3d::cGroupNodeRef pNodeRotor(new breathe::scenegraph3d::cGroupNode);
            pNodeRotor->SetRelativePosition(spitfire::math::cVec3(x, y, z));

            //spitfire::math::cQuaternion rotation;
            //rotation.SetFromAxisAngle(spitfire::math::v3Up, spitfire::math::DegreesToRadians(45.0f));
            //pNodeRotor->SetRelativeRotation(rotation);

            factory.CreateSceneNodeAttachedTo(meshes, pNodeRotor);

            pNode->AttachChild(pNodeRotor);
         }
         {
            // Create this rotor node
            spitfire::string_t sFilename;
            breathe::filesystem::FindResourceFile(TEXT("models/chinook/rotor.obj"), sFilename);

            breathe::render::model::cStaticModelSceneNodeFactory factory;

            std::vector<breathe::render::model::cStaticModelSceneNodeFactoryItem> meshes;
            factory.LoadFromFile(sFilename, meshes);

            const float x = 0.0f;
            const float y = 10.0f;
            const float z = 10.5f;

            breathe::scenegraph3d::cGroupNodeRef pNodeRotor(new breathe::scenegraph3d::cGroupNode);
            pNodeRotor->SetRelativePosition(spitfire::math::cVec3(x, y, z));

            spitfire::math::cQuaternion rotation;
            rotation.SetFromAxisAngle(spitfire::math::v3Up, spitfire::math::DegreesToRadians(180.0f));
            pNodeRotor->SetRelativeRotation(rotation);

            factory.CreateSceneNodeAttachedTo(meshes, pNodeRotor);

            pNode->AttachChild(pNodeRotor);
         }
      }*/
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
        cVehicleBase(_object, TYPE::CAR)
    {
    }

    void cVehicleCar::_Update(sampletime_t currentTime)
    {
      engine.SetAcceleratorInput0To1(fInputAccelerator0To1);
      engine.Update(currentTime);

      // Update the engine first
      engine.SetAcceleratorInput0To1(fInputAccelerator0To1);
      engine.SetInputPressureKPA(fEngineInputAirPressureKPA);
      engine.SetInputTemperatureDegreesCelcius(fEngineInputAirTemperatureDegreesCelcius);

      engine.Update(currentTime);

      fEngineInputAirPressureKPA = fAmbientAirPressureKPA;
      fEngineInputAirTemperatureDegreesCelcius = fAmbientAirTemperatureDegreesCelcius;
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
        // TODO: Go through gearbox, clutch, differential first
        const float_t fRPM = engine.GetRPM();
        const float_t fTorqueNm = engine.GetTorqueNm();

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
      const float_t fNaturalEngineRPM = 1000.0f;
      ASSERT(pSourceEngine != nullptr);
      pSourceEngine->SetPitch(fEngineRPM / fNaturalEngineRPM);


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


      cPhysicsComponent* pPhysicsComponent = object.GetComponentIfEnabled<cPhysicsComponent>(COMPONENT_PHYSICS);
      if (pPhysicsComponent == nullptr) return;

      physics::cPhysicsObjectRef pPhysicsObject = pPhysicsComponent->GetPhysicsObject();
      if (pPhysicsObject == nullptr) return;




      // JUST FOR TESTING
      if (fInputAccelerator0To1 > 0.01f) {
        breathe::math::cVec3 forceKg(fInputAccelerator0To1 * pPhysicsObject->GetWeightKg() * 300.0f * breathe::math::v3Up);
        pPhysicsObject->AddForceRelativeToObjectKg(forceKg);
      }
      if (fInputBrake0To1 > 0.01f) {
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
