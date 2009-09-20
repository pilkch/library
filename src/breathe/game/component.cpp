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

#include <breathe/game/component.h>

namespace breathe
{
  namespace game
  {
    //cGameObject::cGameObject()
    //{
    //}

    void cGameObject::AddComponent(COMPONENT c, cComponent* pComponent)
    {
      ASSERT(pComponent != nullptr);

      // Delete the old component if there is one
      std::map<COMPONENT, cComponent*>::iterator iter = components.find(c);
      if (iter != components.end()) {
        cComponent* pOldComponent = iter->second;
        SAFE_DELETE(pOldComponent);
      }

      // Add our new component
      components[c] = pComponent;
    }

    void cGameObject::RemoveAllComponents()
    {
      std::map<COMPONENT, cComponent*>::iterator iter = components.begin();
      const std::map<COMPONENT, cComponent*>::iterator iterEnd = components.end();
      while (iter != iterEnd) {
        cComponent* pComponent = iter->second;
        SAFE_DELETE(pComponent);

        iter++;
      }
    }

    bool cGameObject::IsComponentPresentAndEnabled(COMPONENT componentType) const
    {
      return (GetComponentIfEnabled<cComponent>(componentType) != nullptr);
    }

    bool cGameObject::IsComponentPresentAndEnabledOrDisabled(COMPONENT componentType) const
    {
      return (GetComponentIfEnabledOrDisabled<cComponent>(componentType) != nullptr);
    }

    void cGameObject::Update(spitfire::sampletime_t currentTime)
    {
      // These components are already sorted in order of priority
      std::map<COMPONENT, cComponent*>::iterator iter = components.begin();
      const std::map<COMPONENT, cComponent*>::iterator iterEnd = components.end();
      while (iter != iterEnd) {
        cComponent* pComponent = iter->second;
        ASSERT(pComponent != nullptr);
        pComponent->Update(currentTime);

        iter++;
      }
    }


    // *** cGameObjectCollection

    void cGameObjectCollection::Add(cGameObjectRef pGameObject)
    {
      ASSERT(pGameObject != nullptr);
      gameobjects.push_back(pGameObject);
    }

    void cGameObjectCollection::Remove(cGameObjectRef pGameObject)
    {
      ASSERT(pGameObject != nullptr);
      gameobjects.remove(pGameObject);
    }

    void cGameObjectCollection::Update(spitfire::sampletime_t currentTime)
    {
      std::list<cGameObjectRef>::iterator iter = gameobjects.begin();
      const std::list<cGameObjectRef>::iterator iterEnd = gameobjects.end();
      while (iter != iterEnd) {
        ASSERT((*iter) != nullptr);
        (*iter)->Update(currentTime);

        iter++;
      }
    }


    // *** Common components

    void cAudioSourceComponent::AddSource(audio::cSourceRef pSource)
    {
      sources.push_back(pSource);
    }

    void cAudioSourceComponent::RemoveSource(audio::cSourceRef pSource)
    {
      std::vector<audio::cSourceRef>::iterator iter = sources.begin();
      const std::vector<audio::cSourceRef>::iterator iterEnd = sources.end();
      while (iter != iterEnd) {
        if (*iter == pSource) sources.erase(iter);

        iter++;
      }
    }

    void cAudioSourceComponent::_Update(spitfire::sampletime_t currentTime)
    {
      const size_t n = sources.size();
      for (size_t i = 0; i < n; i++) {
        sources[i]->SetPosition(object.GetPositionAbsolute());
      }
    }


