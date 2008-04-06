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
			cFont(const string_t& filename, unsigned int height);
			~cFont();

			bool IsValid() const { return (textures != nullptr) && (list_base != 0);}
			float GetHeight() const { return h; }

      // Use this only if you have to format a float/int etc.
      void printf(float x, float y, const char* fmt, ...);

      void PrintAt(float x, float y, const string_t& text);
			void PrintCenteredHorizontally(float x, float y, float width, const string_t& text);
			void PrintCenteredVertically(float x, float y, float height, const string_t& text);
			void PrintCenteredHorizontallyVertically(float x, float y, float height, float width, const string_t& text);

		private:
			void _print(float x, float y, const std::vector<string_t>& lines);
			void _GetDimensions(const string_t& line, float& width, float& height) const;

			float h;			///< Holds the height of the font.
			GLuint* textures;	///< Holds the texture id's 
			GLuint list_base;	///< Holds the first display list id
		};
	}
}

#endif //CFONT
