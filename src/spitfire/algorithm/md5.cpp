/*
 *  RFC 1321 compliant MD5 implementation
 *
 *  Incorporated into:
 *  Various GPL projects by Christopher Pilkington
 *
 *
 *  Copyright (C) 2001-2003  Christophe Devine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <cstdio>
#include <cstring>

// Writing to and from a text file
#include <iostream>
#include <fstream>

#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/algorithm/md5.h>

#ifdef UNICODE
FILE* fopen(const wchar_t* szFilePath, const wchar_t* szMode)
{
  FILE* file = nullptr;
  _wfopen_s(&file, szFilePath, szMode);
  return file;
}
#endif

namespace spitfire
{
  namespace algorithm
  {
  #define GET_UINT32(n,b,i)                             \
  {                                                     \
    (n) = ( (uint32_t) (b)[(i)    ]       )             \
        | ( (uint32_t) (b)[(i) + 1] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 2] << 16 )             \
        | ( (uint32_t) (b)[(i) + 3] << 24 );            \
  }

  #define PUT_UINT32(n,b,i)                             \
  {                                                     \
    (b)[(i)    ] = (unsigned char) ( (n)       );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 3] = (unsigned char) ( (n) >> 24 );       \
  }

  static unsigned char md5_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };


  // *** cMD5_Context

  class cMD5_Context
  {
  public:
    void Start();
    void Update(const unsigned char* pInput, uint32_t length);
    void Finish(unsigned char* pResult, std::string& sResult);
    void Process(const unsigned char data[64]);

    uint32_t total[2];
    uint32_t state[4];
    unsigned char buffer[64];
  };

  void cMD5_Context::Start()
  {
    total[0] = 0;
    total[1] = 0;

    state[0] = 0x67452301;
    state[1] = 0xEFCDAB89;
    state[2] = 0x98BADCFE;
    state[3] = 0x10325476;
  }

  void cMD5_Context::Process(const unsigned char data[64])
  {
    uint32_t X[16];
    uint32_t A = 0;
    uint32_t B = 0;
    uint32_t C = 0;
    uint32_t D = 0;

    GET_UINT32(X[0],  data,  0);
    GET_UINT32(X[1],  data,  4);
    GET_UINT32(X[2],  data,  8);
    GET_UINT32(X[3],  data, 12);
    GET_UINT32(X[4],  data, 16);
    GET_UINT32(X[5],  data, 20);
    GET_UINT32(X[6],  data, 24);
    GET_UINT32(X[7],  data, 28);
    GET_UINT32(X[8],  data, 32);
    GET_UINT32(X[9],  data, 36);
    GET_UINT32(X[10], data, 40);
    GET_UINT32(X[11], data, 44);
    GET_UINT32(X[12], data, 48);
    GET_UINT32(X[13], data, 52);
    GET_UINT32(X[14], data, 56);
    GET_UINT32(X[15], data, 60);

    #define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

    #define P(a,b,c,d,k,s,t)                                \
    {                                                       \
         a += F(b,c,d) + X[k] + t; a = S(a,s) + b;           \
    }

    A = state[0];
    B = state[1];
    C = state[2];
    D = state[3];

    #define F(x,y,z) (z ^ (x & (y ^ z)))

    P( A, B, C, D,  0,  7, 0xD76AA478 );
    P( D, A, B, C,  1, 12, 0xE8C7B756 );
    P( C, D, A, B,  2, 17, 0x242070DB );
    P( B, C, D, A,  3, 22, 0xC1BDCEEE );
    P( A, B, C, D,  4,  7, 0xF57C0FAF );
    P( D, A, B, C,  5, 12, 0x4787C62A );
    P( C, D, A, B,  6, 17, 0xA8304613 );
    P( B, C, D, A,  7, 22, 0xFD469501 );
    P( A, B, C, D,  8,  7, 0x698098D8 );
    P( D, A, B, C,  9, 12, 0x8B44F7AF );
    P( C, D, A, B, 10, 17, 0xFFFF5BB1 );
    P( B, C, D, A, 11, 22, 0x895CD7BE );
    P( A, B, C, D, 12,  7, 0x6B901122 );
    P( D, A, B, C, 13, 12, 0xFD987193 );
    P( C, D, A, B, 14, 17, 0xA679438E );
    P( B, C, D, A, 15, 22, 0x49B40821 );

    #undef F

    #define F(x,y,z) (y ^ (z & (x ^ y)))

    P( A, B, C, D,  1,  5, 0xF61E2562 );
    P( D, A, B, C,  6,  9, 0xC040B340 );
    P( C, D, A, B, 11, 14, 0x265E5A51 );
    P( B, C, D, A,  0, 20, 0xE9B6C7AA );
    P( A, B, C, D,  5,  5, 0xD62F105D );
    P( D, A, B, C, 10,  9, 0x02441453 );
    P( C, D, A, B, 15, 14, 0xD8A1E681 );
    P( B, C, D, A,  4, 20, 0xE7D3FBC8 );
    P( A, B, C, D,  9,  5, 0x21E1CDE6 );
    P( D, A, B, C, 14,  9, 0xC33707D6 );
    P( C, D, A, B,  3, 14, 0xF4D50D87 );
    P( B, C, D, A,  8, 20, 0x455A14ED );
    P( A, B, C, D, 13,  5, 0xA9E3E905 );
    P( D, A, B, C,  2,  9, 0xFCEFA3F8 );
    P( C, D, A, B,  7, 14, 0x676F02D9 );
    P( B, C, D, A, 12, 20, 0x8D2A4C8A );

    #undef F

    #define F(x,y,z) (x ^ y ^ z)

    P( A, B, C, D,  5,  4, 0xFFFA3942 );
    P( D, A, B, C,  8, 11, 0x8771F681 );
    P( C, D, A, B, 11, 16, 0x6D9D6122 );
    P( B, C, D, A, 14, 23, 0xFDE5380C );
    P( A, B, C, D,  1,  4, 0xA4BEEA44 );
    P( D, A, B, C,  4, 11, 0x4BDECFA9 );
    P( C, D, A, B,  7, 16, 0xF6BB4B60 );
    P( B, C, D, A, 10, 23, 0xBEBFBC70 );
    P( A, B, C, D, 13,  4, 0x289B7EC6 );
    P( D, A, B, C,  0, 11, 0xEAA127FA );
    P( C, D, A, B,  3, 16, 0xD4EF3085 );
    P( B, C, D, A,  6, 23, 0x04881D05 );
    P( A, B, C, D,  9,  4, 0xD9D4D039 );
    P( D, A, B, C, 12, 11, 0xE6DB99E5 );
    P( C, D, A, B, 15, 16, 0x1FA27CF8 );
    P( B, C, D, A,  2, 23, 0xC4AC5665 );

    #undef F

    #define F(x,y,z) (y ^ (x | ~z))

    P( A, B, C, D,  0,  6, 0xF4292244 );
    P( D, A, B, C,  7, 10, 0x432AFF97 );
    P( C, D, A, B, 14, 15, 0xAB9423A7 );
    P( B, C, D, A,  5, 21, 0xFC93A039 );
    P( A, B, C, D, 12,  6, 0x655B59C3 );
    P( D, A, B, C,  3, 10, 0x8F0CCC92 );
    P( C, D, A, B, 10, 15, 0xFFEFF47D );
    P( B, C, D, A,  1, 21, 0x85845DD1 );
    P( A, B, C, D,  8,  6, 0x6FA87E4F );
    P( D, A, B, C, 15, 10, 0xFE2CE6E0 );
    P( C, D, A, B,  6, 15, 0xA3014314 );
    P( B, C, D, A, 13, 21, 0x4E0811A1 );
    P( A, B, C, D,  4,  6, 0xF7537E82 );
    P( D, A, B, C, 11, 10, 0xBD3AF235 );
    P( C, D, A, B,  2, 15, 0x2AD7D2BB );
    P( B, C, D, A,  9, 21, 0xEB86D391 );

    #undef F

    state[0] += A;
    state[1] += B;
    state[2] += C;
    state[3] += D;
  }

  void cMD5_Context::Update(const unsigned char* pInput, uint32_t length)
  {
    if (length == 0) return;

    uint32_t left = total[0] & 0x3F;
    uint32_t fill = 64 - left;

    total[0] += length;
    total[0] &= 0xFFFFFFFF;

    if (total[0] < length) total[1]++;

    if ((left != 0) && (length >= fill)) {
      std::memcpy((void*)(buffer + left), (void*)pInput, fill);
      Process(buffer);
      length -= fill;
      pInput  += fill;
      left = 0;
    }

    while(length >= 64) {
      Process(pInput);
      length -= 64;
      pInput  += 64;
    }

    if (length != 0) {
      std::memcpy((void*)(buffer + left), (void*)pInput, length);
    }
  }

  void cMD5_Context::Finish(unsigned char* pResult, std::string& sResult)
  {
    uint32_t last = 0;
    uint32_t padn = 0;
    uint32_t high = 0;
    uint32_t low = 0;
    unsigned char msglen[8];

    high = (total[0] >> 29) | (total[1] <<  3);
    low  = (total[0] <<  3);

    PUT_UINT32( low,  msglen, 0 );
    PUT_UINT32( high, msglen, 4 );

    last = total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    Update(md5_padding, padn);
    Update(msglen, 8);

    PUT_UINT32(state[0], pResult,  0);
    PUT_UINT32(state[1], pResult,  4);
    PUT_UINT32(state[2], pResult,  8);
    PUT_UINT32(state[3], pResult, 12);


    std::ostringstream t;

    t << std::hex << std::setw(2) << std::setfill('0');
    t << static_cast < unsigned short > (pResult[0]);
    t << static_cast < unsigned short > (pResult[1]);
    t << static_cast < unsigned short > (pResult[2]);
    t << static_cast < unsigned short > (pResult[3]);
    t << static_cast < unsigned short > (pResult[4]);
    t << static_cast < unsigned short > (pResult[5]);
    t << static_cast < unsigned short > (pResult[6]);
    t << static_cast < unsigned short > (pResult[7]);
    t << static_cast < unsigned short > (pResult[8]);
    t << static_cast < unsigned short > (pResult[9]);
    t << static_cast < unsigned short > (pResult[10]);
    t << static_cast < unsigned short > (pResult[11]);
    t << static_cast < unsigned short > (pResult[12]);
    t << static_cast < unsigned short > (pResult[13]);
    t << static_cast < unsigned short > (pResult[14]);
    t << static_cast < unsigned short > (pResult[15]);

    sResult = t.str();
  }



  // *** cMD5

  cMD5::cMD5()
  {
    std::memset(result, 0, sizeof(result));
  }

  bool cMD5::CalculateForString(const char* szInput)
  {
    cMD5_Context ctx;
    ctx.Start();
    ctx.Update((const unsigned char*)szInput, uint32_t(strlen(szInput)));
    ctx.Finish(&result[0], sResult);

    return true;
  }

  bool cMD5::CalculateForBuffer(const char* pData, size_t len)
  {
    cMD5_Context ctx;

    ctx.Start();
      uint32_t iSmaller = uint32_t(len);
      ctx.Update((const unsigned char*)pData, iSmaller);
    ctx.Finish(&result[0], sResult);

    return true;
  }

  bool cMD5::CalculateForFile(const string_t& sFilePath)
  {
    if (sFilePath.size() < 2) {
      result[0] = 0;

      return false;
    }

    FILE* f = fopen(sFilePath.c_str(), TEXT("rb"));
    if (f == nullptr) {
      perror("fopen");
      return false;
    }

    cMD5_Context ctx;

    ctx.Start();
      size_t i;
      unsigned char buf[1000];
      while ((i = fread(buf, 1, sizeof(buf), f )) > 0) {
        uint32_t iSmaller = uint32_t(i);
        ctx.Update(buf, iSmaller);
      }
    ctx.Finish(&result[0], sResult);

    fclose(f);

    return true;
  }

  unsigned char cMD5::h2d(unsigned char a, unsigned char b) const
  {
    if (a >= (unsigned char)('A')) a = a - (unsigned char)('A') + (unsigned char)(10);
    else a = a - (unsigned char)('0');

    if (b >= (unsigned char)('A')) b = b - (unsigned char)('A') + (unsigned char)(10);
    else b = b - (unsigned char)('0');

    return (a & (unsigned char)(0xF)) * (unsigned char)(16) + (b & (unsigned char)(0xF));
  }

  bool cMD5::SetResultFromFormatted(const string_t& sMD5Hash)
  {
    for (size_t i = 0; i < 32; i++) {
      if ((sMD5Hash[i] > 'z') || (sMD5Hash[i] < '0')) return false;
    }

    result[0]  = h2d(sMD5Hash[0],  sMD5Hash[1]);
    result[1]  = h2d(sMD5Hash[2],  sMD5Hash[3]);
    result[2]  = h2d(sMD5Hash[4],  sMD5Hash[5]);
    result[3]  = h2d(sMD5Hash[6],  sMD5Hash[7]);

    result[4]  = h2d(sMD5Hash[8],  sMD5Hash[9]);
    result[5]  = h2d(sMD5Hash[10], sMD5Hash[11]);
    result[6]  = h2d(sMD5Hash[12], sMD5Hash[13]);
    result[7]  = h2d(sMD5Hash[14], sMD5Hash[15]);

    result[8]  = h2d(sMD5Hash[16], sMD5Hash[17]);
    result[9]  = h2d(sMD5Hash[18], sMD5Hash[19]);
    result[10] = h2d(sMD5Hash[20], sMD5Hash[21]);
    result[11] = h2d(sMD5Hash[22], sMD5Hash[23]);

    result[12] = h2d(sMD5Hash[24], sMD5Hash[25]);
    result[13] = h2d(sMD5Hash[26], sMD5Hash[27]);
    result[14] = h2d(sMD5Hash[28], sMD5Hash[29]);
    result[15] = h2d(sMD5Hash[30], sMD5Hash[31]);

    return true;
  }

  bool cMD5::operator==(const cMD5 & rhs) const
  {
    return (sResult == rhs.sResult);
  }

  bool cMD5::operator==(const std::string & rhs) const
  {
    return (sResult == rhs);
  }

  std::string cMD5::GetResult() const
  {
    return sResult;
  }

  string_t cMD5::GetResultFormatted() const
  {
    // TODO: Huh? sResult already appears to be formatted. What is this code trying to do?
    //char temp[17];
    //std::strcpy(temp, (char*)result);
    //return std::string(temp);
    return string::ToString_t(sResult);
  }
  }
}

#ifdef BUILD_SPITFIRE_UNITTEST

#include <spitfire/util/unittest.h>

class cMD5UnitTest : protected spitfire::util::cUnitTestBase
{
public:
  cMD5UnitTest() :
    cUnitTestBase(TEXT("cMD5UnitTest"))
  {
  }

  void Test()
  {
    const char szText[] = { "This is the testing string for the md5 unit test" };

    spitfire::algorithm::cMD5 test;

    bool bResult = test.CalculateForString(szText);
    ASSERT(bResult);
    ASSERT(test.GetResultFormatted() == TEXT("fcfb62be1afe450706c373d7b9cbb3e3"));

    bResult = test.CalculateForBuffer(szText, strlen(szText));
    ASSERT(bResult);
    ASSERT(test.GetResultFormatted() == TEXT("fcfb62be1afe450706c373d7b9cbb3e3"));
  }
};

cMD5UnitTest gMD5UnitTest;

#endif // BUILD_DEBUG
