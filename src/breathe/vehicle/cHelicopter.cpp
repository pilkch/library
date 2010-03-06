// Standard library includes
#include <cmath>

#include <vector>
#include <list>
#include <map>

#include <sstream>

// Boost includes
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

// OpenGL
#include <GL/GLee.h>


#include <spitfire/util/log.h>

#include <spitfire/storage/filesystem.h>


#include <breathe/game/component.h>

#include <breathe/vehicle/cHelicopter.h>

#include <breathe/physics/physics.h>

#include <breathe/render/cResourceManager.h>
#include <breathe/render/model/cStaticModelLoader.h>


// CH-47A
//
// Engine power 2x(1800 to 2,796 kW)
// Gross weight 15,000 kg
// Max cargo weight 12,700 kg
// 3 crew, up to 55 passengers
// Length 30.1 m
// Fuselage Width 2.2 m
// Height 5.7 m
// Rotor diameter 18.3 m
// Hoist and cargo hook
// Max speed 315 km/h
// Cruising speed 220 km/h
// Range 741 km
// Rate of climb 10.1 m/s




// Models required

// Chinook
// Bell - small helicopter

// Jeep
// Pallet
// Food crate to put on a pallet
// Barrel of water

// Various trees - palm, fir, bushes, african savannah trees
// Various grass - short, long, tall as a person
// Hook for hooking objects

// Bridges
// Antennas ...look up how to spell
// Fences - cyclone, wood, metal sheets
// Little shanty houses
// Oil rig in the middle of the ocean


// Special effects

// Car paint on helicopters
// Clouds of dust from helicopter
// Smoke from fires
// Rain and lightning




// Editor

// Place points - start point, set down point, landing point




// Pickup crates, deliver, drop crates with parachutes, for food aid drop
// Pickup people, rescue from bushfire, burning building, stranded on houses in flooded areas, at sea on air craft carrier, sinking ship, oil rig, capsized yacht
// Build bridge, place spire onto top of tall building
// Drop water onto fire to put it out, in barrels, tanks, sack of water that is filled up at creeks/ocean/lakes
// Fly tourists through a valley for sight seeing tour
// Fly under bridge/through tunnel/buildings
// Have a building with a hole in the middle that can be flown through
// Crop dust a field, deliver a tractor to a farm on a hill
// Rescue plane crash survivors from snow capped mountains
// helicopter snowboarders to the peak of a mountain (Where they just stand there and wave back)



// Do the sky colour settings, load from xml file



class cHelicopterFactory
{
public:
  cHelicopterFactory();

  void CreateChinook(breathe::scenegraph3d::cGroupNodeRef, breathe::render::cTextureFrameBufferObjectRef& pCubeMapTexture);

private:
  size_t nChinooks;
};

cHelicopterFactory::cHelicopterFactory() :
  nChinooks(0)
{
}

