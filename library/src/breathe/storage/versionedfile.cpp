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
		bool OpenReadText(const std::string& sFilename, std::ifstream& file, uint16_t& version)
		{
			version = 0;

			file.open(sFilename.c_str(), std::ios::in);
			if (!file.is_open()) return false;

			file>>version;
			return true;
		}
		
		bool OpenReadBinary(const std::string& sFilename, std::ifstream& file, uint16_t& version)
		{
			version = 0;

			file.open(sFilename.c_str(), std::ios::in | std::ios::binary);
			if (!file.is_open()) return false;

			file.read((char*)&version, sizeof(version));
			return true;
		}

		bool OpenWriteText(const std::string& sFilename, std::ofstream& file, uint16_t version)
		{
			version = 0;

			file.open(sFilename.c_str(), std::ios::out);
			if (!file.is_open()) return false;
			
			file<<version;
			return true;
		}

		bool OpenWriteBinary(const std::string& sFilename, std::ofstream& file, uint16_t version)
		{
			version = 0;

			file.open(sFilename.c_str(), std::ios::in | std::ios::binary);
			if (!file.is_open()) return false;

			file.write((char*)&version, sizeof(version));
			return true;
		}
	}
}
