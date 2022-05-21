// Standard headers
#include <cmath>

#include <vector>

// Anything else
#include <GL/GLee.h>
#include <GL/glu.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>

namespace spitfire
{
  namespace math
  {
    // We create an enum of the sides so we don't have to call each side 0 or 1.
    // This way it makes it more understandable and readable when dealing with frustum sides.
    enum FRUSTUM_SIDE {
      RIGHT  = 0,    // The RIGHT side of the frustum
      LEFT   = 1,    // The LEFT   side of the frustum
      BOTTOM = 2,    // The BOTTOM side of the frustum
      TOP    = 3,    // The TOP side of the frustum
      BACK   = 4,    // The BACK  side of the frustum
      FRONT  = 5     // The FRONT side of the frustum
    };

    cFrustum::cFrustum() :
      target(0.0f, 0.0f, 0.0f),
      eye(10.0f, 0.0f, 2.0f),
      up(0.0f, 0.0f, 1.0f),

      targetIdeal(0.0f, 0.0f, 0.0f),
      eyeIdeal(10.0f, 0.0f, 2.0f),
      upIdeal(0.0f, 0.0f, 1.0f)
    {
    }


    // http://robertmarkmorley.com/2008/11/16/frustum-culling-in-opengl/
    //
    // Here's what the final view matrix should look like:
    //
    //  |  rx   ry   rz  -(r.e) |
    //  |  ux   uy   uz  -(u.e) |
    //  | -lx  -ly  -lz   (l.e) |
    //  |   0    0    0     1   |
    //
    // Where r = Right vector
    //       u = Up vector
    //       l = Look vector
    //       e = Eye position in world space
    //       . = Dot-product operation
    //
    void cFrustum::SetFrustumFromProjectionAndModelViewMatrices(const cMat4& matProjection, const cMat4& matModelView)
    {
      // Now that we have our modelview and projection matrix, if we combine these 2 matrices,
      // it will give us our clipping planes.  To combine 2 matrices, we multiply them.
      cMat4 clip = matModelView * matProjection;


      // Now we actually want to get the sides of the frustum.  To do this we take
      // the clipping planes we received above and extract the sides from them.

      // This will extract the RIGHT side of the frustum
      m_Frustum[RIGHT].SetNormal(cVec3(clip[ 3] - clip[ 0], clip[ 7] - clip[ 4], clip[11] - clip[ 8]));
      m_Frustum[RIGHT].SetIntercept(clip[15] - clip[12]);

      // Now that we have a normal (A,B,C) and a distance (D) to the plane,
      // we want to normalise that normal and distance.

      // normalise the RIGHT side
      m_Frustum[RIGHT].Normalise();

      // This will extract the LEFT side of the frustum
      m_Frustum[LEFT].SetNormal(cVec3(clip[ 3] + clip[ 0], clip[ 7] + clip[ 4], clip[11] + clip[ 8]));
      m_Frustum[LEFT].SetIntercept(clip[15] + clip[12]);

      // Normalise the LEFT side
      m_Frustum[LEFT].Normalise();

      // This will extract the BOTTOM side of the frustum
      m_Frustum[BOTTOM].SetNormal(cVec3(clip[ 3] + clip[ 1], clip[ 7] + clip[ 5], clip[11] + clip[ 9]));
      m_Frustum[BOTTOM].SetIntercept(clip[15] + clip[13]);

      // Normalise the BOTTOM side
      m_Frustum[BOTTOM].Normalise();

      // This will extract the TOP side of the frustum
      m_Frustum[TOP].SetNormal(cVec3(clip[ 3] - clip[ 1], clip[ 7] - clip[ 5], clip[11] - clip[ 9]));
      m_Frustum[TOP].SetIntercept(clip[15] - clip[13]);

      // Normalise the TOP side
      m_Frustum[TOP].Normalise();

      // This will extract the BACK side of the frustum
      m_Frustum[BACK].SetNormal(cVec3(clip[ 3] - clip[ 2], clip[ 7] - clip[ 6], clip[11] - clip[10]));
      m_Frustum[BACK].SetIntercept(clip[15] - clip[14]);

      // Normalise the BACK side
      m_Frustum[BACK].Normalise();

      // This will extract the FRONT side of the frustum
      m_Frustum[FRONT].SetNormal(cVec3(clip[ 3] + clip[ 2], clip[ 7] + clip[ 6], clip[11] + clip[10]));
      m_Frustum[FRONT].SetIntercept(clip[15] + clip[14]);

      // Normalise the FRONT side
      m_Frustum[FRONT].Normalise();
    }