void cHelicopterFactory::CreateChinook(breathe::scenegraph3d::cGroupNodeRef pGroupNode, breathe::render::cTextureFrameBufferObjectRef& pCubeMapTexture)
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
      const spitfire::string_t sAlias = TEXT("chinook") + spitfire::string::ToString(nChinooks);
      item.pMaterial = pResourceManager->AddMaterialAsAlias(sCarPaintMaterialFilename, sAlias);
      const size_t n = item.pMaterial->vLayer.size();
      for (size_t i = 0; i < n; i++) {
        if (i == 0) {
          item.pMaterial->vLayer[i]->pTexture = pResourceManager->AddTexture(sDiffuseTextureFilename);
        } else if (i == 1) {
          item.pMaterial->vLayer[i]->pTexture = pResourceManager->AddTexture(sGlossAndMetallicTextureFilename);
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
    void cVehicleFactory::CreateHelicopter(breathe::physics::cWorld* pWorld, breathe::game::cGameObjectCollection& gameobjects, breathe::scenegraph3d::cGroupNodeRef pNode, const spitfire::math::cVec3& position, const spitfire::math::cQuaternion& rotation, cVehicle& vehicle) const
    {
      LOG<<"cVehicleFactory::CreateHelicopter"<<std::endl;

      cHelicopterFactory factory;



      breathe::render::cTextureFrameBufferObjectRef pCubeMapTexture;

      factory.CreateChinook(pNode, pCubeMapTexture);
      ASSERT(pCubeMapTexture != nullptr);




      pNode->SetRelativePosition(position);
      pNode->SetRelativeRotation(rotation);



      vehicle.Create(gameobjects, pNode, pCubeMapTexture);

      breathe::game::cGameObjectRef pVehicle = vehicle.GetGameObject();

      pVehicle->SetPositionRelative(position);




      // Test crate
      breathe::physics::cBoxProperties properties;
      properties.SetMassKg(12000.0f);
      properties.SetPositionAbsolute(position);

      breathe::physics::cBodyRef pBody = physics::GetWorld()->CreateBody(properties);




      breathe::game::cPhysicsComponent* pPhysicsComponent = new breathe::game::cPhysicsComponent(*pVehicle);
      pPhysicsComponent->SetBody(pBody);
      pVehicle->AddComponent(breathe::game::COMPONENT_PHYSICS, pPhysicsComponent);


      breathe::game::cVehicleComponent* pVehicleComponent = new breathe::game::cVehicleComponent(*pVehicle);
      pVehicleComponent->SetHelicopter();
      pVehicle->AddComponent(breathe::game::COMPONENT_VEHICLE, pVehicleComponent);


      {
        // Create the front rotor
        spitfire::string_t sFilename;
        breathe::filesystem::FindResourceFile(TEXT("models/chinook/rotor.obj"), sFilename);

        breathe::render::model::cStaticModelSceneNodeFactory factory;

        std::vector<breathe::render::model::cStaticModelSceneNodeFactoryItem> meshes;
        factory.LoadFromFile(sFilename, meshes);

        const float x = 0.0f;
        const float y = 10.4f;
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
        // Create the rear rotor
        spitfire::string_t sFilename;
        breathe::filesystem::FindResourceFile(TEXT("models/chinook/rotor.obj"), sFilename);

        breathe::render::model::cStaticModelSceneNodeFactory factory;

        std::vector<breathe::render::model::cStaticModelSceneNodeFactoryItem> meshes;
        factory.LoadFromFile(sFilename, meshes);

        const float x = 0.0f;
        const float y = -10.0f;
        const float z = 10.5f;

        breathe::scenegraph3d::cGroupNodeRef pNodeRotor(new breathe::scenegraph3d::cGroupNode);
        pNodeRotor->SetRelativePosition(spitfire::math::cVec3(x, y, z));

        spitfire::math::cQuaternion rotation;
        rotation.SetFromAxisAngle(spitfire::math::v3Up, spitfire::math::DegreesToRadians(180.0f));
        pNodeRotor->SetRelativeRotation(rotation);

        factory.CreateSceneNodeAttachedTo(meshes, pNodeRotor);

        pNode->AttachChild(pNodeRotor);
      }
    }
  }
}







    // TODO: Move this somewhere better

    breathe::render::cVertexBufferObjectRef pVehicleVBO;
    breathe::render::material::cMaterialRef pVehicleVBOMaterial;

    void CreateVehicleVBO()
    {
      std::vector<float> vertices;
      std::vector<float> textureCoordinates;
      std::vector<uint16_t> indices;


      const breathe::math::cVec3 vMin(-1.0f, -1.0f, -1.0f);
      const breathe::math::cVec3 vMax(1.0f, 1.0f, 1.0f);

      // Bottom Square
      textureCoordinates.push_back(0.0f);
      textureCoordinates.push_back(0.0f);
      vertices.push_back(vMin.x);
      vertices.push_back(vMin.y);
      vertices.push_back(vMin.z);

      textureCoordinates.push_back(1.0f);
      textureCoordinates.push_back(0.0f);
      vertices.push_back(vMin.x);
      vertices.push_back(vMax.y);
      vertices.push_back(vMin.z);

      textureCoordinates.push_back(1.0f);
      textureCoordinates.push_back(1.0f);
      vertices.push_back(vMax.x);
      vertices.push_back(vMax.y);
      vertices.push_back(vMin.z);

      textureCoordinates.push_back(0.0f);
      textureCoordinates.push_back(1.0f);
      vertices.push_back(vMax.x);
      vertices.push_back(vMin.y);
      vertices.push_back(vMin.z);

      // Side Squares
      textureCoordinates.push_back(0.0f);
      textureCoordinates.push_back(0.0f);
      vertices.push_back(vMin.x);
      vertices.push_back(vMax.y);
      vertices.push_back(vMin.z);

      textureCoordinates.push_back(1.0f);
      textureCoordinates.push_back(0.0f);
      vertices.push_back(vMin.x);
      vertices.push_back(vMax.y);
      vertices.push_back(vMax.z);

      textureCoordinates.push_back(1.0f);
      textureCoordinates.push_back(1.0f);
      vertices.push_back(vMax.x);
      vertices.push_back(vMax.y);
      vertices.push_back(vMax.z);

      textureCoordinates.push_back(0.0f);
      textureCoordinates.push_back(1.0f);
      vertices.push_back(vMax.x);
      vertices.push_back(vMax.y);
      vertices.push_back(vMin.z);



      textureCoordinates.push_back(0.0f);
      textureCoordinates.push_back(0.0f);
      vertices.push_back(vMax.x);
      vertices.push_back(vMax.y);
      vertices.push_back(vMin.z);

      textureCoordinates.push_back(1.0f);
      textureCoordinates.push_back(0.0f);
      vertices.push_back(vMax.x);
      vertices.push_back(vMax.y);
      vertices.push_back(vMax.z);

      textureCoordinates.push_back(1.0f);
      textureCoordinates.push_back(1.0f);
      vertices.push_back(vMax.x);
      vertices.push_back(vMin.y);
      vertices.push_back(vMax.z);

      textureCoordinates.push_back(0.0f);
      textureCoordinates.push_back(1.0f);
      vertices.push_back(vMax.x);
      vertices.push_back(vMin.y);
      vertices.push_back(vMin.z);


      textureCoordinates.push_back(0.0f);
      textureCoordinates.push_back(0.0f);
      vertices.push_back(vMax.x);
      vertices.push_back(vMin.y);
      vertices.push_back(vMin.z);

      textureCoordinates.push_back(1.0f);
      textureCoordinates.push_back(0.0f);
      vertices.push_back(vMax.x);
      vertices.push_back(vMin.y);
      vertices.push_back(vMax.z);

      textureCoordinates.push_back(1.0f);
      textureCoordinates.push_back(1.0f);
      vertices.push_back(vMin.x);
      vertices.push_back(vMin.y);
      vertices.push_back(vMax.z);

      textureCoordinates.push_back(0.0f);
      textureCoordinates.push_back(1.0f);
      vertices.push_back(vMin.x);
      vertices.push_back(vMin.y);
      vertices.push_back(vMin.z);


      textureCoordinates.push_back(0.0f);
      textureCoordinates.push_back(0.0f);
      vertices.push_back(vMin.x);
      vertices.push_back(vMin.y);
      vertices.push_back(vMax.z);

      textureCoordinates.push_back(1.0f);
      textureCoordinates.push_back(0.0f);
      vertices.push_back(vMin.x);
      vertices.push_back(vMax.y);
      vertices.push_back(vMax.z);

      textureCoordinates.push_back(1.0f);
      textureCoordinates.push_back(1.0f);
      vertices.push_back(vMin.x);
      vertices.push_back(vMax.y);
      vertices.push_back(vMin.z);

      textureCoordinates.push_back(0.0f);
      textureCoordinates.push_back(1.0f);
      vertices.push_back(vMin.x);
      vertices.push_back(vMin.y);
      vertices.push_back(vMin.z);

      // Upper Square
      textureCoordinates.push_back(0.0f);
      textureCoordinates.push_back(0.0f);
      vertices.push_back(vMin.x);
      vertices.push_back(vMin.y);
      vertices.push_back(vMax.z);

      textureCoordinates.push_back(1.0f);
      textureCoordinates.push_back(0.0f);
      vertices.push_back(vMax.x);
      vertices.push_back(vMin.y);
      vertices.push_back(vMax.z);

      textureCoordinates.push_back(1.0f);
      textureCoordinates.push_back(1.0f);
      vertices.push_back(vMax.x);
      vertices.push_back(vMax.y);
      vertices.push_back(vMax.z);

      textureCoordinates.push_back(0.0f);
      textureCoordinates.push_back(1.0f);
      vertices.push_back(vMin.x);
      vertices.push_back(vMax.y);
      vertices.push_back(vMax.z);

      pVehicleVBO.reset(new breathe::render::cVertexBufferObject);
      pVehicleVBO->SetVertices(vertices);
      pVehicleVBO->SetTextureCoordinates(textureCoordinates);
      //pVehicleVBO->SetIndices(indices);
      pVehicleVBO->Compile();


      const spitfire::string_t sFilename(TEXT("materials/crate.mat"));
      pVehicleVBOMaterial = pResourceManager->GetMaterial(sFilename);
      ASSERT(pVehicleVBOMaterial != nullptr);
    }


    breathe::game::cGameObjectRef CreateVehicle(breathe::physics::cWorld* pWorld, breathe::scenegraph3d::cSceneGraph& scenegraph, breathe::game::cGameObjectCollection& gameobjects, const breathe::math::cVec3& position)
    {
      LOG<<"CreateVehicle"<<std::endl;

      breathe::game::cGameObjectRef pVehicle(new breathe::game::cGameObject);

      //breathe::string_t sFilename;
      //breathe::filesystem::FindResourceFile(TEXT("vehicle/toyota_gt_one/mesh.3ds"), sFilename);
      //pVehicle->pModel = pRender->GetModel(sFilename);
      //breathe::filesystem::FindResourceFile(TEXT("vehicle/wheel00/mesh.3ds"), sFilename);
      //pVehicle->vWheel[0]->pModel = pVehicle->vWheel[1]->pModel = pVehicle->vWheel[2]->pModel = pVehicle->vWheel[3]->pModel = pRender->GetModel(sFilename);

      // Test crate
      breathe::physics::cBoxProperties properties;
      properties.SetMassKg(1000.0f);
      properties.SetPositionAbsolute(position);

      breathe::physics::cBodyRef pBody = breathe::physics::GetWorld()->CreateBody(properties);


      breathe::scenegraph3d::cModelNodeRef pNode(new breathe::scenegraph3d::cModelNode);
      pNode->SetRelativePosition(position);
      pNode->SetRelativeRotation(spitfire::math::cQuaternion());

      breathe::scenegraph3d::cStateSet& stateset = pNode->GetStateSet();
      stateset.SetStateFromMaterial(pVehicleVBOMaterial);

      breathe::scenegraph_common::cStateVertexBufferObject& vertexBufferObject = stateset.GetVertexBufferObject();
      vertexBufferObject.SetVertexBufferObject(pVehicleVBO);
      vertexBufferObject.SetEnabled(true);
      vertexBufferObject.SetHasValidValue(true);

      // Attach to the root node
      breathe::scenegraph3d::cSceneNodeRef pRoot = scenegraph.GetRoot();
      breathe::scenegraph3d::cGroupNode* pRootAsGroupNode = static_cast<breathe::scenegraph3d::cGroupNode*>(pRoot.get());
      pRootAsGroupNode->AttachChild(pNode);




      breathe::game::cPhysicsComponent* pPhysicsComponent = new breathe::game::cPhysicsComponent(*pVehicle);
      pPhysicsComponent->SetBody(pBody);
      pVehicle->AddComponent(breathe::game::COMPONENT_PHYSICS, pPhysicsComponent);

      breathe::game::cRenderComponent* pRenderComponent = new breathe::game::cRenderComponent(*pVehicle);
      pRenderComponent->SetSceneNode(pNode);
      pVehicle->AddComponent(breathe::game::COMPONENT_RENDERABLE, pRenderComponent);

      breathe::game::cVehicleComponent* pVehicleComponent = new breathe::game::cVehicleComponent(*pVehicle);
      pVehicleComponent->SetHelicopter();
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
    }


