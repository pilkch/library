#ifndef CPHYSICSRAYCAST_H
#define CPHYSICSRAYCAST_H

namespace breathe
{
  namespace physics
  {
    class cPhysicsRayCast
    {
    public:
      cPhysicsRayCast();
      virtual ~cPhysicsRayCast();
      void Create(float fLength);

      virtual void RayCast()=0;
      //static void RayCastCallback(void* data, dGeomID g1, dGeomID g2);

      dGeomID geomRay;
      cContact rayContact;
    };
  }
}

#endif // CPHYSICSRAYCAST_H
