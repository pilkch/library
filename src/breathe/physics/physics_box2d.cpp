// Standard libraries
#include <cmath>
#include <cassert>

#include <vector>
#include <map>
#include <list>
#include <set>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

// Boost includes
#include <boost/smart_ptr.hpp>

// Box2D headers
#include <Box2D/Box2D.h>
#include <Box2D/Rope/b2Rope.h>

// Spitfire
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/string.h>
#include <spitfire/util/log.h>

#include <spitfire/storage/filesystem.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/geometry.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>

#include <breathe/physics/physics.h>
#include <breathe/physics/physics_box2d.h>


namespace breathe
{
  namespace box2d
  {
    // *** Timing
    const unsigned int uiFrequencyHz = 60;
    const float fTimeStep = 1.0f / uiFrequencyHz;
    const uint32_t velocityIterations = 6;
    const uint32_t positionIterations = 2;

    unsigned int GetFrequencyHz() { return uiFrequencyHz; }
    float GetTimeStep() { return fTimeStep; }
    unsigned int GetIterations() { return positionIterations; }

    float fWidth = 100.0f;
    float fHeight = 100.0f;

    float GetWidth() { return fWidth; }
    float GetHeight() { return fHeight; }


    const bool bCanSleep = true;

    // *** Physics data
    const int iMaxContacts = 100;
    const float fFriction = 2000.0f;
    const float fBounce = 0.003f;
    const float fBounceVel = 0.00002f;
    const float fERP = 0.8f;
    const float fCFM = (float)(10e-5);
    const float fGravity = -9.80665f;
    const float fDampTorque = 0.05f;
    const float fDampLinearVel = 0.001f;

    std::list<cBody*> lPhysicsObject;
    typedef std::list<cBody*>::iterator iterator;

    size_t size() { return lPhysicsObject.size(); }
    iterator begin() { return lPhysicsObject.begin(); }
    const iterator end() { return lPhysicsObject.end(); }

    cWorld::cWorld() :
      pWorld(nullptr)
    {
      // Timing
      //uiFrequencyHz = 20;
      //fIntervalMS = 1000.0f / float(uiFrequencyHz);
    }

    bool cWorld::CanSleep() const
    {
      return bCanSleep;
    }

    /*cBody* CreateBoundingWall(float x, float y, float width, float height)
    {
      cBody* pObject = new cBody;

      pObject->fMassKg = 0.0f;
      pObject->SetDimensions(width, height);
      pObject->CreateBox(breathe::math::cVec2(x, y));

      return pObject;
    }*/

    bool cWorld::_Init(const spitfire::math::cVec2& worldDimensions)
    {
      fWidth = worldDimensions.x;
      fHeight = worldDimensions.y;

      const b2Vec2 gravity(0.0f, fGravity);
      pWorld = new b2World(gravity);

      // Create edges around the entire screen
      b2BodyDef groundBodyDef;
      groundBodyDef.position.Set(0,0);
      b2Body* groundBody = pWorld->CreateBody(&groundBodyDef);
      b2EdgeShape groundBox;
      groundBox.Set(b2Vec2(0,0), b2Vec2(fWidth, 0));
      groundBody->CreateFixture(&groundBox, 0);
      groundBox.Set(b2Vec2(0,0), b2Vec2(0, fHeight));
      groundBody->CreateFixture(&groundBox, 0);
      groundBox.Set(b2Vec2(0, fHeight), b2Vec2(fWidth, fHeight));
      groundBody->CreateFixture(&groundBox, 0);
      groundBox.Set(b2Vec2(fWidth, fHeight), b2Vec2(fWidth, 0));
      groundBody->CreateFixture(&groundBox, 0);

      return true;
    }

    void cWorld::_Destroy()
    {
      SAFE_DELETE(pWorld);
    }

