#ifndef GAMECOMPONENT_H
#define GAMECOMPONENT_H

#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>

#include <spitfire/math/cVec3.h>
#include <spitfire/math/cQuaternion.h>

#include <breathe/breathe.h>

#include <breathe/audio/audio.h>

#include <breathe/game/scenegraph.h>

#include <breathe/physics/physics.h>

namespace breathe
{
  namespace game
  {
    enum class COMPONENT {
      USER_HIGH_PRIORITY_0,
      USER_HIGH_PRIORITY_1,
      USER_HIGH_PRIORITY_2,
      PHYSICS, // This component should be close to the start of updating
      FLAMABLE,
      EXPLODABLE,
      BOUYANT,
      ITEM, // An pickup that can be added to an item container, for example a weapon, ammo, a key etc.
      ITEM_CONTAINER, // Contains items, an inventory
      VEHICLE, // Game object is a vehicle of some description, note: this infers drivable, a prop vehicle that no one ever drives will NOT have this
      AUDIOSOURCE,
      TEAM,
      ANIMATION, // This is a wrapper for md3 animation, it requires a COMPONENT::RENDERABLE and cRenderableComponent that is pointing to a cAnimationNode
      USER_NORMAL_PRIORITY_0,
      USER_NORMAL_PRIORITY_1,
      USER_NORMAL_PRIORITY_2,
      USER_NORMAL_PRIORITY_3,
      USER_NORMAL_PRIORITY_4,
      USER_NORMAL_PRIORITY_5,
      USER_NORMAL_PRIORITY_6,
      USER_NORMAL_PRIORITY_7,
      USER_NORMAL_PRIORITY_8,
      USER_NORMAL_PRIORITY_9,
      USER_NORMAL_PRIORITY_10,
      USER_NORMAL_PRIORITY_11,
      USER_NORMAL_PRIORITY_12,
      USER_NORMAL_PRIORITY_13,
      USER_NORMAL_PRIORITY_14,
      USER_NORMAL_PRIORITY_15,
      USER_NORMAL_PRIORITY_16,
      USER_NORMAL_PRIORITY_17,
      USER_NORMAL_PRIORITY_18,
      RENDERABLE, // This one wants to be close to the end, if not absolute last, when updating
      USER_LOW_PRIORITY_0,
      USER_LOW_PRIORITY_1,
      USER_LOW_PRIORITY_2

      // input ie. keyboard, ai?
      // particle system, mesh data, or is that getting too much into a scenegraph
    };


    class cGameObject;
    typedef std::shared_ptr<cGameObject> cGameObjectRef;

    class cComponent
    {
    public:
      explicit cComponent(cGameObject& _object) : object(_object), bIsEnabled(true) {}
      virtual ~cComponent() {}

      bool IsEnabled() const { return bIsEnabled; }

      void Update(spitfire::durationms_t currentTime) { _Update(currentTime); }

    protected:
      cGameObject& object;

    private:
      virtual void _Update(spitfire::durationms_t currentTime) {}

      bool bIsEnabled;
    };

    class cGameObject
    {
    public:
      //cGameObject();
      ~cGameObject() { RemoveAllComponents(); }

      // After adding, the component is then owned by the game object, do not keep a pointer to this component
      // as it may be deleted at any time.  AddComponent will delete an existing component and replace it with this one.
      void AddComponent(COMPONENT componentType, cComponent* pComponent);
      void RemoveAllComponents();

      template <typename T>
      T* GetComponentIfEnabled(COMPONENT componentType) const;
      template <typename T>
      T* GetComponentIfEnabledOrDisabled(COMPONENT componentType) const;

      bool IsComponentPresentAndEnabled(COMPONENT componentType) const;
      bool IsComponentPresentAndEnabledOrDisabled(COMPONENT componentType) const;

      void SetComponentEnabled(COMPONENT componentType);
      void SetComponentDisabled(COMPONENT componentType);


      void Update(spitfire::durationms_t currentTime);

      const string_t& GetName() const { return sName; }
      void SetName(const string_t& _sName) { sName = _sName; }

      spitfire::math::cVec3 GetPositionAbsolute() const { return positionRelative; }
      spitfire::math::cQuaternion GetRotationAbsolute() const { return rotationRelative; }

