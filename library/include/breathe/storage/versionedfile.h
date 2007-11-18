#ifndef VERSIONEDFILE_H
#define VERSIONEDFILE_H

namespace breathe
{
	namespace storage
	{
		bool OpenReadText(const string_t& filename, std::ifstream& file, uint16_t& version);
		bool OpenReadBinary(const string_t& filename, std::ifstream& file, uint16_t& version);

		bool OpenWriteText(const string_t& filename, std::ofstream& file, uint16_t version);
		bool OpenWriteBinary(const string_t& filename, std::ofstream& file, uint16_t version);
	}
}

#endif //VERSIONEDFILE_H