#include <cassert>
#include <cmath>

// writing on a text file
#include <iostream>
#include <fstream>
#include <sstream>

#include <list>
#include <vector>

#include <Box2D/Box2D.h>

#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec2.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cPlane.h>
#include <breathe/math/cQuaternion.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cOctree.h>
#include <breathe/math/cColour.h>

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>

#include <breathe/physics/physics.h>

namespace breathe
{
	namespace physics
	{
    // *** Timing
    const unsigned int uiFrequencyHz = 60;
		const float fTimeStep = 1.0f / uiFrequencyHz;
    const unsigned int uiIterations = 10;

    unsigned int GetFrequencyHz() { return uiFrequencyHz; }
    float GetTimeStep() { return fTimeStep; }
    unsigned int GetIterations() { return uiIterations; }

    float fWidth = 100.0f;
    float fHeight = 100.0f;

    float GetWidth() { return fWidth; }
    float GetHeight() { return fHeight; }

    
    const bool bCanSleep = true;

    bool CanSleep() { return bCanSleep; }

		// *** Physics data
		const int iMaxContacts = 100;
		const float fFriction = 2000.0f;
		const float fBounce = 0.003f;
		const float fBounceVel = 0.00002f;
		const float fERP = 0.8f;
		const float fCFM = (float)(10e-5);
		const float fGravity = -9.8f;
		const float fDampTorque = 0.05f;
		const float fDampLinearVel = 0.001f;

    b2World* world;

		std::list<cPhysicsObject * >lPhysicsObject;

		
		
		// *** Functions

    b2World* GetWorld() { return world; }

		size_t size() { return lPhysicsObject.size(); }
		iterator begin() { return lPhysicsObject.begin(); }
		iterator end() { return lPhysicsObject.end(); }

    cPhysicsObject* border[4];

    cPhysicsObject* GetBorder0() { return border[0]; }
    cPhysicsObject* GetBorder1() { return border[1]; }
    cPhysicsObject* GetBorder2() { return border[2]; }
    cPhysicsObject* GetBorder3() { return border[3]; }

		cPhysicsObject* CreateBoundingWall(float x, float y, float width, float height)
		{
      cPhysicsObject* pObject = new cPhysicsObject;

      pObject->fWeight = 0.0f;
      pObject->SetDimensions(width, height);
      pObject->CreateBox(breathe::math::cVec2(x, y));
      
      return pObject;
		}

    void Init(float width, float height)
		{
      fWidth = width;
      fHeight = height;

      float fBorder = 1.0f;

      b2AABB aabb;
      aabb.minVertex.Set(-fBorder, -fBorder);
      aabb.maxVertex.Set(width + fBorder, height + fBorder);

      b2Vec2 gravity(0.0f, fGravity);  
      world = new b2World(aabb, gravity, CanSleep());

      // Bottom
      border[0] = CreateBoundingWall(width * 0.5f, -0.5f * fBorder, width * 0.5f, fBorder);

      // Top
      border[1] = CreateBoundingWall(width * 0.5f, height + 0.5f * fBorder, width * 0.5f, fBorder);

      // Left
      border[2] = CreateBoundingWall(-0.5f * fBorder, height * 0.5f, fBorder, height * 0.5f);

      // Right
      border[3] = CreateBoundingWall(width + 0.5f * fBorder, height * 0.5f, fBorder, height * 0.5f);
		}

		void Destroy()
		{
      SAFE_DELETE(border[0]);
      SAFE_DELETE(border[1]);
      SAFE_DELETE(border[2]);
      SAFE_DELETE(border[3]);

			SAFE_DELETE(world);
		}

		void AddPhysicsObject(cPhysicsObject *pPhysicsObject)
		{
			lPhysicsObject.push_back(pPhysicsObject);
		}
		
		void RemovePhysicsObject(cPhysicsObject *pPhysicsObject)
		{
			lPhysicsObject.remove(pPhysicsObject);
		}

		void Update(sampletime_t currentTime)
		{
      // Step the world
      world->Step(fTimeStep, uiIterations);

      // Now update our physics objects
			iterator iter = lPhysicsObject.begin();
			iterator iterEnd = lPhysicsObject.end();

      while (iterEnd != iter) {
        (*iter)->Update(currentTime);
        iter++;
			};
		}
	}
}
