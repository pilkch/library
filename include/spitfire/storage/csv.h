#ifndef SPITFIRE_STORAGE_CSV_H
#define SPITFIRE_STORAGE_CSV_H

// Standard headers
#include <iostream>

// Spitfire headers
#include <spitfire/storage/file.h>

namespace spitfire
{
  namespace csv
  {
    // ** cReader

    class cReader
    {
    public:
      bool Open(const string_t& sFilePath);

      bool ReadLine(std::vector<string_t>& values);

    private:
      storage::cReadTextFile file;
    };


    // ** cWriter

    class cWriter
    {
    public:
      cWriter();

      bool Open(const string_t& sFilePath);

      void AddValue(const string_t& sValue);
      void EndRow();

    private:
      std::ofstream o;
      bool bIsFirstValue;
    };
  }
}

#endif // !SPITFIRE_STORAGE_CSV_H
