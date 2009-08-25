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

    void cPhysicsComponent::_Update(spitfire::sampletime_t currentTime)
    {
      ASSERT(pPhysicsObject != nullptr);
      object.SetPositionRelative(pPhysicsObject->GetPositionAbsolute());
      object.SetRotationRelative(pPhysicsObject->GetRotationAbsolute());
    }



    cVehicleComponent::cVehicleComponent(cGameObject& _object, VEHICLE_TYPE _type) :
      cComponent(_object),
      type(_type),
      fInputUp(0.0f),
      fInputDown(0.0f),
      fInputLeft(0.0f),
      fInputRight(0.0f)
    {
    }

    void cVehicleComponent::_Update(spitfire::sampletime_t currentTime)
    {
      cPhysicsComponent* pPhysicsComponent = object.GetComponentIfEnabled<cPhysicsComponent>(COMPONENT_PHYSICS);
      if (pPhysicsComponent == nullptr) return;

      physics::cPhysicsObjectRef pPhysicsObject = pPhysicsComponent->GetPhysicsObject();
      if (pPhysicsObject == nullptr) return;

      switch (type) {
        case VEHICLE_TYPE_CAR: {
          break;
        }
        case VEHICLE_TYPE_PLANE: {
          break;
        }
        case VEHICLE_TYPE_HELICOPTER: {
          if (fInputDown > 0.01f) {
            // This is more of a break than an actual go down method
            fInputUp = 0.0f;
          }
          if (fInputUp > 0.01f) {
            breathe::math::cVec3 forceKg(fInputUp * pPhysicsObject->GetWeightKg() * 300.0f * breathe::math::v3Up);
            pPhysicsObject->AddForceRelativeToObjectKg(forceKg);
          }

          if (fInputLeft > 0.01f) {
            breathe::math::cVec3 torqueNm(fInputLeft * pPhysicsObject->GetWeightKg() * 2.0f * breathe::math::v3Up);
            pPhysicsObject->AddTorqueRelativeToWorldNm(torqueNm);
          }
          if (fInputRight > 0.01f) {
            breathe::math::cVec3 torqueNm(fInputRight * pPhysicsObject->GetWeightKg() * -2.0f * breathe::math::v3Up);
            pPhysicsObject->AddTorqueRelativeToWorldNm(torqueNm);
          }

          break;
        }
      };
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
