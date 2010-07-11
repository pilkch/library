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

// Other libraries
#include <Box2D/Box2D.h>


// Spitfire
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/cString.h>
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

      const size_t n = 4;
      for (size_t i = 0; i < n; i++) border[i] = nullptr;
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

      b2Vec2 gravity(0.0f, fGravity);
      pWorld = new b2World(gravity, CanSleep());

      /*// Create edges around the entire screen
      b2BodyDef groundBodyDef;
      groundBodyDef.position.Set(0,0);
      b2Body* groundBody = pWorld->CreateBody(&groundBodyDef);
      b2PolygonShape groundBox;
      b2FixtureDef boxShapeDef;
      boxShapeDef.shape = &groundBox;
      groundBox.SetAsEdge(b2Vec2(0,0), b2Vec2(fWidth, 0));
      groundBody->CreateFixture(&boxShapeDef);
      groundBox.SetAsEdge(b2Vec2(0,0), b2Vec2(0, fHeight));
      groundBody->CreateFixture(&boxShapeDef);
      groundBox.SetAsEdge(b2Vec2(0, fHeight), b2Vec2(fWidth, fHeight));
      groundBody->CreateFixture(&boxShapeDef);
      groundBox.SetAsEdge(b2Vec2(fWidth, fHeight), b2Vec2(fWidth, 0));
      groundBody->CreateFixture(&boxShapeDef);*/

      /*// Bottom
      border[0] = CreateBoundingWall(fWidth * 0.5f, -0.5f * fBorder, fWidth * 0.5f, fBorder);

      // Top
      border[1] = CreateBoundingWall(fWidth * 0.5f, fHeight + 0.5f * fBorder, fWidth * 0.5f, fBorder);

      // Left
      border[2] = CreateBoundingWall(-0.5f * fBorder, fHeight * 0.5f, fBorder, fHeight * 0.5f);

      // Right
      border[3] = CreateBoundingWall(fWidth + 0.5f * fBorder, fHeight * 0.5f, fBorder, fHeight * 0.5f);*/

      return true;
    }

    void cWorld::_Destroy()
    {
      SAFE_DELETE(border[0]);
      SAFE_DELETE(border[1]);
      SAFE_DELETE(border[2]);
      SAFE_DELETE(border[3]);

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

    void cWorld::_DestroyBody(physics::cBodyRef pBody)
    {
      lPhysicsBody.remove(pBody);
    }

    void cWorld::_DestroyCar(physics::cCarRef pCar)
    {
      //lPhysicsBody.remove(pCar->GetBody());
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

    void cBody::_SetPositionAbsolute(const spitfire::math::cVec2& position)
    {
    }

    void cBody::_SetRotationAbsolute(const float_t& fRotation)
    {
    }

    void cBody::_SetMassKg(float fMassKg)
    {
    }

    void cBody::_AddForceRelativeToWorldKg(const spitfire::math::cVec2& forceKg)
    {
      //b2Vec2 direction = rightWheel->GetTransform().R.col2;
      //direction *= forceKg;
      const b2Vec2 force(forceKg.x, forceKg.y);
      pBody->ApplyForce(force, pBody->GetPosition());
    }

    void cBody::_AddTorqueRelativeToWorldNm(const spitfire::math::cVec2& torqueNm)
    {
    }

    void cBody::_AddForceRelativeToObjectKg(const spitfire::math::cVec2& forceKg)
    {
    }

    void cBody::_AddTorqueRelativeToObjectNm(const spitfire::math::cVec2& torqueNm)
    {
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

      b2BodyDef bodyDef;
      bodyDef.type = b2_dynamicBody;
      bodyDef.position.Set(properties.position.x, properties.position.y);
      //bodyDef.userData = this;
      pBody = pWorld->GetWorld()->CreateBody(&bodyDef);

      b2PolygonShape shapeDef;
      shapeDef.SetAsBox(properties.fWidthMetres, properties.fHeightMetres);

      b2FixtureDef fixtureDef;
      fixtureDef.shape = &shapeDef;
      fixtureDef.density = fMassKg;
      fixtureDef.friction = properties.fFriction;
      fixtureDef.restitution = properties.fRestitution;
      pBody->CreateFixture(&fixtureDef);
    }

    void cBody::CreateSphere(cWorld* _pWorld, const physics::cSphereProperties& properties)
    {
      pWorld = _pWorld;

      b2BodyDef bodyDef;
      bodyDef.type = b2_dynamicBody;
      bodyDef.position.Set(properties.position.x, properties.position.y);
      //bodyDef.userData = this;
      pBody = pWorld->GetWorld()->CreateBody(&bodyDef);

      b2CircleShape shapeDef;
      shapeDef.m_radius = properties.fRadiusMetres;

      b2FixtureDef fixtureDef;
      fixtureDef.shape = &shapeDef;
      fixtureDef.density = properties.fMassKg;
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
        b2Vec2 vertices[4];
        const float x0 = scale.x * float(i);
        const float y0 = scale.y * values[i];
        const float x1 = scale.x * float(i + 1);
        const float y1 = scale.y * values[i + 1];
        vertices[0].Set(x0, 0.0f);
        vertices[1].Set(x1, 0.0f);
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
  }
}

