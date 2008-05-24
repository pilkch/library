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
    // Constants

    const float cPI             = 3.14159265358979323846f;
    const float cPI_PI          = 6.28318530717958647692f;
    const float cPI_DIV_2       = 1.57079632679489661923f;
    const float cPI_DIV_180     = 0.01745329251994329576f;
    const float c180_DIV_PI     = 57.2957795130823208767f;

    const float cSQUARE_ROOT_2  = 1.41421356237309504880f;

    const float cEPSILON        = 0.001f;//10e-6f //std::numeric_limits<float>::epsilon();
    extern const float cINFINITY;


    // http://www.racer.nl/tech/converting.htm

    inline float kphTomph(float kph) { return kph * 1.609344f; }
    inline float mphTokph(float mph) { return mph * 0.621371192f; }

    // Power/Torque
    // kW/Nm : hp/lb-ft

    inline float HPToKw(float hp) { return hp * 0.74569987158227022f; }
    inline float KwToHP(float kW) { return kW * 1.34102209f; }

    inline float NmTolbft(float Nm) { return Nm * 1.3558179483314004f; }
    inline float lbftToNm(float lbft) { return lbft * 0.7375621f; }

    // wheel torque = torque at the engine * gear ratio
    // speed = rpm * gear ratio * circumference of the wheels

    // power = torque * angular_speed
    // angular_speed = 2 * pi * rotational_speed
    // rotational_speed = revolutions per unit time.

    // power (in kW) = torque (in Nm) * 2 * pi * rotational speed (in rpm) / 60000.0f
    // where 60000.0f comes from 60 seconds per minute times 1000.0f watts per kilowatt.

    // kW = Nm * 2 * pi * RPM / 60000;
    // Nm = kW / (2 * pi * RPM / 60000);

    const float c1_OVER_RPM_KW = 1.0f / 60000.0f;

    inline float NmToKw(float Nm, float RPM) { return Nm * cPI_PI * RPM * c1_OVER_RPM_KW; }
    inline float KwToNm(float kW, float RPM) { return kW / (cPI_PI * RPM * c1_OVER_RPM_KW); }

    inline float RPMToRadiansPerSecond(float RPM) { return RPM * 0.10471976f; }
    inline float RadiansPerSecondToRPM(float RadiansPerSecond) { return RadiansPerSecond * 9.5493f; }

    // Utility functions

    /*template <class T>
    inline T ConvertFromHexDigit(char hex)
    {
      if (is_alpha(hex)) return hex - '0';
      else if (isUpper(hex)) return hex - 'A' + 0xA;

      return hex - 'a' + 0xa;
    }

    template <class T>
    inline char ConvertToHexDigit(T value)
    {
      ASSERT(value < 16);

      if (value < 10) return '0' + value;

      return 'a' + value - 10;
    }*/

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

    inline bool PointIsWithinBounds(float x, float y, float bounds_x, float bounds_y, float bounds_width, float bounds_height)
    {
      return
        ((x > bounds_x) && (x < (bounds_x + bounds_width))) &&
        ((y > bounds_y) && (y < (bounds_y + bounds_height)));
    }
	}
}

#endif //CMATH_H
