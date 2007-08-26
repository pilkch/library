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

		extern const float cSQUARE_ROOT_2;
		extern const float cEPSILON;
		extern const float cINFINITY;
		
		template <class T> inline T clamp(T i, T lower, T upper)
		{
			return (i < lower) ? lower : (i > upper) ? upper : i;
		}

		inline float toDegrees(float x)
		{
			return x * c180_DIV_PI;
		}

		inline float toRadians(float x)
		{
			return x * cPI_DIV_180;
		}

		inline float sqrf(float x)
		{
			return x * x;
		}

		inline unsigned int random(unsigned int maximum)
		{
			return rand() % maximum;
		}

		inline unsigned int random(unsigned int minimum, unsigned int maximum)
		{
			return minimum + rand() % maximum;
		}

		inline float randomZeroToOne()
		{
			return float(rand() % 10000) * 0.0001f;
		}

		inline float randomMinusOneToPlusOne()
		{
			return -1.0f + float(rand() % 20000) * 0.0001f;
		}
	}
}

#endif //CMATH_H
