#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <set>

// OpenGL
#include <GL/GLee.h>


// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/geometry.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/util/base.h>

#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>



#include <breathe/physics/physics.h>

#include <breathe/game/cPlayer.h>
#include <breathe/game/cPetrolBowser.h>

#include <breathe/vehicle/cPart.h>
#include <breathe/vehicle/cVehicle.h>
#include <breathe/vehicle/cSeat.h>

namespace breathe
{
#ifdef BUILD_PHYSICS_3D
/*  cBipedPhysicsObject::cBipedPhysicsObject(physics::cWorld* pWorld) :
    physics::cUprightCapsule(pWorld)
  {
  }

  // For raycasting to find out if we standing on anything when we are in walking mode
  void cBipedPhysicsObject::RayCast()
  {
  }

  void cBipedPhysicsObject::RayCastCallback(void* data, dGeomID g1, dGeomID g2)
  {
  }*/
#endif

  cPlayer::cPlayer(physics::cWorld* pWorld) :
    //biped(pWorld),
    state(STATE::WALK),
    pSeat(nullptr)
  {
#ifdef BUILD_PHYSICS_3D
    //SetUseBody(false);

    //biped.fMassKg = 80.0f;
    //biped.position.Set(0.0f, 0.0f, 10.0f);
#endif


#ifdef BUILD_RELEASE
    uiCameraMode = CAMERA::FIRSTPERSON;
#else
    uiCameraMode = CAMERA::FIRSTPERSONFREE;
#endif

    // Movement
    fInputUp = fInputDown = fInputLeft = fInputRight = 0.0f;

    bInputHandbrake = bInputClutch = false;

    bInputJump = bInputCrouch = false;

    fSpeedWalk = 1.0f;
    fSpeedRun = 2.0f;
    fSpeedSprint = 3.0f;

    fDollars = 0.0f;

    fVertical = 0.0f;
    fHorizontal = 0.0f;
  }

  cPlayer::~cPlayer()
  {
#ifdef BUILD_PHYSICS_3D
    // TODO: This doesn't make sense as while physics has a smart pointer to this, it will never be deleted so we will never get here
    // physics::RemovePhysicsObject(this);
#endif
  }


/*  const float fMaxDistance = 2.5f;

  void cPlayer::RayCast()
  {
    math::cVec3 dir = math::v3Down;

#ifdef BUILD_PHYSICS_3D
    rayContact.Clear();

    rayContact.fDepth = fMaxDistance;

    dGeomRaySet(geomRay, position.x, position.y, position.z, dir.x, dir.y, dir.z);
    dGeomRaySetLength(geomRay, fMaxDistance);
    //dSpaceCollide2(geomRay, (dGeomID)physics::GetSpaceStatic(), this, RayCastCallback);
    //dSpaceCollide2(geomRay, (dGeomID)physics::GetSpaceDynamic(), this, RayCastCallback);
#endif
  }

  void cPlayer::RayCastCallback(void* data, dGeomID g1, dGeomID g2)
  {
    cPlayer* p = ((cPlayer*)data);
    if (dGeomGetBody(g1) == p->GetBody()) return;

    dContact c;
    if (dCollide(g2, g1, 1, &c.geom, sizeof(c)) == 1 && c.geom.depth < p->rayContact.fDepth)
      p->rayContact.SetContact(c.geom, p->GetGeom(), c.geom.depth);
  }
*/

