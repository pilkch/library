#include "BREATHE/LOADER_3DS/file.h"
#include "BREATHE/LOADER_3DS/build3ds.h"

#include <iostream>
#include <sstream>
using namespace std;

#include <BREATHE/UTIL/cLog.h>

namespace BREATHE
{
	namespace LOADER_3DS
	{
		Model3DSFile::Model3DSFile(std::string src)
		: file(src.c_str(), ios::binary)
		{

		}

		Model3DSFile::~Model3DSFile()
		{
			file.close();
		}

		Model3DSChunk Model3DSFile::Child()
		{
#ifdef DEBUG3DS
			std::ostringstream t;
			t	<<"SEEK " << 0;
			pLog->Success("c3ds", t.str());
#endif //DEBUG3DS

			file.seekg(0, std::ios::beg);
			return Model3DSChunk(file , FileSize(file));
		}

		int Model3DSFile::FileSize(ifstream &file)
		{
			int curr_pos = file.tellg();

#ifdef DEBUG3DS
			std::ostringstream t;

			t	<<"SEEK " << 0;
			pLog->Success("c3ds", t.str());
#endif //DEBUG3DS

			file.seekg(0, std::ios::beg);
			int beginning = file.tellg();

			file.seekg(0 , ios::end);
			int ending = file.tellg();
			
#ifdef DEBUG3DS
			t.str("");
			t	<<"SEEK " << ending;
			pLog->Success("c3ds", t.str());
#endif //DEBUG3DS

			file.seekg(curr_pos, std::ios::beg);

#ifdef DEBUG3DS
			t.str("");
			t	<<"SEEK " << curr_pos;
			pLog->Success("c3ds", t.str());
#endif //DEBUG3DS

			return ending - beginning;
		}
	}
}