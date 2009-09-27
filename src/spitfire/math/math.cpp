#include <cstdlib>
#include <cmath>

#include <vector>
#include <limits>

// Spitfire Includes
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>

#ifdef max
#undef max
#endif

namespace spitfire
{
  namespace math
  {
    const float_t cINFINITY = std::numeric_limits<float>::max(); //1e30f //std::numeric_limits<float>::infinity();

    cScopedPredictableRandom::cScopedPredictableRandom(uint32_t seed, uint32_t _maximum) :
      maximum(_maximum),
      fOneOver65536By65536(1.0f / (65536.0f * 65536.0f))
    {
      // Make random numbers and put them into the buffer
      uint32_t s = seed;
      for (size_t i = 0; i < 5; i++) {
        s = (s * 29943829) - 1;
        x[i] = s;
      }

      // Randomise some more
      //for (size_t i = 0; i < 19; i++) GetRandomNumber();
    }
  }
}
