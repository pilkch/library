// Standard headers
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <cmath>

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <list>
#include <vector>

// FreeType headers
#include <freetype2/ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

// SDL headers
#include <SDL3_image/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

// libopenglmm headers
#include <libopenglmm/cContext.h>
#include <libopenglmm/cGeometry.h>
#include <libopenglmm/cFont.h>
#include <libopenglmm/opengl.h>

// https://kevinboone.me/fbtextdemo.html

namespace opengl
{
  size_t RoundDownToNearestEvenNumber(size_t value)
  {
    // Round down to the nearest even number
    value = (2 * (value / 2));
    ASSERT((value % 2) == 0);
    return value;
  }

  void DrawGlyphToBuffer(GLubyte* pBuffer, size_t nBufferWidth, size_t nBufferHeight, size_t columns, size_t rows, size_t glyphPixelHeightAndWidth, FT_Face face, char ch, float& outU, float& outV, float& outU_Width, float& outV_Height, float& outWidth, float& outHeight, float& outAdvanceX, float& outAdvanceY, float& outOffsetX, float& outOffsetY)
  {
    (void)nBufferHeight;

    outWidth = 0.0f;
    outHeight = 0.0f;

    // Get FreeType to render our character into a bitmap

    const FT_UInt characterIndex = FT_Get_Char_Index(face, ch);
    if (characterIndex == 0) {
      std::cout<<"DrawGlyphToBuffer FT_Get_Char_Index FAILED"<<std::endl;
      return;
    }

    // Load the Glyph for our character.
    if (FT_Load_Glyph(face, characterIndex, FT_LOAD_DEFAULT)) {
      std::cout<<"DrawGlyphToBuffer FT_Load_Glyph FAILED"<<std::endl;
      return;
    }

    // Move the face's glyph into a Glyph object.
    FT_Glyph glyph;
    if (FT_Get_Glyph(face->glyph, &glyph)) {
      std::cout<<"DrawGlyphToBuffer FT_Get_Glyph FAILED"<<std::endl;
      return;
    }

    const int bbox_ymax = face->bbox.yMax / 64;
    const int glyph_width = face->glyph->metrics.width / 64;
    const int glyph_height = face->glyph->metrics.height / 64;
    //const int advance = face->glyph->metrics.horiAdvance / 64;
    const int advance = face->glyph->metrics.horiAdvance / 64;
    const int x_off = (advance - glyph_width) / 2;
    const int y_off = bbox_ymax - face->glyph->metrics.horiBearingY / 64;

    // Convert the glyph to a bitmap.
    FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

    // This reference will make accessing the bitmap easier
    const FT_Bitmap& bitmap = bitmap_glyph->bitmap;

    const size_t width = glyphPixelHeightAndWidth;
    const size_t height = glyphPixelHeightAndWidth;

    // Copy the Freetype bitmap into pBuffer
    assert(ch >= 0);
    const size_t index = size_t(ch);

    const size_t nDestRowWidthBytes = nBufferWidth * 4;
    const size_t nDestOffset = (((index / columns) * nDestRowWidthBytes * height) + ((index % rows) * width * 4));

    for (size_t y = 0; y < height; y++) {
      for (size_t x = 0; x < width; x++) {
        if ((x < size_t(bitmap.width)) && (y < size_t(bitmap.rows))) {
          const uint8_t value = bitmap.buffer[(bitmap.width * y) + x];
          pBuffer[nDestOffset + ((y * nDestRowWidthBytes) + (x * 4))] = value;
          pBuffer[nDestOffset + ((y * nDestRowWidthBytes) + (x * 4)) + 1] = value;
          pBuffer[nDestOffset + ((y * nDestRowWidthBytes) + (x * 4)) + 2] = value;
          pBuffer[nDestOffset + ((y * nDestRowWidthBytes) + (x * 4)) + 3] = value;
        } else {
          // Outside the Freetype bitmap, just fill in zeroes
          pBuffer[nDestOffset + ((y * nDestRowWidthBytes) + (x * 4))] = 0;
          pBuffer[nDestOffset + ((y * nDestRowWidthBytes) + (x * 4)) + 1] = 0;
          pBuffer[nDestOffset + ((y * nDestRowWidthBytes) + (x * 4)) + 2] = 0;
          pBuffer[nDestOffset + ((y * nDestRowWidthBytes) + (x * 4)) + 3] = 0;
        }
      }
    }


    // NOTE: The UV coordinates are 0..1
    outU = (float(index % rows) * glyphPixelHeightAndWidth) / float(width) / float(columns);
    outV = (float(index / columns) * glyphPixelHeightAndWidth) / float(height) / float(rows);
    outU_Width = float(glyph_width) / float(width) / float(columns);
    outV_Height = float(glyph_height) / float(height) / float(rows);

    // The width, height, advance, and offset values are in pixels
    outWidth = float(face->glyph->metrics.width / 64);
    outHeight = float(face->glyph->metrics.height / 64);
    outAdvanceX = float(face->glyph->advance.x / 64);
    outAdvanceY = float(face->glyph->advance.y / 64);
    outOffsetX = float(x_off);
    outOffsetY = float(y_off);

    //std::cout<<"characterIndex "<<characterIndex<<", ch "<<ch<<", bitmap dimensions "<<bitmap.width<<"x"<<bitmap.rows<<", glyphPixelHeightAndWidth "<<glyphPixelHeightAndWidth<<"x"<<glyphPixelHeightAndWidth<<", advance: "<<advance<<", outAdvanceX "<<outAdvanceX<<std::endl;
  }