    /*void cWorld::CreateGround()
    {
      const math::cVec2 position(0.0f, 0.0f);
      const math::cVec2 normal(math::v2Up);

      ground = dCreatePlane(spaceStatic, normal.x, normal.y, normal.z, normal.DotProduct(position));
    }

    void cWorld::DestroyGround()
    {
      dGeomDestroy(ground);
      ground = NULL;
    }*/

    physics::cBodyRef cWorld::_CreateBody(const physics::cBoxProperties& properties)
    {
      box2d::cBodyRef pBody(new box2d::cBody);
      pBody->CreateBox(this, properties);

      lPhysicsBody.push_back(pBody);

      return pBody;
    }

    physics::cBodyRef cWorld::_CreateBody(const physics::cSphereProperties& properties)
    {
      box2d::cBodyRef pBody(new box2d::cBody);
      pBody->CreateSphere(this, properties);

      lPhysicsBody.push_back(pBody);

      return pBody;
    }

    physics::cHeightmapRef cWorld::_CreateHeightmap(const physics::cHeightmapProperties& properties)
    {
      box2d::cHeightmapRef pHeightmap(new box2d::cHeightmap(properties));
      pHeightmap->CreateHeightmap(this);

      //lPhysicsHeightmap.push_back(pHeightmap);

      return pHeightmap;
    }

    physics::cCarRef cWorld::_CreateCar(const physics::cCarProperties& properties)
    {
      box2d::cBodyRef pBody(new box2d::cBody);

      lPhysicsBody.push_back(pBody);

      box2d::cCarRef pCar(new box2d::cCar(pBody));

      return pCar;
    }

    physics::cRopeRef cWorld::_CreateRope(const physics::cRopeProperties& properties)
    {
      box2d::cRopeRef pRope(new box2d::cRope);

      pRope->Create(this, properties);

      return pRope;
    }

    void cWorld::_DestroyBody(physics::cBodyRef pBody)
    {
      lPhysicsBody.remove(pBody);
    }

    void cWorld::_DestroyCar(physics::cCarRef pCar)
    {
      //lPhysicsBody.remove(pCar->GetBody());
    }

    void cWorld::_DestroyRope(physics::cRopeRef pRope)
    {
      cRope* pRopeBox2D = static_cast<cRope*>(pRope.get());

      pRopeBox2D->Destroy();

      //lPhysicsBody.remove(pRope->GetBody());
    }


    // This class captures the closest hit shape.

    class cRayCastCallback : public b2RayCastCallback
    {
    public:
      explicit cRayCastCallback(physics::cCollisionResult& _result) :
        result(_result),
        m_fixture(nullptr)
      {
      }

      float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
      {
        /*
        // Ignore polygon with index 0
        b2Body* body = fixture->GetBody();
        void* pUserData = body->GetUserData();
        if (pUserData) {
          int32 index = *(int32*)pUserData;
          if (index == 0) {
            // filter
            return -1.0f;
          }
        }*/

        result.SetIsIntersection();
        result.SetIntersectionPoint(spitfire::math::cVec2(point.x, point.y));
        result.SetIntersectionNormal(spitfire::math::cVec2(normal.x, normal.y));

        m_fixture = fixture;
        m_fraction = fraction;

        return fraction;
      }

      physics::cCollisionResult& result;
      b2Fixture* m_fixture;
      float32 m_fraction;
    };

    void cWorld::_CastRay(const spitfire::math::cRay2& ray, physics::cCollisionResult& result)
    {
      cRayCastCallback callback(result);

      const spitfire::math::cVec2& origin = ray.GetOrigin();
      const b2Vec2 point1(origin.x, origin.y);

      const spitfire::math::cVec2& destination = ray.GetOrigin() + (ray.GetLength() * ray.GetDirection());
      const b2Vec2 point2(destination.x, destination.y);

      pWorld->RayCast(&callback, point1, point2);

      if (result.IsIntersection()) result.SetIntersectionLength((ray.GetOrigin() - result.GetIntersectionPoint()).GetLength());
    }

