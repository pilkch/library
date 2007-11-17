#ifndef VERSIONEDFILE_H
#define VERSIONEDFILE_H

namespace breathe
{
	namespace storage
	{
		bool OpenReadText(const std::string& sFilename, std::ifstream& file, uint16_t& version);
		bool OpenReadBinary(const std::string& sFilename, std::ifstream& file, uint16_t& version);

		bool OpenWriteText(const std::string& sFilename, std::ofstream& file, uint16_t version);
		bool OpenWriteBinary(const std::string& sFilename, std::ofstream& file, uint16_t version);
	}
}

#endif //VERSIONEDFILE_H