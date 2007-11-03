/*
	A quick and simple opengl font library that uses GNU freetype2, written
	and distributed as part of a tutorial for nehe.gamedev.net.
	Sven Olsen, 2003
*/

#include <cmath>

//Some STL headers
#include <list>
#include <vector>
#include <string>
#include <map>

#include <iostream>
#include <fstream>

// FreeType Headers
#include <freetype/ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

// OpenGL Headers
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>

// Breathe Headers
#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>
#include <breathe/util/filesystem.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cColour.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>
#include <breathe/render/cFont.h>

namespace breathe
{
	namespace render
	{
		///Create a display list coresponding to the give character.
		void make_dlist(FT_Face face, char ch, GLuint list_base, GLuint * tex_base )
		{
			//The first thing we do is get FreeType to render our character
			//into a bitmap.  This actually requires a couple of FreeType commands:

			//Load the Glyph for our character.
			if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT ))
			{
				LOG.Error("Font", "FT_Load_Glyph failed");
				return;
			}

			//Move the face's glyph into a Glyph object.
			FT_Glyph glyph;
			if(FT_Get_Glyph( face->glyph, &glyph ))
			{
				LOG.Error("Font", "FT_Get_Glyph failed");
				return;
			}

			//Convert the glyph to a bitmap.
			FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
				FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

			//This reference will make accessing the bitmap easier
			FT_Bitmap& bitmap=bitmap_glyph->bitmap;

			//Use our helper function to get the widths of
			//the bitmap data that we will need in order to create
			//our texture.
			int width = math::nextPowerOfTwo(bitmap.width);
			int height = math::nextPowerOfTwo(bitmap.rows);

			//Allocate memory for the texture data.
			GLubyte* expanded_data = new GLubyte[ 2 * width * height];

			//Here we fill in the data for the expanded bitmap.
			//Notice that we are using two channel bitmap (one for
			//luminocity and one for alpha), but we assign
			//both luminocity and alpha to the value that we
			//find in the FreeType bitmap. 
			//We use the ?: operator so that value which we use
			//will be 0 if we are in the padding zone, and whatever
			//is the the Freetype bitmap otherwise.
			for(int j=0; j <height;j++) {
				for(int i=0; i < width; i++){
					expanded_data[2*(i+j*width)]= expanded_data[2*(i+j*width)+1] = 
						(i>=bitmap.width || j>=bitmap.rows) ?
						0 : bitmap.buffer[i + bitmap.width*j];
				}
			}


			//Now we just setup some texture paramaters.
				glBindTexture( GL_TEXTURE_2D, tex_base[ch]);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

