#ifndef CMATH_H
#define CMATH_H

// Standard headers
#include <cmath>
#include <cstdlib>
#include <algorithm>

// Spitfire headers
#include <spitfire/spitfire.h>

// e^(i*pi) + 1 = 0
// e^(i*z) = cos(z) + i*sin(z)
// cos(z) = ( e^(i*z) + e^(-i*z) ) / 2
// sin(z) = ( e^(i*z) - e^(-i*z) ) / (2*i)
// sum(n=0 to k)(z^n) = ( 1-z^(k+1) ) / (1-z)
// sine rule: a/sin(A) = b/sin(B) = c/sin(C)
// cos(a)*cos(a) + sin(a)*sin(a) = 1
// sin(-t) = -sin(t)
// cos(-t) = cos(t)
// tan(-t) = -tan(t)
// sin(pi-t) = sin(t)
// cos(pi-t) = -cos(t)
// tan(pi-t) = -tan(t)
// sin(s+t) = sin(s)*cos(t) + cos(s)*sin(t)
// cos(s+t) = cos(s)*cos(t) - sin(s)*sin(t)
// sin(s-t) = sin(s)*cos(t) - cos(s)*sin(t)
// cos(s-t) = cos(s)*cos(t) + sin(s)*sin(t)
// sin(2*t) = 2*sin(t)*cos(t)
// cos(2*t) = cos(2*t) - sin(2*t) = 2*cos(2*t) - 1 = 1 - 2*sin(2*t)
// sin(t/2) = ±sqrt((1 - cos(t)) / 2)
// cos(t/2) = ±sqrt((1 + cos(t)) / 2)
// sin(s) + sin(t) = 2 * sin((s+t)/2) * cos((s-t)/2)
// sin(s) - sin(t) = 2 * sin((s-t)/2) * cos((s+t)/2)
// cos(s) + cos(t) = 2 * cos((s+t)/2) * cos((s-t)/2)
// cos(s) - cos(t) = -2 * sin((s+t)/2) * sin((s-t)/2)
// sin(s)*cos(t) = ( sin(s+t) + sin(s-t) ) / 2
// cos(s)*cos(t) = ( cos(s+t) + cos(s-t) ) / 2
// sin(s)*sin(t) = ( cos(s-t) - cos(s+t) ) / 2


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


namespace spitfire
{
  namespace math
  {
    // Constants
    constexpr float_t cPI               = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679f;
    constexpr float_t c2_PI             = 6.28318530717958647692f;
    constexpr float_t cPI_SQUARED       = cPI * cPI;
    constexpr float_t cPI_DIV_2         = 1.57079632679489661923f;
    constexpr float_t cPI_DIV_180       = cPI / 180.0f;
    constexpr float_t c180_DIV_PI       = 180.0f / cPI;

    constexpr float_t c4_DIV_3          = 4.0f / 3.0f;
    constexpr float_t cSQUARE_ROOT_OF_2 = 1.4142135623730950488016887242097f;
    constexpr float_t cSQUARE_ROOT_OF_3 = 1.7320508075688772935274463415059f;

    constexpr float_t cEPSILON          = 0.0001f; //10e-6f; //std::numeric_limits<float>::epsilon();
    extern const float_t cINFINITY; // 3.3e+38f;

    constexpr float_t cE               = 2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274f;
    // euler constant
    constexpr float_t cGAMMA           = 0.57721566490153286060651209008240243104215933593992359880576723488486772677766467093694706329174674f;
    // golden ratio = (1+sqrt(5))/2
    constexpr float_t cPHI             = 1.6180339887498948482045868343656381177203091798057628621354486227052604628189024497072072041893911374f;
    // 1/log(2)
    constexpr float_t cINV_LOG2        = 1.44269504088896340735992468100189213742664595415298593413544940693110921918118507988552662289350f;
    // ln(10)
    constexpr float_t cLN10            = 2.3025850929940456840179914546843642076011014886287729760333279009675726096773524802359972050895982983f;
    // ln(pi)
    constexpr float_t cLNPI            = 1.1447298858494001741434273513530587116472948129153115715136230714721377698848260797836232702754897077f;
    // lg(e)
    constexpr float_t cLOG2E           = 1.44269504088896340736f;
    // log(e)
    constexpr float_t cLOG10E          = 0.434294481903251827651f;
    // ln(2)
    constexpr float_t cLN2             = 0.693147180559945309417f;

    inline float RadiansToDegrees(float x) { return x * c180_DIV_PI; }
    inline float DegreesToRadians(float x) { return x * cPI_DIV_180; }


    template<class T>
    T min3(T a, T b, T c)
    {
      return min(min(a, b), c);
    }

    template<class T>
    T min4(T a, T b, T c, T d)
    {
      return min(min(min(a, b), c), d);
    }

    template<class T>
    T max3(T a, T b, T c)
    {
      return max(max(a, b), c);
    }

