#ifndef CFONT_H
#define CFONT_H

namespace BREATHE
{
	namespace RENDER
	{
		//This holds all of the information related to any
		//freetype font that we want to create.  
		class cFont 
		{
		public:
			//The init function will create a font of
			//of the height h from the file fname.
			cFont(const char * fname, unsigned int h);

			//Free all the resources assosiated with the font.
			~cFont();

			float h;			///< Holds the height of the font.
			GLuint * textures;	///< Holds the texture id's 
			GLuint list_base;	///< Holds the first display list id			
		};

		// TODO: Add as member funtcion to cFont?
		//The flagship function of the library - this thing will print
		//out text at window coordinates x,y, using the font ft_font.
		//The current modelview matrix will also be applied to the text. 
		void printf(const cFont* ft_font, float x, float y, const char *fmt, ...);
	}
}

#endif //CFONT
