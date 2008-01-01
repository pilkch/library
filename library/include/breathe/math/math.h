#ifndef CMATH_H
#define CMATH_H

template <class T> inline T min(const T& a, const T& b)
{
  return (a < b) ? a : b;
}

template <class T> inline T max(const T& a, const T& b)
{
  return (a > b) ? a : b;
}

template <class T> inline T min_max(const T& i, const T& lower, const T& upper)
{
	return (i < lower) ? lower : (i > upper) ? upper : i;
}

namespace breathe
{
	namespace math
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

		
		template <class T> inline T clamp(const T& i, const T& lower, const T& upper)
		{
			return min_max(i, lower, upper);
		}
		
		inline float clamp(float i)
		{
			return clamp(i, 0.0f, 1.0f);
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

		inline uint32_t random(uint32_t maximum)
		{
			return rand() % maximum;
		}

		inline uint32_t random(uint32_t minimum, uint32_t maximum)
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
		
		///This function gets the first power of 2 >= the int that we pass it.
		inline int nextPowerOfTwo(int a)
		{
			int rval = 1;
			while(rval < a) rval<<= 1;
			return rval;
		}
	}
}

#endif //CMATH_H
