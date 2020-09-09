#ifndef CCRC_H
#define CCRC_H

// Spitfire headers
#include <spitfire/util/string.h>

namespace spitfire
{
  // NOTE: Implements CRC-16-CCITT (poly 0x1021)
  class cCRC16
  {
  public:
    cCRC16();

    void ProcessBytes(const char* data, size_t data_length);
  
    string_t GetChecksum() const; // Returns something like "9C1D"
  
    static bool CalculateForString(const char* szString, string_t& result);
    static bool CalculateForBuffer(const char* pBuffer, size_t len, string_t& result);
    static bool CalculateForFile(const string_t& sFilename, string_t& result);

  private:
    uint16_t digest;
  };


  class cCRC32
  {
  public:
    cCRC32();

    void ProcessBytes(const char* data, size_t data_length);

    string_t GetChecksum() const; // Returns something like "A8FC45B2"

    static bool CalculateForString(const char* szString, string_t& result);
    static bool CalculateForBuffer(const char* pBuffer, size_t len, string_t& result);
    static bool CalculateForFile(const string_t& sFilename, string_t& result);

  private:
  	void GenerateTable();

    uint32_t table[256];
    uint32_t digest;
  };
}

#endif // CCRC_H
