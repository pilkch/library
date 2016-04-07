// Basic standard library includes
#include <cmath>

// STL
#include <list>
#include <vector>
#include <map>

#include <iostream>
#include <fstream>
#include <sstream>

// Anything else
#include <ode/ode.h>

// Spitfire headers
#include <spitfire/spitfire.h>

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
    cPhysicsRayCast::cPhysicsRayCast(cWorld* pWorld) :
      rayContact(pWorld)
    {
      geomRay = 0;
    }

    cPhysicsRayCast::~cPhysicsRayCast()
    {
      if (geomRay) {
        dGeomDestroy(geomRay);
        geomRay = 0;
      }
    }

    void cPhysicsRayCast::Create(float fLength)
    {
      rayContact.Clear();

      geomRay = dCreateRay(0, fLength);
    }
  }
}
