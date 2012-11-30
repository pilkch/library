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
#include <freetype/ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

// SDL headers
#include <SDL/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

// libopenglmm headers
#include <libopenglmm/cContext.h>
#include <libopenglmm/cGeometry.h>
#include <libopenglmm/cFont.h>
#include <libopenglmm/opengl.h>

namespace opengl
{
  // Create a display list coresponding to the give character.
  void DrawGlyphToBuffer(GLubyte* pBuffer, size_t nBufferWidth, size_t nBufferHeight, size_t columns, size_t rows, size_t glyphPixelHeightAndWidth, FT_Face face, char ch, float& outU, float& outV, float& outWidth, float& outHeight, float& outAdvanceX, float& outAdvanceY)
  {
    (void)nBufferHeight;

    outWidth = 0.0f;
    outHeight = 0.0f;

    // The first thing we do is get FreeType to render our character into a bitmap.  This actually requires a couple of FreeType commands:

    // Load the Glyph for our character.
    if (FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT)) {
      std::cout<<"DrawGlyphToBuffer FT_Load_Glyph FAILED"<<std::endl;
      return;
    }

    // Move the face's glyph into a Glyph object.
    FT_Glyph glyph;
    if (FT_Get_Glyph(face->glyph, &glyph)) {
      std::cout<<"DrawGlyphToBuffer FT_Get_Glyph FAILED"<<std::endl;
      return;
    }

    // Convert the glyph to a bitmap.
    FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

    // This reference will make accessing the bitmap easier
    const FT_Bitmap& bitmap = bitmap_glyph->bitmap;

    // Use our helper function to get the widths of the bitmap data that we will need in order to create our texture.
    //size_t width = spitfire::math::NextPowerOfTwo(bitmap.width);
    //size_t height = spitfire::math::NextPowerOfTwo(bitmap.rows);
    const size_t width = glyphPixelHeightAndWidth;
    const size_t height = glyphPixelHeightAndWidth;

    // Allocate memory for the texture data.
    GLubyte* expanded_data = new GLubyte[width * height];

    const size_t nSrcRowWidthBytes = width;

    // Here we fill in the data for the expanded bitmap.
    // Notice that we are using two channel bitmap (one for
    // luminocity and one for alpha), but we assign
    // both luminocity and alpha to the value that we
    // find in the FreeType bitmap.
    // We use the ?: operator so that value which we use
    // will be 0 if we are in the padding zone, and whatever
    // is the the Freetype bitmap otherwise.
    for (size_t y = 0; y < height; y++) {
      for (size_t x = 0; x < width; x++) {
        if ((x >= size_t(bitmap.width)) || (y >= size_t(bitmap.rows))) {
          expanded_data[(nSrcRowWidthBytes * y) + x] = 0;
        } else {
          expanded_data[(nSrcRowWidthBytes * y) + x] = bitmap.buffer[(bitmap.width * y) + x];
        }
      }
    }


    // Copy expanded_data into pBuffer
    assert(ch >= 0);
    const size_t index = size_t(ch);

    const size_t nDestRowWidthBytes = nBufferWidth * 4;
    const size_t nDestOffset = (((index / columns) * nDestRowWidthBytes * height) + ((index % rows) * width * 4));

    for (size_t y = 0; y < height; y++) {
      for (size_t x = 0; x < width; x++) {
        const uint8_t value = expanded_data[(y * nSrcRowWidthBytes) + x];
        pBuffer[nDestOffset + ((y * nDestRowWidthBytes) + (x * 4))] = value;
        pBuffer[nDestOffset + ((y * nDestRowWidthBytes) + (x * 4)) + 1] = value;
        pBuffer[nDestOffset + ((y * nDestRowWidthBytes) + (x * 4)) + 2] = value;
        pBuffer[nDestOffset + ((y * nDestRowWidthBytes) + (x * 4)) + 3] = value;
      }
    }

    // With the texture created, we don't need to expanded data anymore
    delete [] expanded_data;
    expanded_data = nullptr;


