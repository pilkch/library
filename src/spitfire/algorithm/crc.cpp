// Standard headers
#include <array>
#include <iostream>
#include <fstream>

// Boost headers
#include <boost/crc.hpp>

// Spitfire headers
#include <spitfire/algorithm/crc.h>

namespace spitfire
{
  // Redefine this to change to processing buffer size
  const std::streamsize CRC_BUFFER_SIZE = 1024;


  template <class T, typename I>
  bool CalculateCRCForBuffer(const char* pBuffer, size_t len, string_t& sResult)
  {
    sResult.clear();

    T result;

    result.process_bytes(pBuffer, len);

    const I crc = result.checksum();
    sResult = "0x" + spitfire::string::ToHexString(crc);
    std::cout<<"CalculateCRCForBuffer result="<<sResult<<std::endl;

    return true;
  }

  template <class T, typename I>
  bool CalculateCRCForString(const string_t& sString, string_t& sResult)
  {
    sResult.clear();

    T result;

    result.process_bytes(static_cast<const void*>(sString.c_str()), sString.length());

    const I crc = result.checksum();
    sResult = "0x" + spitfire::string::ToHexString(crc);
    std::cout<<"CalculateCRCForString result="<<sResult<<std::endl;

    return true;
  }

  template <class T, typename I>
  bool CalculateCRCForFile(const string_t& sFilename, string_t& sResult)
  {
    sResult.clear();

    std::ifstream ifs(sFilename, std::ios_base::binary);
    if (!ifs.is_open()) {
      std::cerr<<"CalculateCRCForFile ifstream FAILED opening file '"<<sFilename<<"'."<< std::endl;
      return false;
    }

    T result;

    std::array<char, CRC_BUFFER_SIZE> buffer;

    do {
      ifs.read(buffer.data(), CRC_BUFFER_SIZE);
      result.process_bytes(buffer.data(), ifs.gcount());
    } while (ifs);

    const I crc = result.checksum();
    sResult = "0x" + spitfire::string::ToHexString(crc);
    std::cout<<"CalculateCRCForFile result="<<sResult<<std::endl;
    return true;
  }


  // ** cCRC16

  bool cCRC16::CalculateForString(const char* szString)
  {
    return CalculateCRCForString<boost::crc_16_type, uint16_t>(szString, sResult);
  }

  bool cCRC16::CalculateForBuffer(const char* pBuffer, size_t len)
  {
    return CalculateCRCForBuffer<boost::crc_16_type, uint16_t>(pBuffer, len, sResult);
  }

  bool cCRC16::CalculateForFile(const string_t& sFilename)
  {
    return CalculateCRCForFile<boost::crc_16_type, uint16_t>(sFilename, sResult);
  }

  string_t cCRC16::GetResult() const
  {
    return sResult;
  }


  // ** cCRC32

  bool cCRC32::CalculateForString(const char* szString)
  {
    return CalculateCRCForString<boost::crc_32_type, uint32_t>(szString, sResult);
  }

  bool cCRC32::CalculateForBuffer(const char* pBuffer, size_t len)
  {
    return CalculateCRCForBuffer<boost::crc_32_type, uint32_t>(pBuffer, len, sResult);
  }

  bool cCRC32::CalculateForFile(const string_t& sFilename)
  {
    return CalculateCRCForFile<boost::crc_32_type, uint32_t>(sFilename, sResult);
  }

  string_t cCRC32::GetResult() const
  {
    return sResult;
  }
}


#ifdef BUILD_SPITFIRE_UNITTEST

#include <spitfire/util/unittest.h>

class cCRCUnitTest : protected spitfire::util::cUnitTestBase
{
public:
  cCRCUnitTest() :
    cUnitTestBase(TEXT("cCRCUnitTest"))
  {
  }

  template<class T>
  void TestCRC(const std::string sText, const std::string& sExpectedResult)
  {
    T crc;
    crc.CalculateForString(sText.c_str());

    // Make sure that the text encodes as expected
    std::cout<<"TestCRC result="<<crc.GetResult()<<std::endl;
    ASSERT(crc.GetResult() == sExpectedResult);
  }

  void Test()
  {
    TestCRC<spitfire::cCRC16>("abcdefghijklmnopqrstuvwxyz", "0x9C1D");
    TestCRC<spitfire::cCRC32>("abcdefghijklmnopqrstuvwxyz", "0x4C2750BD");
  }
};

cCRCUnitTest gCRCUnitTest;

#endif // BUILD_SPITFIRE_UNITTEST
