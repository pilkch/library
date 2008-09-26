#ifndef CQUATERNION_H
#define CQUATERNION_H

// A quaternion stores a rotation of t (ccw) about (a, b, c) (normalised) as:
// x = sin(t/2) * a
// y = sin(t/2) * b
// z = sin(t/2) * c
// w = cos(t/2)

namespace breathe
{
	namespace math
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
				if (0==index) return x;
				if (1==index) return y;
				if (2==index) return z;

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
			float GetAngle() const;
			cVec3 GetAxis() const;
			cVec3 GetEuler() const;

			cMat4 GetMatrix() const;


			//default constructor
			cQuaternion()
			{
				LoadIdentity();
			}

      cQuaternion(float x0, float y0, float z0, float w0) :
        w(w0),
        x(x0),
        y(y0),
        z(z0)
      {
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

      //TODO: Think about switching these around to (x, y, z, w)
      float w, x, y, z;
    };
  }
}

#endif // CQUATERNION_H
