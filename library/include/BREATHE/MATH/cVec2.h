#ifndef CVEC2_H
#define CVEC2_H

namespace BREATHE
{
	namespace MATH
	{
		class cVec2
		{
		public:
			cVec2(float newX = 0.0f, float newY = 0.0f)		//constructors
			{x=newX;	y=newY;}
			cVec2(const float * rhs);
			cVec2(const cVec2 & rhs)
			{x=rhs.x;	y=rhs.y;}
			~cVec2() {}	//empty

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
			{	return BREATHE::MATH::sqrt((x*x)+(y*y));	}
			
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
			{	return (rhs==0) ? 0.0f : cVec2(x / rhs, y / rhs);	}

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
			union
			{
				float x;
				float u;
			};

			union
			{
				float y;
				float v;
			};
		};
	}
}

#endif	//CVEC2_H