			//Here we actually create the texture itself, notice
			//that we are using GL_LUMINANCE_ALPHA to indicate that
			//we are using 2 channel data.
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height,
					0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data );

			//With the texture created, we don't need to expanded data anymore
			breathe::SAFE_DELETE_ARRAY(expanded_data);

			//So now we can create the display list
			glNewList(list_base+ch,GL_COMPILE);

			glBindTexture(GL_TEXTURE_2D, tex_base[ch]);

			//first we need to move over a little so that
			//the character has the right amount of space
			//between it and the one before it.
			glTranslatef(static_cast<float>(bitmap_glyph->left), 0, 0);

			//Now we move down a little in the case that the
			//bitmap extends past the bottom of the line 
			//(this is only true for characters like 'g' or 'y'.
			glPushMatrix();
			glTranslatef(0, static_cast<float>(bitmap_glyph->top-bitmap.rows),0);

			//Now we need to account for the fact that many of
			//our textures are filled with empty padding space.
			//We figure what portion of the texture is used by 
			//the actual character and store that information in 
			//the x and y variables, then when we draw the
			//quad, we will only reference the parts of the texture
			//that we contain the character itself.
			float	x=(float)bitmap.width / (float)width,
					y=(float)bitmap.rows / (float)height;

			//Here we draw the texturemaped quads.
			//The bitmap that we got from FreeType was not 
			//oriented quite like we would like it to be,
			//so we need to link the texture to the quad
			//so that the result will be properly aligned.
			glBegin(GL_QUADS);
			glTexCoord2d(0,0); glVertex2f(0, static_cast<float>(bitmap.rows));
			glTexCoord2d(0,y); glVertex2f(0, 0);
			glTexCoord2d(x,y); glVertex2f(static_cast<float>(bitmap.width), 0);
			glTexCoord2d(x,0); glVertex2f(static_cast<float>(bitmap.width), static_cast<float>(bitmap.rows));
			glEnd();
			glPopMatrix();
			glTranslatef(static_cast<float>(face->glyph->advance.x >> 6),0,0);


			//increment the raster position as if we were a bitmap font.
			//(only needed if you want to calculate text length)
			//glBitmap(0,0,0,0,face->glyph->advance.x >> 6,0,NULL);

			//Finnish the display list
			glEndList();
		}



		cFont::cFont(const std::string fname, unsigned int h)
		{
			std::string sFilename = filesystem::FindFile(fname);

			//Allocate some memory to store the texture ids.
			textures = new GLuint[128];

			this->h=static_cast<float>(h);

			//Create and initilize a freetype font library.
			FT_Library library;
			if (FT_Init_FreeType( &library ))
			{
				LOG.Error("Font", "FT_Init_FreeType failed");
				return;
			}

			//The object in which Freetype holds information on a given
			//font is called a "face".
			FT_Face face = NULL;

			//This is where we load in the font information from the file.
			//Of all the places where the code might die, this is the most likely,
			//as FT_New_Face will die if the font file does not exist or is somehow broken.
			if (FT_New_Face( library, sFilename.c_str(), 0, &face ))
			{
				LOG.Error("Font", "FT_New_Face failed to load font \"" + sFilename + "\"");
				return;
			}

			//For some twisted reason, Freetype measures font size
			//in terms of 1/64ths of pixels.  Thus, to make a font
			//h pixels high, we need to request a size of h*64.
			//(h << 6 is just a prettier way of writting h*64)
			FT_Set_Char_Size( face, h << 6, h << 6, 96, 96);

			//Here we ask opengl to allocate resources for
			//all the textures and displays lists which we
			//are about to create.  
			list_base=glGenLists(128);
			glGenTextures( 128, textures );

			//This is where we actually create each of the fonts display lists.
			for(unsigned char i=0;i<128;i++)
				make_dlist(face,i,list_base,textures);

			//We don't need the face information now that the display
			//lists have been created, so we free the assosiated resources.
			FT_Done_Face(face);

			//Ditto for the library.
			FT_Done_FreeType(library);
		}

		cFont::~cFont()
		{
			glDeleteLists(list_base, 128);
			glDeleteTextures(128, textures);
			SAFE_DELETE_ARRAY(textures);
		}

		///Much like Nehe's glPrint function, but modified to work
		///with freetype fonts.
		void cFont::printf(float x, float y, const char *fmt, ...)
		{
			x *= pRender->uiWidth;
			y *= pRender->uiHeight;

			float uiHeight=h/.63f;						//We make the height about 1.5* that of
			
			char		text[256];								// Holds Our String
			va_list		ap;										// Pointer To List Of Arguments

			if (fmt == NULL)									// If There's No Text
				*text=0;											// Do Nothing
			else {
				va_start(ap, fmt);									// Parses The String For Variables
						vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
				va_end(ap);											// Results Are Stored In Text
			}


			//Here is some code to split the text that we have been
			//given into a set of lines.
			//This could be made much neater by using
			//a regular expression library such as the one avliable from
			//boost.org (I've only done it out by hand to avoid complicating
			//this tutorial with unnecessary library dependencies).
			const char *start_line=text;
			std::vector<std::string> lines;

			const char * c = text;

			for(;*c;c++)
			{
				if(*c=='\n')
				{
					std::string line;
					for(const char *n=start_line;n<c;n++) 
						line.append(1,*n);
					lines.push_back(line);
					start_line=c+1;
				}
			}
			if(start_line) 
			{
				std::string line;
				for(const char *n=start_line;n<c;n++) 
					line.append(1,*n);
				lines.push_back(line);
			}

			glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
			glMatrixMode(GL_MODELVIEW);
			glDisable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glListBase(list_base);

			float modelview_matrix[16];	
			glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

			//This is where the text display actually happens.
			//For each line of text we reset the modelview matrix
			//so that the line's text will start in the correct position.
			//Notice that we need to reset the matrix, rather than just translating
			//down by h. This is because when each character is
			//draw it modifies the current matrix so that the next character
			//will be drawn immediatly after it.  
			for(unsigned int i=0;i<lines.size();i++) {
				

				glPushMatrix();
				glLoadIdentity();
				glTranslatef(x,y-uiHeight*i,0);
				glMultMatrixf(modelview_matrix);

				//  The commented out raster position stuff can be useful if you need to
				//  know the length of the text that you are creating.
				//  If you decide to use it make sure to also uncomment the glBitmap command
				//  in make_dlist().
				//	glRasterPos2f(0,0);
				glCallLists(static_cast<unsigned int>(lines[i].length()), GL_UNSIGNED_BYTE, lines[i].c_str());
				//	float rpos[4];
				//	glGetFloatv(GL_CURRENT_RASTER_POSITION ,rpos);
				//	float len=x-rpos[0];

				glPopMatrix();
			}

			
			glEnable(GL_DEPTH_TEST);


			glPopAttrib();
		}
	}
}
