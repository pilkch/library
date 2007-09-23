// a quaternion stores a rotation of t (ccw) about (a, b, c) (normalised) as:
//x = sin(t/2) * a
//y = sin(t/2) * b
//z = sin(t/2) * c
//w = cos(t/2)

#ifndef CQUATERNION_H
#define CQUATERNION_H

namespace breathe
{
	namespace MATH
	{
		class cQuaternion
		{
		public:
			//load the identity quaternion
			void LoadIdentity();

			void Normalise();

			//set from 2 quaternions
			//if the second is backwards, it will be modified
			void Slerp(const cQuaternion & q1, cQuaternion q2, const float interpolation);

			//retrieve a value
			float operator[](int index) const
			{
				if(0==index) return x;
				if(1==index) return y;
				if(2==index) return z;

				return w;
			}

			//cast to pointer to a (float*)
			operator float* () const {return (float*) this;}
			operator const float* () const {return (const float*) this;}

			cQuaternion operator*(cQuaternion& q);
					
			//Invert the quaternion
			void Invert();

			cQuaternion Inverse();
			cQuaternion Conjugate();

			//set from angles of rotation about x, y, z axes
			void SetFromAngles(const cVec3 & v);
			
			//set from axis-angle combination
			void SetFromAxisAngle(const cVec3 & axis, float angle);

			//set from vectors (source and destination)
			void SetFromVectors(const cVec3 & source, const cVec3 & destination);

			//set from ODE style quaternion (w, x, y, z)
			void SetFromODEQuaternion(float* q);

			//retrieve axis/angle
			float GetAngle(void) const;
			cVec3 GetAxis(void) const;
			cVec3 GetEuler(void) const;

			
			//default constructor
			cQuaternion(void)
			{
				LoadIdentity();
			}

			cQuaternion(float x0, float y0, float z0, float w0)
			{
				x=x0;
				y=y0;
				z=z0;
				w=w0;
			}

			//constructor - retrieves values from angle of rotation about x, y, z axes
			cQuaternion(const cVec3 & v)
			{
				SetFromAngles(v);
			}

			//constructor - retrieves values from linear interpolation betwwen 2 quaternions
			cQuaternion(const cQuaternion & q1, cQuaternion q2, float interpolation)
			{
				Slerp(q1, q2, interpolation);
			}

			cMat4 GetMatrix();

			float w, x, y, z;
		};
	}
}

#endif	//CQUATERNION_H
