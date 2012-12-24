#ifndef CPHYSICS_H
#define CPHYSICS_H

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)

// Boost headers
#include <boost/shared_ptr.hpp>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>

#include <spitfire/math/cVec2.h>
#ifdef BUILD_PHYSICS_3D
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cQuaternion.h>
#endif
#include <spitfire/math/geometry.h>

#include <breathe/breathe.h>

#ifdef BUILD_PHYSICS_3D
#include <breathe/game/cHeightmapLoader.h>
#endif

namespace breathe
{
  namespace physics
  {
#ifdef BUILD_PHYSICS_2D
    typedef spitfire::math::cVec2 physvec_t;
    const spitfire::math::cVec2 physveczero(0.0f, 0.0f);
    typedef float_t physrotation_t;
#elif defined(BUILD_PHYSICS_3D)
    typedef spitfire::math::cVec3 physvec_t;
    const spitfire::math::cVec3 physveczero(0.0f, 0.0f, 0.0f);
    typedef spitfire::math::cQuaternion physrotation_t;
#endif


    enum class DRIVER {
#ifdef BUILD_PHYSICS_BULLET
      BULLET,
#endif
#ifdef BUILD_PHYSICS_ODE
      ODE,
#endif
#ifdef BUILD_PHYSICS_BOX2D
      BOX2D,
#endif

#ifdef BUILD_PHYSICS_BULLET
      DEFAULT = BULLET
#elif defined(BUILD_PHYSICS_ODE)
      DEFAULT = ODE
#else
      DEFAULT = BOX2D
#endif
    };


    // Forward declaration
    class cWorld;


    class cBoxProperties;
    class cSphereProperties;

    class cBody;
    typedef cSmartPtr<cBody> cBodyRef;

    class cHeightmapProperties;

    class cHeightmap;
    typedef cSmartPtr<cHeightmap> cHeightmapRef;

    class cSpringProperties;

    class cSpring;
    typedef cSmartPtr<cSpring> cSpringRef;

    class cCarProperties;

    class cCar;
    typedef cSmartPtr<cCar> cCarRef;

    #ifdef BUILD_PHYSICS_2D
    class cRopeProperties;

    class cRope;
    typedef cSmartPtr<cRope> cRopeRef;
    #endif

    class cRayCast;
    class cCollisionResult;


    cWorld* Create(DRIVER driver, const physvec_t& worldDimensions);
    void Destroy(cWorld* pWorld);

    // TODO: Eventually we want to remove this
    cWorld* GetWorld();

    class cWorld
    {
    public:
      virtual ~cWorld() {}

      bool Init(const physvec_t& worldDimensions) { return _Init(worldDimensions); }
      void Destroy() { return _Destroy(); }

      size_t GetFrequencyHz() const { return uiFrequencyHz; }
      float GetIntervalMS() const { return fIntervalMS; }

      cBodyRef CreateBody(const cBoxProperties& properties) { return _CreateBody(properties); }
      cBodyRef CreateBody(const cSphereProperties& properties) { return _CreateBody(properties); }
      cHeightmapRef CreateHeightmap(const cHeightmapProperties& properties) { return _CreateHeightmap(properties); }
      cCarRef CreateCar(const cCarProperties& properties) { return _CreateCar(properties); } // In any particular physics engine a car is usually made up of a body and 4 rays, but this is entirely up to the physics engine
      #ifdef BUILD_PHYSICS_2D
      cRopeRef CreateRope(const cRopeProperties& properties) { return _CreateRope(properties); }
      #endif

      void DestroyBody(cBodyRef pBody) { _DestroyBody(pBody); }
      void DestroyCar(cCarRef pCar) { _DestroyCar(pCar); }
      #ifdef BUILD_PHYSICS_2D
      void DestroyRope(cRopeRef pRope) { _DestroyRope(pRope); }
      #endif

      size_t GetNumberOfBodies() const { return lPhysicsBody.size(); }

      #ifdef BUILD_PHYSICS_2D
      void CastRay(const spitfire::math::cRay2& ray, cCollisionResult& result) { _CastRay(ray, result); }
      #else
      void CastRay(const spitfire::math::cRay3& ray, cCollisionResult& result) { _CastRay(ray, result); }
      #endif
      //void CastRayFromBody(const cBody& body, cCollisionResult& result) { _CastRayFromBody(body, result); }

      void Update(sampletime_t currentTime);

    protected:
      size_t uiFrequencyHz;
      float fIntervalMS;

      std::list<physics::cBodyRef> lPhysicsBody;
      typedef std::list<physics::cBodyRef>::iterator body_iterator;

