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


#include <spitfire/util/log.h>

#include <breathe/game/component.h>


#if defined(BUILD_PHYSICS_BULLET) || defined(BUILD_PHYSICS_ODE)
#include <breathe/vehicle/cCar.h>
//#include <breathe/vehicle/cPlane.h>
#include <breathe/vehicle/cHelicopter.h>
#endif

namespace breathe
{
  namespace game
  {
    COMPONENT cGameObject::GetComponentFromIndex(size_t index)
    {
      switch (index) {
        case 0: return COMPONENT::USER_HIGH_PRIORITY_0;
        case 1: return COMPONENT::USER_HIGH_PRIORITY_1;
        case 2: return COMPONENT::USER_HIGH_PRIORITY_2;
        case 3: return COMPONENT::PHYSICS;
        case 4: return COMPONENT::FLAMABLE;
        case 5: return COMPONENT::EXPLODABLE;
        case 6: return COMPONENT::BOUYANT;
        case 7: return COMPONENT::ITEM;
        case 8: return COMPONENT::ITEM_CONTAINER;
        case 9: return COMPONENT::VEHICLE;
        case 10: return COMPONENT::AUDIOSOURCE;
        case 11: return COMPONENT::TEAM;
        case 12: return COMPONENT::ANIMATION;
        case 13: return COMPONENT::USER_NORMAL_PRIORITY_0;
        case 14: return COMPONENT::USER_NORMAL_PRIORITY_1;
        case 15: return COMPONENT::USER_NORMAL_PRIORITY_2;
        case 16: return COMPONENT::USER_NORMAL_PRIORITY_3;
        case 17: return COMPONENT::USER_NORMAL_PRIORITY_4;
        case 18: return COMPONENT::USER_NORMAL_PRIORITY_5;
        case 19: return COMPONENT::USER_NORMAL_PRIORITY_6;
        case 20: return COMPONENT::USER_NORMAL_PRIORITY_7;
        case 21: return COMPONENT::USER_NORMAL_PRIORITY_8;
        case 22: return COMPONENT::USER_NORMAL_PRIORITY_9;
        case 23: return COMPONENT::USER_NORMAL_PRIORITY_10;
        case 24: return COMPONENT::USER_NORMAL_PRIORITY_11;
        case 25: return COMPONENT::USER_NORMAL_PRIORITY_12;
        case 26: return COMPONENT::USER_NORMAL_PRIORITY_13;
        case 27: return COMPONENT::USER_NORMAL_PRIORITY_14;
        case 28: return COMPONENT::USER_NORMAL_PRIORITY_15;
        case 29: return COMPONENT::USER_NORMAL_PRIORITY_16;
        case 30: return COMPONENT::USER_NORMAL_PRIORITY_17;
        case 31: return COMPONENT::USER_NORMAL_PRIORITY_18;
        case 32: return COMPONENT::RENDERABLE;
        case 33: return COMPONENT::USER_LOW_PRIORITY_0;
        case 34: return COMPONENT::USER_LOW_PRIORITY_1;
        case 35: return COMPONENT::USER_LOW_PRIORITY_2;
      };

      ASSERT(false);
      return COMPONENT::USER_HIGH_PRIORITY_0;
    }

    size_t cGameObject::GetIndexFromComponent(COMPONENT component)
    {
      switch (component) {
        case COMPONENT::USER_HIGH_PRIORITY_0: return 0;
        case COMPONENT::USER_HIGH_PRIORITY_1: return 1;
        case COMPONENT::USER_HIGH_PRIORITY_2: return 2;
        case COMPONENT::PHYSICS: return 3;
        case COMPONENT::FLAMABLE: return 4;
        case COMPONENT::EXPLODABLE: return 5;
        case COMPONENT::BOUYANT: return 6;
        case COMPONENT::ITEM: return 7;
        case COMPONENT::ITEM_CONTAINER: return 8;
        case COMPONENT::VEHICLE: return 9;
        case COMPONENT::AUDIOSOURCE: return 10;
        case COMPONENT::TEAM: return 11;
        case COMPONENT::ANIMATION: return 12;
        case COMPONENT::USER_NORMAL_PRIORITY_0: return 13;
        case COMPONENT::USER_NORMAL_PRIORITY_1: return 14;
        case COMPONENT::USER_NORMAL_PRIORITY_2: return 15;
        case COMPONENT::USER_NORMAL_PRIORITY_3: return 16;
        case COMPONENT::USER_NORMAL_PRIORITY_4: return 17;
        case COMPONENT::USER_NORMAL_PRIORITY_5: return 18;
        case COMPONENT::USER_NORMAL_PRIORITY_6: return 19;
        case COMPONENT::USER_NORMAL_PRIORITY_7: return 20;
        case COMPONENT::USER_NORMAL_PRIORITY_8: return 21;
        case COMPONENT::USER_NORMAL_PRIORITY_9: return 22;
        case COMPONENT::USER_NORMAL_PRIORITY_10: return 23;
        case COMPONENT::USER_NORMAL_PRIORITY_11: return 24;
        case COMPONENT::USER_NORMAL_PRIORITY_12: return 25;
        case COMPONENT::USER_NORMAL_PRIORITY_13: return 26;
        case COMPONENT::USER_NORMAL_PRIORITY_14: return 27;
        case COMPONENT::USER_NORMAL_PRIORITY_15: return 28;
        case COMPONENT::USER_NORMAL_PRIORITY_16: return 29;
        case COMPONENT::USER_NORMAL_PRIORITY_17: return 30;
        case COMPONENT::USER_NORMAL_PRIORITY_18: return 31;
        case COMPONENT::RENDERABLE: return 32;
        case COMPONENT::USER_LOW_PRIORITY_0: return 33;
        case COMPONENT::USER_LOW_PRIORITY_1: return 34;
        case COMPONENT::USER_LOW_PRIORITY_2: return 35;
      };

      ASSERT(false);
      return 0;
    }

