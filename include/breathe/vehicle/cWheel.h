#ifndef CVEHICLEWHEEL_H
#define CVEHICLEWHEEL_H

namespace breathe
{
  namespace vehicle
  {
    class cVehicle2;

    class cWheel
#ifdef BUILD_PHYSICS_3D
      //: public physics::cPhysicsRayCast
#endif
    {
    public:
      cWheel(cVehicle2* p, physics::cWorld* pWorld);

      void Init(bool bFront, float fWRadius, float fInWeight, float fSK, float fSU,
        float fSNormal, float fSMin, float fSMax, const math::cVec3& pos);
      void Update(sampletime_t currentTime);


      // For raycasting to find out if this particular wheel is touching anything
      void RayCast();
      static void RayCastCallback(void* data, dGeomID g1, dGeomID g2);


      bool bFront; // At the moment only the front wheels are steerable

      float fSuspensionK; // spring constant
      float fSuspensionU; // damping constant
      float fSuspensionNormal;
      float fSuspensionMin;
      float fSuspensionMax;

      float fTraction;

      math::cVec3 v3SuspensionTopRel; // Don't use this for rendering
      math::cVec3 v3SuspensionTop;

    private:
      cVehicle2* pParent;

      float fRadius;
      float fWeight;
    };
  }
}

#endif // CVEHICLEWHEEL_H