      const spitfire::math::cVec3& GetPositionRelative() const { return positionRelative; }
      void SetPositionRelative(const spitfire::math::cVec3& _positionRelative) { positionRelative = _positionRelative; }

      const spitfire::math::cQuaternion& GetRotationRelative() const { return rotationRelative; }
      void SetRotationRelative(const spitfire::math::cQuaternion& _rotationRelative) { rotationRelative = _rotationRelative; }

    private:
      static COMPONENT GetComponentFromIndex(size_t index);
      static size_t GetIndexFromComponent(COMPONENT component);

      spitfire::string_t sName;
      spitfire::math::cVec3 positionRelative;
      spitfire::math::cQuaternion rotationRelative;

      std::map<size_t, cComponent*> components;
    };

    template <typename T>
    inline T* cGameObject::GetComponentIfEnabled(COMPONENT componentType) const
    {
      T* pComponent = GetComponentIfEnabledOrDisabled<T>(componentType);
      if ((pComponent != nullptr) && !pComponent->IsEnabled()) pComponent = nullptr;

      return pComponent;
    }

    template <typename T>
    inline T* cGameObject::GetComponentIfEnabledOrDisabled(COMPONENT componentType) const
    {
      const size_t index = GetIndexFromComponent(componentType);

      cComponent* pComponent = nullptr;
      std::map<size_t, cComponent*>::const_iterator iter = components.begin();
      const std::map<size_t, cComponent*>::const_iterator iterEnd = components.end();
      while (iter != iterEnd) {
        if (iter->first == index) {
          pComponent = iter->second;
          break;
        }

        iter++;
      }

      // dynamic_cast?
      return static_cast<T*>(pComponent);
    }



    class cGameObjectCollection
    {
    public:
      void Add(cGameObjectRef pGameObject);
      void Remove(cGameObjectRef pGameObject);

      void Update(spitfire::durationms_t currentTime);

      std::list<cGameObjectRef>& GetGameObjects() { return gameobjects; }
      const std::list<cGameObjectRef>& GetGameObjects() const { return gameobjects; }

      template <class T>
      std::vector<cGameObjectRef> GetGameObjectsWithinVolume(const T& volume);

      template <class T>
      std::vector<cGameObjectRef> GetGameObjectsWithinVolumeWithComponentEnabled(const T& volume, COMPONENT componentType);

    private:
      static bool DistanceCompare(cGameObjectRef lhs, cGameObjectRef rhs, const spitfire::math::cVec3& point);

      std::list<cGameObjectRef> gameobjects;
    };

    inline bool cGameObjectCollection::DistanceCompare(cGameObjectRef lhs, cGameObjectRef rhs, const spitfire::math::cVec3& point)
    {
      const float fDistanceToLeftHandSide = (lhs->GetPositionRelative() - point).GetLength();
      const float fDistanceToRightHandSide = (rhs->GetPositionRelative() - point).GetLength();
      return (fDistanceToLeftHandSide > fDistanceToRightHandSide);
    }

    template <class T>
    std::vector<cGameObjectRef> cGameObjectCollection::GetGameObjectsWithinVolume(const T& volume)
    {
      std::vector<cGameObjectRef> found;

      std::list<cGameObjectRef>::iterator iter = gameobjects.begin();
      const std::list<cGameObjectRef>::iterator iterEnd = gameobjects.end();
      while (iter != iterEnd) {
        // If this object is within the volume add it to the list
        if (volume.IsPointWithinVolume((*iter)->GetPositionRelative())) found.push_back(*iter);

        iter++;
      }

      // Sort our list in order of closest to furthest from the centre of volume
      spitfire::algorithm::SortWithUserData(found.begin(), found.end(), cGameObjectCollection::DistanceCompare, volume.GetPosition());

      // Return our list of found objects
      return found;
    }

    template <class T>
    std::vector<cGameObjectRef> cGameObjectCollection::GetGameObjectsWithinVolumeWithComponentEnabled(const T& volume, COMPONENT componentType)
    {
      std::vector<cGameObjectRef> found;

      std::list<cGameObjectRef>::iterator iter = gameobjects.begin();
      const std::list<cGameObjectRef>::iterator iterEnd = gameobjects.end();
      while (iter != iterEnd) {
        // If this object is within the volume add it to the list
        if (volume.IsPointWithinVolume((*iter)->GetPositionRelative())) {
          // This object must also contain this component
          if ((*iter)->IsComponentPresentAndEnabled(componentType)) found.push_back(*iter);
        }

        iter++;
      }

      // Sort our list in order of closest to furthest from the centre of volume
      SortWithUserData(found.begin(), found.end(), cGameObjectCollection::DistanceCompare, volume.GetPosition());

      // Return our list of found objects
      return found;
    }