    void cWorld::_Update(sampletime_t currentTime)
    {
      // Step the world
      pWorld->Step(fTimeStep, velocityIterations, positionIterations);

      pWorld->ClearForces();

      // Now update our physics objects
      iterator iter = lPhysicsObject.begin();
      iterator iterEnd = lPhysicsObject.end();

      while (iterEnd != iter) {
        (*iter)->Update(currentTime);
        iter++;
      };

      /*
      for (b2Body* b = pWorld->GetBodyList(); b != nullptr; b = b->GetNext()) {
        cBody* pBody = static_cast<cBody*>(b->GetUserData());
        if (pBody != nullptr) {
          pBody->position.Set(b->GetPosition().x, b->GetPosition().y);
          pBody->rotation = -spitfire::math::RadiansToDegrees(b->GetAngle());
        }
      }
      */
    }







    cBody::cBody() :

      pWorld(nullptr),

      bBody(true),
      bDynamic(true),

      fFriction(0.3f),

      pBody(nullptr)
    {
      fMassKg = 1.0f;
      fRadius = 2.0f;
    }

    void cBody::_Remove()
    {
      ASSERT(pWorld != nullptr);

      if (pBody != nullptr) {
        pWorld->GetWorld()->DestroyBody(pBody);
        pBody = nullptr;
      }
    }

    void cBody::_SetPositionAbsolute(const spitfire::math::cVec2& _position)
    {
      const b2Vec2 position(_position.x, _position.y);
      pBody->SetTransform(position, pBody->GetAngle());
    }

    void cBody::_SetRotationAbsolute(const float_t& fRotation)
    {
      pBody->SetTransform(pBody->GetPosition(), spitfire::math::DegreesToRadians(fRotation));
    }

    void cBody::_SetVelocityAbsolute(const spitfire::math::cVec2& _velocity)
    {
      const b2Vec2 velocity(_velocity.x, _velocity.y);
      pBody->SetLinearVelocity(velocity);
    }

    void cBody::_SetRotationalVelocityAbsolute(const float_t& rotationalVelocity)
    {
      const float_t fOmega = rotationalVelocity;
      pBody->SetAngularVelocity(fOmega);
    }

    void cBody::_SetMassKg(float fMassKg)
    {
    }

    void cBody::_AddForceRelativeToWorldKg(const spitfire::math::cVec2& forceKg)
    {
      const b2Vec2 force(forceKg.x, forceKg.y);
      pBody->ApplyForce(force, pBody->GetPosition());
    }

    void cBody::_AddTorqueRelativeToWorldNm(const float& torqueNm)
    {
      pBody->ApplyTorque(torqueNm);
    }

    void cBody::_AddForceRelativeToBodyKg(const spitfire::math::cVec2& forceKg)
    {
      b2Transform transform = pBody->GetTransform();
      const b2Vec2 forceInRelativeDirection = b2Mul(transform.q, b2Vec2(forceKg.x, forceKg.y));
      pBody->ApplyForce(forceInRelativeDirection, pBody->GetPosition());
    }

    void cBody::_AddTorqueRelativeToBodyNm(const float& torqueNm)
    {
      pBody->ApplyTorque(torqueNm);
    }

    /*void cBody::InitCommon(std::list<b2ShapeDef*>& lShapes, const spitfire::math::cVec2& pos, float rot)
    {
      assert((fWidth * fHeight) > math::cEPSILON);

      p.x = pos.x;
      p.y = pos.y;

      m.LoadIdentity();
      //m.SetRotationZ(rot.z * math::cPI_DIV_180);

      m.SetTranslation(p);

      if (bBody) {
        b2BodyDef bodyDef;
        bodyDef.position.Set(p.x, p.y);
        bodyDef.allowSleep = pWorld->CanSleep();
        pBody = pWorld->GetWorld()->CreateBody(&bodyDef);

        std::list<b2ShapeDef*>::iterator iter = lShapes.begin();
        const std::list<b2ShapeDef*>::iterator iterEnd = lShapes.end();
        b2ShapeDef* pShape = nullptr;
        while (iter != iterEnd) {
          pShape = *iter;
          pShape->restitution = 0.1f;
          pBody->CreateShape(pShape);

          iter++;
        }

        if (bDynamic) pBody->SetMassFromShapes();
      }
    }*/

