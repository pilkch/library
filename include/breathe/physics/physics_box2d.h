#ifndef PHYSICS_BOX2D_H
#define PHYSICS_BOX2D_H

// Game engine issues
// http://www.box2d.org/wiki/index.php?title=Programming_FAQ#Game_Engine_Issues

// Explosion creator
/*class cCustomWorld : public b2World
{
public:
  cCustomWorld(b2AABB worldAABB, b2Vec2 gravity, bool doSleep) :
    b2World(worldAABB, gravity, doSleep)
  {
  }

  void explode(b2Vec2 position)
  {
    b2AABB aabb;
    b2Vec2 vMin(position - b2Vec2(100, 100));
    b2Vec2 vMax(position + b2Vec2(100, 100));
    aabb.minVertex = vMin;
    aabb.maxVertex = vMax;
    b2Shape* shapes;
    const size_t n = Query(aabb, &shapes, 100);
    for (size_t i = 0; i < n; i++) {
      b2Body& b = *shapes[i].GetBody();
      b2Vec2 fv = b.GetCenterPosition();
      fv -= position;
      fv.Normalize();
      fv *= 500000;
      b.WakeUp();
      b.ApplyForce(fv, b.GetCenterPosition());
    }
  }
};*/

// Standard headers
#include <list>

// Breathe headers
#include <breathe/physics/physics.h>

namespace breathe
{
  namespace box2d
  {
    // Forward declaration
    class cBody;
    typedef std::shared_ptr<cBody> cBodyRef;

    class cHeightmap;
    typedef std::shared_ptr<cHeightmap> cHeightmapRef;

    class cCar;
    typedef std::shared_ptr<cCar> cCarRef;

    class cRope;
    typedef std::shared_ptr<cRope> cRopeRef;

    class cWorld : public physics::cWorld
    {
    public:
      cWorld();

      b2World* GetWorld() { return pWorld; }

      bool CanSleep() const;

    private:
      virtual bool _Init(const spitfire::math::cVec2& worldDimensions);
      virtual void _Destroy();

      void CreateGround();
      void DestroyGround();

      virtual physics::cBodyRef _CreateBody(const physics::cBoxProperties& properties);
      virtual physics::cBodyRef _CreateBody(const physics::cSphereProperties& properties);
      virtual physics::cHeightmapRef _CreateHeightmap(const physics::cHeightmapProperties& properties);
      virtual physics::cCarRef _CreateCar(const physics::cCarProperties& properties);
      virtual physics::cRopeRef _CreateRope(const physics::cRopeProperties& properties);

      virtual void _DestroyBody(physics::cBodyRef pBody);
      virtual void _DestroyCar(physics::cCarRef pCar);
      virtual void _DestroyRope(physics::cRopeRef pRope);

      virtual void _CastRay(const spitfire::math::cRay2& ray, physics::cCollisionResult& result);

      virtual void _Update(durationms_t currentTime);

      b2World* pWorld;
    };

    // The physics object
    class cBody : public physics::cBody
    {
    public:
      cBody();

      b2Body* GetBody() { return pBody; }

      void CreateBox(cWorld* pWorld, const physics::cBoxProperties& properties);
      void CreateSphere(cWorld* pWorld, const physics::cSphereProperties& properties);

    private:
      NO_COPY(cBody);

      void InitCommon(std::list<b2PolygonShape*>& lShapes, const spitfire::math::cVec2& pos, float rot);

      virtual void _SetPositionAbsolute(const spitfire::math::cVec2& position);
      virtual void _SetRotationAbsolute(const float_t& fRotation);
      virtual void _SetVelocityAbsolute(const spitfire::math::cVec2& velocity);
      virtual void _SetRotationalVelocityAbsolute(const float_t& rotationalVelocity);
      virtual void _SetMassKg(float fMassKg);


      virtual void _AddForceRelativeToWorldKg(const spitfire::math::cVec2& forceKg);
      virtual void _AddTorqueRelativeToWorldNm(const float& torqueNm);

      virtual void _AddForceRelativeToBodyKg(const spitfire::math::cVec2& forceKg);
      virtual void _AddTorqueRelativeToBodyNm(const float& torqueNm);

      virtual void _Update(durationms_t currentTime);

      virtual void _Remove();

      cWorld* pWorld;

      bool bBody;
      bool bDynamic;
      float fFriction;
      float fRestitition;
      float fRadius;

      b2Body* pBody;
    };


    class cHeightmap : public physics::cHeightmap
    {
    public:
      explicit cHeightmap(const physics::cHeightmapProperties& properties);

      void CreateHeightmap(cWorld* pWorld);

    private:
      virtual void _Update(durationms_t currentTime);

      virtual void _Remove();

      cWorld* pWorld;

      b2Body* pBody;
    };


    class cCar : public physics::cCar
    {
    public:
      explicit cCar(cBodyRef pBody);

    private:
      void _Update(durationms_t currentTime);
    };


    // Rope
    //
    // Ropes are constructed like this:
    //
    // pAnchorBody0 - joints[0] - bodies[0] - joints[1] - bodies[1] - joints[2] - pAnchorBody1

    #define BUILD_USE_BOX2D_ROPE_JOINT

    class cRope : public physics::cRope
    {
    public:
      cRope();

      void Create(cWorld* _pWorld, const physics::cRopeProperties& properties);
      void Destroy();

      void GetRopeSegmentPositionAndRotations(std::vector<std::pair<spitfire::math::cVec2, float> >& positionAndRotations) const;

    protected:
      #ifdef BUILD_USE_BOX2D_ROPE_JOINT
      std::list<b2RopeJoint*> joints;
      #else
      std::list<b2DistanceJoint*> joints;
      #endif
      std::list<b2Body*> bodies;

    private:
      #ifdef BUILD_USE_BOX2D_ROPE_JOINT
      b2RopeJoint* CreateRopeJoint(b2Body* pBodyA, b2Body* pBodyB, const spitfire::math::cVec2& anchorPointA, const spitfire::math::cVec2& anchorPointB, float fLength);
      #else
      b2DistanceJoint* CreateRopeJoint(b2Body* pBodyA, b2Body* pBodyB, const spitfire::math::cVec2& anchorPointA, const spitfire::math::cVec2& anchorPointB, float fLength);
      #endif
      b2Body* CreateRopeBody(const spitfire::math::cVec2& position, float fRotationDegrees, float fWidth);

      void _Update(durationms_t currentTime);

      cWorld* pWorld;
    };
  }
}

#endif // PHYSICS_BOX2D_H
