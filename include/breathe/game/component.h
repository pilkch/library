#ifndef GAMECOMPONENT_H
#define GAMECOMPONENT_H

#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>

#include <spitfire/math/cVec3.h>
#include <spitfire/math/cQuaternion.h>

#include <breathe/breathe.h>

#include <breathe/game/scenegraph.h>

#include <breathe/physics/physics.h>
#include <breathe/physics/cPhysicsObject.h>

namespace breathe
{
  namespace game
  {
    enum COMPONENT {
      COMPONENT_USER_HIGH_PRIORITY_0,
      COMPONENT_USER_HIGH_PRIORITY_1,
      COMPONENT_USER_HIGH_PRIORITY_2,
      COMPONENT_PHYSICS, // This component should be close to the start of updating
      COMPONENT_FLAMABLE,
      COMPONENT_EXPLODABLE,
      COMPONENT_BOUYANT,
      COMPONENT_ITEM, // An pickup that can be added to an item container, for example a weapon, ammo, a key etc.
      COMPONENT_ITEM_CONTAINER, // Contains items, an inventory
      COMPONENT_VEHICLE, // Game object is a vehicle of some description, note: this infers drivable, a prop vehicle that no one ever drives will NOT have this
      COMPONENT_AUDIOSOURCE,
      COMPONENT_TEAM,
      COMPONENT_ANIMATION, // This is a wrapper for md3 animation, it requires a COMPONENT_RENDERABLE:cRenderableComponent that is pointing to a cAnimationNode
      COMPONENT_USER_NORMAL_PRIORITY_0,
      COMPONENT_USER_NORMAL_PRIORITY_1,
      COMPONENT_USER_NORMAL_PRIORITY_2,
      COMPONENT_USER_NORMAL_PRIORITY_3,
      COMPONENT_USER_NORMAL_PRIORITY_4,
      COMPONENT_USER_NORMAL_PRIORITY_5,
      COMPONENT_USER_NORMAL_PRIORITY_6,
      COMPONENT_USER_NORMAL_PRIORITY_7,
      COMPONENT_USER_NORMAL_PRIORITY_8,
      COMPONENT_USER_NORMAL_PRIORITY_9,
      COMPONENT_USER_NORMAL_PRIORITY_10,
      COMPONENT_USER_NORMAL_PRIORITY_11,
      COMPONENT_USER_NORMAL_PRIORITY_12,
      COMPONENT_USER_NORMAL_PRIORITY_13,
      COMPONENT_USER_NORMAL_PRIORITY_14,
      COMPONENT_USER_NORMAL_PRIORITY_15,
      COMPONENT_USER_NORMAL_PRIORITY_16,
      COMPONENT_USER_NORMAL_PRIORITY_17,
      COMPONENT_USER_NORMAL_PRIORITY_18,
      COMPONENT_RENDERABLE, // This one wants to be close to the end, if not absolute last, when updating
      COMPONENT_USER_LOW_PRIORITY_0,
      COMPONENT_USER_LOW_PRIORITY_1,
      COMPONENT_USER_LOW_PRIORITY_2

      // input ie. keyboard, ai?
      // particle system, mesh data, or is that getting too much into a scenegraph
    };


    class cGameObject;
    typedef cSmartPtr<cGameObject> cGameObjectRef;

    class cComponent
    {
    public:
      explicit cComponent(cGameObject& _object) : object(_object), bIsEnabled(true) {}
      virtual ~cComponent() {}

      bool IsEnabled() const { return bIsEnabled; }

      void Update(spitfire::sampletime_t currentTime) { _Update(currentTime); }

    protected:
      cGameObject& object;

    private:
      virtual void _Update(spitfire::sampletime_t currentTime) {}

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


      void Update(spitfire::sampletime_t currentTime);

      const string_t& GetName() const { return sName; }
      void SetName(const string_t& _sName) { sName = _sName; }

      const spitfire::math::cVec3& GetPositionRelative() const { return positionRelative; }
      void SetPositionRelative(const spitfire::math::cVec3& _positionRelative) { positionRelative = _positionRelative; }

      const spitfire::math::cQuaternion& GetRotationRelative() const { return rotationRelative; }
      void SetRotationRelative(const spitfire::math::cQuaternion& _rotationRelative) { rotationRelative = _rotationRelative; }

    private:
      spitfire::string_t sName;
      spitfire::math::cVec3 positionRelative;
      spitfire::math::cQuaternion rotationRelative;

      std::map<COMPONENT, cComponent*> components;
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
      cComponent* pComponent = nullptr;
      std::map<COMPONENT, cComponent*>::const_iterator iter = components.begin();
      const std::map<COMPONENT, cComponent*>::const_iterator iterEnd = components.end();
      while (iter != iterEnd) {
        if (iter->first == componentType) {
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

      void Update(spitfire::sampletime_t currentTime);

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




    class cPhysicsComponent : public cComponent
    {
    public:
      explicit cPhysicsComponent(cGameObject& _object) : cComponent(_object) {}

      physics::cPhysicsObjectRef GetPhysicsObject() { return pPhysicsObject; }
      void SetPhysicsObject(physics::cPhysicsObjectRef _pPhysicsObject) { pPhysicsObject = _pPhysicsObject; }

    private:
      void _Update(spitfire::sampletime_t currentTime);

      physics::cPhysicsObjectRef pPhysicsObject;
    };

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


    enum VEHICLE_TYPE {
      VEHICLE_TYPE_CAR,
      VEHICLE_TYPE_PLANE,
      VEHICLE_TYPE_HELICOPTER,
    };

    class cVehicleComponent : public cComponent
    {
    public:
      cVehicleComponent(cGameObject& _object, VEHICLE_TYPE _type);

      void SetTypeCar() { type = VEHICLE_TYPE_CAR; }
      void SetTypePlane() { type = VEHICLE_TYPE_PLANE; }
      void SetTypeHelicopter() { type = VEHICLE_TYPE_HELICOPTER; }

      void SetInputUp(float _fInputUp) { fInputUp = _fInputUp; }
      void SetInputDown(float _fInputDown) { fInputDown = _fInputDown; }
      void SetInputLeft(float _fInputLeft) { fInputLeft = _fInputLeft; }
      void SetInputRight(float _fInputRight) { fInputRight = _fInputRight; }

    private:
      void _Update(spitfire::sampletime_t currentTime);

      VEHICLE_TYPE type;

      float fInputUp;
      float fInputDown;
      float fInputLeft;
      float fInputRight;
    };

    class cAudioSourceComponent : public cComponent
    {
    public:
      explicit cAudioSourceComponent(cGameObject& _object) : cComponent(_object) {}
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
      void _Update(spitfire::sampletime_t currentTime);
    };

    class cRenderComponent : public cComponent
    {
    public:
      explicit cRenderComponent(cGameObject& _object) : cComponent(_object) {}

      void SetSceneNode(scenegraph3d::cSceneNodeRef _pNode) { pNode = _pNode; }
      scenegraph3d::cSceneNodeRef GetSceneNode() const { return pNode; }

    private:
      void _Update(spitfire::sampletime_t currentTime);

      scenegraph3d::cSceneNodeRef pNode;
    };
  }
}

#endif // GAMECOMPONENT_H
