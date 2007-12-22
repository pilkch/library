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

		size_t size() { return lPhysicsObject.size(); }
		iterator begin() { return lPhysicsObject.begin(); }
		iterator end() { return lPhysicsObject.end(); }

    b2World* GetWorld() { return world; }

		void CreateGround(float posX, float posY, float posZ, float nX, float nY, float nZ)
		{
			breathe::math::cVec3 p(posX, posY, posZ);
			breathe::math::cVec3 n(nX, nY, nZ);

			n.Normalize();


		}

		void Init()
		{
      b2AABB aabb;
      aabb.minVertex.Set(-10.0f, -10.0f);
      aabb.maxVertex.Set(10.0f, 10.0f);

      b2Vec2 gravity(0.0f, fGravity);
      bool doSleep = true;
  
      world = new b2World(aabb, gravity, doSleep);
		}

		void Destroy()
		{
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
      world->Step(fTimeStep, uiIterations);

      /*for (uint32_t i = 0; i < 60; ++i)
      {
        b2Vec2 position = body->GetOriginPosition();
        float32_t rotation = body->GetRotation();
        printf("%4.2f %4.2f %4.2f\n", position.x, position.y, rotation);
      }*/
		}
	}
}
