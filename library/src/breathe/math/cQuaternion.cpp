#include <cstdio>
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
#include <breathe/math/cQuaternion.h>
/*#include "breathe/cPlane.h"
#include "breathe/cCollision.h"
#include "breathe/cFrustum.h"
#include "breathe/cOctree.h"
#include "breathe/cColour.h"*/

namespace breathe
{
	namespace math
	{
		//load the identity quaternion
		void cQuaternion::LoadIdentity()
		{
			x = y = z = 0.0f;
			w=1.0f;
		}

		void cQuaternion::Normalise()
		{
			float magnitude = 1.0f / sqrtf(x*x + y*y + z*z + w*w);
			x *= magnitude;
			y *= magnitude;
			z *= magnitude;
			w *= magnitude;
		}

		//invert a quaternion
		void cQuaternion::Invert()
		{
			x=-x;
			y=-y;
			z=-z;
			//Note: w=w;
		}

		cQuaternion cQuaternion::Inverse()
		{
			return cQuaternion(-x, -y, -z, w);
		}

		cQuaternion cQuaternion::Conjugate()
		{
			return cQuaternion(-x, -y, -z, w);
		}

		//set a quaternion from angles of rotation about x, y, z
		void cQuaternion::SetFromAngles(const cVec3 & v)
		{
			float angle = v.z*0.5f;
			double sy = sin(angle);
			double cy = cos(angle);

			angle=v.y*0.5f;
			double sp = sin(angle);
			double cp = cos(angle);

			angle=v.x*0.5f;
			double sr = sin(angle);
			double cr = sin(angle);

			double crcp=cr*cp;
			double srsp=sr*sp;

			x=(float)(sr*cp*cy - cr*sp*sy);
			y=(float)(cr*sp*cy - sr*cp*sy);
			z=(float)(crcp*sy - srsp*cy);
			w=(float)(crcp*cy - srsp*sy);
		}

		void cQuaternion::Slerp(const cQuaternion & q1, cQuaternion q2, const float interpolation)
		{
			//see if q2 is backwards
			int i=0;
			float a=0.0f, b=0.0f;

			for (i=0; i<4; i++)
			{
				a+=(q1[i]-q2[i])*(q1[i]-q2[i]);
				b+=(q1[i]+q2[i])*(q1[i]+q2[i]);
			}
			if (a > b) //then invert q2
				q2.Invert();

			float cosom = q1[0]*q2[0] + q1[1]*q2[1] + q1[2]*q2[2] + q1[3]*q2[3];
			double scaleQ1, scaleQ2;

			if ((1.0 + cosom) > 0.00000001)
			{
				if ((1.0 - cosom) > 0.00000001)
				{
					float omega = acos(cosom);
					float sinom = sin(omega);
					scaleQ1 = sin((1.0f-interpolation)*omega)/sinom;
					scaleQ2 = sin(interpolation*omega)/sinom;
				}
				else
				{
					scaleQ1 = 1.0-interpolation;
					scaleQ2 = interpolation;
				}

				x = (float)(scaleQ1*q1[0] + scaleQ2*q2[0]);
				y = (float)(scaleQ1*q1[1] + scaleQ2*q2[1]);
				z = (float)(scaleQ1*q1[2] + scaleQ2*q2[2]);
				w = (float)(scaleQ1*q1[3] + scaleQ2*q2[3]);
			}
			else
			{
				x =-q1[1];
				y = q1[0];
				z =-q1[3];
				w = q1[2];

				scaleQ1 = sin((1.0f-interpolation)*0.5f*cPI);
				scaleQ2 = sin(interpolation*0.5f*cPI);

				x = (float) (scaleQ1*q1[0] + scaleQ2*x);
				y = (float) (scaleQ1*q1[1] + scaleQ2*y);
				z = (float) (scaleQ1*q1[2] + scaleQ2*z);
			}
		}

		void cQuaternion::SetFromAxisAngle(const cVec3 & axis, float angle)
		{
			cVec3 normAxis=axis.GetNormalized();

			float sinHalfAngle=sin((float)(angle/2.0));
			float cosHalfAngle=cos((float)(angle/2.0));

			x=sinHalfAngle*normAxis.x;
			y=sinHalfAngle*normAxis.y;
			z=sinHalfAngle*normAxis.z;
			w=cosHalfAngle;
		}

