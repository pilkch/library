#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

#include <breathe/breathe.h>
#include <breathe/util/cString.h>

#include <breathe/storage/versionedfile.h>

#ifndef FIRESTARTER
#include <breathe/util/log.h>
#include <breathe/algorithm/md5.h>
#endif


//#ifdef __WIN__
//#include <windows.h>
//#endif

namespace breathe
{
	namespace storage
	{
		bool OpenReadText(const string_t& filename, std::ifstream& file, uint16_t& version)
		{
			version = 0;

			file.open(breathe::string::ToUTF8(filename).c_str(), std::ios::in);
			if (!file.is_open()) return false;

			file>>version;
			return true;
		}
		
		bool OpenReadBinary(const string_t& filename, std::ifstream& file, uint16_t& version)
		{
			version = 0;

			file.open(breathe::string::ToUTF8(filename).c_str(), std::ios::in | std::ios::binary);
			if (!file.is_open()) return false;

			file.read((char*)&version, sizeof(version));
			return true;
		}

		bool OpenWriteText(const string_t& filename, std::ofstream& file, uint16_t version)
		{
			version = 0;

			file.open(breathe::string::ToUTF8(filename).c_str(), std::ios::out);
			if (!file.is_open()) return false;
			
			file<<version;
			return true;
		}

		bool OpenWriteBinary(const string_t& filename, std::ofstream& file, uint16_t version)
		{
			version = 0;

			file.open(breathe::string::ToUTF8(filename).c_str(), std::ios::in | std::ios::binary);
			if (!file.is_open()) return false;

			file.write((char*)&version, sizeof(version));
			return true;
		}
	}
}
