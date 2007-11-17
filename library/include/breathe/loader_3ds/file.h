#ifndef FILE_H
#define FILE_H

#include <fstream>

#include "chunk.h"

namespace breathe
{
	namespace loader_3ds
	{
		class Model3DSFile
		{
		public:
			Model3DSFile(const std::string& src);
			~Model3DSFile();

			Model3DSChunk Child();

		private:
			static int FileSize(std::ifstream &file);
			
			std::ifstream file;
		};
	}
}

#endif
