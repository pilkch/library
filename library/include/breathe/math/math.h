#ifndef CMATH_H
#define CMATH_H

template <class T>
inline T min(const T& a, const T& b)
{
  return (a < b) ? a : b;
}

template <class T>
inline T max(const T& a, const T& b)
{
  return (a > b) ? a : b;
}

// TODO: Rename this clamp
template <class T>
inline T min_max(const T& i, const T& lower, const T& upper)
{
  return (i < lower) ? lower : (i > upper) ? upper : i;
}


namespace breathe
{
  namespace math
  {
    // Constants
    const float_t cPI               = 3.1415926535897932384626433832795f;
    const float_t c2_PI             = 6.28318530717958647692f;
    const float_t cPI_DIV_2         = 1.57079632679489661923f;
    const float_t cPI_DIV_180       = 0.0174532925199432957692369076848861f;
    const float_t c180_DIV_PI       = 57.2957795130823208767981548141052f;

    const float_t c4_DIV_3          = 4.0f / 3.0f;
    const float_t cSQUARE_ROOT_OF_2 = 1.41421356237309504880f;

    const float_t cEPSILON          = 0.001f;//10e-6f //std::numeric_limits<float>::epsilon();
    extern const float_t cINFINITY;

    inline float RadiansToDegrees(float x) { return x * c180_DIV_PI; }
    inline float DegreesToRadians(float x) { return x * cPI_DIV_180; }

    // Utility functions

    // http://www.racer.nl/tech/converting.htm
    // TODO: Consider using Boost.Units library
    // http://www.boost.org/doc/libs/1_37_0/doc/html/boost_units.html

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

    inline float NmToKw(float Nm, float RPM) { return Nm * c2_PI * RPM * c1_OVER_RPM_KW; }
    inline float KwToNm(float kW, float RPM) { return kW / (c2_PI * RPM * c1_OVER_RPM_KW); }

    inline float RPMToRadiansPerSecond(float RPM) { return RPM * 0.10471976f; }
    inline float RadiansPerSecondToRPM(float RadiansPerSecond) { return RadiansPerSecond * 9.5493f; }

    inline float rad_sToRPM(float rad_s) { return rad_s * 30.0f / cPI; }
    inline float rpm_to_rad_s(float rpm) { return rpm * cPI / 30.0f; }

    inline float m_s_to_km_h(float m_s) { return m_s * 3.6f; }
    inline float km_h_to_m_s(float km_h) { return km_h / 3.6f; }

    // ^ = square
    // 1 pascal (Pa) ≡ 1 N/(m^2) ≡ 1 J/(m^3) ≡ 1 kg/(m*(s^2))
    inline float PaTopsi(float Pa) { return Pa * 6894.75728001037f; }
    inline float psiToPa(float psi) { return psi * 0.000145037681f; }

    inline float KPaTopsi(float KPa) { return KPa * 6.89475728001037f; }
    inline float psiToKPa(float psi) { return psi * 0.145037738007f; }


    template<class T> bool IsPowerOf2(T value) { return (value & (~value + 1)) == value; }

    template<class T>
    T GetLog2(T value)
    {
      T power = 0;
      while (value != 0) {
        value >>= 1;
        power++;
      };

      return power - 1;
    }

    // Return -1, 0, 1 for VALUE < 0, == 0, > 0, respectively.
    template <typename T> T sign(T value) { return value == 0 ? 0 : (value > 0 ? 1 : -1); }

    // Clip VALUE to the range LOW--HIGH.
    template <typename T> T clip(T value, T low, T high) { return max(min(value, high), low); }

    // True if VALUE is in the range LOW--HIGH.
    template <typename T> bool IsInRange(T value, T low, T high) { return (value >= low) && (value <= high); }

    // Return LOW or HIGH, whichever is closer to VALUE.
    template <typename T> T closer(T value, T low, T  high) { return (std::abs(value - low) < std::abs(value - high)) ? low : high; }

    // Return angle in the interval [minimum, minimum + 2pi].
    template <typename T> T branch (T angle, T minimum)
    {
      while (angle > minimum + c2_PI) angle -= c2_PI;
      while (angle <= minimum) angle += c2_PI;

      return angle;
    }

    // Square a number
    template <typename T> T square(T value) { return value * value; }

    // Cube a number
    template <typename T> T cube(T value) { return value * value * value; }

    // Areas and volumes
    template<class T> T AreaOfCircle(T r) { return T(cPI) * r * r; }
    template<class T> T SurfaceAreaOfSphere(T r) { return T(4.0f * cPI) * r * r; }
    template<class T> T VolumeOfSphere(T r) { return T(c4_DIV_3 * cPI) * r * r * r; }

    // Return f(x) for a line with slope through (x1, y1).
    template <typename T> T intercept(T x, T x1, T y1, T slope) { return y1 - slope * (x1 - x); }


    // *** Interpolation

    template <class T> inline T lerp(T a, T b, T mu) { return a + ((b - a) * mu); }

    // http://local.wasp.uwa.edu.au/~pbourke/other/interpolation/

