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
#include <algorithm>

// Spitfire Includes
#include <spitfire/spitfire.h>
#include <spitfire/util/string.h>

#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/file.h>

#ifndef FIRESTARTER
#include <spitfire/util/log.h>
#include <spitfire/algorithm/md5.h>
#endif


#ifdef __WIN__
#include <windows.h>
#endif

namespace spitfire
{
  namespace storage
  {
    // From http://en.wikipedia.org/wiki/Byte-order_mark
    //
    // DETECTED:
    // UTF-8 EF BB BF
    // UTF-16 (big-endian) FE FF
    // UTF-16 (little-endian) FF FE
    // UTF-32 (big-endian) 00 00 FE FF
    // UTF-32 (little-endian) FF FE 00 00
    // SCSU (compression) 0E FE FF
    //
    // NOT DETECTED:
    // UTF-16BE, UTF-32BE (big-endian) With no BOM
    // UTF-16LE, UTF-32LE (little-endian) With no BOM
    // UTF-7  2B 2F 76 and one of the following bytes: [ 38 | 39 | 2B | 2F ]
    // UTF-EBCDIC DD 73 66 73
    // BOCU-1 FB EE 28

    BYTEORDER DetectByteOrderMark(const string_t& filename, size_t& bytes)
    {
      bytes = 0;

      std::ifstream file;
      file.open(spitfire::string::ToUTF8(filename).c_str(), std::ios::in | std::ios::binary);

      // Default to UTF8 which will be changed if it is not UTF8
      uint8_t signature[4] = { 0x88, 0x88, 0x88, 0x88 };
      file.read((char*)signature, 4);
      size_t count = file.gcount();
      file.close();

      if (count >= 4) {
        bytes = 4;
        //UTF-32 (big-endian) 00 00 FE FF
        if ((signature[0] == 0x00) && (signature[1] == 0x00) && (signature[2] == 0xFE) && (signature[3] == 0xFF))  {
          SCREEN<<"Error 32bit big endian file signature detected"<<std::endl;
          assert(false);
          return BYTEORDER::UTF32BE;
        }

        //UTF-32 (little-endian) FF FE 00 00
        if ((signature[0] == 0xFF) && (signature[1] == 0xFE) && (signature[2] == 0x00) && (signature[3] == 0x00)) {
          SCREEN<<"Error 32bit little endian file signature detected"<<std::endl;
          assert(false);
          return BYTEORDER::UTF32LE;
        }
      }

      if (count >= 3) {
        bytes = 3;
        //UTF-8 EF BB BF
        if ((signature[0] == 0xEF) && (signature[1] == 0xBB) && (signature[2] == 0xBF)) return BYTEORDER::UTF8;
        //SCSU (compression) 0E FE FF
        if ((signature[0] == 0x0E) && (signature[1] == 0xFE) && (signature[2] == 0xFF)) {
          SCREEN<<"Error unhandled file signature detected SCSU"<<std::endl;
          assert(false);
          return BYTEORDER::INVALID;
        }
      }

      if (count >= 2) {
        bytes = 2;
        //UTF-16 (big-endian) FE FF
        if ((signature[0] == 0xFE) && (signature[1] == 0xFF)) {
          SCREEN<<"Error big endian file signature detected"<<std::endl;
          assert(false);
          return BYTEORDER::UTF16BE;
        }
        //UTF-16 (little-endian) FF FE
        if ((signature[0] == 0xFF) && (signature[1] == 0xFE)) return BYTEORDER::UTF16LE;
      }

      bytes = 0;
      return BYTEORDER::UTF8;
    }

