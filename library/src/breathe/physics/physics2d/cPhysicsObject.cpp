#include <cmath>
#include <cassert>

#include <list>
#include <vector>

#include <iostream>
#include <fstream>

#include <Box2D/Box2D.h>

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

    void cPhysicsObject::InitCommon(std::list<b2ShapeDef*> lShapes, const physvec_t& pos, const physvec_t& rot)
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
        body = GetWorld()->CreateBody(&bodyDef);

        std::list<b2ShapeDef*>::iterator iter = lShapes.begin();
        std::list<b2ShapeDef*>::iterator iterEnd = lShapes.end();
        b2ShapeDef* shape = nullptr;
        while (iter != iterEnd) {
          shape = *iter;
          shape->restitution = 0.1f;
          body->CreateShape(shape);

          iter++;
        }

        if (bDynamic) body->SetMassFromShapes();
      }
    }

    void cPhysicsObject::CreateBox(const physvec_t& pos, const physvec_t& rot)
    {
      if ((fWidth * fHeight) < math::cEPSILON) fWidth = fHeight = 1.0f;

      b2PolygonDef shapeDef;
      shapeDef.SetAsBox(fWidth, fHeight);
      shapeDef.density = fWeight;
      shapeDef.friction = fFriction;

      std::list<b2ShapeDef*> lShapes;
      lShapes.push_back(&shapeDef);
      InitCommon(lShapes, pos, rot);
    }

    void cPhysicsObject::CreateSphere(const physvec_t& pos, const physvec_t& rot)
    {
      if ((fWidth * fHeight) < math::cEPSILON) fWidth = fHeight = 1.0f;

      b2CircleDef shapeDef;
      shapeDef.radius = fRadius;
      shapeDef.density = fWeight;
      shapeDef.friction = fFriction;

      std::list<b2ShapeDef*> lShapes;
      lShapes.push_back(&shapeDef);
      InitCommon(lShapes, pos, rot);
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

    void cPhysicsObject::CreateHeightmap(const std::vector<float>& heightvalues, const physvec_t& scale, const physvec_t& pos)
    {
      size_t n = heightvalues.size();
      assert(n != 0);

      fWidth = n * scale.x;
      fHeight = math::cEPSILON;
      fWeight = 0.0f;
      bDynamic = false;

      std::list<b2ShapeDef*> lShapes;
      b2PolygonDef* pShapeDef = nullptr;

      const float fMinimumHeight = 1.0f;
      float fHeightCurrent = math::cEPSILON;
      float fHeightPrevious = fMinimumHeight + heightvalues[0] * scale.y;
      const float fWidthHalf = 0.5f * scale.x;
      for (size_t i = 1; i < n; i++) {
        fHeightCurrent = fMinimumHeight + heightvalues[i] * scale.y;
        if (fHeightCurrent < fMinimumHeight) fHeightCurrent = fMinimumHeight;

        if (fHeightCurrent > fHeight) fHeight = fHeightCurrent;

        pShapeDef = new b2PolygonDef;
        /*pShapeDef->vertexCount = 4;
        pShapeDef->vertices[0].Set(-fWidthHalf, 0.0f);
        pShapeDef->vertices[1].Set(+fWidthHalf, 0.0f);
        pShapeDef->vertices[2].Set(+fWidthHalf, fHeightCurrent);
        pShapeDef->vertices[3].Set(-fWidthHalf, fHeightPrevious);*/
        //pShapeDef->localPosition.x = scale.x * i;
        //pShapeDef->localPosition.y = -fMinimumHeight;
        b2Vec2 position(scale.x * i, -fMinimumHeight);
        pShapeDef->SetAsBox(scale.x, fHeightCurrent, position, 0);
        pShapeDef->density = 0.0f;
        pShapeDef->friction = fFriction;

        lShapes.push_back(pShapeDef);

        fHeightPrevious = fHeightCurrent;
      }

      const physvec_t newPosition = pos + physvec_t(0.0f, -fMinimumHeight);
      InitCommon(lShapes, pos, physveczero);

      // Now delete the shapes
      std::list<b2ShapeDef*>::iterator iter = lShapes.begin();
      std::list<b2ShapeDef*>::iterator iterEnd = lShapes.end();
      while (iter != iterEnd) {
        SAFE_DELETE(*iter);
        iter++;
      }
    }

    void cPhysicsObject::CreateCombinedShapes(std::list<b2ShapeDef*> lShapes, const physvec_t& pos, const physvec_t& rot)
    {
      if ((fWidth * fHeight) < breathe::math::cEPSILON) fWidth = fHeight = 1.0f;

      InitCommon(lShapes, pos, rot);
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
