#ifndef CPHYSICS_H
#define CPHYSICS_H

#if !defined(BUILD_PHYSICS_2D) && !defined(BUILD_PHYSICS_3D)
#error "Please define either BUILD_PHYSICS_2D or BUILD_PHYSICS_3D in your IDE"
#endif

#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>

#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cQuaternion.h>

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
      DRIVER_BULLET,
#endif
#ifdef BUILD_PHYSICS_ODE
      DRIVER_ODE,
#endif

#ifdef BUILD_PHYSICS_BULLET
      DRIVER_DEFAULT = DRIVER_BULLET
#else
      DRIVER_DEFAULT = DRIVER_ODE
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


    bool Init(DRIVER driver, float fWorldWidth, float fWorldDepth, float fWorldHeight);
    void Destroy();

    cWorld* GetWorld();

    class cWorld
    {
    public:
      virtual ~cWorld() {}

      bool Init(float fWorldWidth, float fWorldDepth, float fWorldHeight) { return _Init(fWorldWidth, fWorldDepth, fWorldHeight); }
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
      virtual bool _Init(float fWorldWidth, float fWorldDepth, float fWorldHeight) = 0;
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

      void SetPositionAbsolute(const spitfire::math::cVec3& _position) { position = _position; }
      void SetRotationAbsolute(const spitfire::math::cQuaternion& _rotation) { rotation = _rotation; }
      void SetWeightKg(float _fWeightKg) { fWeightKg = _fWeightKg; }

      spitfire::math::cVec3 position;
      spitfire::math::cQuaternion rotation;
      float fWeightKg;
      float fWidthMetres;
    #ifdef BUILD_PHYSICS_3D
      float fDepthMetres;
    #endif
      float fHeightMetres;
    };

    class cSphereProperties
    {
    public:
      cSphereProperties();

      void SetPositionAbsolute(const spitfire::math::cVec3& _position) { position = _position; }
      void SetRotationAbsolute(const spitfire::math::cQuaternion& _rotation) { rotation = _rotation; }
      void SetWeightKg(float _fWeightKg) { fWeightKg = _fWeightKg; }
      void SetRadiusMetres(float _fRadiusMetres) { fRadiusMetres = _fRadiusMetres; }

      spitfire::math::cVec3 position;
      spitfire::math::cQuaternion rotation;
      float fWeightKg;
      float fRadiusMetres;
    };


    // The physics object
    class cBody
    {
    public:
      cBody() {}
      virtual ~cBody() {}

      const math::cVec3& GetPositionAbsolute() const { return position; }
      const math::cQuaternion& GetRotationAbsolute() const { return rotation; }
      float GetWeightKg() const { return fWeightKg; }

      void SetPositionAbsolute(const spitfire::math::cVec3& position) { _SetPositionAbsolute(position); }
      void SetRotationAbsolute(const spitfire::math::cQuaternion& rotation) { _SetRotationAbsolute(rotation); }
      void SetWeightKg(float fWeightKg) { _SetWeightKg(fWeightKg); }

      void AddForceRelativeToWorldKg(const physvec_t& forceKg) { _AddForceRelativeToWorldKg(forceKg); }
      void AddTorqueRelativeToWorldNm(const physvec_t& torqueNm) { _AddTorqueRelativeToWorldNm(torqueNm); }

      void AddForceRelativeToObjectKg(const physvec_t& forceKg) { _AddForceRelativeToObjectKg(forceKg); }
      void AddTorqueRelativeToObjectNm(const physvec_t& torqueNm) { _AddTorqueRelativeToObjectNm(torqueNm); }

      void Update(sampletime_t currentTime) { _Update(currentTime); }

      void Remove() { _Remove(); }

    protected:
      math::cVec3 position;
      math::cQuaternion rotation;
      float fWeightKg;

    private:
      NO_COPY(cBody);

      virtual void _SetPositionAbsolute(const spitfire::math::cVec3& position) = 0;
      virtual void _SetRotationAbsolute(const spitfire::math::cQuaternion& rotation) = 0;
      virtual void _SetWeightKg(float fWeightKg) = 0;

      virtual void _AddForceRelativeToWorldKg(const physvec_t& forceKg) = 0;
      virtual void _AddTorqueRelativeToWorldNm(const physvec_t& torqueNm) = 0;

      virtual void _AddForceRelativeToObjectKg(const physvec_t& forceKg) = 0;
      virtual void _AddTorqueRelativeToObjectNm(const physvec_t& torqueNm) = 0;

      virtual void _Update(sampletime_t currentTime) = 0;

      virtual void _Remove() = 0;
    };



    class cHeightmapProperties
    {
    public:
      explicit cHeightmapProperties(const game::cTerrainHeightMap& loader);

      void SetWidth(size_t _width) { width = _width; }
      void SetHeight(size_t _height) { height = _height; }
      void SetPositionAbsolute(const spitfire::math::cVec3& _position) { position = _position; }
      void SetScale(const spitfire::math::cVec3& _scale) { scale = _scale; }

      const game::cTerrainHeightMap& loader;
      size_t width;
      size_t height;
      spitfire::math::cVec3 position;
      spitfire::math::cVec3 scale;
    };

    class cHeightmap
    {
    public:
      explicit cHeightmap(const cHeightmapProperties& properties);
      virtual ~cHeightmap() {}

      const math::cVec3& GetPositionAbsolute() const { return position; }
      const math::cVec3& GetScale() const { return scale; }

      void Update(sampletime_t currentTime) { _Update(currentTime); }

      void Remove() { _Remove(); }

    protected:
      const game::cTerrainHeightMap& loader;
      const size_t width;
      const size_t height;
      math::cVec3 position;
      math::cVec3 scale;

    private:
      NO_COPY(cHeightmap);

      virtual void _Update(sampletime_t currentTime) = 0;

      virtual void _Remove() = 0;
    };



    class cCarProperties
    {
    public:
      cCarProperties();

      void SetPositionAbsolute(const spitfire::math::cVec3& _position) { position = _position; }
      void SetRotationAbsolute(const spitfire::math::cQuaternion& _rotation) { rotation = _rotation; }
      void SetWeightKg(float _fWeightKg) { fWeightKg = _fWeightKg; }

      // Chassis
      spitfire::math::cVec3 position;
      spitfire::math::cQuaternion rotation;
      float fWeightKg;
      float fWidthMetres;
    #ifdef BUILD_PHYSICS_3D
      float fDepthMetres;
    #endif
      float fHeightMetres;

      // Suspension
      float fSuspensionStiffness;
      float fSuspensionCompression;
      float fSuspensionDamping;
      float fSuspensionMaxTravelCentimetres;

      // Wheels
      size_t nWheels;
      float fWheelWeightKg;
      float fTireFrictionSlip;
    };


    class cCar
    {
    public:
      cBodyRef GetBody() { return pBody; }

      const math::cVec3& GetPositionAbsolute() const { ASSERT(pBody != nullptr); return pBody->GetPositionAbsolute(); }
      const math::cQuaternion& GetRotationAbsolute() const { ASSERT(pBody != nullptr); return pBody->GetRotationAbsolute(); }

      const math::cVec3& GetWheelPositionRelative(size_t index) const { ASSERT(index < wheelPositionRelative.size()); return wheelPositionRelative[index]; }
      const math::cQuaternion& GetWheelRotationRelative(size_t index) const { ASSERT(index < wheelRotationRelative.size()); return wheelRotationRelative[index]; }

    protected:
      cBodyRef pBody;

    private:
      std::vector<math::cVec3> wheelPositionRelative;
      std::vector<math::cQuaternion> wheelRotationRelative;
    };
  }
}

#ifdef BUILD_PHYSICS_2D
#include <breathe/physics/physics2d/physics.h>
#include <breathe/physics/physics2d/cContact.h>
#elif defined(BUILD_PHYSICS_3D)
#include <breathe/physics/physics3d/cContact.h>
#include <breathe/physics/physics3d/cRayCast.h>
#endif

#include <breathe/physics/cPhysicsObject.h>

#endif // CPHYSICS_H
