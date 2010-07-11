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

#include <breathe/physics/physics.h>

namespace breathe
{
  namespace box2d
  {
    // Forward declaration
    class cBody;
    typedef cSmartPtr<cBody> cBodyRef;

    class cHeightmap;
    typedef cSmartPtr<cHeightmap> cHeightmapRef;

    class cCar;
    typedef cSmartPtr<cCar> cCarRef;

    class cWorld : public physics::cWorld
    {
    public:
      cWorld();

      b2World* GetWorld() { return pWorld; }

      cBody* GetBorder0();
      cBody* GetBorder1();
      cBody* GetBorder2();
      cBody* GetBorder3();

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

      virtual void _DestroyBody(physics::cBodyRef pBody);
      virtual void _DestroyCar(physics::cCarRef pCar);

      virtual void _Update(sampletime_t currentTime);

      b2World* pWorld;
      cBody* border[4];
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
      virtual void _SetMassKg(float fMassKg);


      virtual void _AddForceRelativeToWorldKg(const spitfire::math::cVec2& forceKg);
      virtual void _AddTorqueRelativeToWorldNm(const spitfire::math::cVec2& torqueNm);

      virtual void _AddForceRelativeToObjectKg(const spitfire::math::cVec2& forceKg);
      virtual void _AddTorqueRelativeToObjectNm(const spitfire::math::cVec2& torqueNm);

      virtual void _Update(sampletime_t currentTime);

      virtual void _Remove();

      cWorld* pWorld;

      bool bBody;
      bool bDynamic;
      float fFriction;
      float fRadius;

      b2Body* pBody;
    };


    class cHeightmap : public physics::cHeightmap
    {
    public:
      explicit cHeightmap(const physics::cHeightmapProperties& properties);

      void CreateHeightmap(cWorld* pWorld);

    private:
      virtual void _Update(sampletime_t currentTime);

      virtual void _Remove();

      cWorld* pWorld;

      b2Body* pBody;
    };


    class cCar : public physics::cCar
    {
    public:
      explicit cCar(cBodyRef pBody);

    private:
      void _Update(sampletime_t currentTime);
    };
  }
}

#endif // PHYSICS_BOX2D_H