    void cFrustum::SetFrustumFromOpenGLProjectionAndModelViewMatrices()
    {
      float proj[16]; // This will hold our projection matrix
      float modl[16]; // This will hold our modelview matrix

      // Get the current PROJECTION matrix from OpenGL
      glGetFloatv(GL_PROJECTION_MATRIX, proj);

      // Get the current MODELVIEW matrix from OpenGL
      glGetFloatv(GL_MODELVIEW_MATRIX, modl);


      cMat4 matProjection;
      matProjection.SetFromOpenGLMatrix(proj);

      cMat4 matModelView;
      matModelView.SetFromOpenGLMatrix(modl);

      SetFrustumFromProjectionAndModelViewMatrices(matProjection, matModelView);
    }

    void cFrustum::Update()
    {
      target = target.lerp(targetIdeal, 0.05f);
      eye = eye.lerp(eyeIdeal, 0.05f);
      up = up.lerp(upIdeal, 0.05f);

      m.SetLookAt(eye, target, up);


      right = target.CrossProduct(up);
      right.Normalise();

      up = right.CrossProduct(target);
      up.Normalise();




      float aspect = 1.3333f;
      float zNear = 1.0f;
      float zFar = 1000.0f;

      const float h = 1.0f / tan(0.5f * DegreesToRadians(fov));
      float neg_depth = zNear - zFar;


      cMat4 matProjection;

      matProjection[0] = h / aspect;
      matProjection[1] = 0.0f;
      matProjection[2] = 0.0f;
      matProjection[3] = 0.0f;

      matProjection[4] = 0.0f;
      matProjection[5] = h;
      matProjection[6] = 0.0f;
      matProjection[7] = 0.0f;

      matProjection[8] = 0.0f;
      matProjection[9] = 0.0f;
      matProjection[10] = (zFar + zNear) / neg_depth;
      matProjection[11] = -1.0f;

      matProjection[12] = 0.0f;
      matProjection[13] = 0.0f;
      matProjection[14] = 2.0f * (zNear * zFar) / neg_depth;
      matProjection[15] = 0.0f;

      /*matProjection[0]  = 1.29904f;
      matProjection[1]  =  0.000000f;
      matProjection[2]  =  0.000000f;
      matProjection[3]  =  0.000000f;
      matProjection[4]  =  0.000000f;
      matProjection[5]  =  1.73205f;
      matProjection[6]  =  0.000000f;
      matProjection[7]  =  0.000000f;
      matProjection[8]  =  0.000000f;
      matProjection[9]  =  0.000000f;
      matProjection[10] =  -1.00200f;
      matProjection[11] =  -1.00000f;
      matProjection[12] =  0.000000f;
      matProjection[13] =  0.000000f;
      matProjection[14] =  -2.00200f;
      matProjection[15] =  0.000000f;*/


      cMat4 matModelView;
      matModelView[0]  =  right.x;
      matModelView[1]  =  up.x;
      matModelView[2]  = -target.x;
      matModelView[3]  =  0.0f;

      matModelView[4]  =  right.y;
      matModelView[5]  =  up.y;
      matModelView[6]  = -target.y;
      matModelView[7]  =  0.0f;

      matModelView[8]  =  right.z;
      matModelView[9]  =  up.z;
      matModelView[10] = -target.z;
      matModelView[11] =  0.0f;

      matModelView[12] = -right.DotProduct(eye);
      matModelView[13] = -up.DotProduct(eye);
      matModelView[14] =  target.DotProduct(eye);
      matModelView[15] =  1.0f;


      SetFrustumFromProjectionAndModelViewMatrices(matProjection, matModelView);
    }

