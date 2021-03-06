// A quick and simple opengl font library that uses GNU freetype2, written
// and distributed as part of a tutorial for nehe.gamedev.net.
// Sven Olsen, 2003

#include <cmath>
#include <cassert>

#include <list>
#include <vector>
#include <string>
#include <map>

#include <iostream>
#include <fstream>
#include <sstream>

// FreeType headers
#include <freetype/ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

// OpenGL headers
#include <GL/GLee.h>

// SDL headers
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/storage/filesystem.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cColour.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/render/cContext.h>
#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cFont.h>

namespace breathe
{
  namespace render
  {
    // Create a display list coresponding to the give character.
    void make_dlist(FT_Face face, char ch, GLuint list_base, GLuint* tex_base, float& outWidth, float& outHeight)
    {
      outWidth = 0.0f;
      outHeight = 0.0f;

      // The first thing we do is get FreeType to render our character into a bitmap.  This actually requires a couple of FreeType commands:

      // Load the Glyph for our character.
      if (FT_Load_Glyph( face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT )) {
        LOG.Error("Font", "FT_Load_Glyph failed");
        return;
      }

      // Move the face's glyph into a Glyph object.
      FT_Glyph glyph;
      if (FT_Get_Glyph( face->glyph, &glyph )) {
        LOG.Error("Font", "FT_Get_Glyph failed");
        return;
      }

      // Convert the glyph to a bitmap.
      FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1 );
      FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

      // This reference will make accessing the bitmap easier
      FT_Bitmap& bitmap = bitmap_glyph->bitmap;

      // Use our helper function to get the widths of the bitmap data that we will need in order to create our texture.
      int width = math::NextPowerOfTwo(bitmap.width);
      int height = math::NextPowerOfTwo(bitmap.rows);

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
      for (int j = 0; j <height; j++) {
        for (int i = 0; i < width; i++){
          expanded_data[2 * (i + j * width)] = expanded_data[2 * (i + j * width) + 1] =
            ((i >= bitmap.width) || (j >= bitmap.rows)) ? 0 : bitmap.buffer[i + bitmap.width * j];
        }
      }


      // Now we just setup some texture paramaters.
      ASSERT(ch >= 0);
      const size_t index = size_t(ch);
      glBindTexture(GL_TEXTURE_2D, tex_base[index]);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

      // Here we actually create the texture itself, notice
      // that we are using GL_LUMINANCE_ALPHA to indicate that
      // we are using 2 channel data.
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data);

      // With the texture created, we don't need to expanded data anymore
      breathe::SAFE_DELETE_ARRAY(expanded_data);

      // So now we can create the display list
      glNewList(list_base + index, GL_COMPILE);

      glBindTexture(GL_TEXTURE_2D, tex_base[index]);

      // First we need to move over a little so that
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
          glTexCoord2d(0.0f,  y);     glVertex2f(0.0f, static_cast<float>(bitmap.rows));
          glTexCoord2d(0.0f,  0.0f);  glVertex2f(0.0f, 0.0f);
          glTexCoord2d(x,     0.0f);  glVertex2f(static_cast<float>(bitmap.width), 0.0f);
          glTexCoord2d(x,     y);     glVertex2f(static_cast<float>(bitmap.width), static_cast<float>(bitmap.rows));
        glEnd();
      glPopMatrix();
      glTranslatef(static_cast<float>(face->glyph->advance.x >> 6), 0, 0);


      // Increment the raster position as if we were a bitmap font.
      // (only needed if you want to calculate text length)
      //glBitmap(0,0,0,0,face->glyph->advance.x >> 6,0,NULL);

      // Finish the display list
      glEndList();

