#include <cstdlib>
#include <cmath>
#include <cassert>

#include <vector>
#include <limits>

// Breathe
#include <breathe/breathe.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec2.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cPlane.h>
#include <breathe/math/cQuaternion.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cColour.h>

namespace breathe
{
  namespace math
  {
    cVec2::cVec2(const float* rhs) :
      u(x),
      v(y)
    {
      assert(rhs != nullptr);

      x=*rhs;
      y=*(rhs+1);
    }

    cVec2::cVec2(const cVec3& rhs) :
      x(rhs.x),
      y(rhs.y),
      u(x),
      v(y)
    {
    }

		void cVec2::Set(float newX, float newY)
		{
			x=newX;
			y=newY;
		}

		void cVec2::LoadZero(void)
		{
			x=0.0f;
			y=0.0f;
		}

		void cVec2::LoadOne(void)
		{
			x=1.0f;
			y=1.0f;
		}

		void cVec2::Normalize()
		{
			float length;
			float scalefactor;
			length=GetLength();

			if (length==1 || length==0)			//return if length is 1 or 0
				return;

			scalefactor = 1.0f/length;
			x *= scalefactor;
			y *= scalefactor;
		}

		void cVec2::SetLength(float length)
		{
			float scalefactor;

			if (length==1 || length==0)			//return if length is 1 or 0
				return;

			scalefactor = 1.0f/length;
			x *= scalefactor;
			y *= scalefactor;
		}

		cVec2 cVec2::SetLength(float length) const
		{
			cVec2 result(*this);

			result.SetLength(length);

			return result;
		}

		cVec2 cVec2::GetNormalized() const
		{
			cVec2 result(*this);

			result.Normalize();

			return result;
		}

		void cVec2::PackTo01()
		{
			(*this)=GetPackedTo01();
		}

		cVec2 cVec2::GetPackedTo01() const
		{
			cVec2 temp(*this);

			temp.Normalize();

			temp=temp*0.5f+cVec2(0.5f, 0.5f);

			return temp;
		}

		cVec2 cVec2::lerp(const cVec2 & v2, float factor)
		{
			cVec2 result;

			result=(*this)*factor + v2*(1.0f-factor);

			return result;
		}

		cVec2 cVec2::operator * (const float rhs) const
		{
			float newX = x * rhs;
			float newY = y * rhs;

			return cVec2(newX, newY);
		}

		bool cVec2::operator==(const cVec2 & rhs) const
		{
			if (x==rhs.x && y==rhs.y)
				return true;

			return false;
		}

		bool cVec2::operator!=(const cVec2 & rhs) const
		{
			return !((*this)==rhs);
		}

		void cVec2::operator-=(const cVec2 & rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
		}

		void cVec2::operator*=(const float rhs)
		{
			x *= rhs;
			y *= rhs;
		}

		void cVec2::operator/=(const float rhs)
		{
			x /= rhs;
			y /= rhs;
		}

    cVec2& cVec2::operator=(const cVec3& rhs)
    {
      x = rhs.x;
      y = rhs.y;
      return *this;
    }
  }
}
