#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <vector>
#include <map>
#include <list>
#include <set>

// Boost headers
#include <boost/smart_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

// OpenGL
#include <GL/GLee.h>


// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
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
#include <breathe/render/model/cStatic.h>


#include <breathe/game/cLevel.h>

#include <breathe/physics/physics.h>

#include <breathe/game/cPlayer.h>
#include <breathe/game/cPetrolBowser.h>
#include <breathe/vehicle/cPart.h>
#include <breathe/vehicle/cWheel.h>
#include <breathe/vehicle/cSeat.h>
#include <breathe/vehicle/cVehicle.h>

namespace breathe
{
  namespace vehicle
  {
    cSeat::cSeat(cVehicle2* v) :
      pPlayer(nullptr)
    {
      ASSERT(v != nullptr);
      pVehicle = v;
    }

    void cSeat::AssignPlayer(cPlayer* p)
    {
      ASSERT(p != nullptr);

      pPlayer = p;
      p->pSeat = this;

      if (pVehicle->vSeat[0] == this) pPlayer->ChangeStateToDriving();
    }

    void cSeat::EjectPlayer()
    {
      ASSERT(pPlayer != nullptr);

      // Store the previous player for later
      cPlayer* pPreviousPlayer = pPlayer;

      // Empty the seat
      pPlayer = nullptr;

      // Make sure the player knows that he is not in the seat any more and move him out of it
      pPreviousPlayer->pSeat = nullptr;
      //pPreviousPlayer->position = pVehicle->position + math::cVec3(0.0f, 0.0f, 3.0f);

      // Set the state of the player to a pedestrian
      pPreviousPlayer->ChangeStateToRunning();
    }
  }
}