    outU = float(bitmap_glyph->left) / float(width) / float(columns);
    outV = float(bitmap_glyph->top) / float(height) / float(rows);
    outWidth = float(bitmap.width) / float(width) / float(columns);
    outHeight = float(bitmap.rows) / float(height) / float(rows);
    outAdvanceX = float(face->glyph->advance.x) / 128.0f / float(width) / float(columns); // 128 is just a magic number that looks ok
    outAdvanceY = float(face->glyph->advance.y) / 128.0f / float(height) / float(rows); // 128 is just a magic number that looks ok
  }



  cFont::cFont() :
    pTexture(nullptr),
    pShader(nullptr)
  {
  }

  cFont::~cFont()
  {
    assert(pTexture == nullptr);
    assert(pShader == nullptr);
  }

  unsigned int RoundDownToNearestEvenNumber(unsigned int value)
  {
    // Round down to the nearest even number
    return (2 * (value / 2));
  }

  bool cFont::Load(cContext& context, const opengl::string_t& sFilename, unsigned int _height, const opengl::string_t& sVertexShader, const opengl::string_t& sFragmentShader)
  {
    std::cout<<"cFont::Load \""<<opengl::string::ToUTF8(sFilename)<<"\""<<std::endl;

    // Create and initilize a freetype font library.
    FT_Library library;
    if (FT_Init_FreeType(&library)) {
      std::cout<<"cFont::cFont FT_Init_FreeType FAILED, returning false"<<std::endl;
      return false;
    }

    // This is where we load in the font information from the file.
    // Of all the places where the code might die, this is the most likely,
    // as FT_New_Face will die if the font file does not exist or is somehow broken.
    FT_Face face = NULL;
    if (FT_New_Face(library, opengl::string::ToUTF8(sFilename).c_str(), 0, &face )) {
      std::cout<<"cFont::cFont FT_New_Face FAILED to load font \""<<opengl::string::ToUTF8(sFilename)<<"\", returning false"<<std::endl;
      return false;
    }

    // For some twisted reason, Freetype measures font size
    // in terms of 1/64ths of pixels.  Thus, to make a font
    // h pixels high, we need to request a size of h*64.
    const unsigned int height = RoundDownToNearestEvenNumber(_height);
    ASSERT((height % 2) == 0);
    FT_Set_Char_Size(face, height * 64, height * 64, 96, 96);


    const size_t n = 128;
    const size_t rows = 16;
    const size_t columns = 16;

    fGlyphU.insert(fGlyphU.begin(), n, 0.0f);
    fGlyphV.insert(fGlyphV.begin(), n, 0.0f);
    fGlyphWidth.insert(fGlyphWidth.begin(), n, 0.0f);
    fGlyphHeight.insert(fGlyphHeight.begin(), n, 0.0f);
    fGlyphAdvanceX.insert(fGlyphAdvanceX.begin(), n, 0.0f);
    fGlyphAdvanceY.insert(fGlyphAdvanceY.begin(), n, 0.0f);

    const size_t glyphPixelHeightAndWidth = spitfire::math::NextPowerOfTwo(height * 2);

    const size_t nBufferWidth = columns * glyphPixelHeightAndWidth;
    const size_t nBufferHeight = rows * glyphPixelHeightAndWidth;
    const size_t nBufferSizeBytes = nBufferWidth * nBufferHeight * 4;

    uint8_t* pBuffer = new uint8_t[nBufferSizeBytes];

    // This is where we actually create each of the fonts display lists.
    for (size_t i = 0; i < n; i++) {
      DrawGlyphToBuffer(pBuffer, nBufferWidth, nBufferHeight, columns, rows, glyphPixelHeightAndWidth, face, char(i), fGlyphU[i], fGlyphV[i], fGlyphWidth[i], fGlyphHeight[i], fGlyphAdvanceX[i], fGlyphAdvanceY[i]);
    }

    // We don't need the face information now that the display
    // lists have been created, so we free the assosiated resources.
    FT_Done_Face(face);

    // Ditto for the library.
    FT_Done_FreeType(library);

    // Create our texture from the buffer
    pTexture = context.CreateTextureFromBuffer(pBuffer, nBufferWidth, nBufferHeight, PIXELFORMAT::R8G8B8A8);

    delete [] pBuffer;
    pBuffer = nullptr;

    if (pTexture == nullptr) {
      std::cout<<"cFont::cFont CreateTextureFromBuffer FAILED, returning false"<<std::endl;
      return false;
    }

    pShader = context.CreateShader(sVertexShader, sFragmentShader);
    if (pShader == nullptr) {
      std::cout<<"cFont::cFont CreateShader FAILED, returning false"<<std::endl;
      return false;
    }

    std::cout<<"cFont::Load returning true"<<std::endl;
    return true;
  }

  void cFont::Destroy(cContext& context)
  {
    if (pShader != nullptr) {
      context.DestroyShader(pShader);
      pShader = nullptr;
    }

    if (pTexture != nullptr) {
      context.DestroyTexture(pTexture);
      pTexture = nullptr;
    }
  }

  spitfire::math::cVec2 cFont::GetDimensions(const opengl::string_t& sText) const
  {
    spitfire::math::cVec2 dimensions;

    float fCharacterWidth = 0.0f;
    float fCharacterHeight = 0.0f;

    const size_t n = sText.size();
    for (size_t i = 0; i < n; i++) {
      // Get the character that this is
      assert(sText[i] >= 0);
      size_t c = size_t(sText[i]);

      // Now lookup the character in the array of widths and heights
      fCharacterWidth = fGlyphWidth[c];
      fCharacterHeight = fGlyphHeight[c];

      // Add the character width and glyph advance value
      dimensions.x += fCharacterWidth + fGlyphAdvanceX[i];

      // If this is the tallest character so far then set our current tallest character to us
      if (fCharacterHeight > dimensions.y) dimensions.y = fCharacterHeight;
    }

    // I'm not sure why 1000?  This seems to work and look nice but I don't have a clue what
    // the right value is/where it is from, I thought I wouldn't have to do anything to this number, we could just use it directly?
    //const float fOneOver1000 = 1.0f / 1000;
    //dimensions.x *= fOneOver1000;
    //dimensions.y *= fOneOver1000;

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

  void cFont::PushBack(opengl::cGeometryBuilder_v2_c4_t2& builder, const opengl::string_t& sText, const spitfire::math::cColour& colour, const spitfire::math::cVec2& _position, float fRotationDegrees, const spitfire::math::cVec2& scale) const
  {
    (void)fRotationDegrees;
    (void)scale;

#if 1
    spitfire::math::cVec2 position(_position);

    const size_t rows = 16;
    const size_t columns = 16;

    const float fGridWidth = 1.0f / float(rows);
    const float fGridHeight = 1.0f / float(columns);

    // For each character calculate the position in the world and the position in the texture and add a quad to the buffer
    const std::string sTextUTF8 = opengl::string::ToUTF8(sText);
    const size_t n = sTextUTF8.length();
    for (size_t i = 0; i < n; i++) {
      const char c = sTextUTF8[i];
      //std::cout<<"cFont::PushBack c="<<c<<", x="<<position.x<<std::endl;
      const size_t index = size_t(c);

      const float fCharacterX = position.x - fGlyphU[index];
      const float fCharacterY = position.y - fGlyphV[index];

      const float fCharacterWidth = fGlyphWidth[index];
      const float fCharacterHeight = fGlyphHeight[index];

      // TODO: Find out where in the texture this character is
      const float fTextureCharacterOffsetU = (float(index % rows) * fGridWidth) + fGlyphU[i];
      const float fTextureCharacterOffsetV = (float(index / columns) * fGridHeight) + fGlyphV[i];

      const float fTextureCharacterWidth = fGlyphWidth[index];
      const float fTextureCharacterHeight = fGlyphHeight[index];

      builder.PushBack(spitfire::math::cVec2(fCharacterX, fCharacterY + fCharacterHeight), colour, spitfire::math::cVec2(fTextureCharacterOffsetU, fTextureCharacterOffsetV + fTextureCharacterHeight));
      builder.PushBack(spitfire::math::cVec2(fCharacterX + fCharacterWidth, fCharacterY + fCharacterHeight), colour, spitfire::math::cVec2(fTextureCharacterOffsetU + fTextureCharacterWidth, fTextureCharacterOffsetV + fTextureCharacterHeight));
      builder.PushBack(spitfire::math::cVec2(fCharacterX + fCharacterWidth, fCharacterY), colour, spitfire::math::cVec2(fTextureCharacterOffsetU + fTextureCharacterWidth, fTextureCharacterOffsetV));
      builder.PushBack(spitfire::math::cVec2(fCharacterX + fCharacterWidth, fCharacterY), colour, spitfire::math::cVec2(fTextureCharacterOffsetU + fTextureCharacterWidth, fTextureCharacterOffsetV));
      builder.PushBack(spitfire::math::cVec2(fCharacterX, fCharacterY), colour, spitfire::math::cVec2(fTextureCharacterOffsetU, fTextureCharacterOffsetV));
      builder.PushBack(spitfire::math::cVec2(fCharacterX, fCharacterY + fCharacterHeight), colour, spitfire::math::cVec2(fTextureCharacterOffsetU, fTextureCharacterOffsetV + fTextureCharacterHeight));

      // Move the cursor for the next character
      position.x += fCharacterWidth + fGlyphAdvanceX[i];
      //position.y += fGlyphAdvanceY[i];

      // TODO: Use rotation
      //position.x += fCharacterWidth * cosf(fRotationDegrees);
      //position.y += fCharacterWidth * sinf(fRotationDegrees);

      // TODO: Use scale
      //position.x += scale.x * fCharacterWidth * cosf(fRotationDegrees);
      //position.y += scale.y * fCharacterWidth * sinf(fRotationDegrees);
    }
#else
    // For viewing the whole font
    builder.PushBack(spitfire::math::cVec2(0.0f, 1.0f), colour, spitfire::math::cVec2(0.0f, 1.0f));
    builder.PushBack(spitfire::math::cVec2(1.0f, 1.0f), colour, spitfire::math::cVec2(1.0f, 1.0f));
    builder.PushBack(spitfire::math::cVec2(1.0f, 0.0f), colour, spitfire::math::cVec2(1.0f, 0.0f));
    builder.PushBack(spitfire::math::cVec2(1.0f, 0.0f), colour, spitfire::math::cVec2(1.0f, 0.0f));
    builder.PushBack(spitfire::math::cVec2(0.0f, 0.0f), colour, spitfire::math::cVec2(0.0f, 0.0f));
    builder.PushBack(spitfire::math::cVec2(0.0f, 1.0f), colour, spitfire::math::cVec2(0.0f, 1.0f));
#endif
  }
}
