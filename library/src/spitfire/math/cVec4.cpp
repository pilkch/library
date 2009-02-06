#include <cstdlib>
#include <cmath>

#include <vector>
#include <limits>

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
#include <spitfire/math/cColour.h>

namespace spitfire
{
  namespace math
  {
    cVec4::cVec4(float newX, float newY, float newZ, float newW)
    {
      x=newX;
      y=newY;
      z=newZ;
      w=newW;
    }

    cVec4::cVec4(const float * rhs)
    {
      x=*rhs;
      y=*(rhs+1);
      z=*(rhs+2);
      w=*(rhs+3);
    }

    cVec4::cVec4(const cVec4 & rhs)
    {
      x=rhs.x;
      y=rhs.y;
      z=rhs.z;
      w=rhs.w;
    }

    cVec4::cVec4(const cVec3 & rhs)
    {
      x=rhs.x;
      y=rhs.y;
      z=rhs.z;
      w=1.0f;
    }

    void cVec4::Set(float newX, float newY, float newZ, float newW)
    {
      x=newX;
      y=newY;
      z=newZ;
      w=newW;
    }

    // TODO: Inline this
    void cVec4::SetZero(void)
    {
      x=0.0f;
      y=0.0f;
      z=0.0f;
      w=0.0f;
    }

    // TODO: Inline this
    void cVec4::SetOne(void)
    {
      x=1.0f;
      y=1.0f;
      z=1.0f;
      w=1.0f;
    }

    void cVec4::Normalise()
    {
      float length;
      float scalefactor;
      length=GetLength();

      if (length==1 || length==0)      //return if length is 1 or 0
        return;

      scalefactor = 1.0f/length;
      x *= scalefactor;
      y *= scalefactor;
      z *= scalefactor;
    }

    float cVec4::DotProduct(const cVec4 & rhs) const
    {
      float result;

      result = x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.z;

      return result;
    }

    void cVec4::RotateX(double angle)
    {
      (*this)=GetRotatedX(angle);
    }

    cVec4 cVec4::GetRotatedX(double angle) const
    {
      cVec3 v3d(x, y, z);

      v3d.RotateX(angle);

      return cVec4(v3d.x, v3d.y, v3d.z, w);
    }

    void cVec4::RotateY(double angle)
    {
      (*this)=GetRotatedY(angle);
    }

    cVec4 cVec4::GetRotatedY(double angle) const
    {
      cVec3 v3d(x, y, z);

      v3d.RotateY(angle);

      return cVec4(v3d.x, v3d.y, v3d.z, w);
    }

    void cVec4::RotateZ(double angle)
    {
      (*this)=GetRotatedZ(angle);
    }

    cVec4 cVec4::GetRotatedZ(double angle) const
    {
      cVec3 v3d(x, y, z);

      v3d.RotateZ(angle);

      return cVec4(v3d.x, v3d.y, v3d.z, w);
    }

    void cVec4::RotateAxis(double angle, const cVec3 & axis)
    {
      (*this)=GetRotatedAxis(angle, axis);
    }

    cVec4 cVec4::GetRotatedAxis(double angle, const cVec3 & axis) const
    {
      cVec3 v3d(x, y, z);

      v3d.RotateAxis(angle, axis);

      return cVec4(v3d.x, v3d.y, v3d.z, w);
    }

    cVec4 cVec4::lerp(const cVec4 & v2, float factor)
    {
      cVec4 result;

      result=(*this)*factor + v2*(1.0f-factor);

      return result;
    }

    cVec4 cVec4::operator + (const cVec4 & rhs) const
    {
      float newX=x + rhs.x;
      float newY=y + rhs.y;
      float newZ=z + rhs.z;
      float newW=w + rhs.w;

      return cVec4(newX, newY, newZ, newW);
    }

    cVec4 cVec4::operator - (const cVec4 & rhs) const
    {
      float newX=x - rhs.x;
      float newY=y - rhs.y;
      float newZ=z - rhs.z;
      float newW=w - rhs.w;

      return cVec4(newX, newY, newZ, newW);
    }

    cVec4 cVec4::operator * (const float rhs) const
    {
      float newX=x * rhs;
      float newY=y * rhs;
      float newZ=z * rhs;
      float newW=w * rhs;

      return cVec4(newX, newY, newZ, newW);
    }

    cVec4 cVec4::operator / (const float rhs) const
    {
      if (rhs==0.0f) return cVec4(0.0f, 0.0f, 0.0f, 0.0f);  //cant divide by 0

      float newX=x / rhs;
      float newY=y / rhs;
      float newZ=z / rhs;
      float newW=w / rhs;

      return cVec4(newX, newY, newZ, newW);
    }

    bool cVec4::operator==(const cVec4 & rhs) const
    {
      if (x==rhs.x && y==rhs.y && z==rhs.z && w==rhs.w)
        return true;

      return false;
    }

    bool cVec4::operator!=(const cVec4 & rhs) const
    {
      return !((*this)==rhs);
    }

    void cVec4::operator+=(const cVec4 & rhs)
    {
      x+=rhs.x;
      y+=rhs.y;
      z+=rhs.z;
      w+=rhs.w;
    }

    void cVec4::operator-=(const cVec4 & rhs)
    {
      x-=rhs.x;
      y-=rhs.y;
      z-=rhs.z;
      w-=rhs.w;
    }

    void cVec4::operator*=(const float rhs)
    {
      x*=rhs;
      y*=rhs;
      z*=rhs;
      w*=rhs;
    }

    void cVec4::operator/=(const float rhs)
    {
      x/=rhs;
      y/=rhs;
      z/=rhs;
      w/=rhs;
    }
  }
}