    // ** Two points

    // Return f(mu) for the linear interpolation between (x1, y1) and (x2, y2) (where x1 and x2 are not equally spaced?)
    // mu is between x1 and x2
    template <typename T>
    T interpolate_linear(T x1, T y1, T x2, T y2, T mu)
    {
      ASSERT(mu >= x1);
      ASSERT(mu <= x2);
      return y1 + (y2 - y1) * (mu - x1) / (x2 - x1);
    }

    // Return f(mu) for the linear interpolation where y0, y1, y2, y3 are all equally spaced
    // mu is between 0.0f and 1.0f
    template <typename T>
    T interpolate_linear(T y1, T y2, T mu)
    {
      ASSERT(mu >= 0.0f);
      ASSERT(mu <= 1.0f);
      return y1 * (1.0f - mu) + (y2 * mu);
    }

    // Return f(mu) for the cosine interpolation where y0, y1, y2, y3 are all equally spaced
    template <typename T>
    T interpolate_cosine(T y1, T y2, T mu)
    {
      const T mu2 = (1.0f - cos(mu * cPI)) * 0.5f;
      return (y1 * (1.0f - mu2) + y2 * mu2);
    }


    // ** Four points

    // Return f(mu) for the cubic interpolation where y0, y1, y2, y3 are all equally spaced
    template <typename T>
    T interpolate_cubic(T y0, T y1, T y2, T y3, T mu)
    {
      const T a0 = y3 - y2 - y0 + y1;
      const T a1 = y0 - y1 - a0;
      const T a2 = y2 - y0;
      const T a3 = y1;

      return (
        (a0 * cube(mu)) +
        (a1 * square(mu)) +
        (a2 * mu) +
        (a3)
      );
    }


    template <class T> inline T clamp(const T& i, const T& lower, const T& upper)
    {
      return min_max(i, lower, upper);
    }

    inline float clamp(float i)
    {
      return clamp(i, 0.0f, 1.0f);
    }

    // Lineary interpolate from a to b using s = { 0..1 }
    inline float mix(float a, float b, float s)
    {
      if (s > 1.0f) return b;
      if (s <= 0.0f) return a;

      return ((1.0f - s) * a  + s * b);
    }

    inline float squared(float x)
    {
      return x * x;
    }

    inline float sqrtf(float x)
    {
      return ::sqrtf(x);
    }

    // This function gets the first power of 2 >= the int that we pass it.
    inline int nextPowerOfTwo(int a)
    {
      int rval = 1;
      while(rval < a) rval<<= 1;
      return rval;
    }

    inline bool AreApproximatelyEqual(float_t a, float_t b)
    {
      return (((a + cEPSILON) > b) && ((a - cEPSILON) < b));
    }

    inline bool IsApproximatelyZero(float_t value)
    {
      return ((value < cEPSILON) && (value > -cEPSILON));
    }

    inline bool PointIsWithinBounds(float x, float y, float bounds_x, float bounds_y, float bounds_width, float bounds_height)
    {
      return
        ((x > bounds_x) && (x < (bounds_x + bounds_width))) &&
        ((y > bounds_y) && (y < (bounds_y + bounds_height)));
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

    class cScopedPredictableRandom
    {
    public:
      cScopedPredictableRandom(uint32_t seed, uint32_t maximum);

      uint32_t GetRandomNumber0ToMaximum() const;
      float GetRandomNumber0To1() const;

    private:
      uint32_t GetRandomNumber() const;

      // The maximum number that can be generated by this generator
      const uint32_t maximum;

      // For random number generation
      const float fOneOver65536By65536;
      mutable uint32_t x[5]; // History buffer
    };

    inline uint32_t cScopedPredictableRandom::GetRandomNumber() const
    {
      // Calculate the new result
      const uint64_t sum =
        2111111111ul * uint64_t(x[3]) +
        1492ul * uint64_t(x[2]) +
        1776ul * uint64_t(x[1]) +
        5115ul * uint64_t(x[0]) +
        uint64_t(x[4]);

      // Shift the results and add our current result
      x[3] = x[2];
      x[2] = x[1];
      x[1] = x[0];
      x[0] = uint32_t(sum);       // Low 32 bits of sum
      x[4] = uint32_t(sum >> 32); // Remainder

      return x[0];
    }

    inline uint32_t cScopedPredictableRandom::GetRandomNumber0ToMaximum() const
    {
      return GetRandomNumber() % maximum;
    }

    inline float cScopedPredictableRandom::GetRandomNumber0To1() const
    {
      return float(GetRandomNumber()) * fOneOver65536By65536;
    }


    // This is for calling with cVec2, cVec3 or cVec4
    template <class T>
    inline float GetDistance(const T& from, const T& to)
    {
      return (from - to).GetLength();
    }


    // This is for calling with int, float or double
    template <class T>
    inline bool IsApproximatelyEqual(const T& lhs, const T& rhs)
    {
      const T temp(lhs - rhs);
      return (temp > -cEPSILON) && (temp < cEPSILON);
    }
  }
}

#endif // CMATH_H
