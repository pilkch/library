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
#include <vector>

// FreeType headers
#include <freetype/ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

// OpenGL headers
#include <GL/GLee.h>
#include <GL/glu.h>

// SDL headers
#include <SDL/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

// libopenglmm headers
#include <libopenglmm/cContext.h>
#include <libopenglmm/cGeometry.h>
#include <libopenglmm/cFont.h>

namespace opengl
{
  // Create a display list coresponding to the give character.
  void DrawGlyphToBuffer(GLubyte* pBuffer, size_t nBufferWidth, size_t nBufferHeight, size_t glyphPixelHeightAndWidth, FT_Face face, char ch, float& outWidth, float& outHeight)
  {
    outWidth = 0.0f;
    outHeight = 0.0f;

    // The first thing we do is get FreeType to render our character into a bitmap.  This actually requires a couple of FreeType commands:

    // Load the Glyph for our character.
    if (FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT)) {
      std::cout<<"make_dlist FT_Load_Glyph failed"<<std::endl;
      return;
    }

    // Move the face's glyph into a Glyph object.
    FT_Glyph glyph;
    if (FT_Get_Glyph(face->glyph, &glyph)) {
      std::cout<<"make_dlist FT_Get_Glyph failed"<<std::endl;
      return;
    }

    // Convert the glyph to a bitmap.
    FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

    // This reference will make accessing the bitmap easier
    const FT_Bitmap& bitmap = bitmap_glyph->bitmap;

    // Use our helper function to get the widths of the bitmap data that we will need in order to create our texture.
    size_t width = spitfire::math::NextPowerOfTwo(bitmap.width);
    size_t height = spitfire::math::NextPowerOfTwo(bitmap.rows);

    // Allocate memory for the texture data.
    GLubyte* expanded_data = new GLubyte[2 * width * height];

    // Here we fill in the data for the expanded bitmap.
    // Notice that we are using two channel bitmap (one for
    // luminocity and one for alpha), but we assign
    // both luminocity and alpha to the value that we
    // find in the FreeType bitmap.
    // We use the ?: operator so that value which we use
    // will be 0 if we are in the padding zone, and whatever
    // is the the Freetype bitmap otherwise.
    for (size_t j = 0; j < height; j++) {
      for (size_t i = 0; i < width; i++) {
        expanded_data[2 * ((j * width) + i)] = expanded_data[2 * ((j * width) + i) + 1] =
          ((i >= size_t(bitmap.width)) || (j >= size_t(bitmap.rows))) ? 0 : bitmap.buffer[(bitmap.width * j) + i];
      }
    }


    // Now we just setup some texture paramaters.
    assert(ch >= 0);
    const size_t index = size_t(ch);

    const size_t nSrcRowWidthBytes = width;
    const size_t nDestRowWidthBytes = nBufferWidth * glyphPixelHeightAndWidth;
    const size_t nDestOffset = index * nBufferWidth * nDestRowWidthBytes;

    // Copy expanded_data into pBuffer
    for (size_t y = 0; y < height; y++) {
      for (size_t x = 0; x < width; x++) {
        pBuffer[nDestOffset + ((y * nDestRowWidthBytes) + x)] = expanded_data[(y * nSrcRowWidthBytes) + x];
      }
    }

    // With the texture created, we don't need to expanded data anymore
    delete [] expanded_data;
    expanded_data = nullptr;