    template<class T>
    T max4(T a, T b, T c, T d)
    {
      return max(max(max(a, b), c), d);
    }


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

    // Round a to nearest int
    template <typename T> T round(T value) { return ((value) > 0 ? int(value + 0.5) : -int(0.5 - value)); }

    // Performs a fast round on a float
    inline int round(const float_t f)
    {
      // Add a magical cookie to the float to transform its bits into its rounded integer representation
      // http://www.d6.com/users/checker/pdfs/gdmfp.pdf
      return int(double(f + 6755399441055744.L));
    }

    inline float_t RoundDown(float_t fValue)
    {
      //return int(fValue);

      return floorf(fValue);
    }

    inline float_t RoundUp(float_t fValue)
    {
      //return int(fValue) + 1;

      return ceilf(fValue);
    }

    inline int RoundUpToNearestInt(float_t fValue)
    {
      return static_cast<int>(std::ceil(fValue));
    }


    inline int GetIntegerPart(float_t fValue)
    {
      return int(floor(fValue));
    }

    inline void GetIntegerAndFractionParts(float_t fValue, int& integer, float_t& fraction)
    {
      double dInteger = 0;
      double dFraction = modf(fValue , &dInteger);
      integer = int(dInteger);
      fraction = float_t(dFraction);
    }


    // Clip VALUE to the range LOW--HIGH.
    template <typename T> T clip(T value, T low, T high) { return max(min(value, high), low); }

    // True if VALUE is in the range LOW--HIGH.
    template <typename T> bool IsInRange(T value, T low, T high) { return (value >= low) && (value <= high); }

    // Return LOW or HIGH, whichever is closer to VALUE.
    template <typename T> T closer(T value, T low, T  high) { return (std::abs(value - low) < std::abs(value - high)) ? low : high; }


    // Square a number
    template <typename T> T square(T value) { return value * value; }

    inline float squared(float x)
    {
      return (x * x);
    }

    inline float sqrtf(float x)
    {
      return ::sqrtf(x);
    }

    template <class T> inline bool IsDivisibleByTwo(T value) { return ((value % 2) == 0); }

    // This function gets the first power of 2 >= the int that we pass it.
    inline int NextPowerOfTwo(int n)
    {
      // TODO: Surely 2 is the first power of two after 0?
      int value = 1;
      // Keep looping through the powers of two until we find one that is >= n
      while (value < n) value <<= 1;
      return value;
    }

    template<class T> inline bool IsPowerOfTwo(T value) { return (value != 0) && ((value & (value - 1)) == 0); }

    inline bool IsApproximatelyEqual(float_t a, float_t b)
    {
      return (((a + cEPSILON) > b) && ((a - cEPSILON) < b));
    }

    inline bool IsApproximatelyZero(float_t value)
    {
      return ((value < cEPSILON) && (value > -cEPSILON));
    }


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


    // Mod a value by 1.0f and restrict it to 0..1
    inline float Mod0To1(float fValue)
    {
      return fValue - (1.0f * floor(fValue / 1.0f));
    }

    // Mod a value by fMax and restrict it to 0..fMax
    inline float Mod0ToMax(float fValue, float fMax)
    {
      return fValue - (fMax * floor(fValue / fMax));
    }


    // Clamp an angle to -180..180
    template <typename T>
    T ClampAngle(T angle)
    {
      if (angle < 0.0f) angle = 180.0f - (T)fmod(fabs(angle), 360.0f);
      else angle = -180.0f + (T)fmod(angle, 360.0f);

      return angle;
    }

    // Return angle in the interval [minimum, minimum + 2pi]
    template <typename T>
    T branch(T angle, T minimum)
    {
      while (angle > minimum + c2_PI) angle -= c2_PI;
      while (angle <= minimum) angle += c2_PI;

      return angle;
    }

    template <class T>
    T clamp(const T& i, const T& lower, const T& upper)
    {
      return (i < lower) ? lower : (i > upper) ? upper : i;
    }

    // Smooth step from a to b using mu = { 0..1 }
    template <class T>
    T smooth_step(T a, T b, float mu)
    {
      const T num = clamp(mu, 0.0f, 1.0f);
      return lerp(a, b, (num * num) * (3.0f - (2.0f * num)));
    }

    // Linearly interpolate from a to b using mu = { 0..1 }
    inline float mix(float a, float b, float mu)
    {
      if (mu > 1.0f) return b;
      if (mu <= 0.0f) return a;

      return ((1.0f - mu) * a)  + (mu * b);
    }

    // Same as above but smoother, m approaches 1.0f at x = 0.0f and x = 1.0f
    // http://www.walterzorn.com/grapher/grapher_e.htm
    inline float mix_smooth(float a, float b, float mu)
    {
      if (mu > 1.0f) return b;
      if (mu <= 0.0f) return a;

      const float fAmountOfA = 0.5f + (0.5f * sinf(cPI * (mu + 1.5f)));

      return (fAmountOfA * a) + ((1.0f - fAmountOfA) * b);
    }