    class cPickupComponent : public cComponent
    {
    public:
      explicit cPickupComponent(cGameObject& _object) : cComponent(_object) {}
    };

    class cBackpackComponent : public cComponent
    {
    public:
      explicit cBackpackComponent(cGameObject& _object) : cComponent(_object) {}

      //std::vector<cItem*> items;
    };


#if defined(BUILD_PHYSICS_BULLET) || defined(BUILD_PHYSICS_ODE)
    class cPhysicsComponent : public cComponent
    {
    public:
      explicit cPhysicsComponent(cGameObject& _object) : cComponent(_object) {}

      physics::cBodyRef GetBody() { return pBody; }
      void SetBody(physics::cBodyRef _pBody) { pBody = _pBody; }

    private:
      void _Update(spitfire::durationms_t currentTime);

      physics::cBodyRef pBody;
    };


    class cVehicleComponent;

    class cVehicleBase
    {
    public:
      enum class TYPE {
        CAR,
        PLANE,
        HELICOPTER,
      };

      explicit cVehicleBase(cGameObject& object, TYPE type);
      virtual ~cVehicleBase() {}

      bool IsCar() const { return (type == TYPE::CAR); }
      bool IsPlane() const { return (type == TYPE::PLANE); }
      bool IsHelicopter() const { return (type == TYPE::HELICOPTER); }

      void Init() { _Init(); }


      // General Keyboard/Mouse/Joystick Inputs
      void SetInputAccelerator0To1(float_t fInput0To1) { fInputAccelerator0To1 = fInput0To1; }
      void SetInputBrake0To1(float_t fInput0To1) { fInputBrake0To1 = fInput0To1; }
      void SetInputForward0To1(float_t fInput0To1) { fInputForward0To1 = fInput0To1; }
      void SetInputBackward0To1(float_t fInput0To1) { fInputBackward0To1 = fInput0To1; }
      void SetInputLeft0To1(float_t fInput0To1) { fInputLeft0To1 = fInput0To1; }
      void SetInputRight0To1(float_t fInput0To1) { fInputRight0To1 = fInput0To1; }

      // Car Specific
      void SetInputHandBrake0To1(float_t fInput0To1) { fInputHandBrake0To1 = fInput0To1; }
      void SetInputClutch0To1(float_t fInput0To1) { fInputClutch0To1 = fInput0To1; }
      void SetInputChangeGearUp() { bIsInputChangeGearUp = true; }
      void SetInputChangeGearDown() { bIsInputChangeGearDown = true; }

      // Plane Specific

      // Helicopter Specific

      // Plane and Helicopter Specific
      void SetInputYawLeft0To1(float_t fInput0To1) { fInputYawLeft0To1 = fInput0To1; }
      void SetInputYawRight0To1(float_t fInput0To1) { fInputYawRight0To1 = fInput0To1; }



      void Update(durationms_t currentTime) { _Update(currentTime); }

    protected:
      cGameObject& object;

      // General Keyboard/Mouse/Joystick Inputs
      float_t fInputAccelerator0To1;
      float_t fInputBrake0To1;
      float_t fInputForward0To1;
      float_t fInputBackward0To1;
      float_t fInputLeft0To1;
      float_t fInputRight0To1;

      // Car Specific
      float_t fInputHandBrake0To1;
      float_t fInputClutch0To1;
      bool bIsInputChangeGearUp;
      bool bIsInputChangeGearDown;

      // Plane Specific

      // Helicopter Specific


      // Plane and Helicopter Specific
      float_t fInputYawLeft0To1;
      float_t fInputYawRight0To1;

    private:
      virtual void _Init() {}
      virtual void _Update(durationms_t currentTime) {}

      TYPE type;
    };



    class cVehicleComponent : public cComponent
    {
    public:
      friend class cVehicleBase;