    // The code below will allow us to make checks within the frustum.  For example,
    // if we want to see if a point, a sphere, or a cube lies inside of the frustum.
    // Because all of our planes point INWARDS (The normals are all pointing inside the frustum)
    // we then can assume that if a point is in FRONT of all of the planes, it's inside.

    bool cFrustum::PointInFrustum(const cVec3& point) const
    {
      if (m_Frustum[0].ClassifyPoint(point) != PLANE_POINT_CLASSIFICATION::POINT_BEHIND_PLANE) return false;
      if (m_Frustum[1].ClassifyPoint(point) != PLANE_POINT_CLASSIFICATION::POINT_BEHIND_PLANE) return false;
      if (m_Frustum[2].ClassifyPoint(point) != PLANE_POINT_CLASSIFICATION::POINT_BEHIND_PLANE) return false;
      if (m_Frustum[3].ClassifyPoint(point) != PLANE_POINT_CLASSIFICATION::POINT_BEHIND_PLANE) return false;
      if (m_Frustum[4].ClassifyPoint(point) != PLANE_POINT_CLASSIFICATION::POINT_BEHIND_PLANE) return false;
      if (m_Frustum[5].ClassifyPoint(point) != PLANE_POINT_CLASSIFICATION::POINT_BEHIND_PLANE) return false;

      return true;
    }

    bool cFrustum::SphereInFrustum(const cVec3& point, float radius) const
    {
      // If the center of the sphere is farther away from the plane than the radius
      for (size_t i = 0; i < 6; i++) {
        const cVec3& normal = m_Frustum[i].GetNormal();
        const float_t intercept = m_Frustum[i].GetIntercept();
        if (((normal.x * point.x) + (normal.y * point.y) + (normal.z * point.z) + intercept) <= -radius ) return false;
      }

      return true;
    }

    bool cFrustum::CubeInFrustum(float x, float y, float z, float size) const
    {
      // Basically, we are given the center of the cube, and half the length.
      // Think of half the length as being like a radius.  Then we check each point
      // in the cube and seeing if it is inside the frustum.  If a point is found in front
      // of a side, then we skip to the next side.  If we get to a plane that does NOT have
      // a point in front of it, then it will return false.

      for (size_t i = 0; i < 6; i++ ) {
        const cVec3 normal = m_Frustum[i].GetNormal();
        const float_t intercept = m_Frustum[i].GetIntercept();
        if ((normal.x * (x - size) + normal.y * (y - size) + normal.z * (z - size) + intercept) > 0.0f)
          continue;
        if ((normal.x * (x + size) + normal.y * (y - size) + normal.z * (z - size) + intercept) > 0.0f)
          continue;
        if ((normal.x * (x - size) + normal.y * (y + size) + normal.z * (z - size) + intercept) > 0.0f)
          continue;
        if ((normal.x * (x + size) + normal.y * (y + size) + normal.z * (z - size) + intercept) > 0.0f)
          continue;
        if ((normal.x * (x - size) + normal.y * (y - size) + normal.z * (z + size) + intercept) > 0.0f)
          continue;
        if ((normal.x * (x + size) + normal.y * (y - size) + normal.z * (z + size) + intercept) > 0.0f)
          continue;
        if ((normal.x * (x - size) + normal.y * (y + size) + normal.z * (z + size) + intercept) > 0.0f)
          continue;
        if ((normal.x * (x + size) + normal.y * (y + size) + normal.z * (z + size) + intercept) > 0.0f)
          continue;

        // Not in the frustum
        return false;
      }

      return true;
    }

    cRay3 cFrustum::CreatePickingRay() const
    {
      cRay3 ray;

      const cVec3 direction = (target - eye).GetNormalised();
      ray.SetOriginAndDirection(eye, direction);

      return ray;
    }
  }
}
