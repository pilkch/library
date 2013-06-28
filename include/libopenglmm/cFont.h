/*************************************************************************
 *                                                                       *
 * libopenglmm Library, Copyright (C) 2009 Onwards Chris Pilkington         *
 * All rights reserved.  Web: http://chris.iluo.net                      *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2.1 of the  *
 * License, or (at your option) any later version. The text of the GNU   *
 * General Public License is included with this library in the           *
 * file license.txt.                                                     *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 * See the file GPL.txt for more details.                                *
 *                                                                       *
 *************************************************************************/

// This is a simple wrapper around OpenGL to make it a little bit more modern and easier to work with

#ifndef LIBOPENGLMM_CFONT_H
#define LIBOPENGLMM_CFONT_H

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cColour.h>

// libopenglmm headers
#include <libopenglmm/libopenglmm.h>

struct SDL_Surface;

namespace opengl
{
  class cContext;
  class cTexture;
  class cShader;
  class cGeometryBuilder_v2_c4_t2;

  class cFont
  {
  public:
    friend class cContext;

    cFont();
    ~cFont();

    bool IsValid() const { return (pTexture != nullptr) && (pShader != nullptr); }

    void PushBack(cGeometryBuilder_v2_c4_t2& builder, const string_t& sText, const spitfire::math::cColour& colour, const spitfire::math::cVec2& position) const { PushBack(builder, sText, colour, position, 0.0f, spitfire::math::cVec2(1.0f, 1.0f)); }
    void PushBack(cGeometryBuilder_v2_c4_t2& builder, const string_t& sText, const spitfire::math::cColour& colour, const spitfire::math::cVec2& position, float fRotationDegrees) const { PushBack(builder, sText, colour, position, fRotationDegrees, spitfire::math::cVec2(1.0f, 1.0f)); }
    void PushBack(cGeometryBuilder_v2_c4_t2& builder, const string_t& sText, const spitfire::math::cColour& colour, const spitfire::math::cVec2& position, float fRotationDegrees, const spitfire::math::cVec2& scale) const;

    spitfire::math::cVec2 GetDimensions(const string_t& sText) const;
    spitfire::math::cVec2 GetDimensionsLineWrap(const string_t& sText, float fMaxWidthOfLine) const;

  protected:
    bool Load(cContext& context, const string_t& sFilename, unsigned int height, const opengl::string_t& sVertexShader, const opengl::string_t& sFragmentShader);
    void Destroy(cContext& context);

    cTexture* pTexture;
    cShader* pShader;

  private:
    std::vector<float> fGlyphU;
    std::vector<float> fGlyphV;
    std::vector<float> fGlyphWidth;
    std::vector<float> fGlyphHeight;
    std::vector<float> fGlyphAdvanceX;
    std::vector<float> fGlyphAdvanceY;
  };
}

#endif // LIBOPENGLMM_CFONT_H