      explicit cVehicleComponent(cGameObject& _object);
      ~cVehicleComponent();

      bool IsCar() const { ASSERT(pVehicle != nullptr); return pVehicle->IsCar(); }
      bool IsPlane() const { ASSERT(pVehicle != nullptr); return pVehicle->IsPlane(); }
      bool IsHelicopter() const { ASSERT(pVehicle != nullptr); return pVehicle->IsHelicopter(); }

      void SetCar(physics::cCarRef pCar, const std::vector<scenegraph3d::cGroupNodeRef>& wheels);
      void SetPlane();
      void SetHelicopter();

      cVehicleBase* GetVehicle() { ASSERT(pVehicle != nullptr); return pVehicle; }


      // General Keyboard/Mouse/Joystick Inputs
      void SetInputAccelerator0To1(float_t fInput0To1) { ASSERT(pVehicle != nullptr); pVehicle->SetInputAccelerator0To1(fInput0To1); }
      void SetInputBrake0To1(float_t fInput0To1) { ASSERT(pVehicle != nullptr); pVehicle->SetInputBrake0To1(fInput0To1); }
      void SetInputForward0To1(float_t fInput0To1) { ASSERT(pVehicle != nullptr); pVehicle->SetInputForward0To1(fInput0To1); }
      void SetInputBackward0To1(float_t fInput0To1) { ASSERT(pVehicle != nullptr); pVehicle->SetInputBackward0To1(fInput0To1); }
      void SetInputLeft0To1(float_t fInput0To1) { ASSERT(pVehicle != nullptr); pVehicle->SetInputLeft0To1(fInput0To1); }
      void SetInputRight0To1(float_t fInput0To1) { ASSERT(pVehicle != nullptr); pVehicle->SetInputRight0To1(fInput0To1); }

      // Car Specific
      void SetInputHandBrake0To1(float_t fInput0To1) { ASSERT(pVehicle != nullptr); pVehicle->SetInputHandBrake0To1(fInput0To1); }
      void SetInputClutch0To1(float_t fInput0To1) { ASSERT(pVehicle != nullptr); pVehicle->SetInputClutch0To1(fInput0To1); }
      void SetInputChangeGearUp() { ASSERT(pVehicle != nullptr); pVehicle->SetInputChangeGearUp(); }
      void SetInputChangeGearDown() { ASSERT(pVehicle != nullptr); pVehicle->SetInputChangeGearDown(); }

      // Plane Specific

      // Helicopter Specific

      // Plane and Helicopter Specific
      void SetInputYawLeft0To1(float_t fInput0To1) { ASSERT(pVehicle != nullptr); pVehicle->SetInputYawLeft0To1(fInput0To1); }
      void SetInputYawRight0To1(float_t fInput0To1) { ASSERT(pVehicle != nullptr); pVehicle->SetInputYawRight0To1(fInput0To1); }

    private:
      void _Update(durationms_t currentTime);

      cVehicleBase* pVehicle;
    };
#endif



    class cAudioSourceComponent : public cComponent
    {
    public:
      explicit cAudioSourceComponent(cGameObject& _object) : cComponent(_object) {}

      void AddSource(audio::cSourceRef pSource);
      void RemoveSource(audio::cSourceRef pSource);

    private:
      void _Update(spitfire::durationms_t currentTime);

      std::vector<audio::cSourceRef> sources;
    };

    class cTeamComponent : public cComponent
    {
    public:
      explicit cTeamComponent(cGameObject& _object) : cComponent(_object) {}

      spitfire::string_t sTeam;
    };


    class cAnimationComponent : public cComponent
    {
    public:
      explicit cAnimationComponent(cGameObject& _object) : cComponent(_object) {}

    private:
      void _Update(spitfire::durationms_t currentTime);
    };

    class cRenderComponent : public cComponent
    {
    public:
      explicit cRenderComponent(cGameObject& _object) : cComponent(_object) {}

      void SetSceneNode(scenegraph3d::cSceneNodeRef _pNode) { pNode = _pNode; }
      scenegraph3d::cSceneNodeRef GetSceneNode() const { return pNode; }

    private:
      void _Update(spitfire::durationms_t currentTime);

      scenegraph3d::cSceneNodeRef pNode;
    };
  }
}

#endif // GAMECOMPONENT_H