    //cGameObject::cGameObject()
    //{
    //}

    void cGameObject::AddComponent(COMPONENT componentType, cComponent* pComponent)
    {
      ASSERT(pComponent != nullptr);

      const size_t index = GetIndexFromComponent(componentType);

      // Delete the old component if there is one
      std::map<size_t, cComponent*>::iterator iter = components.find(index);
      if (iter != components.end()) {
        cComponent* pOldComponent = iter->second;
        SAFE_DELETE(pOldComponent);
      }

      // Add our new component
      components[index] = pComponent;
    }

    void cGameObject::RemoveAllComponents()
    {
      std::map<size_t, cComponent*>::iterator iter = components.begin();
      const std::map<size_t, cComponent*>::iterator iterEnd = components.end();
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

    void cGameObject::Update(spitfire::durationms_t currentTime)
    {
      // These components are already sorted in order of priority
      std::map<size_t, cComponent*>::iterator iter = components.begin();
      const std::map<size_t, cComponent*>::iterator iterEnd = components.end();
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

    void cGameObjectCollection::Update(spitfire::durationms_t currentTime)
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

    void cAudioSourceComponent::_Update(spitfire::durationms_t currentTime)
    {
      const size_t n = sources.size();
      for (size_t i = 0; i < n; i++) {
        sources[i]->SetPosition(object.GetPositionAbsolute());
      }
    }


#if defined(BUILD_PHYSICS_BULLET) || defined(BUILD_PHYSICS_ODE)
    void cPhysicsComponent::_Update(spitfire::durationms_t currentTime)
    {
      ASSERT(pBody != nullptr);
      object.SetPositionRelative(pBody->GetPositionAbsolute());
      object.SetRotationRelative(pBody->GetRotationAbsolute());
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

    void cVehicleComponent::SetCar(physics::cCarRef pCar, const std::vector<scenegraph3d::cGroupNodeRef>& wheels)
    {
      SAFE_DELETE(pVehicle);

      pVehicle = new cVehicleCar(object, pCar, wheels);

      pVehicle->Init();
    }

    void cVehicleComponent::SetPlane()
    {
      SAFE_DELETE(pVehicle);

      ASSERT(false);
      //pVehicle = new cVehiclePlane(object);

      //pVehicle->Init();
    }

    void cVehicleComponent::SetHelicopter()
    {
      SAFE_DELETE(pVehicle);

      pVehicle = new cVehicleHelicopter(object);

      pVehicle->Init();
    }
/*
...
class cLevelOrWhatever
{
public:
   float_t GetAmbientAirPressureKPA() const;
   float_t GetAmbientAirTemperatureDegreesCelcius() const;

private:
   math::cCurve timeOfDayToTemperatureDegreesCelciusCurve;
};

void cLevelOrWhatever::Load()
{
  // 25C at midday, 15C at 6am and 6pm, 10C overnight
  timeOfDayToTemperatureDegreesCelciusCurve.AddPoint(0.0f, 10.0f);
  timeOfDayToTemperatureDegreesCelciusCurve.AddPoint(0.25f, 15.0f);
  timeOfDayToTemperatureDegreesCelciusCurve.AddPoint(0.5f, 25.0f);
  timeOfDayToTemperatureDegreesCelciusCurve.AddPoint(0.25f, 15.0f);
  timeOfDayToTemperatureDegreesCelciusCurve.AddPoint(1.0f, 10.0f);
}*/

    void cVehicleComponent::_Update(durationms_t currentTime)
    {
      ASSERT(pVehicle != nullptr);

      //pVehicle->SetAmbientAirPressureKPA(cLevelOrWhatever.GetAmbientAirPressureKPA());
      //pVehicle->SetAmbientAirTemperatureDegreesCelcius(cLevelOrWhatever.GetAmbientAirTemperatureDegreesCelcius());

      pVehicle->Update(currentTime);
    }
#endif






    void cAnimationComponent::_Update(spitfire::durationms_t currentTime)
    {
      //ASSERT(pNode != nullptr);
      //pNode->SetRelativePosition(object.GetPositionRelative());
      //pNode->SetRelativeRotation(object.GetRotationRelative());
    }

    void cRenderComponent::_Update(spitfire::durationms_t currentTime)
    {
      ASSERT(pNode != nullptr);
      pNode->SetRelativePosition(object.GetPositionRelative());
      pNode->SetRelativeRotation(object.GetRotationRelative());
    }
  }
}
