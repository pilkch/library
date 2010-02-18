#ifndef CAIPLANNER_H
#define CAIPLANNER_H

// Spitfire headers

#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cQuaternion.h>

// Breathe headers

#include <breathe/game/cAIPathFinder.h>

namespace breathe
{
  namespace ai
  {
    typedef uint16_t entityid_t;
    typedef uint16_t eventid_t;

    // From the F.E.A.R AI paper for GDC 2006 by Jeff Orkin
    // http://web.media.mit.edu/~jorkin/gdc2006_orkin_jeff_fear.pdf

    // We start our seven layer dip with the basics; the beans. A.I. fire their weapons when they
    // detect the player. They can accomplish this with the KillEnemy goal, which they satisfy with
    // the Attack action.
    // We want A.I. to value their lives, so next we add the guacamole. The A.I. dodges when a gun
    // is aimed at him. We add a goal and two actions. The Dodge goal can be satisfied with either
    // DodgeShuffle or DodgeRoll.
    // Next we add the sour cream. When the player gets close enough, A.I. use melee attacks
    // instead of wasting bullets. This behavior requires the addition of only one new AttackMelee
    // action. We already have the KillEnemy goal, which AttackMelee satisfies.
    // If A.I. really value their lives, they won’t just dodge, they’ll take cover. This is the salsa! We
    // add the Cover goal. A.I. get themselves to cover with the GotoNode action, at which point
    // the KillEnemy goal takes priority again. A.I. use the AttackFromCover action to satisfy the
    // KillEnemy goal, while they are positioned at a Cover node. We already handled dodging
    // with the guacamole, but now we would like A.I. to dodge in a way that is context-sensitive to
    // taking cover, so we add another action, DodgeCovered.
    // Dodging is not always enough, so we add the onions; blind firing. If the A.I. gets shot while in
    // cover, he blind fires for a while to make himself harder to hit. This only requires adding one
    // BlindFireFromCover action.
    // The cheese is where things really get exciting. We give the A.I. the ability to reposition when
    // his current cover position is invalidated. This simply requires adding the Ambush goal. When
    // an A.I.’s cover is compromised, he will try to hide at a node designated by designers as an
    // Ambush node. The final layer is the olives, which really bring out the flavor. For this layer, we
    // add dialogue that lets the player know what the A.I. is thinking, and allows the A.I. to
    // communicate with squad members. We’ll discuss this a little later.


    class cAgentOrSquadProperties
    {
    public:
      const spitfire::math::cVec3& GetPosition() const { return position; }
      const spitfire::math::cQuaternion& GetRotation() const { return rotation; }

      spitfire::math::cVec3 position;
      spitfire::math::cQuaternion rotation;
    };

    class cBlackBoardEntityData
    {
    public:
      spitfire::math::cVec3 position;
      spitfire::math::cQuaternion rotation;
    };

    // The current "knowledge" of an agent or squad of agents in the simulation
    class cAgentOrSquadBlackBoard
    {
    public:
      void AddOrUpdateEntityPosition(entityid_t idEntity, const spitfire::math::cVec3& position);
      void AddOrUpdateEntityRotation(entityid_t idEntity, const spitfire::math::cQuaternion& rotation);

      void AddSuspiciousEvent(eventid_t idEntity, const spitfire::math::cVec3& position);

    private:
      std::map<entityid_t, cBlackBoardEntityData> knownEntities;
      std::map<eventid_t, spitfire::math::cVec3> knownSuspiciousEvents;
    };

    class cCondition
    {
    public:
      cCondition();

      bool operator==(const cCondition& rhs);

      bool IsTrue(const cAgentOrSquadProperties& properties, const cAgentOrSquadBlackBoard& blackBoard) const; // returns true if the current properties and blackboard state result in this condition being true
    };

    class cAction
    {
    public:
      std::vector<cCondition> vPreConditionsRequired;
      std::vector<cCondition> vPostConditionsSatisfied;
    };

    class cGoal
    {
    public:
      bool IsAllPreConditionsRequiredSatisfied(const cAgentOrSquadProperties& properties, const cAgentOrSquadBlackBoard& blackBoard) const; // returns true if the current properties and blackboard state result in all of the pre conditions being true
      const std::vector<cCondition>& GetPostSatisfiedConditions() const { return vPostConditionsSatisfied; }

    private:
      std::vector<cCondition> vPreConditionsRequired;
      std::vector<cCondition> vPostConditionsSatisfied;

      std::list<cAction> lActions;
    };

    class cAgent
    {
    public:


    private:
      std::list<cAction> lPossibleActions; // After being set up, this is constant throughout the game

      cAgentOrSquadProperties properties;
      cAgentOrSquadBlackBoard blackBoard;

      std::list<cGoal> lGoals;
    };

    class cSquad
    {
    public:


    private:
      std::list<cAction> lPossibleActions; // After being set up, this is constant throughout the game

      cAgentOrSquadProperties properties;
      cAgentOrSquadBlackBoard blackBoard;

      std::list<cGoal> lGoals;
    };

    class cPlanner
    {
    public:
      void PlanForAgent(cAgent& agent);
      void PlanForSquad(cSquad& squad);
    };

