#pragma once

#include <spitfire/math/math.h>

namespace spitfire
{
  namespace audio
  {
    constexpr float GetVolumeFromDB(float fDB)
    {
      return powf(10.0f, 0.05f * fDB);
    }

    constexpr float GetDBFromVolume(float fVolume)
    {
      return 20.0f * log10f(fVolume);
    }
  }
}
