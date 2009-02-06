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

#ifndef CMD5_H
#define CMD5_H

namespace spitfire
{
  class cMD5_Context
  {
  public:
    uint32_t total[2];
    uint32_t state[4];
    unsigned char buffer[64];
  };

  class cMD5
  {
  public:
    cMD5();

    bool CheckString(const char* szInput);
    bool CheckBuffer(const char* pInput, size_t len);
    bool CheckFile(const std::string& sFilename);

    bool SetResultFromFormatted(const char* szMD5Hash);

    bool operator==(const cMD5& rhs) const;
    bool operator!=(const cMD5& rhs) const;

    bool operator==(const std::string& rhs) const;
    bool operator!=(const std::string& rhs) const;

    std::string GetResult() const;
    std::string GetResultFormatted() const;

  private:
    unsigned char h2d(unsigned char a, unsigned char b);

    cMD5_Context ctx;

    void Start();
    void Update(cMD5_Context* ctx, unsigned char* pInput, uint32_t length);
    void Finish(cMD5_Context* ctx);
    void Process(cMD5_Context* ctx, unsigned char data[64]);

    unsigned char result[16]; //Raw result
    std::string sResult; //Result formatted
  };
}

#endif // CMD5_H
