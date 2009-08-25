#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include <vector>
#include <map>
#include <list>
#include <set>

#include <string>
#include <sstream>

#include <iostream>
#include <fstream>

// Boost headers
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

// OpenGL headers
#include <GL/GLee.h>

// SDL headers
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>
#include <spitfire/util/cTimer.h>

#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/xml.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/geometry.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/game/brightness.h>

namespace breathe
{
  namespace brightness
  {
    cHumanEyeExposureControl::cHumanEyeExposureControl() :
      fEyeApeture0To1(1.0f), // Quite high to give a nice fade in effect at the start of the level?
      fSceneBrightness0To1(0.0f)
    {
    }

    float_t cHumanEyeExposureControl::GetPerceivedBrightness0To1() const
    {
      const float_t fDifferenceMinus1ToPlus1 = (1.0f - fEyeApeture0To1) - fSceneBrightness0To1;

      const float_t fBrightness0To1 = clamp(fSceneBrightness0To1 + fDifferenceMinus1ToPlus1, 0.0f, 1.0f);

      return fBrightness0To1;
    }

    cHumanEyeExposureControl::Update(sampletime_t currentTime, float_t _fSceneBrightness0To1)
    {
      // Update scene brightness
      fSceneBrightness0To1 = clamp(_fSceneBrightness0To1, 0.0f, 1.0f);

      // Calculate our target apeture amount
      const float_t fTargetApeture0To1 = (1.0f - fSceneBrightness0To1);
      const float_t fDifferenceMinus1ToPlus1 = fEyeApeture0To1 - fTargetApeture0To1;

      // Adjust actual eye apeture based on our target apeture
      fEyeApeture0To1 += 0.001f * fDifferenceMinus1ToPlus1;

      // Make sure that we are still within the range of 0.0f to 1.0f
      fEyeApeture0To1 = clamp(fEyeApeture0To1, 0.0f, 1.0f);
    }
  }
}
