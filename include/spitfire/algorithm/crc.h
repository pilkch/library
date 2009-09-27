#ifndef CCRC_H
#define CCRC_H

namespace spitfire
{
  class cCRC
  {
  public:
    cCRC() { SetTypeCRC32(); }

    void SetTypeCRC16() { bIsCRC32 = false; sResult.clear(); }
    void SetTypeCRC32() { bIsCRC32 = true; sResult.clear(); }

    bool CalculateCRC16ForString(char* szString);
    bool CalculateCRC16ForBuffer(char* pBuffer, size_t len);
    bool CalculateCRC16ForFile(const string_t& sFilename);

    bool CalculateCRC32ForString(char* szString);
    bool CalculateCRC32ForBuffer(char* pBuffer, size_t len);
    bool CalculateCRC32ForFile(const string_t& sFilename);

    bool operator==(const cCRC & rhs) const { return sResult == rhs.sResult; }
    bool operator!=(const cCRC & rhs) const { return sResult != rhs.sResult; }

    bool operator==(const string_t& rhs) const { return sResult == rhs; }
    bool operator!=(const string_t& rhs) const { return sResult != rhs; }

    string_t GetResult() const;
    string_t GetResultFormatted() const;

  private:
    bool bIsCRC32;
    string_t sResult;
  };
}
#endif // CCRC_H
