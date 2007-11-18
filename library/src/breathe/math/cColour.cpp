#include <cmath>

#include <vector>

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
#include <breathe/math/cOctree.h>
#include <breathe/math/cColour.h>

namespace breathe
{
	namespace math
	{
		cColour::cColour(float newR, float newG, float newB, float newA)
		{
			r=newR;
			g=newG;
			b=newB;
			a=newA;

			Clamp();
		}

		cColour::cColour(const float* rhs)
		{
			r=*rhs;
			g=*(rhs+1);
			b=*(rhs+2);
			a=*(rhs+3);

			Clamp();
		}

		cColour::cColour(const cColour& rhs)
		{
			r=rhs.r;
			g=rhs.g;
			b=rhs.b;
			a=rhs.a;
		}

		void cColour::Set(float newR, float newG, float newB, float newA)
		{
			r=newR;
			g=newG;
			b=newB;
			a=newA;

			Clamp();
		}

		void cColour::Clamp()
		{
			if (r>1.0f)
				r=1.0f;
			if (r<0.0f)
				r=0.0f;

			if (g>1.0f)
				g=1.0f;
			if (g<0.0f)
				g=0.0f;

			if (b>1.0f)
				b=1.0f;
			if (b<0.0f)
				b=0.0f;

			if (a>1.0f)
				a=1.0f;
			if (a<0.0f)
				a=0.0f;
		}

		cColour cColour::lerp(const cColour & c2, float factor)
		{
			cColour result;
			result=(*this)*factor + c2*(1.0f-factor);

			return result;
		}

		cColour cColour::operator +(const cColour & rhs) const
		{
			cColour result;

			result.r=r+rhs.r;
			result.g=g+rhs.g;
			result.b=b+rhs.b;
			result.a=a+rhs.a;

			result.Clamp();

			return result;
		}

		cColour cColour::operator -(const cColour & rhs) const
		{
			cColour result;

			result.r=r-rhs.r;
			result.g=g-rhs.g;
			result.b=b-rhs.b;
			result.a=a-rhs.a;

			result.Clamp();

			return result;
		}

		cColour cColour::operator *(const cColour & rhs) const
		{
			cColour result;

			result.r=r*rhs.r;
			result.g=g*rhs.g;
			result.b=b*rhs.b;
			result.a=a*rhs.a;

			result.Clamp();

			return result;
		}

		cColour cColour::operator /(const cColour & rhs) const
		{
			cColour result;

			result.r=r/rhs.r;
			result.g=g/rhs.g;
			result.b=b/rhs.b;
			result.a=a/rhs.a;

			result.Clamp();

			return result;
		}

		cColour cColour::operator *(const float rhs) const
		{
			cColour result;

			result.r=r*rhs;
			result.g=g*rhs;
			result.b=b*rhs;
			result.a=a*rhs;

			result.Clamp();

			return result;
		}

		cColour cColour::operator /(const float rhs) const
		{
			cColour result;

			result.r=r/rhs;
			result.g=g/rhs;
			result.b=b/rhs;
			result.a=a/rhs;

			result.Clamp();

			return result;
		}

		bool cColour::operator ==(const cColour & rhs) const
		{
			if (r != rhs.r)
				return false;
			if (g != rhs.g)
				return false;
			if (b != rhs.b)
				return false;
			if (a != rhs.a)
				return false;

			return true;
		}

		bool cColour::operator !=(const cColour & rhs) const
		{
			return !((*this)==rhs);
		}

		cColour cColour::operator +=(const cColour & rhs)
		{
			(*this)=(*this)+rhs;

			return (*this);
		}

		cColour cColour::operator -=(const cColour & rhs)
		{
			(*this)=(*this)-rhs;

			return (*this);
		}

		cColour cColour::operator *=(const cColour & rhs)
		{
			(*this)=(*this)*rhs;

			return (*this);
		}

		cColour cColour::operator /=(const cColour & rhs)
		{
			(*this)=(*this)/rhs;

			return (*this);
		}

		cColour cColour::operator *=(const float rhs)
		{
			(*this)=(*this)*rhs;

			return (*this);
		}


		cColour cColour::operator /=(const float rhs)
		{
			(*this)=(*this)/rhs;

			return (*this);
		}
	}
}
