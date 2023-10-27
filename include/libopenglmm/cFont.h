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
#include <libopenglmm/cShader.h>
#include <libopenglmm/cTexture.h>

struct SDL_Surface;

namespace opengl
{
  class cContext;
  class cShader;
  class cGeometryBuilder_v2_t2_c4;

  class cFont
  {
  public:
    friend class cContext;

    cFont();
    ~cFont();

    bool IsValid() const { return (texture.IsValid() && shader.IsCompiledProgram()); }

    enum FLAGS {
      HORIZONTAL_ALIGNMENT_LEFT_ALIGNED = 0,
      HORIZONTAL_ALIGNMENT_CENTERED = 1
    };

    spitfire::math::cVec2 GetDimensions(const string_t& sText) const;
    spitfire::math::cVec2 GetDimensionsLineWrap(const string_t& sText, float fMaxWidthOfLine) const;
    void Measure(const opengl::string_t& sText, const spitfire::math::cColour& colour, const spitfire::math::cVec2& position, float fRotationDegrees, const spitfire::math::cVec2& scale, spitfire::math::cVec2& outDimensions) const;

    void PushBack(cGeometryBuilder_v2_t2_c4& builder, const string_t& sText, const spitfire::math::cColour& colour, const spitfire::math::cVec2& position) const { PushBack(builder, sText, colour, HORIZONTAL_ALIGNMENT_LEFT_ALIGNED, position, 0.0f, spitfire::math::cVec2(1.0f, 1.0f)); }
    void PushBack(cGeometryBuilder_v2_t2_c4& builder, const string_t& sText, const spitfire::math::cColour& colour, FLAGS flags, const spitfire::math::cVec2& position) const { PushBack(builder, sText, colour, flags, position, 0.0f, spitfire::math::cVec2(1.0f, 1.0f)); }
    void PushBack(cGeometryBuilder_v2_t2_c4& builder, const string_t& sText, const spitfire::math::cColour& colour, FLAGS flags,const spitfire::math::cVec2& position, float fRotationDegrees) const { PushBack(builder, sText, colour, flags, position, fRotationDegrees, spitfire::math::cVec2(1.0f, 1.0f)); }
    void PushBack(cGeometryBuilder_v2_t2_c4& builder, const string_t& sText, const spitfire::math::cColour& colour, FLAGS flags,const spitfire::math::cVec2& position, float fRotationDegrees, const spitfire::math::cVec2& scale) const;

  protected:
    bool Load(cContext& context, const string_t& sFilename, size_t height, const opengl::string_t& sVertexShader, const opengl::string_t& sFragmentShader);
    void Destroy(cContext& context);

    cTexture texture;
    cShader shader;

  private:
    void MeasureOrPushBack(opengl::cGeometryBuilder_v2_t2_c4* pBuilder, const opengl::string_t& sText, const spitfire::math::cColour& colour, const spitfire::math::cVec2& _position, float fRotationDegrees, const spitfire::math::cVec2& scale, spitfire::math::cVec2* pOutDimensions) const;

    // TODO: Use cTextureAtlas? Or don't use textures at all and create a VBO from the text outline instead?

    size_t glyphPixelHeightAndWidth; // The width and height of the area set aside for each glyph in the bitmap image

    std::vector<float> fGlyphU;
    std::vector<float> fGlyphV;
    std::vector<float> fGlyphU_Width;
    std::vector<float> fGlyphV_Height;
    std::vector<float> fGlyphWidth;
    std::vector<float> fGlyphHeight;
    std::vector<float> fGlyphAdvanceX;
    std::vector<float> fGlyphAdvanceY;
    std::vector<float> fGlyphOffsetX;
    std::vector<float> fGlyphOffsetY;
  };
}

#endif // LIBOPENGLMM_CFONT_H