    private:
      virtual bool _Init(const physvec_t& worldDimensions) = 0;
      virtual void _Destroy() = 0;

      virtual cBodyRef _CreateBody(const cBoxProperties& properties) = 0;
      virtual cBodyRef _CreateBody(const cSphereProperties& properties) = 0;
      virtual cHeightmapRef _CreateHeightmap(const cHeightmapProperties& properties) = 0;
      virtual cCarRef _CreateCar(const cCarProperties& properties) = 0;
      #ifdef BUILD_PHYSICS_2D
      virtual cRopeRef _CreateRope(const cRopeProperties& properties) = 0;
      #endif

      virtual void _DestroyBody(cBodyRef pBody) = 0;
      virtual void _DestroyCar(cCarRef pCar) = 0;
      #ifdef BUILD_PHYSICS_2D
      virtual void _DestroyRope(cRopeRef pRope) = 0;
      #endif

      #ifdef BUILD_PHYSICS_2D
      virtual void _CastRay(const spitfire::math::cRay2& ray, cCollisionResult& result) = 0;
      #else
      virtual void _CastRay(const spitfire::math::cRay3& ray, cCollisionResult& result) = 0;
      #endif
      //virtual void _CastRayFromBody(const cBody& body, cCollisionResult& result) = 0;

      virtual void _Update(sampletime_t currentTime) = 0;
    };



    class cBoxProperties
    {
    public:
      cBoxProperties();

      void SetDynamic() { bIsDynamic = true; }
      void SetStatic() { bIsDynamic = false; }
      void SetPositionAbsolute(const physvec_t& _position) { position = _position; }
      void SetRotationAbsolute(const physrotation_t& _rotation) { rotation = _rotation; }
    #ifdef BUILD_PHYSICS_3D
      void SetWidthDepthHeightMetres(float _fWidthMetres, float _fDepthMetres, float _fHeightMetres) { fWidthMetres = _fWidthMetres; fDepthMetres = _fDepthMetres; fHeightMetres = _fHeightMetres; }
    #else
      void SetWidthHeightMetres(float _fWidthMetres, float _fHeightMetres) { fWidthMetres = _fWidthMetres; fHeightMetres = _fHeightMetres; }
    #endif
      void SetMassKg(float _fMassKg) { fMassKg = _fMassKg; }
      void SetFriction(float _fFriction) { fFriction = _fFriction; }
      void SetRestitution(float _fRestitution) { fRestitution = _fRestitution; }

      bool bIsDynamic;
      physvec_t position;
      physrotation_t rotation;
      float fWidthMetres;
    #ifdef BUILD_PHYSICS_3D
      float fDepthMetres;
    #endif
      float fHeightMetres;
      float fMassKg;
      float fFriction;
      float fRestitution;
    };

    class cSphereProperties
    {
    public:
      cSphereProperties();

      void SetDynamic() { bIsDynamic = true; }
      void SetStatic() { bIsDynamic = false; }
      void SetPositionAbsolute(const physvec_t& _position) { position = _position; }
      void SetRotationAbsolute(const physrotation_t& _rotation) { rotation = _rotation; }
      void SetRadiusMetres(float _fRadiusMetres) { fRadiusMetres = _fRadiusMetres; }
      void SetMassKg(float _fMassKg) { fMassKg = _fMassKg; }
      void SetFriction(float _fFriction) { fFriction = _fFriction; }
      void SetRestitution(float _fRestitution) { fRestitution = _fRestitution; }

      bool bIsDynamic;
      physvec_t position;
      physrotation_t rotation;
      float fRadiusMetres;
      float fMassKg;
      float fFriction;
      float fRestitution;
    };


    // The physics object
    class cBody
    {
    public:
      cBody() {}
      virtual ~cBody() {}

      const physvec_t& GetPositionAbsolute() const { return position; }
      const physrotation_t& GetRotationAbsolute() const { return rotation; }
      const physvec_t& GetVelocityAbsolute() const { return velocity; }
      float GetMassKg() const { return fMassKg; }

      void SetPositionAbsolute(const physvec_t& position) { _SetPositionAbsolute(position); }
      void SetRotationAbsolute(const physrotation_t& rotation) { _SetRotationAbsolute(rotation); }
      void SetVelocityAbsolute(const physvec_t& velocity) { _SetVelocityAbsolute(velocity); }
      void SetRotationalVelocityAbsolute(const physrotation_t& rotationalVelocity) { _SetRotationalVelocityAbsolute(rotationalVelocity); }
      void SetMassKg(float fMassKg) { _SetMassKg(fMassKg); }

