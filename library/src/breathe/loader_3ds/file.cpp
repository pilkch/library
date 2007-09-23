#include <sstream>

#include <list>

// writing on a text file
#include <iostream>
#include <fstream>

// Breathe
#include <breathe/breathe.h>
#include <breathe/util/log.h>

#include <breathe/loader_3ds/file.h>
#include <breathe/loader_3ds/build3ds.h>

namespace breathe
{
	namespace loader_3ds
	{
		Model3DSFile::Model3DSFile(std::string src)
			: file(src.c_str(), std::ios::binary)
		{

		}

		Model3DSFile::~Model3DSFile()
		{
			file.close();
		}

		Model3DSChunk Model3DSFile::Child()
		{
#ifdef DEBUG3DS
			LOG.Success("c3ds", "SEEK 0");
#endif //DEBUG3DS

			file.seekg(0, std::ios::beg);
			return Model3DSChunk(file , FileSize(file));
		}

		int Model3DSFile::FileSize(std::ifstream &file)
		{
			int curr_pos = file.tellg();

#ifdef DEBUG3DS
			LOG.Success("c3ds", "SEEK 0");
#endif //DEBUG3DS

			file.seekg(0, std::ios::beg);
			int beginning = file.tellg();

			file.seekg(0 , std::ios::end);
			int ending = file.tellg();
			
#ifdef DEBUG3DS
			std::ostringstream t;
			t	<<"SEEK " << ending;
			LOG.Success("c3ds", t.str());
#endif //DEBUG3DS

			file.seekg(curr_pos, std::ios::beg);

#ifdef DEBUG3DS
			t.str("");
			t	<<"SEEK " << curr_pos;
			LOG.Success("c3ds", t.str());
#endif //DEBUG3DS

			return ending - beginning;
		}
	}
}