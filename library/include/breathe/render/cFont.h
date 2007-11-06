#ifndef CFONT_H
#define CFONT_H

namespace breathe
{
	namespace render
	{
		//This holds all of the information related to any
		//freetype font that we want to create.  
		class cFont 
		{
		public:
			cFont(const std::string fname, unsigned int h);
			~cFont();
      
			void printf(float x, float y, const char* fmt, ...);

			float h;			///< Holds the height of the font.
			GLuint* textures;	///< Holds the texture id's 
			GLuint list_base;	///< Holds the first display list id
		};
	}
}

#endif //CFONT