      void AddForceRelativeToWorldKg(const physvec_t& forceKg) { _AddForceRelativeToWorldKg(forceKg); }
      #ifdef BUILD_PHYSICS_3D
      void AddTorqueRelativeToWorldNm(const physvec_t& torqueNm) { _AddTorqueRelativeToWorldNm(torqueNm); }
      #else
      void AddTorqueRelativeToWorldNm(const physrotation_t& torqueNm) { _AddTorqueRelativeToWorldNm(torqueNm); }
      #endif

      void AddForceRelativeToBodyKg(const physvec_t& forceKg) { _AddForceRelativeToBodyKg(forceKg); }
      #ifdef BUILD_PHYSICS_3D
      void AddTorqueRelativeToBodyNm(const physvec_t& torqueNm) { _AddTorqueRelativeToBodyNm(torqueNm); }
      #else
      void AddTorqueRelativeToBodyNm(const physrotation_t& torqueNm) { _AddTorqueRelativeToBodyNm(torqueNm); }
      #endif

      void Update(sampletime_t currentTime) { _Update(currentTime); }

      void Remove() { _Remove(); }

    protected:
      physvec_t position;
      physrotation_t rotation;
      physvec_t velocity;
      // TODO: Add rotational velocity
      float fMassKg;

    private:
      NO_COPY(cBody);

      virtual void _SetPositionAbsolute(const physvec_t& position) = 0;
      virtual void _SetRotationAbsolute(const physrotation_t& rotation) = 0;
      virtual void _SetVelocityAbsolute(const physvec_t& velocity) = 0;
      virtual void _SetRotationalVelocityAbsolute(const physrotation_t& rotationalVelocity) = 0;
      virtual void _SetMassKg(float fMassKg) = 0;

      virtual void _AddForceRelativeToWorldKg(const physvec_t& forceKg) = 0;
      #ifdef BUILD_PHYSICS_3D
      virtual void _AddTorqueRelativeToWorldNm(const physvec_t& torqueNm) = 0;
      #else
      virtual void _AddTorqueRelativeToWorldNm(const physrotation_t& torqueNm) = 0;
      #endif

      virtual void _AddForceRelativeToBodyKg(const physvec_t& forceKg) = 0;
      #ifdef BUILD_PHYSICS_3D
      virtual void _AddTorqueRelativeToBodyNm(const physvec_t& torqueNm) = 0;
      #else
      virtual void _AddTorqueRelativeToBodyNm(const physrotation_t& torqueNm) = 0;
      #endif

      virtual void _Update(sampletime_t currentTime) = 0;

      virtual void _Remove() = 0;
    };



#ifdef BUILD_PHYSICS_3D
    class cHeightmapProperties
    {
    public:
      explicit cHeightmapProperties(const game::cTerrainHeightMap& loader);

      void SetWidthHeight(size_t _width, size_t _height) { width = _width; height = _height; }
      void SetPositionAbsolute(const physvec_t& _position) { position = _position; }
      void SetScale(const physvec_t& _scale) { scale = _scale; }
      void SetFriction(float _fFriction) { fFriction = _fFriction; }
      void SetRestitution(float _fRestitution) { fRestitution = _fRestitution; }

      const game::cTerrainHeightMap& loader;
      size_t width; // How many samples wide
      size_t height; // How many samples high
      physvec_t position;
      physvec_t scale;
      float fFriction;
      float fRestitution;
    };

    class cHeightmap
    {
    public:
      explicit cHeightmap(const cHeightmapProperties& properties);
      virtual ~cHeightmap() {}

      const physvec_t& GetPositionAbsolute() const { return position; }
      const physvec_t& GetScale() const { return scale; }

      void Update(sampletime_t currentTime) { _Update(currentTime); }

      void Remove() { _Remove(); }

    protected:
      const game::cTerrainHeightMap& loader;
      const size_t width; // How many samples wide
      const size_t height; // How many samples high
      physvec_t position;
      physvec_t scale;
      float fFriction;
      float fRestitution;

    private:
      NO_COPY(cHeightmap);

      virtual void _Update(sampletime_t currentTime) = 0;

      virtual void _Remove() = 0;
    };
#else
    class cHeightmapProperties
    {
    public:
      explicit cHeightmapProperties(const std::vector<float>& values);

      void SetWidth(size_t _width) { width = _width; }
      void SetPositionAbsolute(const physvec_t& _position) { position = _position; }
      void SetScale(const physvec_t& _scale) { scale = _scale; }
      void SetFriction(float _fFriction) { fFriction = _fFriction; }
      void SetRestitution(float _fRestitution) { fRestitution = _fRestitution; }

