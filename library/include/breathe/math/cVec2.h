#ifndef CVEC2_H
#define CVEC2_H

namespace breathe
{
	namespace math
	{
		class cVec2
		{
		public:
			inline cVec2() {}
			inline cVec2(float newX, float newY) {x=newX;	y=newY;}
			inline cVec2(const float * rhs);
			inline cVec2(const cVec2 & rhs) {x=rhs.x;	y=rhs.y;}
			inline ~cVec2() {}	//empty

			void Set(float newX, float newY);	//set member variables
			
			//Accessors kept for compatibility
			void SetX(float newX) {x = newX;}
			void SetY(float newY) {y = newY;}

			float GetX() const {return x;}	//public accessor functions
			float GetY() const {return y;}	//inline, const

			void LoadZero(void);
			void LoadOne(void);					//fill with (1, 1, 1)
			
			void Normalize();
			cVec2 GetNormalized() const;
			
			void SetLength(float length);
			cVec2 SetLength(float length) const;

			float GetLength() const
			{	return sqrtf((x*x)+(y*y));	}
			
			float GetSquaredLength() const
			{	return (x*x)+(y*y);	}

			//pack to [0,1] for color
			void PackTo01();
			cVec2 GetPackedTo01() const;

			//linear interpolate
			cVec2 lerp(const cVec2 & v2, float factor);

			//overloaded operators
			//binary operators
			cVec2 operator+(const cVec2 & rhs) const
			{	return cVec2(x + rhs.x, y + rhs.y);	}

			cVec2 operator-(const cVec2 & rhs) const
			{	return cVec2(x - rhs.x, y - rhs.y);	}

			cVec2 operator*(const float rhs) const;
			
			cVec2 operator/(const float rhs) const
			{	return (rhs==0) ? cVec2(0.0f, 0.0f) : cVec2(x / rhs, y / rhs);	}

			cVec2 operator/(const cVec2 & rhs) const
			{	return cVec2(x / rhs.x, y / rhs.y);	}

			cVec2 operator*(const cVec2 & rhs) const
			{	return cVec2(x * rhs.x, y * rhs.y);	}

			bool operator==(const cVec2 & rhs) const;
			bool operator!=(const cVec2 & rhs) const;

			//self-add etc
			void operator+=(const cVec2 & rhs)
			{	x+=rhs.x;	y+=rhs.y;}

			void operator-=(const cVec2 & rhs);
			void operator*=(const float rhs);
			void operator/=(const float rhs);

			//unary operators
			cVec2 operator-(void) const {return cVec2(-x, -y);}
			cVec2 operator+(void) const {return *this;}

			//cast to pointer to a (float *) for glVertex3fv etc
			operator float* () const {return (float*) this;}
			operator const float* () const {return (const float*) this;}

			//member variables

			// We do this so that x maps to u and y maps to v
			// for use in texture mapping as well as position coords
			union
			{
				struct
				{
					float x;
					float y;
				};

				struct
				{
					float u;
					float v;
				};
			};
		};
	}
}

#endif	//CVEC2_H
