#ifndef CVEC3_H
#define CVEC3_H

namespace BREATHE
{
	namespace MATH
	{
		//forward declaration
		class cQuaternion;

		class cVec3
		{
		public:
			inline cVec3() {}
			inline cVec3(float newX, float newY, float newZ)	{x=newX;	y=newY;		z=newZ;}
			inline cVec3(const float * rhs) { x=*rhs; y=*(rhs+1); z=*(rhs+2); }
			inline cVec3(const cVec3 & rhs) {x=rhs.x;	y=rhs.y;	z=rhs.z;}

			inline void Set(float newX, float newY, float newZ) { x=newX; y=newY; z=newZ; }
			inline void Set(const float *rhs) { x=rhs[0]; y=rhs[1]; z=rhs[2]; }

			inline float GetX() const {return x;}	
			inline float GetY() const {return y;}
			inline float GetZ() const {return z;}
			
			bool isZeroVector(void);

			inline void LoadZero(void) { x=y=z=0.0f; }
			void LoadOne(void);					//fill with (1, 1, 1)
			void Negate();
			
			//vector algebra
			void Cross(const cVec3 & a, const cVec3 & b);
			cVec3 CrossProduct(const cVec3 & rhs) const;
			
			inline float DotProduct(const cVec3 & rhs) const { return x*rhs.x + y*rhs.y + z*rhs.z; }
			
			
			void Normalize();
			cVec3 GetNormalized() const;
			
			void SetLength(float length);
			cVec3 SetLength(float length) const;

			float GetLength() const;

			float GetMagnitude() const;
			
			float GetSquaredLength() const;

			//rotations
			void RotateX(double angle);
			cVec3 GetRotatedX(double angle) const;
			void RotateY(double angle);
			cVec3 GetRotatedY(double angle) const;
			void RotateZ(double angle);
			cVec3 GetRotatedZ(double angle) const;
			void RotateAxis(double angle, const cVec3 & axis);
			cVec3 GetRotatedAxis(double angle, const cVec3 & axis) const;

			//rotate a vector by a cQuaternion
			void RotateByQuaternion(const cQuaternion & rhs);
			cVec3 GetRotatedByQuaternion(const cQuaternion & rhs);

			//pack to [0,1] for color
			void PackTo01();
			cVec3 GetPackedTo01() const;

			//linear interpolate
			cVec3 lerp(const cVec3 & v2, float factor);

			cVec3 GetEdgeVector(cVec3 & point2);

			//overloaded operators
			//binary operators
			inline cVec3 operator+(const cVec3 & rhs) const {	return cVec3(x + rhs.x, y + rhs.y, z + rhs.z);	}

			inline cVec3 operator-(const cVec3 & rhs) const {	return cVec3(x - rhs.x, y - rhs.y, z - rhs.z);	}

			cVec3 operator*(const float rhs) const;

			friend cVec3 operator*(const float rhs,const cVec3 &v)
			{
				return cVec3(rhs*v.x, rhs*v.y, rhs*v.z);
			}
			
			cVec3 operator/(const float rhs) const
			{	return (rhs==0) ? cVec3(0.0f, 0.0f, 0.0f) : cVec3(x / rhs, y / rhs, z / rhs);	}

			cVec3 operator/(const cVec3 & rhs) const
			{	return cVec3(x / rhs.x, y / rhs.y, z / rhs.z);	}

			cVec3 operator*(const cVec3 & rhs) const
			{	return cVec3(x * rhs.x, y * rhs.y, z * rhs.z);	}

			bool operator==(const cVec3 & rhs) const;
			bool operator!=(const cVec3 & rhs) const;

			//self-add etc
			void operator+=(const cVec3 & rhs)
			{	x+=rhs.x;	y+=rhs.y;	z+=rhs.z;	}

			void operator-=(const cVec3 & rhs);
			void operator*=(const float rhs);
			void operator/=(const float rhs);
			
			inline void operator=(const cVec3 &rhs) { x=rhs.x; y=rhs.y; z=rhs.z; }


			void operator*=(const cVec3 & rhs)
			{	x *= rhs.x; y *= rhs.y; z *= rhs.z;	}

			//unary operators
			cVec3 operator-(void) const {return cVec3(-x, -y, -z);}
			cVec3 operator+(void) const {return *this;}

			//cast to pointer to a (float *) for glVertex3fv etc
			operator float* () const {return (float*) this;}
			operator const float* () const {return (const float*) this;}

			float &operator[](int i)
			{
				if(0==i)
					return x;
				else if(1==i)
					return y;
				
				return z;
			}

			//member variables
			float x;
			float y;
			float z;
		};

		const cVec3 v3Up(0.0f, 0.0f, 1.0f);
		const cVec3 v3Down(0.0f, 0.0f, -1.0f);
		const cVec3 v3Left(1.0f, 0.0f, 0.0f);
		const cVec3 v3Right(-1.0f, 0.0f, 0.0f);
		const cVec3 v3Front(0.0f, 1.0f, 0.0f);
		const cVec3 v3Back(0.0f, -1.0f, 0.0f);
	}
}

#endif	//CVEC3_H