namespace breathe
{
  namespace game
  {
    class cEngineAudio
    {
    public:
      ~cEngineAudio();

      //void AddSample(audio::cBufferRef pBuffer, float_t fNaturalRPM);
      void SetSource(audio::cSourceRef _pSource, float_t _fNaturalRPM) { pSource = _pSource; fNaturalRPM = _fNaturalRPM; }

      void SetRPM(float_t _fRPM) { fRPM = _fRPM; }
      void SetAccelerator0To1(float_t _fAccelerator0To1) { fAccelerator0To1 = _fAccelerator0To1; }

      void Update(sampletime_t currentTime);

    private:
      /*class cEngineAudioSample;
      cEngineAudioSample GetClosestMatchToRPM();

      class cEngineAudioSample
      {
      public:
        audio::cBufferRef pBuffer;
        float_t fNaturalRPM;

      private:
        void Assign(const cEngineAudioSample& rhs);
      };

      std::vector<cEngineAudioSample*> samples;*/

      float_t fNaturalRPM;
      audio::cSourceRef pSource;

      float_t fRPM;
      float_t fAccelerator0To1;
    };


    cEngineAudio::~cEngineAudio()
    {
      //.. delete all samples
      //for (size_t i = 0; i < n; i++) {
      //  ...
      //}
    }