    /*// First we need to move over a little so that
    // the character has the right amount of space
    // between it and the one before it.
    glTranslatef(static_cast<float>(bitmap_glyph->left), 0, 0);

    // Now we move down a little in the case that the
    // bitmap extends past the bottom of the line
    // (this is only true for characters like 'g' or 'y'.
    glPushMatrix();
      glTranslatef(0, - static_cast<float>(bitmap_glyph->top), 0);

      // Now we need to account for the fact that many of
      // our textures are filled with empty padding space.
      // We figure what portion of the texture is used by
      // the actual character and store that information in
      // the x and y variables, then when we draw the
      // quad, we will only reference the parts of the texture
      // that we contain the character itself.
      float x = static_cast<float>(bitmap.width) / static_cast<float>(width);
      float y = static_cast<float>(bitmap.rows) / static_cast<float>(height);

      // Here we draw the texturemaped quads.
      // The bitmap that we got from FreeType was not
      // oriented quite like we would like it to be,
      // so we need to link the texture to the quad
      // so that the result will be properly aligned.
      glBegin(GL_QUADS);
        glVertex2f(0.0f, static_cast<float>(bitmap.rows)); glTexCoord2f(x, 0.0f);
        glVertex2f(static_cast<float>(bitmap.width), static_cast<float>(bitmap.rows)); glTexCoord2f(x, y);
        glVertex2f(static_cast<float>(bitmap.width), 0.0f); glTexCoord2f(0.0f, y);
        glVertex2f(0.0f, 0.0f); glTexCoord2f(0.0f, 0.0f);
      glEnd();
    glPopMatrix();
    glTranslatef(static_cast<float>(face->glyph->advance.x >> 6), 0, 0);*/