    void cPhysicsComponent::_Update(spitfire::sampletime_t currentTime)
    {
      ASSERT(pPhysicsObject != nullptr);
      object.SetPositionRelative(pPhysicsObject->GetPositionAbsolute());
      object.SetRotationRelative(pPhysicsObject->GetRotationAbsolute());
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
      pVehicleVBOMaterial = pRender->GetMaterial(sFilename);
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
    cVehicleBase::cVehicleBase(cGameObject& _object, TYPE _type) :
      object(_object),

      fInputAccelerator0To1(0.0f),
      fInputBrake0To1(0.0f),
      fInputForward0To1(0.0f),
      fInputBackward0To1(0.0f),
      fInputLeft0To1(0.0f),
      fInputRight0To1(0.0f),
      fInputHandBrake0To1(0.0f),
      fInputClutch0To1(0.0f),
      bIsInputChangeGearUp(false),
      bIsInputChangeGearDown(false),
      fInputYawLeft0To1(0.0f),
      fInputYawRight0To1(0.0f),

      type(_type)
    {
    }




    class cVehicleHelicopter : public cVehicleBase
    {
    public:
      cVehicleHelicopter(cGameObject& object);

    private:
      virtual void _Update(sampletime_t currentTime);
    };

    cVehicleHelicopter::cVehicleHelicopter(cGameObject& _object) :
      cVehicleBase(_object, TYPE::HELICOPTER)
    {
    }

    void cVehicleHelicopter::_Update(sampletime_t currentTime)
    {
      cPhysicsComponent* pPhysicsComponent = object.GetComponentIfEnabled<cPhysicsComponent>(COMPONENT_PHYSICS);
      if (pPhysicsComponent == nullptr) return;

      physics::cPhysicsObjectRef pPhysicsObject = pPhysicsComponent->GetPhysicsObject();
      if (pPhysicsObject == nullptr) return;


      if (fInputAccelerator0To1 > 0.01f) {
        breathe::math::cVec3 forceKg(fInputAccelerator0To1 * pPhysicsObject->GetWeightKg() * 300.0f * breathe::math::v3Up);
        pPhysicsObject->AddForceRelativeToObjectKg(forceKg);
      }
      if (fInputBrake0To1 > 0.01f) {
        // This is more of a brake than an actual go down method
        fInputAccelerator0To1 = 0.0f;
      }


      const float_t fPitchRollFactor = 0.001f;
      if (fInputForward0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputForward0To1 * pPhysicsObject->GetWeightKg() * fPitchRollFactor * breathe::math::v3Right);
        pPhysicsObject->AddTorqueRelativeToWorldNm(torqueNm);
      }
      if (fInputBackward0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputBackward0To1 * pPhysicsObject->GetWeightKg() * -fPitchRollFactor * breathe::math::v3Right);
        pPhysicsObject->AddTorqueRelativeToWorldNm(torqueNm);
      }
      if (fInputLeft0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputLeft0To1 * pPhysicsObject->GetWeightKg() * fPitchRollFactor * breathe::math::v3Front);
        pPhysicsObject->AddTorqueRelativeToWorldNm(torqueNm);
      }
      if (fInputRight0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputRight0To1 * pPhysicsObject->GetWeightKg() * -fPitchRollFactor * breathe::math::v3Front);
        pPhysicsObject->AddTorqueRelativeToWorldNm(torqueNm);
      }


      if (fInputYawLeft0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputYawLeft0To1 * pPhysicsObject->GetWeightKg() * 2.0f * breathe::math::v3Up);
        pPhysicsObject->AddTorqueRelativeToWorldNm(torqueNm);
      }
      if (fInputYawRight0To1 > 0.01f) {
        breathe::math::cVec3 torqueNm(fInputYawRight0To1 * pPhysicsObject->GetWeightKg() * -2.0f * breathe::math::v3Up);
        pPhysicsObject->AddTorqueRelativeToWorldNm(torqueNm);
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


    cVehicleComponent::cVehicleComponent(cGameObject& _object) :
      cComponent(_object),
      pVehicle(nullptr)
    {
    }

    cVehicleComponent::~cVehicleComponent()
    {
      SAFE_DELETE(pVehicle);
    }

    void cVehicleComponent::SetCar()
    {
      SAFE_DELETE(pVehicle);

      ASSERT(false);
      //pVehicle = new cVehicleCar;
    }

    void cVehicleComponent::SetPlane()
    {
      SAFE_DELETE(pVehicle);

      ASSERT(false);
      //pVehicle = new cVehiclePlane;
    }

    void cVehicleComponent::SetHelicopter()
    {
      SAFE_DELETE(pVehicle);

      pVehicle = new cVehicleHelicopter(object);
    }

    void cVehicleComponent::_Update(sampletime_t currentTime)
    {
      ASSERT(pVehicle != nullptr);

      pVehicle->Update(currentTime);
    }






    void cAnimationComponent::_Update(spitfire::sampletime_t currentTime)
    {
      //ASSERT(pNode != nullptr);
      //pNode->SetRelativePosition(object.GetPositionRelative());
      //pNode->SetRelativeRotation(object.GetRotationRelative());
    }

    void cRenderComponent::_Update(spitfire::sampletime_t currentTime)
    {
      ASSERT(pNode != nullptr);
      pNode->SetRelativePosition(object.GetPositionRelative());
      pNode->SetRelativeRotation(object.GetRotationRelative());
    }
  }
}
