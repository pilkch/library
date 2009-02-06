
#include <boost/crc.hpp>  // for boost::crc_32_type

// Redefine this to change to processing buffer size
#ifndef PRIVATE_BUFFER_SIZE
#define PRIVATE_BUFFER_SIZE  1024
#endif

#include <spitfire/include/algorithm/crc.h>

namespace spitfire
{
  // Global objects
  const std::streamsize buffer_size = PRIVATE_BUFFER_SIZE;


  template <class T>
  bool CalculateCRCForBuffer(const char* pBuffer, size_t len, string_t& sResult)
  {
    sResult.clear();

    T result;

    result.process_bytes(pBuffer, len);

    sResult = result.checksum();
    std::cout<<std::hex<<std::uppercase<<sResult<<std::endl;

    return true;
  }

  template <class T>
  bool CalculateCRCForString(const string_t& sString, string_t& sResult)
  {
    sResult.clear();

    T result;

    result.process_bytes(sString, sString.length());

    sResult = result.checksum();
    std::cout<<std::hex<<std::uppercase<<sResult<<std::endl;

    return true;
  }

  template <class T>
  bool CalculateCRCForFile(const string_t& sFilename, string_t& sResult)
  {
    sResult.clear();

    std::ifstream ifs(sFilename, std::ios_base::binary);
    if (ifs.is_open()) {
      T result;

      char buffer[buffer_size];

      do {
        ifs.read(buffer, buffer_size);
        result.process_bytes(buffer, ifs.gcount());
      } while (ifs);

      sResult = result.checksum();
      std::cout<<std::hex<<std::uppercase<<sResult<<std::endl;
    }

    std::cerr<<"cCRC::CalculateCRC32ForFile ifstream FAILED opening file '"<<sFilename<<"'."<< std::endl;
    return false;
  }


  bool cCRC::CalculateCRC16ForString(char* szString)
  {
    bIsCRC32 = false;
    return CalculateCRCForString<crc_16_type>(szString, sResult);
  }

  bool cCRC::CalculateCRC16ForBuffer(char* pBuffer, size_t len)
  {
    bIsCRC32 = false;
    return CalculateCRCForBuffer<crc_16_type>(pBuffer, len, sResult);
  }

  bool cCRC::CalculateCRC16ForFile(const string_t& sFilename)
  {
    bIsCRC32 = false;
    return CalculateCRCForFile<boost::crc_16_type>(sFilename, sResult);
  }


  bool cCRC::CalculateCRC32ForString(char* szString)
  {
    bIsCRC32 = true;
    return CalculateCRCForString<crc_32_type>(szString, sResult);
  }

  bool cCRC::CalculateCRC32ForBuffer(char* pBuffer, size_t len)
  {
    bIsCRC32 = true;
    return CalculateCRCForBuffer<crc_32_type>(pBuffer, len, sResult);
  }

  bool cCRC::CalculateCRC32ForFile(const string_t& sFilename)
  {
    bIsCRC32 = true;
    return CalculateCRCForFile<boost::crc_32_type>(sFilename, sResult);
  }
}
