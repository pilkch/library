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

// OpenGL headers
#include <GL/GLee.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>
#include <spitfire/util/cTimer.h>
#include <spitfire/util/unittest.h>

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

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

#include <breathe/render/cContext.h>
#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/model/cMd3.h>

namespace breathe
{
  namespace character
  {
    // ** cAnimationPart

    cAnimationPart::cAnimationPart() :
      animations(1),
      fBlendWeight0(1.0f),
      fBlendWeight1(0.0f),
      fBlendWeight2(0.0f)
    {
    }

    void cAnimationPart::SetAnimation(const string_t& _sAnimation0)
    {
      animations = 1;
      sAnimation0 = _sAnimation0;

      fBlendWeight0 = 1.0f;
      fBlendWeight1 = 0.0f;
      fBlendWeight2 = 0.0f;
    }

    // This is where we want 2 animations at the same time and we specify 1 blending between them
    void cAnimationPart::SetAnimation(const string_t& _sAnimation0, const string_t& _sAnimation1)
    {
      animations = 2;
      sAnimation0 = _sAnimation0;
      sAnimation1 = _sAnimation1;

      fBlendWeight0 = 1.0f;
      fBlendWeight1 = 0.0f;
      fBlendWeight2 = 0.0f;
    }

    // This is where we want 3 animations at the same time and we specify 3 blendings between them
    void cAnimationPart::SetAnimation(const string_t& _sAnimation0, const string_t& _sAnimation1, const string_t& _sAnimation2)
    {
      animations = 3;
      sAnimation0 = _sAnimation0;
      sAnimation1 = _sAnimation1;
      sAnimation2 = _sAnimation2;

      fBlendWeight0 = 1.0f;
      fBlendWeight1 = 0.0f;
      fBlendWeight2 = 0.0f;
    }

    void cAnimationPart::SetAnimationBlendWeight0To1(float_t fBlendWeightFirst0To1)
    {
      fBlendWeight0 = fBlendWeightFirst0To1;
      fBlendWeight1 = (1.0f - fBlendWeight0);
    }

    void cAnimationPart::SetAnimationBlendWeight0To1(float_t fBlendWeightFirst0To1, float_t fBlendWeightSecond0To1)
    {
      fBlendWeight0 = fBlendWeightFirst0To1;
      fBlendWeight1 = fBlendWeightSecond0To1;
      fBlendWeight2 = (1.0f - (fBlendWeight0 + fBlendWeight1));
    }

    // ** cAnimation

    bool cAnimation::LoadFromFile(const string_t& sFullFilename)
    {
      //return (model.Load(breathe::string::ToUTF8(sFullFilename).c_str()) == 0);
      return false;
    }
  }
}
