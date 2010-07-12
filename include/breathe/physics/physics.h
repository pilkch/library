#ifndef CPHYSICS_H
#define CPHYSICS_H

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)

#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>

#include <spitfire/math/cVec2.h>
#ifdef BUILD_PHYSICS_3D
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cQuaternion.h>
#endif

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

    class cCarProperties;

    class cCar;
    typedef cSmartPtr<cCar> cCarRef;


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

      void DestroyBody(cBodyRef pBody) { _DestroyBody(pBody); }
      void DestroyCar(cCarRef pCar) { _DestroyCar(pCar); }

      size_t GetNumberOfBodies() const { return lPhysicsBody.size(); }

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

      virtual void _DestroyBody(cBodyRef pBody) = 0;
      virtual void _DestroyCar(cCarRef pCar) = 0;

      virtual void _Update(sampletime_t currentTime) = 0;
    };



    class cBoxProperties
    {
    public:
      cBoxProperties();

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

      void SetPositionAbsolute(const physvec_t& _position) { position = _position; }
      void SetRotationAbsolute(const physrotation_t& _rotation) { rotation = _rotation; }
      void SetRadiusMetres(float _fRadiusMetres) { fRadiusMetres = _fRadiusMetres; }
      void SetMassKg(float _fMassKg) { fMassKg = _fMassKg; }
      void SetFriction(float _fFriction) { fFriction = _fFriction; }
      void SetRestitution(float _fRestitution) { fRestitution = _fRestitution; }

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
      float GetMassKg() const { return fMassKg; }

      void SetPositionAbsolute(const physvec_t& position) { _SetPositionAbsolute(position); }
      void SetRotationAbsolute(const physrotation_t& rotation) { _SetRotationAbsolute(rotation); }
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
      // TODO: Add velocity
      // TODO: Add rotational velocity
      float fMassKg;

    private:
      NO_COPY(cBody);

      virtual void _SetPositionAbsolute(const physvec_t& position) = 0;
      virtual void _SetRotationAbsolute(const physrotation_t& rotation) = 0;
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
