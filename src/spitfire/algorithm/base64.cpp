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

    std::string Base64Encode(const unsigned char* pBuffer, unsigned int len)
    {
      std::string ret;

      int i = 0;
      unsigned char char_array_3[3] = { 0, 0, 0 };
      unsigned char char_array_4[4] = { 0, 0, 0, 0 };

      while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);

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
        for (int j = i; j < 3; j++) char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (int j = 0; (j < i + 1); j++) ret += base64_chars[char_array_4[j]];

        while(i++ < 3) {
          ret += '=';
        }
      }

      return ret;
    }

    std::string Base64Decode(const std::string& sText)
    {
      std::string ret;

      int len = encoded_string.size();
      int i = 0;
      int k = 0;
      unsigned char char_array_3[3] = { 0, 0, 0 };
      unsigned char char_array_4[4] = { 0, 0, 0, 0 };

      while (len-- && (encoded_string[k] != '=') && IsBase64Character(encoded_string[k])) {
        char_array_4[i++] = encoded_string[k];
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
        for (int j = i; j < 4; j++) char_array_4[j] = 0;

        for (int j = 0; j < 4; j++) char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (int j = 0; (j < i - 1); j++) ret += char_array_3[j];
      }

      return ret;
    }
  }
}


#ifdef BUILD_DEBUG
#include <spitfire/util/log.h>
#include <spitfire/util/unittest.h>

class cStringUnitTest : protected spitfire::util::cUnitTestBase
{
public:
  cStringUnitTest() :
    cUnitTestBase(TEXT("cStringUnitTest"))
  {
  }
  void TestString(const std::string sText, const std::string& sEncoded)
  {
    const std::string sResult = spitfire::util::Base64Encode(sText);

    // Make sure that the text encodes as expected
    ASSERT(sResult == sEncoded);

    const std::string sDecoded = spitfire::util::Base64Decode(sResult);

    // Make sure that the encoded string decodes back to the original text as expected
    ASSERT(sText == sDecoded);
  }

  void Test()
  {
    TestString("");
    TestString("chris");
    TestString("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
    TestBuffer();
  }
};

cStringUnitTest gStringUnitTest;
#endif // BUILD_DEBUG