      const std::vector<float>& values;
      size_t width; // How many samples wide
      physvec_t position;
      physvec_t scale;
      float fFriction;
      float fRestitution;
    };

    class cHeightmap
    {
    public:
      explicit cHeightmap(const cHeightmapProperties& properties);
      virtual ~cHeightmap() {}

      const physvec_t& GetPositionAbsolute() const { return position; }
      const physvec_t& GetScale() const { return scale; }

      void Update(sampletime_t currentTime) { _Update(currentTime); }

      void Remove() { _Remove(); }

    protected:
      const std::vector<float>& values;
      const size_t width; // How many samples wide
      physvec_t position;
      physvec_t scale;
      float fFriction;
      float fRestitution;

    private:
      NO_COPY(cHeightmap);

      virtual void _Update(sampletime_t currentTime) = 0;

      virtual void _Remove() = 0;
    };
#endif


    class cSpringProperties
    {
    public:
      cSpringProperties();

      void SetBodyAndOffset0(cBodyRef _pBody0, physvec_t _offset0) { pBody0 = _pBody0; offset0 = _offset0; }
      void SetBodyAndOffset1(cBodyRef _pBody1, physvec_t _offset1) { pBody1 = _pBody1; offset1 = _offset1; }
      void SetSpringConstantK(float _fK) { fK = _fK; }
      void SetFriction(float _fFriction) { fFriction = _fFriction; }

      cBodyRef pBody0;
      cBodyRef pBody1;
      physvec_t offset0;
      physvec_t offset1;
      float fK;
      float fFriction;
      float fDesiredDistance;

      /*void addSpringForce(float k, float friction, float fDesiredDistance) {
        Vec2 pA = pBody0->pBody->getWorldPoint(offset0);
        Vec2 pB = pBody1->pBody->getWorldPoint(offset1);
        Vec2 diff = pB.sub(pA);
        //Find velocities of attach points
        Vec2 vA = pBody0->pBody->m_linearVelocity.sub(Vec2.cross(pBody0->pBody->getWorldVector(offset0), pBody0->pBody->m_angularVelocity));
        Vec2 vB = pBody1->pBody->m_linearVelocity.sub(Vec2.cross(pBody1->pBody->getWorldVector(offset1), pBody1->pBody->m_angularVelocity));
        Vec2 vdiff = vB.sub(vA);
        float dx = diff.normalize(); //normalizes diff and puts length into dx
        float vrel = vdiff.x*diff.x + vdiff.y * diff.y;
        float forceMag = -k * (dx - fDesiredDistance) - friction * vrel;
        diff.mulLocal(forceMag); // diff *= forceMag
        pBody1->pBody->applyForce(diff, pBody0->pBody->getWorldPoint(offset0));
        pBody0->pBody->applyForce(diff.mulLocal(-1f), pBody1->pBody->getWorldPoint(offset1));
        pBody0->pBody->wakeUp();
        pBody1->pBody->wakeUp();
      }*/
    };

    class cSpring
    {
    public:
      explicit cSpring(const cSpringProperties& properties);

    private:
      // TODO: Preferably we would not hold onto these
      cBodyRef pBody0;
      cBodyRef pBody1;
    };


    class cCarProperties
    {
    public:
      cCarProperties();

      void SetPositionAbsolute(const physvec_t& _position) { position = _position; }
      void SetRotationAbsolute(const physrotation_t& _rotation) { rotation = _rotation; }
      void SetMassKg(float _fMassKg) { fMassKg = _fMassKg; }

      // Chassis
      physvec_t position;
      physrotation_t rotation;
      float fMassKg;
      float fWidthMetres;
    #ifdef BUILD_PHYSICS_3D
      float fDepthMetres;
    #endif
      float fHeightMetres;

      // Suspension
      float fSuspensionStiffness;
      float fSuspensionCompression;
      float fSuspensionDamping;
      float fSuspensionRestLengthCentimetres;
      float fSuspensionMaxTravelCentimetres;

      // Wheels
      size_t nWheels;
      float fWheelMassKg;
      float fWheelWidthCentimetres;
      float fWheelRadiusCentimetres;
      float fTireFrictionSlip;
    };


    class cCar
    {
    public:
      cCar();

      cBodyRef GetChassis() { return pChassis; }

      const physvec_t& GetPositionAbsolute() const { ASSERT(pChassis != nullptr); return pChassis->GetPositionAbsolute(); }
      const physrotation_t& GetRotationAbsolute() const { ASSERT(pChassis != nullptr); return pChassis->GetRotationAbsolute(); }
      const physvec_t& GetVelocityAbsolute() const { ASSERT(pChassis != nullptr); return pChassis->GetVelocityAbsolute(); }