  void cPlayer::Update(durationms_t currentTime)
  {
    // biped.Update(currentTime);

    float fSpeed = ((STATE::WALK == state) ? fSpeedWalk : ((STATE::RUN == state) ? fSpeedRun : fSpeedSprint));

    //if (STATE::DRIVE == state) position = pSeat->pVehicle->position;
    //else if (STATE::PASSENGER == state) position = pSeat->pVehicle->position;

#ifdef BUILD_DEBUG
    /*else*/ if (uiCameraMode == CAMERA::FIRSTPERSONFREE) {
      if (
        (fInputUp > math::cEPSILON) || (fInputDown > math::cEPSILON) ||
        (fInputLeft > math::cEPSILON) || (fInputRight > math::cEPSILON)
      ) {
        float fDirection = fHorizontal + math::DegreesToRadians(90.0f);
        fSpeed = 10.0f;

        if (fInputUp > math::cEPSILON && fInputUp > fInputDown) {
          if (fInputLeft > math::cEPSILON && fInputLeft > fInputRight) fDirection += math::DegreesToRadians(45.0f);
          else if (fInputRight > math::cEPSILON) fDirection -= math::DegreesToRadians(45.0f);

          position.z += fSpeed * sinf(fVertical - math::DegreesToRadians(90.0f));
        } else if (fInputDown > math::cEPSILON) {
          if (fInputLeft > math::cEPSILON && fInputLeft > fInputRight) fDirection += math::DegreesToRadians(125.0f);
          else if (fInputRight > math::cEPSILON) fDirection -= math::DegreesToRadians(125.0f);
          else fDirection += math::DegreesToRadians(180.0f);

          position.z += fSpeed * sinf(fVertical + math::DegreesToRadians(90.0f));
        } else if (fInputLeft > math::cEPSILON && fInputLeft > fInputRight) {
          fDirection += math::DegreesToRadians(90.0f);
          fSpeed *= fInputLeft;
        } else if (fInputRight > math::cEPSILON) {
          fDirection -= math::DegreesToRadians(90.0f);
          fSpeed *= fInputRight;
        }

        position.x += fSpeed * cosf(fDirection);
        position.y += fSpeed * sinf(fDirection);
      }
    }
#endif
#if 0
    else if (HasBody()) {
      physics::cUprightCapsule::Update(currentTime);

      dBodySetPosition(GetBody(), 20.0f, 20.0f, 20.0f);

      //const dReal* pos = dBodyGetPosition(GetBody());


      /*RayCast();

      if (rayContact.bContact) {
        // Push us out of whatever surface we are falling towards
        // We want to negate the downwards motion with an upwards one and then
        // add a little bit, enough to get out of the surface
        {
          const dReal* pos = dBodyGetPosition(GetBody());
          dBodySetPosition(GetBody(), pos[0], pos[1], pos[2] + (0.03f * rayContact.fDepth) + math::cEPSILON);

          const dReal* vel = dBodyGetLinearVel(GetBody());
          dBodySetLinearVel(GetBody(), vel[0], vel[1], ((vel[2] < 0.0f) ? 0.0f : vel[2]) + math::cEPSILON);
        }

        // Handle the actual movement
        {
          // Get a unit vector that represents which way the player is heading
          math::cVec3 v(fInputRight - fInputLeft, fInputUp - fInputDown, 0.0f);
          v.Normalise();

          // Bias the inputs slightly so that walking forwards is fastest, then left/right
          // and then backwards is slowest to make for slightly more interesting gameplay
          v.x *= 0.8f;
          if (v.y < 0.0f) v.y *= 0.5f;

          // We have a vector saying which direction to go but it is not in world coordinates,
          // add it to our current heading vector (A whole matrix is probably not needed, just
          // makes it a lot easier)
          math::cMat4 m;
          m.SetRotationEuler(0.0f, 0.0f, fHorizontal);

          // Multiply our movement vector by our speed
          v = m.GetRotatedVec3(v) * fSpeed * 50.0f;

          // Get our current velocity add our movement velocity
          const dReal* vel = dBodyGetLinearVel(GetBody());
          dBodySetLinearVel(GetBody(), v.x, v.y, vel[2] + v.z);

          position = m.GetPosition();
        }
      } else {
        //const dReal* v0 = dBodyGetLinearVel(GetBody());
        //dBodySetLinearVel(GetBody(), 0.0f, 0.0f, v0[2]);
      }

      //dBodySetPosition(GetBody(), 20.0f, 20.0f, 20.0f);
      //position.Set(20.0f, 20.0f, 10.0f);//m.GetPosition();*/
    }
#endif
  }

  bool cPlayer::IsInACar() const
  {
    return (NULL != pSeat);
  }

  bool cPlayer::IsDriving() const
  {
    return ((NULL != pSeat) && (this == pSeat->pVehicle->vSeat[0]->pPlayer));
  }

  bool cPlayer::IsThirdPersonCamera() const
  {
#ifdef BUILD_RELEASE
    return (uiCameraMode == CAMERA::THIRDPERSON);
#else
    return (uiCameraMode == CAMERA::THIRDPERSON) || (uiCameraMode == CAMERA::THIRDPERSONFREE);
#endif
  }


  void cPlayer::ChangeStateToDriving()
  {
    state = STATE::DRIVE;

    // TODO: This doesn't make sense as while physics has a smart pointer to this, it will never be deleted so we will never get here
    //physics::RemovePhysicsObject(this);

    //biped.DestroyGeom();
    //biped.DestroyBody();

    //if (biped.geomRay) {
    //  dGeomDestroy(biped.geomRay);
    //  biped.geomRay = 0;
    //}
  }

  void cPlayer::ChangeStateToRunning()
  {
    state = STATE::RUN;

#ifdef BUILD_DEBUG
    if (uiCameraMode != CAMERA::FIRSTPERSONFREE) {
#endif
      //SetUseBody(true);
      //CreateCapsule(pWorld, position);
      // TODO: This doesn't make sense as while physics has a smart pointer to this, it will never be deleted so we will never get here
      // pWorld->AddPhysicsObject(this);

      //cPhysicsRayCast::Create(1.0f);
#ifdef BUILD_DEBUG
    }
#endif
  }

  void cPlayer::ChangeItemUp()
  {

  }

  void cPlayer::ChangeItemDown()
  {

  }


  /*bool cPlayer::InViewCone(const math::cVec3& vecSpot, float tolerance)
  {
    math::cVec3 lineOfSight(vecSpot - position);

    // Restrict to x/y axis only
    lineOfSight.z = 0;

    lineOfSight.Normalise();

    const math::cVec3 facingDir = GetViewAngle();

    const float flDot = lineOfSight.DotProduct(facingDir);
    if (flDot > tolerance) return true;

    return false;
  }*/
}