    /*void cEngineAudio::AddSample(audio::cBufferRef pBuffer, float_t fNaturalRPM)
    {
      ...
    }

    cEngineAudio::cEngineAudioSample cEngineAudio::GetClosestMatchToRPM()
    {
      cEngineAudioSample sampleClosest;

      const size_t n = samples.size();

      // Set our closest to the first sample
      ASSERT(n != 0);
      sampleClosest = *samples[0];

      float_t fDistanceToClosest = fabs(fRPM - sampleClosest.fNaturalRPM);

      for (size_t i = 1; i < n; i++) {
        const float_t fDistanceToCurrent = fabs(fRPM - samples[i]->fNaturalRPM);
        if (fDistanceToCurrent < fDistanceToClosest) {
          sampleClosest = *samples[i];
          fDistanceToClosest = fDistanceToCurrent;
        }
      }

      return sampleClosest;
    }*/

    void cEngineAudio::Update(sampletime_t currentTime)
    {
      // We want this in the range of 0.5f .. 1.5f
      pSource->SetPitch(1.0f + sinf(0.001f * currentTime));

      //cEngineAudioSample sample = GetClosestMatchToRPM();
      //
      //ASSERT(sample.fNaturalRPM != 0.0f);
      //pSource->SetPitch(fRPM / sample.fNaturalRPM);
    }







