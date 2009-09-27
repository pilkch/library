#ifndef CFRUSTUM_H
#define CFRUSTUM_H

#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/geometry.h>

namespace spitfire
{
  namespace math
  {
    class cFrustum
    {
    public:
      cFrustum();
      ~cFrustum() {}

      void Update();


      // Call this every time the camera moves to update the cFrustum
      //void Update(cVec3 & newLook, cVec3 & newEye, cVec3 & newUp);

      // This takes a 3D point and returns true if it's inside of the cFrustum
      bool PointInFrustum(const cVec3& point) const;

      // This takes a 3D point and a radius and returns true if the sphere is inside of the cFrustum
      bool SphereInFrustum(const cVec3& point, float radius) const;

      // This takes the center and half the length of the cube.
      bool CubeInFrustum(float x, float y, float z, float size) const;

      cRay3 CreatePickingRay() const;

      float fov;

      cVec3 target;
      cVec3 eye;
      cVec3 up;
      cVec3 right;

      cVec3 targetIdeal;
      cVec3 eyeIdeal;
      cVec3 upIdeal;
      cVec3 rightIdeal;

      cMat4 m;

    private:
      void SetFrustumFromOpenGLProjectionAndModelViewMatrices();
      void SetFrustumFromProjectionAndModelViewMatrices(const cMat4& matProjection, const cMat4& matModelView);

      // This holds the A B C and D values for each side of our cFrustum.
      cPlane m_Frustum[6];
    };
  }
}

#endif // CFRUSTUM_H
