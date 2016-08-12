#ifndef CCRC_H
#define CCRC_H

// Spitfire headers
#include <spitfire/util/string.h>

namespace spitfire
{
  class cCRC16
  {
  public:
    bool CalculateForString(const char* szString);
    bool CalculateForBuffer(const char* pBuffer, size_t len);
    bool CalculateForFile(const string_t& sFilename);

    bool operator==(const cCRC16 & rhs) const { return sResult == rhs.sResult; }
    bool operator!=(const cCRC16 & rhs) const { return sResult != rhs.sResult; }

    bool operator==(const string_t& rhs) const { return sResult == rhs; }
    bool operator!=(const string_t& rhs) const { return sResult != rhs; }

    string_t GetResult() const;
    string_t GetResultFormatted() const;

  private:
    string_t sResult;
  };


  class cCRC32
  {
  public:
    bool CalculateForString(const char* szString);
    bool CalculateForBuffer(const char* pBuffer, size_t len);
    bool CalculateForFile(const string_t& sFilename);

    bool operator==(const cCRC32 & rhs) const { return sResult == rhs.sResult; }
    bool operator!=(const cCRC32 & rhs) const { return sResult != rhs.sResult; }

    bool operator==(const string_t& rhs) const { return sResult == rhs; }
    bool operator!=(const string_t& rhs) const { return sResult != rhs; }

    string_t GetResult() const;
    string_t GetResultFormatted() const;

  private:
    string_t sResult;
  };
}
#endif // CCRC_H