      outWidth = static_cast<float>(face->glyph->advance.x >> 6);
      outHeight = static_cast<float>(face->glyph->advance.y >> 6);
    }



    cFont::cFont(const string_t& sNewFilename, unsigned int height) :
      h(float(height)),
      textures(nullptr),
      list_base(0)
    {
      string_t sFilename;
      if (!breathe::filesystem::FindResourceFile(TEXT("fonts/"), sNewFilename, sFilename)) {
        LOG<<"cFont::cFont File "<<sNewFilename<<" not found in fonts/"<<std::endl;
        return;
      }

      // Create and initilize a freetype font library.
      FT_Library library;
      if (FT_Init_FreeType(&library)) {
        LOG.Error("Font", "FT_Init_FreeType failed");
        return;
      }

      // The object in which Freetype holds information on a given
      // font is called a "face".
      FT_Face face = NULL;

      // This is where we load in the font information from the file.
      // Of all the places where the code might die, this is the most likely,
      // as FT_New_Face will die if the font file does not exist or is somehow broken.
      if (FT_New_Face(library, breathe::string::ToUTF8(sFilename).c_str(), 0, &face )) {
        LOG.Error("Font", "FT_New_Face failed to load font \"" + breathe::string::ToUTF8(sFilename) + "\"");
        return;
      }

      // For some twisted reason, Freetype measures font size
      // in terms of 1/64ths of pixels.  Thus, to make a font
      // h pixels high, we need to request a size of h*64.
      FT_Set_Char_Size(face, height * 64, height * 64, 96, 96);


      const size_t n = 128;

      // Allocate some memory to store the texture ids.
      textures = new GLuint[n];

      // Here we ask opengl to allocate resources for
      // all the textures and displays lists which we
      // are about to create.
      list_base = glGenLists(n);
      glGenTextures(n, textures);

      breathe::vector::push_back(fGlyphWidth, n, 0.0f);
      breathe::vector::push_back(fGlyphHeight, n, 0.0f);

      // This is where we actually create each of the fonts display lists.
      for (size_t i = 0; i < n; i++) {
        make_dlist(face, i, list_base, textures, fGlyphWidth[i], fGlyphHeight[i]);
      }

      // We don't need the face information now that the display
      // lists have been created, so we free the assosiated resources.
      FT_Done_Face(face);

      // Ditto for the library.
      FT_Done_FreeType(library);
    }

    cFont::~cFont()
    {
      if (list_base != 0) {
        glDeleteLists(list_base, 128);
        glDeleteTextures(128, textures);
      }

      SAFE_DELETE_ARRAY(textures);
    }

    void cFont::_GetDimensions(const string_t& line, float& width, float& height) const
    {
      width = 0.0f;
      height = 0.0f;

      float characterWidth = 0.0f;
      float characterHeight = 0.0f;

      const size_t n = line.size();
      for (size_t i = 0; i < n; i++) {
        // Get the character that this is
        ASSERT(line[i] >= 0);
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

    void cFont::_GetDimensions(const std::vector<string_t> lines, float& width, float& height) const
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

    // Much like Nehe's glPrint function, but modified to work with freetype fonts.
    void cFont::_print(float x, float y, const std::vector<string_t>& lines)
    {
      glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
          glEnable(GL_TEXTURE_2D);
          glDisable(GL_DEPTH_TEST);
          glEnable(GL_BLEND);
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

          glListBase(list_base);

          //This is where the text display actually happens.
          //For each line of text we reset the modelview matrix
          //so that the line's text will start in the correct position.
          //Notice that we need to reset the matrix, rather than just translating
          //down by h. This is because when each character is
          //draw it modifies the current matrix so that the next character
          //will be drawn immediatly after it.
          const size_t n = lines.size();
          for (size_t i = 0; i < n; i++) {
            glPushMatrix();
              glTranslatef(x, y, 0.0f);
              glScalef(0.001f, 0.001f, 1.0f);
              glCallLists(static_cast<unsigned int>(lines[i].length()), GL_UNSIGNED_BYTE, breathe::string::ToUTF8(lines[i]).c_str());
            glPopMatrix();
          }

        glEnable(GL_DEPTH_TEST);

      glPopAttrib();
    }

    void cFont::printf(float x, float y, const char* fmt, ...)
    {
      if (fmt == nullptr) return;

      ASSERT(strlen(fmt) < 1024);

      char formatted[1024];            // Holds Our String
      va_list ap;                      // Pointer To List Of Arguments

      va_start(ap, fmt);               // Parses The String For Variables
        vsprintf(formatted, fmt, ap);  // And Converts Symbols To Actual Numbers
      va_end(ap);                      // Results Are Stored In Text

      string_t text(breathe::string::ToString_t(formatted));

      PrintAt(x, y, text);
    }

    void cFont::PrintAt(float x, float y, const string_t& text)
    {
      std::vector<string_t> lines;
      breathe::string::SplitOnNewLines(text, lines);

      _print(x, y, lines);
    }

    void cFont::PrintCenteredHorizontally(float x, float y, float width, const string_t& text)
    {
      std::vector<string_t> lines;
      breathe::string::SplitOnNewLines(text, lines);

      x += width * 0.5f;

      float w;
      float h;
      _GetDimensions(lines, w, h);

      x -= w * 0.5f;

      _print(x, y, lines);
    }

    void cFont::PrintCenteredVertically(float x, float y, float height, const string_t& text)
    {
      std::vector<string_t> lines;
      breathe::string::SplitOnNewLines(text, lines);

      y += height * 0.5f;

      float w;
      float h;
      _GetDimensions(lines, w, h);

      y -= h * 0.5f;

      _print(x, y, lines);
    }

    void cFont::PrintCenteredHorizontallyVertically(float x, float y, float width, float height, const string_t& text)
    {
      std::vector<string_t> lines;
      breathe::string::SplitOnNewLines(text, lines);

      x += width * 0.5f;
      y += height * 0.5f;

      float w;
      float h;
      _GetDimensions(lines, w, h);

      x -= w * 0.5f;
      y -= h * 0.5f;

      _print(x, y, lines);
    }
  }
}
