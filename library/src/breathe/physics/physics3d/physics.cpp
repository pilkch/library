#include <cmath>

#include <iostream>
#include <fstream>
#include <sstream>

#include <list>
#include <vector>

// Boost includes
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

//#include <opal/opal.h>

#include <ode/ode.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

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

#include <breathe/physics/physics.h>

namespace breathe
{
  namespace physics
  {
    // *** Timing
    const unsigned int uiFrequencyHz = 20;
    const float fInterval = 1000.0f / uiFrequencyHz;

    // *** Physics data
    const int iMaxContacts = 100;
    const float fFriction = 2000.0f;
    const float fBounce = 0.003f;
    const float fBounceVel = 0.00002f;
    const float fERP = 0.8f;
    const float fCFM = (float)(10e-5);
    const float fGravity = -9.80665f;
    const float fDampTorque = 0.02f;
    const float fDampLinearVel = 0.02f;

    dWorldID world = 0;
    dSpaceID spaceStatic = 0;
    dSpaceID spaceDynamic = 0;
    dJointGroupID contactgroup = 0;
    dGeomID ground = 0;



    // *** World

    size_t cWorld::GetFrequencyHz() const
    {
      return uiFrequencyHz;
    }

    float cWorld::GetInterval() const
    {
      return fInterval;
    }

    dSpaceID cWorld::GetSpaceStatic() { return spaceStatic; }
    dSpaceID cWorld::GetSpaceDynamic() { return spaceDynamic; }

    dWorldID cWorld::GetWorld() { return world; }

    dJointGroupID cWorld::GetContactGroup() { return contactgroup; }

    size_t cWorld::size() const { return lPhysicsObject.size(); }
    iterator cWorld::begin() { return lPhysicsObject.begin(); }
    iterator cWorld::end() { return lPhysicsObject.end(); }

    // This wrapper is called by the physics library to get information
    // about object collisions
    void nearCallbackStatic(void *pData, dGeomID o1, dGeomID o2);
    void nearCallbackDynamic(void *pData, dGeomID o1, dGeomID o2);

    // This actually implements the collision callback
    void nearCallbackStatic(dGeomID o1, dGeomID o2);
    void nearCallbackDynamic(dGeomID o1, dGeomID o2);


    void cWorld::CreateGround(float posX, float posY, float posZ, float nX, float nY, float nZ)
    {
      breathe::math::cVec3 p(posX, posY, posZ);
      breathe::math::cVec3 n(nX, nY, nZ);

      n.Normalise();

      ground = dCreatePlane(spaceStatic, n.x, n.y, n.z, n.DotProduct(p));
    }

