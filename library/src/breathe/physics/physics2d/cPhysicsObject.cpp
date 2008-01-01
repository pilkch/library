#include <cmath>
#include <cassert>

#include <list>
#include <vector>

// writing on a text file
#include <iostream>
#include <fstream>

#include <Box2D/Box2D.h>


#include <breathe/breathe.h>

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
    cPhysicsObject::cPhysicsObject() :
      cObject(),
        
      //type(object_type_box),

			bBody(true),
			bDynamic(true),

      fFriction(0.3f),

      body(nullptr)
		{
			fWeight=1.0f;
			fRadius=2.0f;

			p[0]=0;
			p[1]=0;
			p[2]=0;

			v[0]=0;
			v[1]=0;
			v[2]=0;

			m.LoadIdentity();
		}

		cPhysicsObject::~cPhysicsObject()
		{
			RemoveFromWorld();
		}

		void cPhysicsObject::RemoveFromWorld()
		{
      RemovePhysicsObject(this);
      if (body != nullptr) {
        GetWorld()->DestroyBody(body);
        body = nullptr;
      }
		}

		void cPhysicsObject::InitCommon(b2ShapeDef& shapeDef, const physvec_t& pos, const physvec_t& rot)
		{
      assert((fWidth * fHeight) > math::cEPSILON);

      p.x = pos.x;
      p.y = pos.y;

			m.LoadIdentity();
			//m.SetRotationZ(rot.z * math::cPI_DIV_180);

			m.SetTranslation(p);

			if (bBody)
			{
        b2BodyDef bodyDef;
        bodyDef.position.Set(p.x, p.y);
        bodyDef.allowSleep = CanSleep();
        bodyDef.AddShape(&shapeDef);

        body = GetWorld()->CreateBody(&bodyDef);
			}
		}
		
		void cPhysicsObject::CreateBox(const physvec_t& pos, const physvec_t& rot)
		{
      if ((fWidth * fHeight) < math::cEPSILON) fWidth = fHeight = 1.0f;

      //type = object_type_box;

      b2BoxDef shapeDef;
      shapeDef.extents.Set(fWidth, fHeight);
      shapeDef.density = fWeight;
      shapeDef.friction = fFriction;

      InitCommon(shapeDef, pos, rot);
		}
		
		void cPhysicsObject::CreateSphere(const physvec_t& pos, const physvec_t& rot)
		{
      if ((fWidth * fHeight) < math::cEPSILON) fWidth = fHeight = 1.0f;

      //type = object_type_sphere;

      b2CircleDef shapeDef;
      shapeDef.radius = fRadius;
      shapeDef.density = fWeight;
      shapeDef.friction = fFriction;

      InitCommon(shapeDef, pos, rot);
		}

		void cPhysicsObject::CreateCapsule(const physvec_t& pos, const physvec_t& rot)
		{
      if ((fWidth * fHeight) < math::cEPSILON) fWidth = fHeight = 1.0f;

			assert(false);
		}

		void cPhysicsObject::CreateCylinder(const physvec_t& pos, const physvec_t& rot)
		{
      if ((fWidth * fHeight) < math::cEPSILON) fWidth = fHeight = 1.0f;

      CreateBox(pos, rot);
		}

		void cPhysicsObject::Update(sampletime_t currentTime)
		{
			if (bDynamic)
			{
				/*dReal *p0;
				dReal *r0;

				if (bBody)
				{
					p0=const_cast<dReal*>(dBodyGetPosition(body));
					r0=const_cast<dReal*>(dBodyGetRotation(body));
					const dReal *v0=dBodyGetLinearVel(body);
					//const dReal *a0=dBodyGetAngularVel(body);
					

					v[0]=v0[0];
					v[1]=v0[1];
					v[2]=v0[2];
				}
				else
				{
					p0=const_cast<dReal*>(dGeomGetPosition(geom));
					r0=const_cast<dReal*>(dGeomGetRotation(geom));

					//Trimeshes are static for the moment
					v[0]=0.0f;
					v[1]=0.0f;
					v[2]=0.0f;
				}

				m[0]  =     r0[0];
				m[1]  =     r0[4];
				m[2]  =     r0[8];
				m[3]  =     0;
				m[4]  =     r0[1];
				m[5]  =     r0[5];
				m[6]  =     r0[9];
				m[7]  =     0;
				m[8]  =     r0[2];
				m[9]  =     r0[6];
				m[10] =     r0[10];
				m[11] =     0;
				m[12] =     p0[0];
				m[13] =     p0[1];
				m[14] =     p0[2];
				m[15] =     1;*/
			}

			p = m.GetPosition();
		}
	}
}