    outWidth = static_cast<float>(face->glyph->advance.x >> 6);
    outHeight = static_cast<float>(face->glyph->advance.y >> 6);
  }



  cFont::cFont() :
    pTexture(nullptr)
  {
  }

  cFont::~cFont()
  {
    assert(pTexture == nullptr);
  }

  bool cFont::Load(cContext& context, const std::string& sFilename, unsigned int height)
  {
    std::cout<<"cFont::Load \""<<sFilename<<"\""<<std::endl;

    // Create and initilize a freetype font library.
    FT_Library library;
    if (FT_Init_FreeType(&library)) {
      std::cout<<"cFont::cFont FT_Init_FreeType failed"<<std::endl;
      return false;
    }

    // The object in which Freetype holds information on a given
    // font is called a "face".
    FT_Face face = NULL;

    // This is where we load in the font information from the file.
    // Of all the places where the code might die, this is the most likely,
    // as FT_New_Face will die if the font file does not exist or is somehow broken.
    if (FT_New_Face(library, sFilename.c_str(), 0, &face )) {
      std::cout<<"cFont::cFont FT_New_Face failed to load font \""<<sFilename<<"\""<<std::endl;
      return false;
    }

    // For some twisted reason, Freetype measures font size
    // in terms of 1/64ths of pixels.  Thus, to make a font
    // h pixels high, we need to request a size of h*64.
    FT_Set_Char_Size(face, height * 64, height * 64, 96, 96);


    const size_t n = 128;
    const size_t rows = 16;
    const size_t columns = 16;

    //fGlyphU.insert(fGlyphU.begin(), n, 0.0f);
    //fGlyphV.insert(fGlyphV.begin(), n, 0.0f);
    fGlyphWidth.insert(fGlyphWidth.begin(), n, 0.0f);
    fGlyphHeight.insert(fGlyphHeight.begin(), n, 0.0f);

    const size_t nBufferWidth = columns * 64;
    const size_t nBufferHeight = rows * 64;
    const size_t nBufferSizeBytes = nBufferWidth * nBufferHeight * 4;

    //const size_t glyphPixelHeightAndWidth = 64;

    uint8_t* pBuffer = new uint8_t[nBufferSizeBytes];

    // This is where we actually create each of the fonts display lists.
    for (size_t i = 0; i < nBufferSizeBytes; i++) {
      pBuffer[i] = 255;
    }
    //for (size_t i = 0; i < n; i++) {
    //  DrawGlyphToBuffer(pBuffer, nBufferWidth, nBufferHeight, glyphPixelHeightAndWidth, face, i, fGlyphWidth[i], fGlyphHeight[i]);
    //}

    // We don't need the face information now that the display
    // lists have been created, so we free the assosiated resources.
    FT_Done_Face(face);

    // Ditto for the library.
    FT_Done_FreeType(library);

    // Create our texture from the buffer
    pTexture = context.CreateTextureFromBuffer(pBuffer, nBufferWidth, nBufferHeight, PIXELFORMAT::R8G8B8A8);
    //pTexture = context.CreateTexture("../openglmm_fbo/textures/diffuse.png");
    assert(pTexture != nullptr);

    delete [] pBuffer;
    pBuffer = nullptr;

    std::cout<<"cFont::Load returning true"<<std::endl;
    return true;
  }

  void cFont::Destroy(cContext& context)
  {
    if (pTexture != nullptr) {
      context.DestroyTexture(pTexture);
      pTexture = nullptr;
    }
  }

  void cFont::_GetDimensions(const std::string& line, float& width, float& height) const
  {
    width = 0.0f;
    height = 0.0f;

    float characterWidth = 0.0f;
    float characterHeight = 0.0f;

    const size_t n = line.size();
    for (size_t i = 0; i < n; i++) {
      // Get the character that this is
      assert(line[i] >= 0);
      size_t c = size_t(line[i]);

      // Now lookup the character in the array of widths and heights
      characterWidth = fGlyphWidth[c];
      characterHeight = fGlyphHeight[c];

      // Add the characterWidth and if this is the tallest character so far then set our current tallest character to us
      width += characterWidth;
      if (characterHeight > height) height = characterHeight;
    }

    // I'm not sure why 1000?  This seems to work and look nice but I don't have a clue what
    // the right value is/where it is from, I thought I wouldn't have to do anything to this number, we could just use it directly?
    const float fOneOver1000 = 1.0f / 1000;
    width *= fOneOver1000;
    height *= fOneOver1000;
  }

  void cFont::_GetDimensions(const std::vector<std::string> lines, float& width, float& height) const
  {
    width = 0.0f;
    height = 0.0f;

    float lineWidth = 0.0f;
    float lineHeight = 0.0f;

    const size_t n = lines.size();
    for (size_t i = 0; i < n; i++) {
      _GetDimensions(lines[i], lineWidth, lineHeight);

      // Add the lineHeight and if this is the widest line so far then set our current widest line to us
      if (lineWidth > width) width = lineWidth;
      height += lineHeight;
    }
  }

  void cFont::PushBack(opengl::cGeometryBuilder_v2_c4_t2& builder, const std::string& sText, const spitfire::math::cColour& colour, const spitfire::math::cVec2& _position, float fRotationDegrees, const spitfire::math::cVec2& scale) const
  {
    spitfire::math::cVec2 position(_position);

    // For each character calculate the position in the world and the position in the texture and add a quad to the buffer
    const size_t n = sText.length();
    for (size_t i = 0; i < n; i++) {
      const float fCharacterWidth = 0.1f;
      const float fCharacterHeight = 0.1f;

      //const char c = sText[i];

      // TODO: Find out where in the texture this character is
      float fTextureCharacterOffsetU = 0.0f;
      float fTextureCharacterOffsetV = 0.0f;
      float fTextureCharacterWidth = 1.0f;
      float fTextureCharacterHeight = 1.0f;

      builder.PushBack(spitfire::math::cVec2(position.x, position.y + fCharacterHeight), colour, spitfire::math::cVec2(fTextureCharacterOffsetU, fTextureCharacterOffsetV));
      builder.PushBack(spitfire::math::cVec2(position.x + fCharacterWidth, position.y + fCharacterHeight), colour, spitfire::math::cVec2(fTextureCharacterOffsetU + fTextureCharacterWidth, fTextureCharacterOffsetV));
      builder.PushBack(spitfire::math::cVec2(position.x + fCharacterWidth, position.y), colour, spitfire::math::cVec2(fTextureCharacterOffsetU + fTextureCharacterWidth, fTextureCharacterOffsetV + fTextureCharacterHeight));
      builder.PushBack(spitfire::math::cVec2(position.x, position.y), colour, spitfire::math::cVec2(fTextureCharacterOffsetU, fTextureCharacterOffsetV + fTextureCharacterHeight));

      // Move the cursor for the next character
      position.x += fCharacterWidth;

      // TODO: Use rotation
      //position.x += fCharacterWidth * cosf(fRotationDegrees);
      //position.y += fCharacterWidth * sinf(fRotationDegrees);

      // TODO: Use scale
      //position.x += scale.x * fCharacterWidth * cosf(fRotationDegrees);
      //position.y += scale.y * fCharacterWidth * sinf(fRotationDegrees);
    }
  }

}