  // ** cFont

  cFont::cFont() :
    glyphPixelHeightAndWidth(0)
  {
  }

  cFont::~cFont()
  {
    assert(!texture.IsValid());
    assert(!shader.IsCompiledProgram());
  }

  bool cFont::Load(cContext& context, const opengl::string_t& sFilename, size_t _height, const opengl::string_t& sVertexShader, const opengl::string_t& sFragmentShader)
  {
    std::cout<<"cFont::Load \""<<opengl::string::ToUTF8(sFilename)<<"\""<<std::endl;

    // Create and initilize a freetype font library
    FT_Library library;
    if (FT_Init_FreeType(&library)) {
      std::cout<<"cFont::cFont FT_Init_FreeType FAILED, returning false"<<std::endl;
      return false;
    }

    // This is where we load in the font information from the file.
    // Of all the places where the code might die, this is the most likely,
    // as FT_New_Face will die if the font file does not exist or is somehow broken.
    FT_Face face = nullptr;
    if (FT_New_Face(library, opengl::string::ToUTF8(sFilename).c_str(), 0, &face )) {
      std::cout<<"cFont::cFont FT_New_Face FAILED to load font \""<<opengl::string::ToUTF8(sFilename)<<"\", returning false"<<std::endl;
      return false;
    }

    // For some twisted reason, Freetype measures font size
    // in terms of 1/64ths of pixels.  Thus, to make a font
    // h pixels high, we need to request a size of h*64.
    const size_t height = RoundDownToNearestEvenNumber(_height);
    //FT_Set_Pixel_Sizes(face, 0, int(height * 64));
    FT_Set_Pixel_Sizes(face, 0, height);


    const size_t rows = 16;
    const size_t columns = 16;
    const size_t n = rows * columns;

    fGlyphU.insert(fGlyphU.begin(), n, 0.0f);
    fGlyphV.insert(fGlyphV.begin(), n, 0.0f);
    fGlyphU_Width.insert(fGlyphU_Width.begin(), n, 0.0f);
    fGlyphV_Height.insert(fGlyphV_Height.begin(), n, 0.0f);
    fGlyphWidth.insert(fGlyphWidth.begin(), n, 0.0f);
    fGlyphHeight.insert(fGlyphHeight.begin(), n, 0.0f);
    fGlyphAdvanceX.insert(fGlyphAdvanceX.begin(), n, 0.0f);
    fGlyphAdvanceY.insert(fGlyphAdvanceY.begin(), n, 0.0f);
    fGlyphOffsetX.insert(fGlyphOffsetX.begin(), n, 0.0f);
    fGlyphOffsetY.insert(fGlyphOffsetY.begin(), n, 0.0f);

    glyphPixelHeightAndWidth = spitfire::math::NextPowerOfTwo(int(height * 2));

    const size_t nBufferWidth = columns * glyphPixelHeightAndWidth;
    const size_t nBufferHeight = rows * glyphPixelHeightAndWidth;
    const size_t nBufferSizeBytes = nBufferWidth * nBufferHeight * 4;

    std::vector<uint8_t> buffer;
    buffer.resize(nBufferSizeBytes);

    // Render first 128 characters to a bitmap
    for (size_t i = 0; i < n; i++) {
      DrawGlyphToBuffer(buffer.data(), nBufferWidth, nBufferHeight, columns, rows, glyphPixelHeightAndWidth, face, char(i), fGlyphU[i], fGlyphV[i], fGlyphU_Width[i], fGlyphV_Height[i], fGlyphWidth[i], fGlyphHeight[i], fGlyphAdvanceX[i], fGlyphAdvanceY[i], fGlyphOffsetX[i], fGlyphOffsetY[i]);
    }

    // We are done with font now
    FT_Done_Face(face);

    // And the library
    FT_Done_FreeType(library);

    // Create our texture from the buffer
    context.CreateTextureFromBufferNoMipMaps(texture, buffer.data(), nBufferWidth, nBufferHeight, PIXELFORMAT::R8G8B8A8);

    if (!texture.IsValid()) {
      std::cout<<"cFont::cFont CreateTextureFromBuffer FAILED, returning false"<<std::endl;
      return false;
    }

    context.CreateShader(shader, sVertexShader, sFragmentShader);
    if (!shader.IsCompiledProgram()) {
      std::cout<<"cFont::cFont CreateShader FAILED, returning false"<<std::endl;
      return false;
    }

    std::cout<<"cFont::Load returning true"<<std::endl;
    return true;
  }