		void cQuaternion::SetFromVectors(const cVec3 & source, const cVec3 & destination)
		{
			cVec3 axis=source.CrossProduct(destination);

			float angle=acos(source.GetNormalized().DotProduct(destination.GetNormalized()));

			SetFromAxisAngle(axis, angle);
		}

		void cQuaternion::SetFromODEQuaternion(float* q)
		{
			w=q[0];
			x=q[1];
			y=q[2];
			z=q[3];
		}


		float cQuaternion::GetAngle(void) const
		{
			return 2.0f*acos(w);
		}

		cVec3 cQuaternion::GetAxis(void) const
		{
			double angle=GetAngle();

			float scale=1.0f/(sin((float)angle*0.5f));

			cVec3 result;
			result.x=scale*x;
			result.y=scale*y;
			result.z=scale*z;

			return result;
		}


		cVec3 cQuaternion::GetEuler(void) const
		{
			float sqw = w*w;
			float sqx = x*x;
			float sqy = y*y;
			float sqz = z*z;

			return cVec3(	(atan2(2.0f * (y*z + x*w) , (-sqx - sqy + sqz + sqw)) * c180_DIV_PI),
										(asin(-2.0f * (x*z - y*w)) * c180_DIV_PI),
										(atan2(2.0f * (x*y + z*w),(sqx - sqy - sqz + sqw))	* c180_DIV_PI));
		}

		cQuaternion cQuaternion::operator*(cQuaternion& q)
		{
			return cQuaternion(	w*q.x + x*q.w + y*q.z - z*q.y,
													w*q.y + y*q.w + z*q.x - x*q.z,
													w*q.z + z*q.w + x*q.y - y*q.x,
													w*q.w - x*q.x - y*q.y - z*q.z);
		}


		cMat4 cQuaternion::GetMatrix() const
		{
			double x2,y2,z2,w2,xy,yz,xz,wx,wy,wz;
			//double xx,yy,zz;
			cMat4 mat;

			x2 = x*x;
			y2 = y*y;
			z2 = z*z;
			w2 = w*w;

			xy = x * y;
			xz = x * z;
			yz = y * z;
			wx = w * x;
			wy = w * y;
			wz = w * z;

			/*mat[0] = w2 + x2 -y2 - z2;
			mat[1] = 2 * (xy + wz);
			mat[2] = 2 * (xz - wy);
			mat[3] = 0.0;

			mat[4] = 2 * (xy - wz);
			mat[5] = w2 - x2 + y2 - z2;
			mat[6] = 2 * (yz + wx);
			mat[7] = 0.0;

			mat[8] = 2 * (xz + wy);
			mat[9] = 2 * (yz - wx);
			mat[10] = w2 - x2 - y2 + z2;
			mat[11] = 0.0;

			mat[12] = 0.0;
			mat[13] = 0.0;
			mat[14] = 0.0;
			mat[15] = 1.0;*/


			mat[ 0] = 1.0f - 2.0f * ( y * y + z * z );
			mat[ 1] = 2.0f * (x * y + z * w);
			mat[ 2] = 2.0f * (x * z - y * w);
			mat[ 3] = 0.0f;

			mat[ 4] = 2.0f * ( x * y - z * w );
			mat[ 5] = 1.0f - 2.0f * ( x * x + z * z );
			mat[ 6] = 2.0f * (z * y + x * w );
			mat[ 7] = 0.0f;

			mat[ 8] = 2.0f * ( x * z + y * w );
			mat[ 9] = 2.0f * ( y * z - x * w );
			mat[10] = 1.0f - 2.0f * ( x * x + y * y );
			mat[11] = 0.0f;

			mat[12] = 0;
			mat[13] = 0;
			mat[14] = 0;
			mat[15] = 1.0f;

			/*printf( "GetMatrix()\n"
			"%.02f, %.02f, %.02f, %.02f\n"
			"%.02f, %.02f, %.02f, %.02f\n"
			"%.02f, %.02f, %.02f, %.02f\n"
			"%.02f, %.02f, %.02f, %.02f\n\n",
			mat[0], mat[1], mat[2], mat[3],
			mat[4], mat[5], mat[6], mat[7],
			mat[8], mat[9], mat[10], mat[11],
			mat[12], mat[13], mat[14], mat[15]);*/

			return mat;
		}
	}
}
