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
			cFont(const std::string filename, unsigned int height);
			~cFont();

			bool IsValid() const { return (textures != nullptr) && (list_base != 0);}
			float GetHeight() const { return h; }

			void printf(float x, float y, const char* fmt, ...);
			void printfCenteredHorizontally(float x, float y, float width, const char* fmt, ...);
			void printfCenteredVertically(float x, float y, float height, const char* fmt, ...);
			void printfCenteredHorizontallyVertically(float x, float y, float height, float width, const char* fmt, ...);

		private:
			void _print(float x, float y, const std::vector<std::string>& lines);
			void _GetDimensions(const std::string& line, float& width, float& height) const;

			float h;			///< Holds the height of the font.
			GLuint* textures;	///< Holds the texture id's 
			GLuint list_base;	///< Holds the first display list id
		};
	}
}

#endif //CFONT
