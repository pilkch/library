#ifndef CMATH_H
#define CMATH_H

namespace BREATHE
{
	namespace MATH
	{
		//Constants
		extern const float cPI;
		extern const float cPI_PI;
		extern const float cPI_DIV_2;
		extern const float cPI_DIV_180;
		extern const float c180_DIV_PI;

		extern const float cEPSILON;
		extern const float cINFINITY;
		
		template <class T> inline T clamp(T i, T lower, T upper)
		{ 
			return (i < lower) ? lower : (i > upper) ? upper : i;
		}

		//Square
		inline float sqr(float a)
		{
			return a*a;
		}

		//Square Root
		inline float sqrt(float a)
		{
			//return sqrtf(a);

			__asm
			{
				fld a;
				fsqrt;
				fstp a;
			}
			return a;
		}

		//Trig functions
		inline float acos(float x)
		{
			return acosf(x);
		}
		
		inline float cos(float x)
		{
			return cosf(x);
		}

		inline float sin(float x)
		{
			return sinf(x);
		}

		inline float atan2(float x, float y)
		{
			return atan2f(x, y);
		}
	}
}

#endif //CMATH_H
