#ifndef CPLAYER_H
#define CPLAYER_H

#ifdef BUILD_PHYSICS_3D
#include <breathe/physics/physics.h>
#include <breathe/physics/cPhysicsObject.h>
#endif

namespace breathe
{
  namespace vehicle
  {
    class cSeat;
  }

  class cItem;

  enum class STATE {
    DEAD,
    WALK,
    RUN,
    SPRINT,
    DRIVE,
    PASSENGER
  };

  enum class CAMERA {
    FIRSTPERSON,
    THIRDPERSON
#ifdef BUILD_DEBUG
    , THIRDPERSONFREE,
    FIRSTPERSONFREE
#endif
  };


  // Valid types:
  // Any item that has been loaded from an xml file can be picked up, for vehicle parts the properties are loaded from the xml file
  // knife, ak47, grenade_explosive, grenade_smoke, grenade_flashbang, turbo_small, turbo_medium, turbo_large,
  // give ak47
  // give money 10000
  // give health 100
  // give armour 100

  // Valid slots:
  // weapon_melee
  // weapon_small
  // weapon_medium
  // weapon_large
  // grenade
  // ammo
  // armour_torso
  // armour_head
  // armour_legs
  // other
  // none

  // All slots are one item except for SLOT_OTHER which can carry as much as it likes
  // Each weapon slot can carry one weapon of the particular type, ie. a knife (melee weapon) can only go into SLOT_WEAPON_MELEE and
  // replaces any weapon that was already there
  // Money and health are handle separately
  const size_t SLOT_WEAPON_MELEE = 0;
  const size_t SLOT_WEAPON_SMALL = 1;
  const size_t SLOT_WEAPON_MEDIUM = 2;
  const size_t SLOT_WEAPON_LARGE = 3;
  const size_t SLOT_GRENADE = 4; // Grenade slot can carry a few grenades, application specific
  const size_t SLOT_AMMO = 6; // Ammo slot can carry a few clips, application specific
  const size_t SLOT_ARMOUR_TORSO = 7;
  const size_t SLOT_ARMOUR_HEAD = 8;
  const size_t SLOT_ARMOUR_LEGS = 9;
  const size_t SLOT_OTHER = 10; // Anything else, keys, keycards, books, pieces of paper, fruit, etc.  Can carry as many items as we like
  const size_t SLOT_NONE = 11; // Not a valid slot, this is the default, ie. you can't pick this item up

  // The game rules can choose what each team gets at start of each round.

  // Example
  // Default: knife and a pistol

  // Hi/Low map:
  // Team 0 are in a pit,
  // Team 1 are standing on a glass platform above
  // map_gamerules.xml
  // <gamerules>
  //   <team id="0">
  //     <kit>
  //       <weapon type="knife"/>
  //       <weapon type="mp5navy"/>
  //     </kit>
  //   </team>
  //   <team id="1">
  //     <kit>
  //       <weapon type="knife"/>
  //       <weapon type="grenade"/>
  //       <weapon type="grenade"/>
  //       <weapon type="grenade"/>
  //       <weapon type="grenade"/>
  //     </kit>
  //   </team>
  // </gamerules>

  // if (game rules specify) use those settings.
  // else if (map overrides) use those settings.
  // else set defaults
/*
#ifdef BUILD_PHYSICS_3D
  class cBipedPhysicsObject :
    public physics::cUprightCapsule
  {
  public:
    explicit cBipedPhysicsObject(physics::cWorld* pWorld);

    // For raycasting to find out if we standing on anything when we are in walking mode
    void RayCast();
    static void RayCastCallback(void* data, dGeomID g1, dGeomID g2);
  };
#endif*/

  class cPlayer
  {
  public:
    explicit cPlayer(physics::cWorld* pWorld);
    ~cPlayer();

    void Update(durationms_t currentTime);

    bool IsInACar() const;
    bool IsDriving() const;
    bool IsThirdPersonCamera() const;

    void ChangeStateToDriving();
    void ChangeStateToRunning();

    void ChangeItemUp();
    void ChangeItemDown();

    // TODO: Remove this and use a gameobject instead
    spitfire::math::cVec3 position;

#ifdef BUILD_PHYSICS_3D
//    cBipedPhysicsObject biped;
#endif

    STATE state;

    CAMERA uiCameraMode;

    float fInputUp;
    float fInputDown;
    float fInputLeft;
    float fInputRight;

    bool bInputHandbrake;
    bool bInputClutch;

    bool bInputJump;
    bool bInputCrouch;


    float fSpeedWalk;
    float fSpeedRun;
    float fSpeedSprint;

    float fVertical;
    float fHorizontal;

    float fDollars;

    //list of items
    std::vector<cItem*>vItem;
    vehicle::cSeat* pSeat;
  };
}

#endif // CPLAYER_H