    // NOTE: This function does not actually convert between encodings properly,
    // we basically just assign and hope for the best
    template <class T>
    void ReadStringsFromLittleEndianFile(std::ifstream& file, std::vector<std::wstring>& contents)
    {
      const size_t bufferSize = 1024;
      T buffer[bufferSize];
      std::wostringstream o;
      while (file.good()) {
        //CONSOLE<<"ReadStringsFromFile Reading buffer"<<std::endl;
        file.read((char*)&buffer[0], bufferSize * sizeof(T));
        CONSOLE<<"ReadStringsFromFile Finding how many read characters"<<std::endl;
        const size_t n = file.gcount() / sizeof(T);

        //CONSOLE<<"ReadStringsFromFile Interpreting line"<<std::endl;
        for (size_t i = 0; i < n; i++) o<<wchar_t(buffer[i]);
      }

      //CONSOLE<<"ReadStringsFromFile Adding line to lines"<<std::endl;
      std::vector<std::wstring> lines;
      spitfire::string::SplitOnNewLines(o.str(), lines);

      //CONSOLE<<"ReadStringsFromFile Adding lines to contents"<<std::endl;
      const size_t n = lines.size();
      for (size_t i = 0; i < n; i++) {
        //CONSOLE<<"ReadStringsFromFile Adding line["<<i<<"] "<<lines[i]<<" to contents"<<std::endl;
        contents.push_back(lines[i]);
      }
    }

    void ReadText(const string_t& filename, std::vector<std::string>& contents)
    {
      contents.clear();

      std::vector<std::wstring> temp;
      ReadText(filename, temp);

      const size_t n = temp.size();
      contents.reserve(n);
      for (size_t i = 0; i < n; i++) contents.push_back(spitfire::string::ToUTF8(temp[i]));
    }

    void ReadText(const string_t& filename, std::vector<std::wstring>& contents)
    {
      CONSOLE<<"ReadText "<<filename<<std::endl;

      contents.clear();

      if (!filesystem::FileExists(filename)) {
        CONSOLE<<"ReadText File not found "<<filename<<std::endl;
        return;
      }

      // Get byte order and size
      size_t signature_bytes = 0;
      BYTEORDER byteOrder = DetectByteOrderMark(filename, signature_bytes);

      CONSOLE<<"ReadText ByteOrderMark Detected signature_bytes="<<signature_bytes<<std::endl;

      std::ifstream file;
      file.open(spitfire::string::ToUTF8(filename).c_str(), std::ios::in);

      if (!file.good()) {
        CONSOLE<<"ReadText File not opened"<<std::endl;
        return;
      }

      if (signature_bytes != 0) {
        CONSOLE<<"ReadText Seeking to "<<signature_bytes<<std::endl;
        // Seek to after the byte order mark
        file.seekg(signature_bytes, std::ios::beg);
      }

      CONSOLE<<"ReadText Reading from file"<<std::endl;
      if (BYTEORDER::UTF8 == byteOrder) ReadStringsFromLittleEndianFile<char>(file, contents);
      else if (BYTEORDER::UTF16LE == byteOrder) ReadStringsFromLittleEndianFile<char16_t>(file, contents);
      else if (BYTEORDER::UTF32LE == byteOrder) {
          std::vector<std::wstring> lines;
          ReadStringsFromLittleEndianFile<char32_t>(file, lines);
      } else {
        CONSOLE<<"ReadText ByteOrderMark Not handled signature_bytes="<<signature_bytes<<std::endl;
        assert(false);

        // Swap
        //size_t i = 0;
        //for (;i<count; i++) buffer[i] = (buffer[i]<<8)|(buffer[i]>>8);

        //UTF-16 (big-endian) FE FF
        //UTF-16BE, UTF-32BE (big-endian) No BOM!
        //UTF-32 (big-endian) 00 00 FE FF
        //SCSU (compression) 0E FE FF
      }

      // Close the file
      file.close();

      CONSOLE<<"ReadText returning"<<std::endl;
    }


    void AppendText(const string_t& filename, const std::string& contents)
    {
      //size_t signature_bytes = 0;
      //BYTEORDER byteOrder = DetectByteOrderMark(filename, signature_bytes);

      std::ofstream file;
      file.open(spitfire::string::ToUTF8(filename).c_str(), std::ios::out | std::ios::app);

      file<<contents;
      /*if (BYTEORDER::UTF8 == byteOrder)
      {

      }*/

      file.close();
    }

    void AppendText(const string_t& filename, const std::wstring& contents)
    {
      AppendText(filename, spitfire::string::ToUTF8(contents));
    }


    // ** cReadFile

    cReadFile::cReadFile()
    {
    }

    cReadFile::cReadFile(const string_t& sFilePath)
    {
      Open(sFilePath);
    }

    cReadFile::cReadFile(const string_t& sFilePath, uint32_t uTimeoutMS)
    {
      Open(sFilePath, uTimeoutMS);
    }

