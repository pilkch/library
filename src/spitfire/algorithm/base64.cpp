#include <cassert>
#include <cctype> // for toupper/tolower

#include <string>
#include <sstream>
#include <iterator>    // for back_inserter

#include <iostream>
#include <fstream>
#include <iomanip>

#include <spitfire/spitfire.h>

#include <spitfire/algorithm/base64.h>

namespace spitfire
{
  namespace algorithm
  {
    const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/"
    ;

    inline bool IsBase64Character(unsigned char c)
    {
      return (isalnum(c) || (c == '+') || (c == '/'));
    }

    std::string Base64Encode(const void* _pBuffer, size_t len)
    {
      const unsigned char* pBuffer = static_cast<const unsigned char*>(_pBuffer);

      std::string ret;

      size_t i = 0;
      unsigned char char_array_3[3] = { 0, 0, 0 };
      unsigned char char_array_4[4] = { 0, 0, 0, 0 };

      while (len--) {
        char_array_3[i++] = *(pBuffer++);

        if (i == 3) {
          char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
          char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
          char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
          char_array_4[3] = char_array_3[2] & 0x3f;

          for (i = 0; (i < 4) ; i++) ret += base64_chars[char_array_4[i]];

          i = 0;
        }
      }

      if (i != 0) {
        for (size_t j = i; j < 3; j++) char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (size_t j = 0; (j < i + 1); j++) ret += base64_chars[char_array_4[j]];

        while (i++ < 3) {
          ret += '=';
        }
      }

      return ret;
    }

    std::string Base64Encode(const std::string& sText)
    {
      return Base64Encode(static_cast<const void*>(sText.c_str()), sText.length());
    }

    std::string Base64Decode(const std::string& sText)
    {
      std::string ret;

      size_t len = sText.size();
      size_t i = 0;
      size_t k = 0;
      unsigned char char_array_3[3] = { 0, 0, 0 };
      unsigned char char_array_4[4] = { 0, 0, 0, 0 };

      while (len-- && (sText[k] != '=') && IsBase64Character(sText[k])) {
        char_array_4[i++] = sText[k];
        k++;

        if (i == 4) {
          for (i = 0; i < 4; i++) char_array_4[i] = base64_chars.find(char_array_4[i]);

          char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
          char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
          char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

          for (i = 0; (i < 3); i++) ret += char_array_3[i];

          i = 0;
        }
      }

      if (i) {
        for (size_t j = i; j < 4; j++) char_array_4[j] = 0;

        for (size_t j = 0; j < 4; j++) char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (size_t j = 0; (j < i - 1); j++) ret += char_array_3[j];
      }

      return ret;
    }
  }
}