  void cFont::Destroy(cContext& context)
  {
    if (shader.IsCompiledProgram()) context.DestroyShader(shader);

    if (texture.IsValid()) context.DestroyTexture(texture);
  }

  spitfire::math::cVec2 cFont::GetDimensions(const opengl::string_t& sText) const
  {
    spitfire::math::cVec2 dimensions;

    //float fCharacterWidth = 0.0f;
    float fCharacterHeight = 0.0f;

    const size_t n = sText.size();
    for (size_t i = 0; i < n; i++) {
      // Get the character that this is
      assert(sText[i] >= 0);
      size_t c = size_t(sText[i]);

      // Now lookup the character in the array of widths and heights
      //fCharacterWidth = fGlyphWidth[c];
      fCharacterHeight = fGlyphHeight[c];

      // Add the glyph advance value
      dimensions.x += fGlyphAdvanceX[i];

      // If this is the tallest character so far then set our current tallest character to us
      if (fCharacterHeight > dimensions.y) dimensions.y = fCharacterHeight;
    }

    return dimensions;
  }

  spitfire::math::cVec2 cFont::GetDimensionsLineWrap(const opengl::string_t& sText, float fMaxWidthOfLine) const
  {
    (void)fMaxWidthOfLine;

    spitfire::math::cVec2 dimensions;

    //float characterWidth = 0.0f;
    //float characterHeight = 0.0f;

    const size_t n = sText.size();
    for (size_t i = 0; i < n; i++) {
      /*_GetDimensions(sText[i], lineWidth, lineHeight);

      // Add the lineHeight and if this is the widest line so far then set our current widest line to us
      if (lineWidth > dimensions.x) dimensions.x = lineWidth;
      dimensions.y += lineHeight;*/
    }

    return dimensions;
  }