    void cBody::CreateBox(cWorld* _pWorld, const physics::cBoxProperties& properties)
    {
      pWorld = _pWorld;

      // Set properties
      position = properties.position;
      rotation = properties.rotation;
      fMassKg = properties.fMassKg;

      b2BodyDef bodyDef;
      bodyDef.type = properties.bIsDynamic ? b2_dynamicBody : b2_staticBody;
      bodyDef.position.Set(properties.position.x, properties.position.y);
      //bodyDef.linearDamping = 0.0f;
      bodyDef.angularDamping = 0.01f;
      //bodyDef.userData = this;
      pBody = pWorld->GetWorld()->CreateBody(&bodyDef);

      b2PolygonShape shapeDef;
      shapeDef.SetAsBox(properties.fWidthMetres, properties.fHeightMetres);

      b2FixtureDef fixtureDef;
      fixtureDef.shape = &shapeDef;
      fixtureDef.density = properties.bIsDynamic ? properties.fMassKg : 0.0f; // TODO: This should be the actual density, not the total mass
      fixtureDef.friction = properties.fFriction;
      fixtureDef.restitution = properties.fRestitution;
      pBody->CreateFixture(&fixtureDef);
    }

    void cBody::CreateSphere(cWorld* _pWorld, const physics::cSphereProperties& properties)
    {
      pWorld = _pWorld;

      // Set properties
      position = properties.position;
      rotation = properties.rotation;
      fMassKg = properties.fMassKg;

      b2BodyDef bodyDef;
      bodyDef.type = properties.bIsDynamic ? b2_dynamicBody : b2_staticBody;
      bodyDef.position.Set(properties.position.x, properties.position.y);
      //bodyDef.linearDamping = 0.0f;
      bodyDef.angularDamping = 0.01f;
      //bodyDef.userData = this;
      pBody = pWorld->GetWorld()->CreateBody(&bodyDef);

      b2CircleShape shapeDef;
      shapeDef.m_radius = properties.fRadiusMetres;

      b2FixtureDef fixtureDef;
      fixtureDef.shape = &shapeDef;
      fixtureDef.density = properties.bIsDynamic ? properties.fMassKg : 0.0f; // TODO: This should be the actual density, not the total mass
      fixtureDef.friction = properties.fFriction;
      fixtureDef.restitution = properties.fRestitution;
      pBody->CreateFixture(&fixtureDef);
    }

#if 0
    void cBody::CreateCapsule(const physvec_t& pos, const physvec_t& rot)
    {
      assert(false);
    }

    void cBody::CreateCylinder(const physvec_t& pos, const physvec_t& rot)
    {
      CreateBox(pos, rot);
    }

    void cBody::CreateCombinedShapes(std::list<b2ShapeDef*> lShapes, const cCombinedShapesProperties& properties)
    {
      InitCommon(lShapes, properties.position, properties.rotation);
    }
#endif

    void cBody::_Update(sampletime_t currentTime)
    {
      assert(pBody != nullptr);

      position.x = pBody->GetPosition().x;
      position.y = pBody->GetPosition().y;

      rotation = spitfire::math::RadiansToDegrees(pBody->GetAngle());

      velocity.x = pBody->GetLinearVelocity().x;
      velocity.y = pBody->GetLinearVelocity().y;
    }


    cHeightmap::cHeightmap(const physics::cHeightmapProperties& properties) :
      physics::cHeightmap(properties),

      pWorld(nullptr),

