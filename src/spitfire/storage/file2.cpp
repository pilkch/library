// Note: This file is not named file.cpp because of a stupid bug in Visual Studio 2003
// When we upgrade to 2005+ rename it to file.cpp

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
#include <spitfire/util/cString.h>

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
    const string_t& sLINE_ENDING_UNIX_AND_LINUX = sLINE_ENDING_LF;
    const string_t& sLINE_ENDING_WINDOWS = sLINE_ENDING_CRLF;

#ifdef PLATFORM_LINUX_OR_UNIX
    const string_t& sLINE_ENDING_NATIVE = sLINE_ENDING_UNIX_AND_LINUX;
#else
    const string_t& sLINE_ENDING_NATIVE = sLINE_ENDING_WINDOWS;
#endif

    /*
      From http://en.wikipedia.org/wiki/Byte-order_mark

      DETECTED:
      UTF-8 EF BB BF
      UTF-16 (big-endian) FE FF
      UTF-16 (little-endian) FF FE
      UTF-32 (big-endian) 00 00 FE FF
      UTF-32 (little-endian) FF FE 00 00
      SCSU (compression) 0E FE FF

      NOT DETECTED:
      UTF-16BE, UTF-32BE (big-endian) With no BOM
      UTF-16LE, UTF-32LE (little-endian) With no BOM
      UTF-7  2B 2F 76 and one of the following bytes: [ 38 | 39 | 2B | 2F ]
      UTF-EBCDIC DD 73 66 73
      BOCU-1 FB EE 28
    */

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
          return BYTEORDER_UTF32BE;
        }

        //UTF-32 (little-endian) FF FE 00 00
        if ((signature[0] == 0xFF) && (signature[1] == 0xFE) && (signature[2] == 0x00) && (signature[3] == 0x00)) {
          SCREEN<<"Error 32bit little endian file signature detected"<<std::endl;
          assert(false);
          return BYTEORDER_UTF32LE;
        }
      }

      if (count >= 3) {
        bytes = 3;
        //UTF-8 EF BB BF
        if ((signature[0] == 0xEF) && (signature[1] == 0xBB) && (signature[2] == 0xBF)) return BYTEORDER_UTF8;
        //SCSU (compression) 0E FE FF
        if ((signature[0] == 0x0E) && (signature[1] == 0xFE) && (signature[2] == 0xFF)) {
          SCREEN<<"Error unhandled file signature detected SCSU"<<std::endl;
          assert(false);
          return BYTEORDER_INVALID;
        }
      }

      if (count >= 2) {
        bytes = 2;
        //UTF-16 (big-endian) FE FF
        if ((signature[0] == 0xFE) && (signature[1] == 0xFF)) {
          SCREEN<<"Error big endian file signature detected"<<std::endl;
          assert(false);
          return BYTEORDER_UTF16BE;
        }
        //UTF-16 (little-endian) FF FE
        if ((signature[0] == 0xFF) && (signature[1] == 0xFE)) return BYTEORDER_UTF16LE;
      }

      bytes = 0;
      return BYTEORDER_UTF8;
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
        CONSOLE<<"ReadStringsFromFile Reading buffer"<<std::endl;
        file.read((char*)&buffer[0], bufferSize * sizeof(T));
        CONSOLE<<"ReadStringsFromFile Finding how many read characters"<<std::endl;
        const size_t n = file.gcount() / sizeof(T);

        CONSOLE<<"ReadStringsFromFile Interpreting line"<<std::endl;
        for (size_t i = 0; i < n; i++) o<<wchar_t(buffer[i]);
      }

      CONSOLE<<"ReadStringsFromFile Adding line to lines"<<std::endl;
      std::vector<std::wstring> lines;
      spitfire::string::SplitOnNewLines(o.str(), lines);

      CONSOLE<<"ReadStringsFromFile Adding lines to contents"<<std::endl;
      const size_t n = lines.size();
      for (size_t i = 0; i < n; i++) {
        CONSOLE<<"ReadStringsFromFile Adding line["<<i<<"] "<<lines[i]<<" to contents"<<std::endl;
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
      if (BYTEORDER_UTF8 == byteOrder) ReadStringsFromLittleEndianFile<char>(file, contents);
      else if (BYTEORDER_UTF16LE == byteOrder) ReadStringsFromLittleEndianFile<char16_t>(file, contents);
      else if (BYTEORDER_UTF32LE == byteOrder) {
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
      /*if (BYTEORDER_UTF8 == byteOrder)
      {

      }*/

      file.close();
    }

    void AppendText(const string_t& filename, const std::wstring& contents)
    {
      AppendText(filename, spitfire::string::ToUTF8(contents));
    }
  }
}