    void cPlanner::PlanForSquad(cSquad& squad)
    {
      /*... Find the agents that can fill the slots in this squad
      if (found participants) {
        ... send orders to each participant
        ... monitor how each participant is going with previous orders
      }*/
    }



    class cState
    {
    public:
      virtual ~cState() {}
    };










    class cGoalKillEnemy : public cGoal
    {
    public:

    };

    class cGoalPatrol : public cGoal
    {
    public:

    };

    class cGoalPatrolToDestination : public cGoal
    {
    public:

    };

    class cGoalRespondToOrderFromSquadLeader : public cGoal
    {
    public:

    };

    class cGoalRespondToAlertWhatsYourStatus : public cGoal
    {
    public:

    };

    class cGoalRespondToAlertEnemySightedFromSquad : public cGoal
    {
    public:

    };

    class cGoalRespondToAlertGrenadeFromSquad : public cGoal
    {
    public:

    };



    class cActionIdle : public cAction
    {
    public:

    };

    class cActionAnimate : public cAction
    {
    public:

    };

    class cActionGoToNode : public cAction
    {
    public:

    };

    class cActionAttack : public cAction
    {
    public:

    };

    class cActionAttackCrouched : public cAction
    {
    public:

    };

    class cActionAttackFromCover : public cAction
    {
    public:

    };

    class cActionAttackThrowGrenade : public cAction
    {
    public:

    };

    class cActionAttackThrowGrenadeFromCover : public cAction
    {
    public:

    };

    class cActionReloadCrouched : public cAction
    {
    public:

    };

    class cActionReloadCovered : public cAction
    {
    public:

    };

    class cActionReloadGetIntoCover : public cAction
    {
    public:

    };

    class cActionFlushOutWithSuppressionFire : public cAction
    {
    public:

    };

    class cActionFlushOutWithGrenade : public cAction
    {
    public:

    };

    class cActionFlushOutWithTorchLight : public cAction
    {
    public:

    };

    class cActionLookAtDisturbance : public cAction
    {
    public:

    };

    class cActionGoToDisturbance : public cAction
    {
    public:

    };

    class cActionInspectDisturbance : public cAction
    {
    public:

    };

    class cActionLookAround : public cAction
    {
    public:

    };

    class cActionDodgeLeftOrRight : public cAction
    {
    public:

    };

    class cActionSelectWeaponNone : public cAction
    {
    public:

    };

    class cActionSelectWeaponPrimary : public cAction
    {
    public:

    };

    class cActionSelectWeaponSecondary : public cAction
    {
    public:

    };

    class cActionOpenDoor : public cAction
    {
    public:

    };

    class cActionCloseDoor : public cAction
    {
    public:

    };

    class cActionWaitToStartAmbush : public cAction
    {
    public:

    };

    class cActionDie : public cAction
    {
    public:

    };




    class cActionSquadAdvance : public cAction
    {
    public:

    };

    class cActionSquadAdvanceCover : public cAction
    {
    public:

    };

    class cActionSquadTakeCover : public cAction
    {
    public:

    };

    class cActionSquadPatrol : public cAction
    {
    public:

    };

    class cActionSquadSearch : public cAction
    {
    public:

    };

    class cActionSquadAmbush : public cAction
    {
    public:

    };

    class cActionSquadWaitToStartAmbush : public cAction
    {
    public:

    };

    class cActionSquadRaiseAlertWhatsYourStatus : public cAction
    {
    public:

    };

    class cActionSquadRaiseAlertEnemySighted : public cAction
    {
    public:

    };

    class cActionSquadRaiseAlertGrenade : public cAction
    {
    public:

    };




    // TODO: Should these be more generic ie. cStateAttack?

    class cStateAttack : public cState
    {
    public:

    };

    class cStateReload : public cState
    {
    public:

    };

    class cStateTakeCover : public cState
    {
    public:

    };

    class cStateRetreat : public cState
    {
    public:

    };

    class cStateSearch : public cState
    {
    public:

    };




    class cStateShootSecondaryWeapon : public cState
    {
    public:

    };

    class cStateShootPrimaryWeapon : public cState
    {
    public:

    };

    class cStateThrowGrenade : public cState
    {
    public:

    };

    class cStateReloadSecondaryWeapon : public cState
    {
    public:

    };

    class cStateReloadPrimaryWeapon : public cState
    {
    public:

    };

    // http://en.wikipedia.org/wiki/Flanking_maneuver
    class cStateFlankEnemy : public cState
    {
    public:

    };

    class cStateSearchForWeaponsAndOrAmmunition : public cState
    {
    public:

    };

    class cStatePatrolToDestination : public cState
    {
    public:

    };

    class cStatePatrolArea : public cState
    {
    public:

    };















    // *** Wandering Bunny Rabbit AI

    // ** Goals

    // cGoalPatrol

    class cGoalLookAround : public cGoal
    {
    public:

    };

    class cGoalEatFood : public cGoal
    {
    public:

    };

    class cGoalStayAwayFromDanger : public cGoal
    {
    public:

    };


    // ** Actions

    // cActionGoTo

    class cActionLookAt : public cAction
    {
    public:

    };

    class cActionEatOffGround : public cAction
    {
    public:

    };
  }
}

#endif // CAIPLANNER_H