    void cWorld::Init(float width, float height, float depth)
    {
      LOG<<"physics::cWorld::Init"<<std::endl;

      dInitODE2(0);
      world = dWorldCreate();

      dWorldSetGravity(world, 0, 0, fGravity);

      // This function sets the depth of the surface layer around the world objects. Contacts are allowed to sink into
      // each other up to this depth. Setting it to a small value reduces the amount of jittering between contacting
      // objects, the default value is 0.
      dWorldSetContactSurfaceLayer(world, 0.001f);

      dWorldSetERP(world, fERP);
      dWorldSetCFM(world, fCFM);

      dWorldSetContactMaxCorrectingVel(world, 1.0f);
      dWorldSetAutoDisableFlag(world, 1);

      spaceStatic = dHashSpaceCreate(0);
      spaceDynamic = dHashSpaceCreate(0);

      contactgroup = dJointGroupCreate(10000);

      CreateGround(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    void cWorld::Destroy()
    {
      lPhysicsObject.clear();

      dJointGroupDestroy(contactgroup);

      dSpaceDestroy(spaceDynamic);
      dSpaceDestroy(spaceStatic);

      dWorldDestroy(world);

      dCloseODE();
    }

    void cWorld::AddPhysicsObject(cPhysicsObjectRef pPhysicsObject)
    {
      lPhysicsObject.push_back(pPhysicsObject);
    }

    void cWorld::RemovePhysicsObject(cPhysicsObjectRef pPhysicsObject)
    {
      lPhysicsObject.remove(pPhysicsObject);
    }

    void cWorld::Update(sampletime_t currentTime)
    {
      {
        // First Iteration
        // Generic rotational and linear dampening

        iterator iter = lPhysicsObject.begin();
        const iterator iterEnd = lPhysicsObject.end();
        while(iterEnd != iter) {
          ASSERT((*iter)->HasBody());

          dBodyID b = (*iter)->GetBody();
          if (b != NULL) {
            dReal const* av = dBodyGetAngularVel(b);
            dBodySetAngularVel(b, av[0] - av[0] * fDampTorque, av[1] - av[1] * fDampTorque, av[2] - av[2] * fDampTorque);
            dReal const* lv = dBodyGetLinearVel(b);
            dBodySetLinearVel(b, lv[0] - lv[0] * fDampLinearVel, lv[1] - lv[1] * fDampLinearVel, lv[2] - lv[2] * fDampLinearVel);
          }

          iter++;
        };
      }

      // For triggers
      // Was using an object with this code
      // dSpaceCollide(spaceDynamic, this, nearCallbackDynamic);
      // dSpaceCollide2((dGeomID)spaceDynamic, (dGeomID)spaceStatic, this, &nearCallbackStatic);
      // dSpaceCollide2((dGeomID)spaceDynamic, (dGeomID)spaceTrigger, this, &nearCallbackTrigger);

      dSpaceCollide(spaceDynamic, NULL, nearCallbackDynamic);
      dSpaceCollide2((dGeomID)spaceDynamic, (dGeomID)spaceStatic, NULL, &nearCallbackStatic);

      // For triggers
      //dSpaceCollide2((dGeomID)spaceDynamic, (dGeomID)spaceTrigger, NULL, &nearCallbackTrigger);

      dWorldQuickStep(world, GetInterval() / 1000.0f);
      dJointGroupEmpty(contactgroup);




      {
        std::list<cPhysicsObjectRef>::iterator iter = lPhysicsObject.begin();
        const std::list<cPhysicsObjectRef>::iterator iterEnd = lPhysicsObject.end();
        while (iter != iterEnd) {
          (*iter)->Update(currentTime);

          iter++;
        }
      }
    }

    void nearCallbackStatic(void* f, dGeomID o1, dGeomID o2)
    {
      //Ignore collisions between NULL geometry
      if (!(o1 && o2)) {
        LOG.Error("nearCallbackStatic", "NULL geometry");
        return;
      }

      //Ignore collisions between bodies that are connected by the same joint
      dBodyID Body1 = NULL;
      dBodyID Body2 = NULL;

      if (o1) Body1 = dGeomGetBody (o1);
      if (o2) Body2 = dGeomGetBody (o2);

      if (Body1 && Body2 && dAreConnected (Body1, Body2)) return;

      const int N = iMaxContacts;
      dContact contact[N];

      const size_t n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));
      for (size_t i = 0; i < n; i++) {
        contact[i].surface.mode = dContactBounce;
        contact[i].surface.mu = fFriction;
        contact[i].surface.bounce = fBounce;
        contact[i].surface.bounce_vel = fBounceVel;

        dJointID c = dJointCreateContact(world, contactgroup, &contact[i]);
        dJointAttach(c, Body1, Body2);
      }
    }

    void nearCallbackDynamic(void* f, dGeomID o1, dGeomID o2)
    {
      //Ignore collisions between NULL geometry
      if (!(o1 && o2)) {
        LOG.Error("nearCallbackDynamic", "NULL geometry");
        return;
      }

      //Ignore collisions between bodies that are connected by the same joint
      dBodyID Body1 = NULL;
      dBodyID Body2 = NULL;

      if (o1) Body1 = dGeomGetBody(o1);
      if (o2) Body2 = dGeomGetBody(o2);

      if (Body1 && Body2 && dAreConnected(Body1, Body2)) return;

      const int N = iMaxContacts;
      dContact contact[N];

      const size_t n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));
      for (size_t i = 0; i < n; i++) {
        contact[i].surface.mode = dContactBounce;
        contact[i].surface.mu = fFriction;
        contact[i].surface.bounce = fBounce;
        contact[i].surface.bounce_vel = fBounceVel;

        dJointID c = dJointCreateContact(world, contactgroup, &contact[i]);
        dJointAttach(c, Body1, Body2);
      }
    }
  }
}
