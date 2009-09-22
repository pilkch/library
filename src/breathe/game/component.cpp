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

//#include <breathe/vehicle/cCar.h>
//#include <breathe/vehicle/cPlane.h>
#include <breathe/vehicle/cHelicopter.h>

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
