#ifndef FILTERS_H
#define FILTERS_H

#include <spitfire/math/math.h>

namespace spitfire
{
  class cFilter
  {
  public:
    float_t GetValueMinus1ToPlus1(float_t fPointXMinus1ToPlus1, float_t fPointYMinus1ToPlus1) const;

  private:
    virtual float_t _GetValueMinus1ToPlus1(float_t fPointXMinus1ToPlus1, float_t fPointYMinus1ToPlus1) const = 0;
  };

  inline float_t cFilter::GetValueMinus1ToPlus1(float_t fPointXMinus1ToPlus1, float_t fPointYMinus1ToPlus1) const
  {
    ASSERT((fPointXMinus1ToPlus1 >= -1.0f) && (fPointXMinus1ToPlus1 <= 1.0f));
    ASSERT((fPointYMinus1ToPlus1 >= -1.0f) && (fPointYMinus1ToPlus1 <= 1.0f));

    return _GetValueMinus1ToPlus1(fPointXMinus1ToPlus1, fPointYMinus1ToPlus1);
  }


  // A sine wave generator, goes right to the edges of the square (-1, -1) to (+1, +1)
  class cFilterSineWave : public cFilter
  {
  public:
    cFilterSineWave();

    void SetFrequency(float_t _fFrequency) { fFrequency = _fFrequency; }

  private:
    float_t _GetValueMinus1ToPlus1(float_t fPointXMinus1ToPlus1, float_t fPointYMinus1ToPlus1) const;
  };

  inline cFilterSineWave::cFilterSineWave() :
    fFrequency(1.0f)
  {
  }

  inline float_t cFilterSineWave::_GetValueMinus1ToPlus1(float_t fPointXMinus1ToPlus1, float_t fPointYMinus1ToPlus1) const
  {
    return sinf(fFrequency * math::cVec2(fPointXMInus1ToPlus1, fPointYMinus1ToPlus1).GetLength());
  }


  // A solid circle with radius of 1, returns 1.0f for inside the circle, 0.0f for outside
  class cFilterBooleanCircle : public cFilter
  {
  public:

  private:
    float_t _GetValueMinus1ToPlus1(float_t fPointXMinus1ToPlus1, float_t fPointYMinus1ToPlus1) const;
  };

  inline float_t cFilterBooleanCircle::_GetValueMinus1ToPlus1(float_t fPointXMinus1ToPlus1, float_t fPointYMinus1ToPlus1) const
  {
    return (math::cVec2(fPointXMInus1ToPlus1, fPointYMinus1ToPlus1).GetLength() <= 1.0f) ? 1.0f : 0.0f;
  }


  // A gradient generator, goes right to the edges of the square (-1, -1) to (+1, +1) (Although, outside the centre circle of radius 1.0f, this will always return 0.0f)
  class cFilterGradient : public cFilter
  {
  public:

  private:
    float_t _GetValueMinus1ToPlus1(float_t fPointXMinus1ToPlus1, float_t fPointYMinus1ToPlus1) const;
  };

  inline float_t cFilterGradient::_GetValueMinus1ToPlus1(float_t fPointXMinus1ToPlus1, float_t fPointYMinus1ToPlus1) const
  {
    float_t fLength = clamp(math::cVec2(fPointXMInus1ToPlus1, fPointYMinus1ToPlus1).GetLength(), 1.0f);
    return (1.0f - fLength);
  }


#if 0
  // TODO: Actually create this filter
  // A perlin noise generator, goes right to the edges of the square (-1, -1) to (+1, +1)
  class cFilterPerlinNoise : public cFilter
  {
  public:

  private:
    float_t _GetValueMinus1ToPlus1(float_t fPointXMinus1ToPlus1, float_t fPointYMinus1ToPlus1) const;
  };

  inline float_t cFilterPerlinNoise::_GetValueMinus1ToPlus1(float_t fPointXMinus1ToPlus1, float_t fPointYMinus1ToPlus1) const
  {
    return ...;
  }
#endif
}

#endif // FILTERS_H
