#pragma once

// Standard headers
#include <cmath>
#include <random>

namespace spitfire {

namespace math {

class SimplexNoise {
public:
  SimplexNoise();
  explicit SimplexNoise(std::mt19937& rng);

  float SignedFBM(float xPos, float yPos, size_t octaves, float lacunarity, float gain);
  float SignedRawNoise(float xPos, float yPos);
  float UnsignedFBM(float xPos, float yPos, size_t octaves, float lacunarity, float gain);
  float UnsignedRawNoise(float xPos, float yPos);

private:
  float CalculateCornerValue(float x, float y, int gradientIndex) const;

  constexpr uint16_t Hash(int i) const;

  std::uint8_t permutation[256];
};

inline float SimplexNoise::UnsignedRawNoise(float xPos, float yPos)
{
  return SignedRawNoise(xPos, yPos) / 2.0 + 0.5;
}

inline float SimplexNoise::UnsignedFBM(float xPos, float yPos, size_t octaves, float lacunarity, float gain)
{
  return SignedFBM(xPos, yPos, octaves, lacunarity, gain) / 2.0 + 0.5;
}

inline constexpr uint16_t SimplexNoise::Hash(int i) const
{
  return permutation[static_cast<unsigned int>(i) & 255];
}

}

}