      pBody(nullptr)
    {
    }

    void cHeightmap::CreateHeightmap(cWorld* _pWorld)
    {
      pWorld = _pWorld;

      b2BodyDef bodyDef;
      bodyDef.type = b2_staticBody;
      bodyDef.position.Set(0.0f, 0.0f);//position.x, position.y);
      //bodyDef.userData = this;
      pBody = pWorld->GetWorld()->CreateBody(&bodyDef);

      assert(width != 0);
      const size_t n = width - 1;
      for (size_t i = 0; i < n; i++) {
        // Box2D accepts polygons with points specified in CCW order
        // We drop the bottom of the polygons to -1.0f to ensure that they are at least 1 unit tall
        b2Vec2 vertices[4];
        const float x0 = scale.x * float(i);
        const float y0 = scale.y * values[i];
        const float x1 = scale.x * float(i + 1);
        const float y1 = scale.y * values[i + 1];
        ASSERT(y0 >= 0.0f);
        ASSERT(y1 >= 0.0f);
        vertices[0].Set(x0, -1.0f);
        vertices[1].Set(x1, -1.0f);
        vertices[2].Set(x1, y1);
        vertices[3].Set(x0, y0);

        int32_t count = 4;

        b2PolygonShape shapeDef;
        shapeDef.Set(vertices, count);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shapeDef;
        fixtureDef.friction = fFriction;
        fixtureDef.restitution = fRestitution;
        pBody->CreateFixture(&fixtureDef);
      }
    }

    void cHeightmap::_Update(sampletime_t currentTime)
    {
    }

    void cHeightmap::_Remove()
    {
      ASSERT(pWorld != nullptr);

      if (pBody != nullptr) {
        pWorld->GetWorld()->DestroyBody(pBody);
        pBody = nullptr;
      }
    }


    cCar::cCar(cBodyRef _pBody)
    {
      pChassis = _pBody;
    }

    void cCar::_Update(sampletime_t currentTime)
    {
    }


    cRope::cRope() :
      pWorld(nullptr)
    {
    }

    void cRope::Create(cWorld* _pWorld, const physics::cRopeProperties& properties)
    {
      pWorld = _pWorld;

      pAnchorBody0 = properties.pAnchorBody0;
      pAnchorBody1 = properties.pAnchorBody1;

      cBody* pBody0 = static_cast<cBody*>(pAnchorBody0.get());
      cBody* pBody1 = static_cast<cBody*>(pAnchorBody1.get());

      b2Body* pBodyA = pBody0->GetBody();
      b2Body* pBodyB = pBody1->GetBody();
      const b2Vec2 anchorA(properties.anchorPoint0.x, properties.anchorPoint0.y);
      const b2Vec2 anchorB(properties.anchorPoint1.x, properties.anchorPoint1.y);

      b2RopeJointDef definition;
      definition.bodyA = pBodyA;
      definition.bodyB = pBodyB;
      definition.localAnchorA = anchorA;
      definition.localAnchorB = anchorB;

      // Max length of joint = current distance between bodies * sag
      float32 ropeLength = (pBodyA->GetWorldPoint(anchorA) - pBodyB->GetWorldPoint(anchorB)).Length() * properties.fSag;
      definition.maxLength = ropeLength;

      // Create joint
      b2RopeJoint* pRopeJoint = (b2RopeJoint*)pWorld->GetWorld()->CreateJoint(&definition);

      joints.push_back(pRopeJoint);
    }

    void cRope::Destroy()
    {
      // Destroy the rope joints
      std::list<b2RopeJoint*>::iterator iter = joints.begin();
      const std::list<b2RopeJoint*>::iterator iterEnd = joints.end();
      while (iter != iterEnd) {
        pWorld->GetWorld()->DestroyJoint(*iter);

        iter++;
      }

      joints.clear();
    }

    void cRope::_Update(sampletime_t currentTime)
    {
    }
  }
}

