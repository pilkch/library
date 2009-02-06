#ifndef CFRUSTUM_H
#define CFRUSTUM_H

namespace spitfire
{
  namespace math
  {
    class cFrustum
    {
    public:
      cFrustum();
      ~cFrustum();

      void Update();


      void NormalisePlane(float frustum[6][4], int side);

      // Call this every time the camera moves to update the cFrustum
      //void Update(cVec3 & newLook, cVec3 & newEye, cVec3 & newUp);

      // This takes a 3D point and returns true if it's inside of the cFrustum
      bool PointInFrustum(float x, float y, float z) const;

      // This takes a 3D point and a radius and returns true if the sphere is inside of the cFrustum
      bool SphereInFrustum(float x, float y, float z, float radius) const;

      // This takes the center and half the length of the cube.
      bool CubeInFrustum(float x, float y, float z, float size) const;


      float fov;

      cVec3 target, eye, up, right;
      cVec3 targetIdeal, eyeIdeal, upIdeal, rightIdeal;
      cMat4 m;

    private:
      cMat4 proj;                // This will hold our projection matrix
      cMat4 modl;                // This will hold our modelview matrix
      cMat4 clip;                // This will hold the clipping planes

      // This holds the A B C and D values for each side of our cFrustum.
      float m_Frustum[6][4];
    };
  }
}

#endif // CFRUSTUM_H
