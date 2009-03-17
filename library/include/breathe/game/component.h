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
      COMPONENT_AUDIOSOURCE,
      COMPONENT_TEAM,
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
      COMPONENT_RENDERABLE, // This one wants to be close to the end, if not last when updating
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
      explicit cComponent(cGameObject& _object) : object(_object) {}
      virtual ~cComponent() {}

      void Update(spitfire::sampletime_t currentTime) { _Update(currentTime); }

    protected:
      cGameObject& object;

    private:
      virtual void _Update(spitfire::sampletime_t currentTime) {}
    };

    class cGameObject
    {
    public:
      //cGameObject();
      ~cGameObject() { RemoveAllComponents(); }

      // After adding the component is then owned by the game object, do not keep a pointer to this component
      // as it may be deleted at any time.  AddComponent will delete an existing component and replace it with this one.
      void AddComponent(COMPONENT componentType, cComponent* pComponent);
      void RemoveAllComponents();

      cComponent* GetComponentIfEnabled(COMPONENT componentType);
      cComponent* GetComponentIfEnabledOrDisabled(COMPONENT componentType);

      bool IsComponentPresentAndEnabled(COMPONENT componentType) const;
      bool IsComponentPresentAndEnabledOrDisabled(COMPONENT componentType) const;

      void SetComponentEnabled(COMPONENT componentType);
      void SetComponentDisabled(COMPONENT componentType);


      void Update(spitfire::sampletime_t currentTime);

      string_t GetName() const { return sName; }
      void SetName(const string_t& _sName) { sName = _sName; }

      spitfire::math::cVec3 GetPositionRelative() const { return positionRelative; }
      void SetPositionRelative(const spitfire::math::cVec3& _positionRelative) { positionRelative = _positionRelative; }

      spitfire::math::cQuaternion GetRotationRelative() const { return rotationRelative; }
      void SetRotationRelative(const spitfire::math::cQuaternion& _rotationRelative) { rotationRelative = _rotationRelative; }

    private:
      spitfire::string_t sName;
      spitfire::math::cVec3 positionRelative;
      spitfire::math::cQuaternion rotationRelative;

      std::map<COMPONENT, bool> componentEnabled;
      std::map<COMPONENT, cComponent*> components;
    };


    class cGameObjectCollection
    {
    public:
      void Add(cGameObjectRef pGameObject);
      void Remove(cGameObjectRef pGameObject);

      void Update(spitfire::sampletime_t currentTime);

      std::list<cGameObjectRef>& GetGameObjects() { return gameobjects; }
      const std::list<cGameObjectRef>& GetGameObjects() const { return gameobjects; }

    private:
      std::list<cGameObjectRef> gameobjects;
    };




    class cPhysicsComponent : public cComponent
    {
    public:
      cPhysicsComponent(cGameObject& _object) : cComponent(_object) {}

      physics::cPhysicsObjectRef GetPhysicsObject() { return pPhysicsObject; }
      void SetPhysicsObject(physics::cPhysicsObjectRef _pPhysicsObject) { pPhysicsObject = _pPhysicsObject; }

    private:
      void _Update(spitfire::sampletime_t currentTime);

      physics::cPhysicsObjectRef pPhysicsObject;
    };

    class cPickupComponent : public cComponent
    {
    public:
      cPickupComponent(cGameObject& _object) : cComponent(_object) {}
      
    };

    class cBackpackComponent : public cComponent
    {
    public:
      cBackpackComponent(cGameObject& _object) : cComponent(_object) {}

      //std::vector<cItem*> items;
    };

    class cAudioSourceComponent : public cComponent
    {
    public:
      cAudioSourceComponent(cGameObject& _object) : cComponent(_object) {}
      
    };

    class cTeamComponent : public cComponent
    {
    public:
      cTeamComponent(cGameObject& _object) : cComponent(_object) {}

      spitfire::string_t sTeam;
    };

    class cRenderComponent : public cComponent
    {
    public:
      cRenderComponent(cGameObject& _object) : cComponent(_object) {}

      void SetSceneNode(scenegraph3d::cSceneNodeRef _pNode) { pNode = _pNode; }

    private:
      void _Update(spitfire::sampletime_t currentTime);

      scenegraph3d::cSceneNodeRef pNode;
    };
  }
}

#endif // GAMECOMPONENT_H
