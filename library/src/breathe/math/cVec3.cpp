#include <cstdlib>
#include <cmath>

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
		cVec3 cVec3::GetEdgeVector(cVec3 & point2)
		{
			cVec3 temp_vector;
			temp_vector.x = x - point2.x;
			temp_vector.y = y - point2.y;
			temp_vector.z = z - point2.z;
			return temp_vector;
		}

		float cVec3::GetLength() const
		{	
			return sqrtf((x*x)+(y*y)+(z*z));
		}

		float cVec3::GetMagnitude() const
		{	
			return GetLength(); 
		}
			
		float cVec3::GetSquaredLength() const
		{	
			return (x*x)+(y*y)+(z*z);	
		}

		bool cVec3::isZeroVector(void)
		{
			return (x<cEPSILON && x>-cEPSILON &&
							y<cEPSILON && y>-cEPSILON &&
							z<cEPSILON && z>-cEPSILON);
		}

		void cVec3::LoadOne(void)
		{
			x=1.0f;
			y=1.0f;
			z=1.0f;
		}

		void cVec3::Negate(void)
		{
			x=-x;
			y=-y;
			z=-z;
		}

		void cVec3::Cross(const cVec3 & a, const cVec3 & b)
		{
			x = a.y*b.z - a.z*b.y;
			y = a.z*b.x - a.x*b.z;
			z = a.x*b.y - a.y*b.x;
		}

		cVec3 cVec3::CrossProduct(const cVec3 & rhs) const
		{
			cVec3 result;

			result.x = y*rhs.z - z*rhs.y;
			result.y = z*rhs.x - x*rhs.z;
			result.z = x*rhs.y - y*rhs.x;

			return result;
		}

		void cVec3::Normalize()
		{
			float length;
			float scalefactor;
			length=GetLength();

			if(length==1 || length==0)			//return if length is 1 or 0
				return;

			scalefactor = 1.0f/length;

			if(cEPSILON>x && -cEPSILON<x)
				x=0.0f;
			else
        x *= scalefactor;

			if(cEPSILON>y && -cEPSILON<y)
				y=0.0f;
			else
        y *= scalefactor;

			if(cEPSILON>z && -cEPSILON<z)
				z=0.0f;
			else
        z *= scalefactor;
		}

		void cVec3::SetLength(float length)
		{
			float scalefactor;
			
			if(length==1 || length==0)			//return if length is 1 or 0
				return;

			scalefactor = 1.0f/length;
			x *= scalefactor;
			y *= scalefactor;
			z *= scalefactor;
		}

		cVec3 cVec3::SetLength(float length) const
		{
			cVec3 result(*this);

			result.SetLength(length);

			return result;
		}

		cVec3 cVec3::GetNormalized() const
		{
			cVec3 result(*this);

			result.Normalize();

			return result;
		}

		cVec3 cVec3::GetRotatedX(double angle) const
		{
			return cVec3(	x,
						y*cos((float)angle) - z*sin((float)angle),
						y*sin((float)angle) + z*cos((float)angle));
		}

		void cVec3::RotateX(double angle)
		{
			(*this)=GetRotatedX(angle);
		}

		cVec3 cVec3::GetRotatedY(double angle) const
		{
			return cVec3(	x*cos((float)angle) + z*sin((float)angle),
										y,
										-x*sin((float)angle) + z*cos((float)angle));
		}

		void cVec3::RotateY(double angle)
		{
			(*this)=GetRotatedY(angle);
		}

		cVec3 cVec3::GetRotatedZ(double angle) const
		{
			return cVec3(	x*cos((float)angle) - y*sin((float)angle),
										x*sin((float)angle) + y*cos((float)angle),
										z);
		}

		void cVec3::RotateZ(double angle)
		{
			(*this)=GetRotatedZ(angle);
		}

		cVec3 cVec3::GetRotatedAxis(double angle, const cVec3 & axis) const
		{
			cVec3 u=axis.GetNormalized();

			cVec3 rotMatrixRow0, rotMatrixRow1, rotMatrixRow2;

			cVec3 result;

			float sinAngle=sin((float)angle);
			float cosAngle=cos((float)angle);
			float oneMinusCosAngle=1.0f-cosAngle;

			rotMatrixRow0.x=(u.x)*(u.x) + cosAngle*(1-(u.x)*(u.x));
			rotMatrixRow0.y=(u.x)*(u.y)*(oneMinusCosAngle) - sinAngle*u.z;
			rotMatrixRow0.z=(u.x)*(u.z)*(oneMinusCosAngle) + sinAngle*u.y;

			rotMatrixRow1.x=(u.x)*(u.y)*(oneMinusCosAngle) + sinAngle*u.z;
			rotMatrixRow1.y=(u.y)*(u.y) + cosAngle*(1-(u.y)*(u.y));
			rotMatrixRow1.z=(u.y)*(u.z)*(oneMinusCosAngle) - sinAngle*u.x;
			
			rotMatrixRow2.x=(u.x)*(u.z)*(oneMinusCosAngle) - sinAngle*u.y;
			rotMatrixRow2.y=(u.y)*(u.z)*(oneMinusCosAngle) + sinAngle*u.x;
			rotMatrixRow2.z=(u.z)*(u.z) + cosAngle*(1-(u.z)*(u.z));

			result.x=this->DotProduct(rotMatrixRow0);
			result.y=this->DotProduct(rotMatrixRow1);
			result.z=this->DotProduct(rotMatrixRow2);

			return result;
		}

		void cVec3::RotateAxis(double angle, const cVec3 & axis)
		{
			(*this)=GetRotatedAxis(angle, axis);
		}

		void cVec3::RotateByQuaternion(const cQuaternion & rhs)
		{
			RotateAxis(rhs.GetAngle(), rhs.GetAxis());
		}

		cVec3 cVec3::GetRotatedByQuaternion(const cQuaternion & rhs)
		{
			return cVec3(GetRotatedAxis(rhs.GetAngle(), rhs.GetAxis()));
		}

		void cVec3::PackTo01()
		{
			(*this)=GetPackedTo01();	
		}

		cVec3 cVec3::GetPackedTo01() const
		{
			cVec3 temp(*this);

			temp.Normalize();

			temp=temp*0.5f+cVec3(0.5f, 0.5f, 0.5f);
			
			return temp;
		}

		cVec3 cVec3::lerp(const cVec3 & v2, float factor)
		{
			cVec3 result;

			result=(*this)*factor + v2*(1.0f-factor);

			return result;
		}

		cVec3 cVec3::operator*(const float rhs) const
		{
			float newX=x * rhs;
			float newY=y * rhs;
			float newZ=z * rhs;

			return cVec3(newX, newY, newZ);
		}

		bool cVec3::operator==(const cVec3 & rhs) const
		{
			return (x==rhs.x && y==rhs.y && z==rhs.z);

			if(x==rhs.x && y==rhs.y && z==rhs.z)
				return true;

			return false;
		}

		bool cVec3::operator!=(const cVec3 & rhs) const
		{
			return !((*this)==rhs);
		}

		void cVec3::operator-=(const cVec3 & rhs)
		{
			x-=rhs.x;
			y-=rhs.y;
			z-=rhs.z;
		}

		void cVec3::operator*=(const float rhs)
		{
			x*=rhs;
			y*=rhs;
			z*=rhs;
		}

		void cVec3::operator/=(const float rhs)
		{
			x/=rhs;
			y/=rhs;
			z/=rhs;
		}
	}
}