  void cFont::MeasureOrPushBack(opengl::cGeometryBuilder_v2_t2_c4* pBuilder, const opengl::string_t& sText, const spitfire::math::cColour& colour, const spitfire::math::cVec2& _position, float fRotationDegrees, const spitfire::math::cVec2& scale, spitfire::math::cVec2* pOutDimensions) const
  {
    (void)fRotationDegrees;

    spitfire::math::cVec2 position(_position);

     if (pOutDimensions != nullptr) {
       pOutDimensions->x = 0.0f;
       pOutDimensions->y = 0.0f;
     }

#if 1
    // For each character calculate the position in the world and the position in the texture and add a quad to the buffer
    const std::string sTextUTF8 = opengl::string::ToUTF8(sText);
    const size_t n = sTextUTF8.length();
    for (size_t i = 0; i < n; i++) {
      const char c = sTextUTF8[i];
      const size_t index = size_t(c);

      const float fCharacterWidth = fGlyphWidth[index];
      const float fCharacterHeight = fGlyphHeight[index];

      // Find out where in the texture this character is
      const float fTextureCharacterOffsetU = fGlyphU[index];
      const float fTextureCharacterOffsetV = fGlyphV[index];
 
      const float fTextureCharacterWidth = fGlyphU_Width[index];
      const float fTextureCharacterHeight = fGlyphV_Height[index];

      const spitfire::math::cVec2 offset(fGlyphOffsetX[index], fGlyphOffsetY[index]);

      if (pBuilder != nullptr) {
        pBuilder->PushBack(position + scale * (offset + spitfire::math::cVec2(0.0f, fCharacterHeight)), spitfire::math::cVec2(fTextureCharacterOffsetU, fTextureCharacterOffsetV + fTextureCharacterHeight), colour);
        pBuilder->PushBack(position + scale * (offset + spitfire::math::cVec2(fCharacterWidth, fCharacterHeight)), spitfire::math::cVec2(fTextureCharacterOffsetU + fTextureCharacterWidth, fTextureCharacterOffsetV + fTextureCharacterHeight), colour);
        pBuilder->PushBack(position + scale * (offset + spitfire::math::cVec2(fCharacterWidth, 0.0f)), spitfire::math::cVec2(fTextureCharacterOffsetU + fTextureCharacterWidth, fTextureCharacterOffsetV), colour);
        pBuilder->PushBack(position + scale * (offset + spitfire::math::cVec2(fCharacterWidth, 0.0f)), spitfire::math::cVec2(fTextureCharacterOffsetU + fTextureCharacterWidth, fTextureCharacterOffsetV), colour);
        pBuilder->PushBack(position + scale * (offset + spitfire::math::cVec2(0.0f, 0.0f)), spitfire::math::cVec2(fTextureCharacterOffsetU, fTextureCharacterOffsetV), colour);
        pBuilder->PushBack(position + scale * (offset + spitfire::math::cVec2(0.0f, fCharacterHeight)), spitfire::math::cVec2(fTextureCharacterOffsetU, fTextureCharacterOffsetV + fTextureCharacterHeight), colour);
      } else {
        pOutDimensions->x += scale.x * fGlyphAdvanceX[index];
        pOutDimensions->y = max(pOutDimensions->y, scale.y * fGlyphAdvanceY[index]); // Take the great of our current value and the advance y for this glyph
      }

      //std::cout<<"Before position.x "<<position.x<<", i "<<i<<", c "<<c<<", index "<<index<<", scale x "<<scale.x<<
      //  ", fGlyphAdvanceX "<<fGlyphAdvanceX[index]<<", fGlyphOffsetX "<<fGlyphOffsetX[index]<<", fGlyphU "<<fGlyphU[index]<<", fGlyphV "<<fGlyphV[index]<<", fGlyphU_Width "<<fGlyphU_Width[index]<<", fGlyphV_Height "<<fGlyphV_Height[index]<<", fGlyphWidth "<<fGlyphWidth[index]<<", fGlyphHeight "<<fGlyphHeight[index]<<std::endl;

      // Move the cursor for the next character
      // TODO: Work out how to incorporate this with the scale and rotation
      position.x += scale.x * fGlyphAdvanceX[index];// * cosf(fRotationDegrees);

      //std::cout<<"After position.x "<<position.x<<std::endl;
    }
#else
    if (pBuilder != nullptr) {
      // TODO: This doesn't allow any measuring
      // For viewing the whole font
      const spitfire::math::cVec2 position(_position);
      const size_t rows = 16;
      const size_t columns = 16;
      const float fWidth = float(columns) * glyphPixelHeightAndWidth;
      const float fHeight = float(rows) * glyphPixelHeightAndWidth;
      pBuilder->PushBack(position + scale * spitfire::math::cVec2(0.0f, fHeight), spitfire::math::cVec2(0.0f, 1.0f), colour);
      pBuilder->PushBack(position + scale * spitfire::math::cVec2(fWidth, fHeight), spitfire::math::cVec2(1.0f, 1.0f), colour);
      pBuilder->PushBack(position + scale * spitfire::math::cVec2(fWidth, 0.0f), spitfire::math::cVec2(1.0f, 0.0f), colour);
      pBuilder->PushBack(position + scale * spitfire::math::cVec2(fWidth, 0.0f), spitfire::math::cVec2(1.0f, 0.0f), colour);
      pBuilder->PushBack(position + scale * spitfire::math::cVec2(0.0f, 0.0f), spitfire::math::cVec2(0.0f, 0.0f), colour);
      pBuilder->PushBack(position + scale * spitfire::math::cVec2(0.0f, fHeight), spitfire::math::cVec2(0.0f, 1.0f), colour);
    }
#endif
  }

  void cFont::Measure(const opengl::string_t& sText, const spitfire::math::cColour& colour, const spitfire::math::cVec2& position, float fRotationDegrees, const spitfire::math::cVec2& scale, spitfire::math::cVec2& outDimensions) const
  {
    MeasureOrPushBack(nullptr, sText, colour, position, fRotationDegrees, scale, &outDimensions);
  }

  void cFont::PushBack(opengl::cGeometryBuilder_v2_t2_c4& builder, const opengl::string_t& sText, const spitfire::math::cColour& colour, FLAGS flags, const spitfire::math::cVec2& _position, float fRotationDegrees, const spitfire::math::cVec2& scale) const
  {
    spitfire::math::cVec2 position(_position);

    if ((flags & HORIZONTAL_ALIGNMENT_CENTERED) == HORIZONTAL_ALIGNMENT_CENTERED) {
      spitfire::math::cVec2 dimensions;
      MeasureOrPushBack(nullptr, sText, colour, position, fRotationDegrees, scale, &dimensions);
      position.x -= 0.5f * dimensions.x;
    }

    MeasureOrPushBack(&builder, sText, colour, position, fRotationDegrees, scale, nullptr);
  }
}
