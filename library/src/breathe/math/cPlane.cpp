#include <cstdlib>
#include <cmath>

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
		cPlane::cPlane(const cVec3& origin, const cVec3& normal)
		{
			this->normal = normal;
			this->origin = origin;
			equation[0] = normal.x;
			equation[1] = normal.y;
			equation[2] = normal.z;
			equation[3] = -(normal.x*origin.x+normal.y*origin.y+normal.z*origin.z);
		}

		// Construct from triangle:
		cPlane::cPlane(const cVec3& p1,const cVec3& p2,const cVec3& p3)
		{
			normal = (p2-p1).CrossProduct(p3-p1);
			normal.Normalise();
			origin = p1;
			equation[0] = normal.x;
			equation[1] = normal.y;
			equation[2] = normal.z;
			equation[3] = -(normal.x*origin.x+normal.y*origin.y+normal.z*origin.z);
		}

		bool cPlane::IsFrontFacingTo(const cVec3& direction) const
		{
			double dot = normal.DotProduct(direction);
			return (dot <= 0);
		}

		double cPlane::SignedDistanceTo(const cVec3& point) const
		{
			return (point.DotProduct(normal)) + equation[3];
		}






		cPlane::cPlane(const cPlane & rhs)
		{
			normal=rhs.normal;
			intercept=rhs.intercept;
		}

		void cPlane::SetFromPoints(const cVec3 & p0, const cVec3 & p1, const cVec3 & p2)
		{
			normal=(p1-p0).CrossProduct(p2-p0);

                                                normal.Normalise();

			CalculateIntercept(p0);
		}

                                void cPlane::Normalise()
		{
			float normalLength=normal.GetLength();
			normal/=normalLength;
			intercept/=normalLength;
		}

		bool cPlane::Intersect3(const cPlane & p2, const cPlane & p3, cVec3 & result)//find point of intersection of 3 planes
		{
			float denominator=normal.DotProduct((p2.normal).CrossProduct(p3.normal));
													//scalar triple product of normals
			if (denominator==0.0f)									//if zero
				return false;										//no intersection

			cVec3 temp1, temp2, temp3;
			temp1=(p2.normal.CrossProduct(p3.normal))*intercept;
			temp2=(p3.normal.CrossProduct(normal))*p2.intercept;
			temp3=(normal.CrossProduct(p2.normal))*p3.intercept;

			result=(temp1+temp2+temp3)/(-denominator);

			return true;
		}

		cVec3 cPlane::GetReflected(cVec3 v)
		{
      return cVec3(-2.0f*(v.DotProduct(normal)*normal + v));
		}

		float cPlane::DistancePoint(const cVec3 & point) const
		{
			return point.DotProduct(normal)+intercept;
		}

		int cPlane::ClassifyPoint(const cVec3 & point) const
		{
			if (point.DotProduct(normal)+intercept==0.0f)
				return POINT_ON_cPlane;

			if (point.DotProduct(normal)+intercept>0.0f)
				return POINT_IN_FRONT_OF_cPlane;

			return POINT_BEHIND_cPlane;	//otherwise
		}

		cPlane cPlane::lerp(const cPlane & p2, float factor)
		{
			cPlane result;
			result.normal=normal*factor + p2.normal*(1.0f-factor);
                                                result.normal.Normalise();

			result.intercept=intercept*factor + p2.intercept*(1.0f-factor);

			return result;
		}

		bool cPlane::operator ==(const cPlane & rhs) const
		{
			if (normal==rhs.normal && intercept==rhs.intercept)
				return true;

			return false;
		}

		bool cPlane::operator !=(const cPlane & rhs) const
		{
			return!((*this)==rhs);
		}
	}
}