      const physvec_t& GetWheelPositionRelative(size_t index) const { ASSERT(index < 4); return wheelPositionRelative[index]; }
      const physrotation_t& GetWheelRotationRelative(size_t index) const { ASSERT(index < 4); return wheelRotationRelative[index]; }

      void SetWheelAccelerationForceNewtons(size_t wheel, float_t fAccelerationForceNewtons) { ASSERT(wheel < 4); fWheelAccelerationForceNewtons[wheel] = fAccelerationForceNewtons; }
      void SetWheelBrakingForceNewtons(size_t wheel, float_t fBrakingForceNewtons) { ASSERT(wheel < 4); fWheelBrakingForceNewtons[wheel] = fBrakingForceNewtons; }
      void SetWheelSteeringAngleMinusOneToPlusOne(size_t wheel, float_t fSteeringAngleMinusOneToPlusOne) { ASSERT(wheel < 4);  fWheelSteeringAngleMinusOneToPlusOne[wheel] = fSteeringAngleMinusOneToPlusOne; }

      void Update(sampletime_t currentTime) { _Update(currentTime); }

    protected:
      cBodyRef pChassis;

      float fWheelAccelerationForceNewtons[4];
      float fWheelBrakingForceNewtons[4];
      float fWheelSteeringAngleMinusOneToPlusOne[4];
      physvec_t wheelPositionRelative[4];
      physrotation_t wheelRotationRelative[4];

    private:
      virtual void _Update(sampletime_t currentTime) = 0;
    };


    #ifdef BUILD_PHYSICS_2D
    class cRopeProperties
    {
    public:
      cRopeProperties();

      void SetAnchorPoint0(cBodyRef pBody, const physvec_t anchorPoint) { pAnchorBody0 = pBody; anchorPoint0 = anchorPoint; }
      void SetAnchorPoint1(cBodyRef pBody, const physvec_t anchorPoint) { pAnchorBody1 = pBody; anchorPoint1 = anchorPoint; }
      void SetSag(float _fSag) { fSag = _fSag; }

      cBodyRef pAnchorBody0;
      cBodyRef pAnchorBody1;

      physvec_t anchorPoint0;
      physvec_t anchorPoint1;

      float fSag;
    };


    // http://www.raywenderlich.com/14793/how-to-make-a-game-like-cut-the-rope-part-1
    // http://www.raywenderlich.com/14812/how-to-make-a-game-like-cut-the-rope-part-2
    class cRope
    {
    public:
      cRope();
      virtual ~cRope() {}

      cBodyRef GetAnchorBody0() { return pAnchorBody0; }
      cBodyRef GetAnchorBody1() { return pAnchorBody1; }

      void Update(sampletime_t currentTime) { _Update(currentTime); }

    protected:
      cBodyRef pAnchorBody0;
      cBodyRef pAnchorBody1;

    private:
      virtual void _Update(sampletime_t currentTime) = 0;
    };
    #endif


    class cCollisionResult
    {
    public:
      cCollisionResult() { bIsIntersection = false; }

      void Clear() { bIsIntersection = false; }

      bool IsIntersection() const { return bIsIntersection; }
      void SetIsIntersection() { bIsIntersection = true; }

      const float& GetIntersectionLength() const { return fIntersectionLength; }
      void SetIntersectionLength(float _fIntersectionLength) { fIntersectionLength = _fIntersectionLength; }

      const physvec_t& GetIntersectionPoint() const { return intersectionPoint; }
      void SetIntersectionPoint(const physvec_t& _intersectionPoint) { intersectionPoint = _intersectionPoint; }

      const physvec_t& GetIntersectionNormal() const { return intersectionNormal; }
      void SetIntersectionNormal(const physvec_t& _intersectionNormal) { intersectionNormal = _intersectionNormal; }

      //const cGeometry& GetGeometry() const;
      //void SetGeometry(const cGeometry& geometry);

    private:
      bool bIsIntersection;
      float fIntersectionLength;
      physvec_t intersectionPoint;
      physvec_t intersectionNormal;

      //const cGeometry* pGeometry;
    };
  }
}

#ifdef BUILD_PHYSICS_2D
//#include <breathe/physics/physics2d/cContact.h>
//#include <breathe/physics/physics2d/cRayCast.h>
#elif defined(BUILD_PHYSICS_3D)
#include <breathe/physics/physics3d/cContact.h>
#include <breathe/physics/physics3d/cRayCast.h>
#include <breathe/physics/cPhysicsObject.h>
#endif

#endif

#endif // CPHYSICS_H