    bool cReadFile::Open(const string_t& sFilePath)
    {
      file.open(spitfire::string::ToUTF8(sFilePath).c_str(), std::ios::in | std::ios::binary);
      return file.is_open();
    }

    bool cReadFile::Open(const string_t& sFilePath, uint32_t uTimeoutMS)
    {
      // TODO: Implement this function
      return false;
    }

    void cReadFile::Close()
    {
      file.close();
    }

    size_t cReadFile::Read(void* pBuffer, size_t uiBufferSizeBytes)
    {
      file.read(static_cast<char*>(pBuffer), uiBufferSizeBytes);
      return file.gcount();
    }

    bool cReadFile::ReadLine(std::string& sLine)
    {
      std::cout<<"cReadFile::ReadLine"<<std::endl;

      sLine.clear();

      std::ostringstream o;
      bool bFoundNewLine = false;
      char c;
      while (file.good() && !bFoundNewLine) {
        file.read(&c, sizeof(c));
        const size_t nRead = file.gcount();
        if (nRead == 0) break; // End of file

        if ((c == '\r') || (c == '\n')) {
          if (c == '\r') {
            if (file.peek() == '\n') file.read(&c, sizeof(c));
          }

          bFoundNewLine = true;
          break;
        }

        if (!bFoundNewLine) {
          // Add the string to our output buffer
          o<<c;
        }
      }

      sLine = o.str();

      std::cout<<"cReadFile::ReadLine Finished reading line \""<<sLine<<"\", returning "<<bFoundNewLine<<std::endl;
      return bFoundNewLine;
    }

    bool cReadFile::ReadLine(std::wstring& sLine)
    {
      std::cout<<"cReadFile::ReadLine"<<std::endl;

      sLine.clear();

      std::wostringstream o;
      bool bFoundNewLine = false;
      wchar_t c;
      while (file.good() && !bFoundNewLine) {
        file.read((char*)&c, sizeof(c));
        const size_t nRead = file.gcount();
        if (nRead == 0) break; // End of file

        if ((c == L'\r') || (c == L'\n')) {
          if (c == L'\r') {
            if (file.peek() == L'\n') file.read((char*)&c, sizeof(c));
          }

          bFoundNewLine = true;
          break;
        }

        if (!bFoundNewLine) {
          // Add the string to our output buffer
          o<<c;
        }
      }

      sLine = o.str();

      std::wcout<<"cReadFile::ReadLine Finished reading line \""<<sLine<<"\", returning "<<bFoundNewLine<<std::endl;
      return bFoundNewLine;
    }

    bool cReadFile::ReadLineUTF8(std::string& sLine)
    {
      return ReadLine(sLine);
    }

    bool cReadFile::ReadLineUTF8(std::wstring& sLine)
    {
      std::string sLineUTF8;
      bool bResult = ReadLine(sLineUTF8);
      sLine = string::ToWchar_t(sLineUTF8);
      return bResult;
    }


    // ** cWriteFile

    cWriteFile::cWriteFile()
    {
    }

    cWriteFile::cWriteFile(const string_t& sFilePath)
    {
      Open(sFilePath);
    }

    cWriteFile::cWriteFile(const string_t& sFilePath, uint32_t uTimeoutMS)
    {
      Open(sFilePath, uTimeoutMS);
    }

    bool cWriteFile::Open(const string_t& sFilePath)
    {
      file.open(spitfire::string::ToUTF8(sFilePath).c_str(), std::ios::out | std::ios::binary);
      return file.is_open();
    }

    bool cWriteFile::Open(const string_t& sFilePath, uint32_t uTimeoutMS)
    {
      // TODO: Implement this function
      return false;
    }

    void cWriteFile::Close()
    {
      file.close();
    }

    size_t cWriteFile::Write(const void* pBuffer, size_t uiBufferSizeBytes)
    {
      file.write(static_cast<const char*>(pBuffer), uiBufferSizeBytes);
      return true;
    }

    bool cWriteFile::WriteStringUTF8(const string_t& str)
    {
      const std::string sStringUTF8 = string::ToUTF8(str);
      file.write(static_cast<const char*>(sStringUTF8.c_str()), sStringUTF8.length());
      return true;
    }
  }
}