    cVehicleHelicopter::cVehicleHelicopter(cGameObject& _object) :
      cVehicleBase(_object, TYPE::HELICOPTER)
    {
    }

    void cVehicleHelicopter::_Init()
    {
      LOG<<"cVehicleHelicopter::_Init"<<std::endl;

      spitfire::string_t sFilename;
      spitfire::filesystem::FindResourceFile(TEXT("audio/heli_out.wav"), sFilename);


      breathe::audio::cBufferRef pBuffer = breathe::audio::GetManager()->CreateBuffer(sFilename);
      if (pBuffer == nullptr) {
        SCREEN<<"cVehicleHelicopter::_Init pBuffer=NULL, returning"<<std::endl;
        return;
      }

      pSourceEngine = breathe::audio::GetManager()->CreateSourceAttachedToObject(pBuffer);
      if (pSourceEngine == nullptr) {
        SCREEN<<"cVehicleHelicopter::_Init pSourceEngine=NULL, returning"<<std::endl;
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

      LOG<<"cVehicleHelicopter::_Init Playing source"<<std::endl;
      pSourceEngine->Play();
    }

    void cVehicleHelicopter::_Update(sampletime_t currentTime)
    {
      cPhysicsComponent* pPhysicsComponent = object.GetComponentIfEnabled<cPhysicsComponent>(COMPONENT_PHYSICS);
      if (pPhysicsComponent == nullptr) return;

      physics::cBodyRef pBody = pPhysicsComponent->GetBody();
      if (pBody == nullptr) return;


      if (fInputAccelerator0To1 > 0.01f) {
        breathe::math::cVec3 forceKg(fInputAccelerator0To1 * pBody->GetMassKg() * 300.0f * breathe::math::v3Up);
        pBody->AddForceRelativeToObjectKg(forceKg);
      }
      if (fInputBrake0To1 > 0.01f) {
        // This is more of a brake than an actual go down method
        fInputAccelerator0To1 = 0.0f;
      }


      const float_t fPitchRollFactor = 0.0005f;
      if (fInputForward0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputForward0To1 * pBody->GetMassKg() * -fPitchRollFactor * breathe::math::v3Right);
        pBody->AddTorqueRelativeToWorldNm(torqueNm);
      }
      if (fInputBackward0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputBackward0To1 * pBody->GetMassKg() * fPitchRollFactor * breathe::math::v3Right);
        pBody->AddTorqueRelativeToWorldNm(torqueNm);
      }
      if (fInputLeft0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputLeft0To1 * pBody->GetMassKg() * fPitchRollFactor * breathe::math::v3Front);
        pBody->AddTorqueRelativeToWorldNm(torqueNm);
      }
      if (fInputRight0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputRight0To1 * pBody->GetMassKg() * -fPitchRollFactor * breathe::math::v3Front);
        pBody->AddTorqueRelativeToWorldNm(torqueNm);
      }


      if (fInputYawLeft0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputYawLeft0To1 * pBody->GetMassKg() * 2.0f * breathe::math::v3Up);
        pBody->AddTorqueRelativeToWorldNm(torqueNm);
      }
      if (fInputYawRight0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputYawRight0To1 * pBody->GetMassKg() * -2.0f * breathe::math::v3Up);
        pBody->AddTorqueRelativeToWorldNm(torqueNm);
      }



      // Set values back to defaults for next time
      fInputAccelerator0To1 = 0.0f;
      fInputBrake0To1 = 0.0f;
      fInputForward0To1 = 0.0f;
      fInputBackward0To1 = 0.0f;
      fInputLeft0To1 = 0.0f;
      fInputRight0To1 = 0.0f;
      fInputYawLeft0To1 = 0.0f;
      fInputYawRight0To1 = 0.0f;
    }
  }
}