    inline float slow_in_out(float value)
    {
      if (value < 1.0f) value = 1.0f / (1.0f + exp((-value * 12.0f) + 6.0f));

      return std::min(value, 1.0f);
    }


    inline bool PointIsWithinBounds(float x, float y, float bounds_x, float bounds_y, float bounds_width, float bounds_height)
    {
      return
        ((x > bounds_x) && (x < (bounds_x + bounds_width))) &&
        ((y > bounds_y) && (y < (bounds_y + bounds_height)));
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

    inline float_t Atan2Degrees(float_t x, float_t y)
    {
      if ((fabs(x) < cEPSILON) && (fabs(y) < cEPSILON)) return 0.0f;

      return RadiansToDegrees(atan2(x, y));
    }


    // *** Random Number Generation

    inline void SetRandomSeed(uint32_t seed)
    {
      return srand(seed);
    }

    inline uint32_t random(uint32_t maximum)
    {
      return rand() % maximum;
    }

    inline uint32_t random(uint32_t minimum, uint32_t maximum)
    {
      return minimum + rand() % maximum;
    }

    inline float randomZeroToOnef()
    {
      return float(rand() % 10000) * 0.0001f;
    }

    inline float randomMinusOneToPlusOnef()
    {
      return -1.0f + float(rand() % 20000) * 0.0001f;
    }

    inline float randomf(float fMax)
    {
      return fMax * randomZeroToOnef();
    }

    inline float randomf(float fMin, float fMax)
    {
      return fMin + randomf(fMax - fMin);
    }


    // For random number generation
    const float fOneOver65536By65536 = 1.0f / (65536.0f * 65536.0f);

    class cScopedPredictableRandom
    {
    public:
      explicit cScopedPredictableRandom(uint32_t seed);

      uint32_t GetRandomNumber0ToN(uint32_t maximum);
      float GetRandomNumber0To1();

    private:
      uint32_t GetRandomNumberAndIncrement();

      // For random number generation
      uint32_t x[5]; // History buffer
    };

    inline uint32_t cScopedPredictableRandom::GetRandomNumberAndIncrement()
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

    inline uint32_t cScopedPredictableRandom::GetRandomNumber0ToN(uint32_t maximum)
    {
      return (GetRandomNumberAndIncrement() % maximum);
    }

    inline float cScopedPredictableRandom::GetRandomNumber0To1()
    {
      return (float(GetRandomNumberAndIncrement()) * fOneOver65536By65536);
    }



    // ** cTimeStep

    class cTimeStep
    {
    public:
      cTimeStep(durationms_t currentTimeMS, float fStepMS);

      spitfire::durationms_t GetCurrentTimeMS() const { return currentTimeMS; }
      float GetStepMS() const { return fStepMS; }

    private:
      spitfire::durationms_t currentTimeMS;
      float fStepMS;
    };

    inline cTimeStep::cTimeStep(spitfire::durationms_t _currentTimeMS, float _fStepMS) :
      currentTimeMS(_currentTimeMS),
      fStepMS(_fStepMS)
    {
    }


    // ** cSpring
    // A generic spring class that can use 1, 2 or 3 dimensional vector
    // NOTE: This shouldn't really be here but there isn't any where else to put it

    template <class T>
    class cSpring
    {
    public:
      cSpring();

      void SetMass(float fMass);
      void SetK(float fK);
      void SetDampening(float fDampening);

      T GetPosition() const { return position; }
      void SetPosition(const T& position);

      void SetVelocity(const T& velocity);

      void Update(const cTimeStep& timeStep);

    private:
      T dv(const T& x, const T& v) const;

      float fMass;
      float fK;
      float fDampening;
      T position;
      T velocity;
    };

    template <class T>
    inline cSpring<T>::cSpring() :
      fMass(0.5f),
      fK(0.3f),
      fDampening(0.8f),
      position(0.0f, 0.0f)
    {
    }

    template <class T>
    inline void cSpring<T>::SetMass(float _fMass)
    {
      fMass = _fMass;
    }

    template <class T>
    inline void cSpring<T>::SetK(float _fK)
    {
      fK = _fK;
    }

    template <class T>
    inline void cSpring<T>::SetDampening(float _fDampening)
    {
      fDampening = _fDampening;
    }

    template <class T>
    inline void cSpring<T>::SetPosition(const T& _position)
    {
      position = _position;
    }

    template <class T>
    inline void cSpring<T>::SetVelocity(const T& _velocity)
    {
      velocity = _velocity;
    }

    template <class T>
    inline T cSpring<T>::dv(const T& x, const T& v) const
    {
      return (-fK / fMass) * x - (fDampening / fMass) * v;
    }

    template <class T>
    inline void cSpring<T>::Update(const cTimeStep& timeStep)
    {
      velocity += dv(position, velocity);
      position += velocity;
    }
  }
}

#endif // CMATH_H
